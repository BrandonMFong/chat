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
#include <openssl/rsa.h>
#include <openssl/pem.h>

CipherAsymmetric::CipherAsymmetric() : Cipher() {
	this->_libctx = NULL;
	this->_keys = NULL;
}

CipherAsymmetric::~CipherAsymmetric() {
}

CipherType CipherAsymmetric::type() {
	return kCipherTypeAsymmetric;
}

/* A property query used for selecting algorithm implementations. */
static const char *propq = NULL;

/*
 * Generates an RSA public-private key pair and returns it.
 * The number of bits is specified by the bits argument.
 *
 * This uses a more concise way of generating an RSA key, which is suitable for
 * simple cases. It is used if -s is passed on the command line, otherwise the
 * long method above is used. The ability to choose between these two methods is
 * shown here only for demonstration; the results are equivalent.
 */
static EVP_PKEY *generate_rsa_key_short(OSSL_LIB_CTX * libctx, unsigned int bits) {
    return EVP_PKEY_Q_keygen(libctx, propq, "RSA", (size_t)bits);
}

int CipherAsymmetric::init() {
	this->_keys = generate_rsa_key_short(this->_libctx, 4096);
	return 0;
}

int CipherAsymmetric::deinit() {
	EVP_PKEY_free(this->_keys);
	OSSL_LIB_CTX_free(this->_libctx);
	return 0;
}

/*
 * The length of the input data that can be encrypted is limited by the
 * RSA key length minus some additional bytes that depends on the padding mode.
 *
 */
int do_encrypt(OSSL_LIB_CTX * libctx, EVP_PKEY * pub_key,
                      const unsigned char * in, size_t in_len,
                      unsigned char ** out, size_t * out_len)
{
    int ret = 1;
    size_t buf_len = 0;
    unsigned char * buf = NULL;
    const char * propq = NULL;
    EVP_PKEY_CTX * ctx = NULL;

    /* Get public key */
    if (pub_key == NULL) {
        fprintf(stderr, "Get public key failed.\n");
        goto cleanup;
    }
    ctx = EVP_PKEY_CTX_new_from_pkey(libctx, pub_key, propq);
    if (ctx == NULL) {
        fprintf(stderr, "EVP_PKEY_CTX_new_from_pkey() failed.\n");
        goto cleanup;
    }
    /* If no optional parameters are required then NULL can be passed */
    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
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
    ret = 0;

cleanup:
    if (ret)
        OPENSSL_free(buf);
    EVP_PKEY_CTX_free(ctx);
    return ret;
}

int do_decrypt(
	OSSL_LIB_CTX * libctx,
	EVP_PKEY * priv_key,
	const unsigned char * in,
	size_t in_len,
	unsigned char ** out,
	size_t * out_len)
{
    int ret = 1, pub = 0;
    size_t buf_len = 0;
    unsigned char * buf = NULL;
    const char * propq = NULL;
    EVP_PKEY_CTX * ctx = NULL;

    /* Get private key */
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
    /* If no optional parameters are required then NULL can be passed */
    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
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
    ret = 0;

cleanup:
    if (ret)
        OPENSSL_free(buf);
    EVP_PKEY_CTX_free(ctx);
    return ret;
}

int CipherAsymmetric::encrypt(BF::Data & in, BF::Data & out) {
    size_t encrypted_len = 0;
    unsigned char * encrypted = NULL;
	int result = do_encrypt(
		this->_libctx,
		this->_keys,
		(const unsigned char *) in.buffer(),
		in.size(),
		&encrypted,
		&encrypted_len);

	out.alloc(encrypted_len, encrypted);
	OPENSSL_free(encrypted);

	return result;
}

int CipherAsymmetric::decrypt(BF::Data & in, BF::Data & out) {
    size_t decrypted_len = 0;
    unsigned char * decrypted = NULL;
	int result = do_decrypt(
			this->_libctx,
			this->_keys,
			(const unsigned char *) in.buffer(),
			in.size(),
			&decrypted,
			&decrypted_len);

	out.alloc(decrypted_len, decrypted);
	OPENSSL_free(decrypted);

	return result;
}

