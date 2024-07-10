/**
 * author: brando
 * date: 5/29/24
 *
 * https://www.openssl.org/docs/manmaster/man3/EVP_PKEY_encrypt_init_ex.html
 * https://medium.com/@amit.kulkarni/encrypting-decrypting-a-file-using-openssl-evp-b26e0e4d28d4
 * https://github.com/openssl/openssl/blob/master/demos/encrypt/rsa_encrypt.c
 *
 * https://www.openssl.org/docs/manmaster/man7/EVP_PKEY-RSA.html
 */

#include "cipherasymmetric.hpp"
#include "log.hpp"
#include <string.h>

CipherAsymmetric::CipherAsymmetric() : Cipher() {
	this->_libctx = NULL;
	this->_peerdata = {"peer 1", "P-256"};
}

CipherAsymmetric::~CipherAsymmetric() {
}

CipherType CipherAsymmetric::type() {
	return kCipherTypeAsymmetric;
}

/*
 * The public key needs to be given to the other peer
 * The following code extracts the public key data from the private key
 * and then builds an EVP_KEY public key.
 */
static int get_peer_public_key(PEER_DATA *peer, OSSL_LIB_CTX *libctx)
{
    int ret = 0;
    EVP_PKEY_CTX *ctx;
    OSSL_PARAM params[3];
    unsigned char pubkeydata[256];
    size_t pubkeylen;

    /* Get the EC encoded public key data from the peers private key */
    if (!EVP_PKEY_get_octet_string_param(peer->priv, OSSL_PKEY_PARAM_PUB_KEY,
                                         pubkeydata, sizeof(pubkeydata),
                                         &pubkeylen))
        return 0;

    /* Create a EC public key from the public key data */
    ctx = EVP_PKEY_CTX_new_from_name(libctx, "EC", NULL);
    if (ctx == NULL)
        return 0;
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME,
                                                 (char *)peer->curvename, 0);
    params[1] = OSSL_PARAM_construct_octet_string(OSSL_PKEY_PARAM_PUB_KEY,
                                                  pubkeydata, pubkeylen);
    params[2] = OSSL_PARAM_construct_end();
    ret = EVP_PKEY_fromdata_init(ctx) > 0
          && (EVP_PKEY_fromdata(ctx, &peer->pub, EVP_PKEY_PUBLIC_KEY,
                                params) > 0);
    EVP_PKEY_CTX_free(ctx);
    return ret;
}

static int create_peer(PEER_DATA *peer, OSSL_LIB_CTX *libctx)
{
    int ret = 0;
    EVP_PKEY_CTX *ctx = NULL;
    OSSL_PARAM params[2];

    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME,
                                                 (char *)peer->curvename, 0);
    params[1] = OSSL_PARAM_construct_end();

    ctx = EVP_PKEY_CTX_new_from_name(libctx, "EC", NULL);
    if (ctx == NULL)
        return 0;

    if (EVP_PKEY_keygen_init(ctx) <= 0
            || !EVP_PKEY_CTX_set_params(ctx, params)
            || EVP_PKEY_generate(ctx, &peer->priv) <= 0
            || !get_peer_public_key(peer, libctx)) {
        EVP_PKEY_free(peer->priv);
        peer->priv = NULL;
        goto err;
    }
    ret = 1;
err:
    EVP_PKEY_CTX_free(ctx);
    return ret;
}

static void destroy_peer(PEER_DATA *peer)
{
    EVP_PKEY_free(peer->priv);
    EVP_PKEY_free(peer->pub);
}

static int generate_secret(PEER_DATA *peerA, EVP_PKEY *peerBpub,
                           OSSL_LIB_CTX *libctx)
{
    unsigned char *secret = NULL;
    size_t secretlen = 0;
    EVP_PKEY_CTX *derivectx;

    /* Create an EVP_PKEY_CTX that contains peerA's private key */
    derivectx = EVP_PKEY_CTX_new_from_pkey(libctx, peerA->priv, NULL);
    if (derivectx == NULL)
        return 0;

    if (EVP_PKEY_derive_init(derivectx) <= 0)
        goto cleanup;
    /* Set up peerB's public key */
    if (EVP_PKEY_derive_set_peer(derivectx, peerBpub) <= 0)
        goto cleanup;

    /*
     * For backwards compatibility purposes the OpenSSL ECDH provider supports
     * optionally using a X963KDF to expand the secret data. This can be done
     * with code similar to the following.
     *
     *   OSSL_PARAM params[5];
     *   size_t outlen = 128;
     *   unsigned char ukm[] = { 1, 2, 3, 4 };
     *   params[0] = OSSL_PARAM_construct_utf8_string(OSSL_EXCHANGE_PARAM_KDF_TYPE,
     *                                                "X963KDF", 0);
     *   params[1] = OSSL_PARAM_construct_utf8_string(OSSL_EXCHANGE_PARAM_KDF_DIGEST,
     *                                                "SHA256", 0);
     *   params[2] = OSSL_PARAM_construct_size_t(OSSL_EXCHANGE_PARAM_KDF_OUTLEN,
     *                                           &outlen);
     *   params[3] = OSSL_PARAM_construct_octet_string(OSSL_EXCHANGE_PARAM_KDF_UKM,
     *                                                 ukm, sizeof(ukm));
     *   params[4] = OSSL_PARAM_construct_end();
     *   if (!EVP_PKEY_CTX_set_params(derivectx, params))
     *       goto cleanup;
     *
     * Note: After the secret is generated below, the peer could alternatively
     * pass the secret to a KDF to derive additional key data from the secret.
     * See demos/kdf/hkdf.c for an example (where ikm is the secret key)
     */

    /* Calculate the size of the secret and allocate space */
    if (EVP_PKEY_derive(derivectx, NULL, &secretlen) <= 0)
        goto cleanup;
    secret = (unsigned char *)OPENSSL_malloc(secretlen);
    if (secret == NULL)
        goto cleanup;

    /*
     * Derive the shared secret. In this example 32 bytes are generated.
     * For EC curves the secret size is related to the degree of the curve
     * which is 256 bits for P-256.
     */
    if (EVP_PKEY_derive(derivectx, secret, &secretlen) <= 0)
        goto cleanup;
    peerA->secret = secret;
    peerA->secretlen = secretlen;

    printf("Shared secret (%s):\n", peerA->name);
    BIO_dump_indent_fp(stdout, peerA->secret, peerA->secretlen, 2);
    putchar('\n');

    return 1;
cleanup:
    OPENSSL_free(secret);
    EVP_PKEY_CTX_free(derivectx);
    return 0;
}

int CipherAsymmetric::init() {
	if (!create_peer(&this->_peerdata, this->_libctx)) {
		return 1;
	}
	return 0;
}

int CipherAsymmetric::deinit() {
	destroy_peer(&this->_peerdata);
	return 0;
}

int CipherAsymmetric::encrypt(BF::Data & in, BF::Data & out) {
	return 0;
}

int CipherAsymmetric::decrypt(BF::Data & in, BF::Data & out) {
	return 0;
}

