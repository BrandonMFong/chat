/**
 * author: brando
 * date: 5/29/24
 *
 * https://www.openssl.org/docs/man1.1.1/man3/EVP_CIPHER_CTX_set_padding.html
 */

#include "ciphersymmetric.hpp"
#include "log.hpp"
#include <openssl/rand.h>
#include <openssl/evp.h>

using namespace BF;

int _encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, Data & out);
int _decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, Data & out);

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

int CipherSymmetric::encrypt(Data & in, Data & out) {
	return _encrypt((unsigned char *) in.buffer(), in.size(), this->_key, this->_iv, out);
}

int _encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, Data & out) {
	int result = 0;
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
	const size_t blocksize = 16;
	
	/* 
	 * figure out cipher buffer length
	 */
	size_t newsize = ((plaintext_len / blocksize) + 1) * blocksize;
	out.alloc(newsize);

    /* Create and initialise the context */
	if (!result) {
		if(!(ctx = EVP_CIPHER_CTX_new())) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		}
	}

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
	if (!result) {
		if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		}
	}

	if (!result) {
		//EVP_CIPHER_CTX_set_padding(ctx, 1);
		/*
		 * Provide the message to be encrypted, and obtain the encrypted output.
		 * EVP_EncryptUpdate can be called multiple times if necessary
		 */
		if(1 != EVP_EncryptUpdate(ctx, (unsigned char *) out.buffer(), &len, plaintext, plaintext_len)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		}
	}
    
	if (!result) {
		ciphertext_len = len;

		/*
		 * Finalise the encryption. Further ciphertext bytes may be written at
		 * this stage.
		 */
		if(1 != EVP_EncryptFinal_ex(ctx, (unsigned char *) out.buffer() + len, &len)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		} else {
			ciphertext_len += len;

			size_t newsize = ((ciphertext_len / blocksize) + 1) * blocksize;
			out.resize(ciphertext_len);

			if (out.size() % blocksize) {
				result = 1;
			}
		}
	}

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return result;
}

int CipherSymmetric::decrypt(BF::Data & in, BF::Data & out) {
	return _decrypt((unsigned char *) in.buffer(), in.size(), this->_key, this->_iv, out);
}

int _decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, Data & out) {
	int result = 0;
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
	
	/*
	 * out text should be as long as in text
	 */
	out.alloc(ciphertext_len);

    /* Create and initialise the context */
	if (!result) {
		if(!(ctx = EVP_CIPHER_CTX_new())) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		}
	}

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
	if (!result) {
		if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		}
	}

	if (!result) {
		//EVP_CIPHER_CTX_set_padding(ctx, 0);
		/*
		 * Provide the message to be decrypted, and obtain the plaintext output.
		 * EVP_DecryptUpdate can be called multiple times if necessary.
		 */
		if(1 != EVP_DecryptUpdate(ctx, (unsigned char *) out.buffer(), &len, ciphertext, ciphertext_len)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		}
	}

	if (!result) {
		plaintext_len = len;

		/*
		 * Finalise the decryption. Further plaintext bytes may be written at
		 * this stage.
		 */
		if(1 != EVP_DecryptFinal_ex(ctx, (unsigned char *) out.buffer() + len, &len)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		} else {
			plaintext_len += len;
		}
	}

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    
	return result;
}

