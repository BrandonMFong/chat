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
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/decoder.h>
#include <openssl/core_names.h>

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
int get_peer_public_key(PEER_DATA *peer, OSSL_LIB_CTX *libctx)
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

int create_peer(PEER_DATA *peer, OSSL_LIB_CTX *libctx)
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

void destroy_peer(PEER_DATA *peer)
{
    EVP_PKEY_free(peer->priv);
    EVP_PKEY_free(peer->pub);
}

int generate_secret(PEER_DATA *peerA, EVP_PKEY *peerBpub,
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
/*
 * For do_encrypt(), load an RSA public key from pub_key_der[].
 * For do_decrypt(), load an RSA private key from priv_key_der[].
 */
static EVP_PKEY *get_key(OSSL_LIB_CTX *libctx, const char *propq, int pub)
{
    OSSL_DECODER_CTX *dctx = NULL;
    EVP_PKEY *pkey = NULL;
    int selection;
    const unsigned char *data;
    size_t data_len;

    if (pub) {
        selection = EVP_PKEY_PUBLIC_KEY;
        data = pub_key_der;
        data_len = sizeof(pub_key_der);
    } else {
        selection = EVP_PKEY_KEYPAIR;
        data = priv_key_der;
        data_len = sizeof(priv_key_der);
    }
    dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "DER", NULL, "RSA",
                                         selection, libctx, propq);
    (void)OSSL_DECODER_from_data(dctx, &data, &data_len);
    OSSL_DECODER_CTX_free(dctx);
    return pkey;
}

/* Set optional parameters for RSA OAEP Padding */
static void set_optional_params(OSSL_PARAM *p, const char *propq)
{
    static unsigned char label[] = "label";

    /* "pkcs1" is used by default if the padding mode is not set */
    *p++ = OSSL_PARAM_construct_utf8_string(OSSL_ASYM_CIPHER_PARAM_PAD_MODE,
                                            OSSL_PKEY_RSA_PAD_MODE_OAEP, 0);
    /* No oaep_label is used if this is not set */
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_ASYM_CIPHER_PARAM_OAEP_LABEL,
                                             label, sizeof(label));
    /* "SHA1" is used if this is not set */
    *p++ = OSSL_PARAM_construct_utf8_string(OSSL_ASYM_CIPHER_PARAM_OAEP_DIGEST,
                                            "SHA256", 0);
    /*
     * If a non default property query needs to be specified when fetching the
     * OAEP digest then it needs to be specified here.
     */
    if (propq != NULL)
        *p++ = OSSL_PARAM_construct_utf8_string(OSSL_ASYM_CIPHER_PARAM_OAEP_DIGEST_PROPS,
                                                (char *)propq, 0);

    /*
     * OSSL_ASYM_CIPHER_PARAM_MGF1_DIGEST and
     * OSSL_ASYM_CIPHER_PARAM_MGF1_DIGEST_PROPS can also be optionally added
     * here if the MGF1 digest differs from the OAEP digest.
     */

    *p = OSSL_PARAM_construct_end();
}

/*
 * The length of the input data that can be encrypted is limited by the
 * RSA key length minus some additional bytes that depends on the padding mode.
 *
 */
static int do_encrypt(OSSL_LIB_CTX *libctx,
                      const unsigned char *in, size_t in_len,
                      unsigned char **out, size_t *out_len)
{
    int ret = 0, pub = 1;
    size_t buf_len = 0;
    unsigned char *buf = NULL;
    const char *propq = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *pub_key = NULL;
    OSSL_PARAM params[5];

    /* Get public key */
    pub_key = get_key(libctx, propq, pub);
    if (pub_key == NULL) {
        fprintf(stderr, "Get public key failed.\n");
        goto cleanup;
    }
    ctx = EVP_PKEY_CTX_new_from_pkey(libctx, pub_key, propq);
    if (ctx == NULL) {
        fprintf(stderr, "EVP_PKEY_CTX_new_from_pkey() failed.\n");
        goto cleanup;
    }
    set_optional_params(params, propq);
    /* If no optional parameters are required then NULL can be passed */
    if (EVP_PKEY_encrypt_init_ex(ctx, params) <= 0) {
        fprintf(stderr, "EVP_PKEY_encrypt_init_ex() failed.\n");
        goto cleanup;
    }
    /* Calculate the size required to hold the encrypted data */
    if (EVP_PKEY_encrypt(ctx, NULL, &buf_len, in, in_len) <= 0) {
        fprintf(stderr, "EVP_PKEY_encrypt() failed.\n");
        goto cleanup;
    }
    buf = (unsigned char *) OPENSSL_zalloc(buf_len);
    if (buf  == NULL) {
        fprintf(stderr, "Malloc failed.\n");
        goto cleanup;
    }
    if (EVP_PKEY_encrypt(ctx, buf, &buf_len, in, in_len) <= 0) {
        fprintf(stderr, "EVP_PKEY_encrypt() failed.\n");
        goto cleanup;
    }
    *out_len = buf_len;
    *out = buf;
    fprintf(stdout, "Encrypted:\n");
    BIO_dump_indent_fp(stdout, buf, buf_len, 2);
    fprintf(stdout, "\n");
    ret = 1;

cleanup:
    if (!ret)
        OPENSSL_free(buf);
    EVP_PKEY_free(pub_key);
    EVP_PKEY_CTX_free(ctx);
    return ret;
}

static int do_decrypt(OSSL_LIB_CTX *libctx, const unsigned char *in, size_t in_len,
                      unsigned char **out, size_t *out_len)
{
    int ret = 0, pub = 0;
    size_t buf_len = 0;
    unsigned char *buf = NULL;
    const char *propq = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *priv_key = NULL;
    OSSL_PARAM params[5];

    /* Get private key */
    priv_key = get_key(libctx, propq, pub);
    if (priv_key == NULL) {
        fprintf(stderr, "Get private key failed.\n");
        goto cleanup;
    }
    ctx = EVP_PKEY_CTX_new_from_pkey(libctx, priv_key, propq);
    if (ctx == NULL) {
        fprintf(stderr, "EVP_PKEY_CTX_new_from_pkey() failed.\n");
        goto cleanup;
    }

    /* The parameters used for encryption must also be used for decryption */
    set_optional_params(params, propq);
    /* If no optional parameters are required then NULL can be passed */
    if (EVP_PKEY_decrypt_init_ex(ctx, params) <= 0) {
        fprintf(stderr, "EVP_PKEY_decrypt_init_ex() failed.\n");
        goto cleanup;
    }
    /* Calculate the size required to hold the decrypted data */
    if (EVP_PKEY_decrypt(ctx, NULL, &buf_len, in, in_len) <= 0) {
        fprintf(stderr, "EVP_PKEY_decrypt() failed.\n");
        goto cleanup;
    }
    buf = (unsigned char *) OPENSSL_zalloc(buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Malloc failed.\n");
        goto cleanup;
    }
    if (EVP_PKEY_decrypt(ctx, buf, &buf_len, in, in_len) <= 0) {
        fprintf(stderr, "EVP_PKEY_decrypt() failed.\n");
        goto cleanup;
    }
    *out_len = buf_len;
    *out = buf;
    fprintf(stdout, "Decrypted:\n");
    BIO_dump_indent_fp(stdout, buf, buf_len, 2);
    fprintf(stdout, "\n");
    ret = 1;

cleanup:
    if (!ret)
        OPENSSL_free(buf);
    EVP_PKEY_free(priv_key);
    EVP_PKEY_CTX_free(ctx);
    return ret;
}

int CipherAsymmetric::encrypt(BF::Data & in, BF::Data & out) {
    size_t encrypted_len = 0;
    unsigned char * encrypted = NULL;
	do_encrypt(this->_libctx,
                      (const unsigned char *) in.buffer(), in.size(),
                      &encrypted, &encrypted_len);

	//out.alloc(encrypted_len, encrypted);
	OPENSSL_free(encrypted);

	return 0;
}

int CipherAsymmetric::decrypt(BF::Data & in, BF::Data & out) {
	return 0;
}

