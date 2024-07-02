/**
 * author: brando
 * date: 5/29/24
 */

#include "ciphersymmetric.hpp"
#include "log.hpp"
#include <openssl/rand.h>
#include <openssl/evp.h>

int _encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int _decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);

CipherSymmetric::CipherSymmetric() : Cipher() {

}

CipherSymmetric::~CipherSymmetric() {

}

int CipherSymmetric::init() {
	if (RAND_bytes(this->_key, CIPHER_SYMMETRIC_KEY_SIZE) != 1) {
		LOG_DEBUG("couldn't generate key");
		return 1;
	} else if (RAND_bytes(this->_iv, CIPHER_SYMMETRIC_IV_SIZE) != 1) {
		LOG_DEBUG("couldn't generate iv");
		return 2;
	}

	return 0;
}

int CipherSymmetric::encrypt(BF::Data & in, BF::Data & out) {
	unsigned char ciphertext[CIPHER_SYMMETRIC_CIPHER_TEXT_SIZE];
	int cipherlen = _encrypt((unsigned char *) in.buffer(), in.size(), this->_key, this->_iv, (unsigned char *) ciphertext);
	if (cipherlen == -1) {
		return -1;
	}

	out.alloc(cipherlen, ciphertext);
	return 0;
}

int _encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) {
		LOG_DEBUG("%s:%d", __FILE__, __LINE__);
		return -1;
	}

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
		LOG_DEBUG("%s:%d", __FILE__, __LINE__);
		return -1;
	}

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
		LOG_DEBUG("%s:%d", __FILE__, __LINE__);
		return -1;
	}
    
	ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
		LOG_DEBUG("%s:%d", __FILE__, __LINE__);
		return -1;
	}

    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int CipherSymmetric::decrypt(BF::Data & in, BF::Data & out) {
	unsigned char plaintext[CIPHER_SYMMETRIC_PLAIN_TEXT_SIZE];
	int plainlen = _decrypt((unsigned char *) in.buffer(), in.size(), this->_key, this->_iv, plaintext);

	if (plainlen == -1) {
		return -1;
	}

	out.alloc(plainlen, plaintext);
	return 0;
}

int _decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) {
		LOG_DEBUG("%s:%d", __FILE__, __LINE__);
		return -1;
	}

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
		LOG_DEBUG("%s:%d", __FILE__, __LINE__);
		return -1;
	}

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
		LOG_DEBUG("%s:%d", __FILE__, __LINE__);
		return -1;
	}
    
	plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
		LOG_DEBUG("%s:%d", __FILE__, __LINE__);
		return -1;
	}

    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

