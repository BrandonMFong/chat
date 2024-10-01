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
#include <bflibcpp/bflibcpp.hpp>
#include <openssl/err.h>

using namespace BF;
const size_t kCipherBlocksize = 16;

int _encrypt(Data & in, const unsigned char *key,
            const unsigned char *iv, Data & out);
int _decrypt(Data & in, const unsigned char *key,
            const unsigned char *iv, Data & out);

CipherSymmetric::CipherSymmetric() : Cipher() {
	this->_key.clear();
	this->_iv.clear();
}

CipherSymmetric::~CipherSymmetric() {

}

CipherType CipherSymmetric::type() {
	return kCipherTypeSymmetric;
}

bool CipherSymmetric::isReady() const {
	bool keyisgood = false;
	bool ivisgood = false;
	unsigned char * key = (unsigned char *) this->_key.buffer();
	unsigned char * iv = (unsigned char *) this->_iv.buffer();

	for (int i = 0; i < this->_key.size(); i++) {
		keyisgood = key[i] != 0;
		if (keyisgood) {
			break;
		}
	}

	for (int i = 0; i < this->_iv.size(); i++) {
		ivisgood = iv[i] != 0;
		if (ivisgood) {
			break;
		}
	}

	return keyisgood && ivisgood;
}

int CipherSymmetric::genkey() {
	unsigned char key[CIPHER_SYMMETRIC_KEY_SIZE];
	unsigned char iv[CIPHER_SYMMETRIC_IV_SIZE];

	if (RAND_bytes(key, CIPHER_SYMMETRIC_KEY_SIZE) != 1) {
		LOG_DEBUG("couldn't generate key");
		return 1;
	} else if (RAND_bytes(iv, CIPHER_SYMMETRIC_IV_SIZE) != 1) {
		LOG_DEBUG("couldn't generate iv");
		return 2;
	}

	this->_key.alloc(CIPHER_SYMMETRIC_KEY_SIZE, key);
	this->_iv.alloc(CIPHER_SYMMETRIC_IV_SIZE, iv);

	return 0;
}

int CipherSymmetric::getkey(Data & key) const {
	const size_t s = CIPHER_SYMMETRIC_KEY_SIZE + CIPHER_SYMMETRIC_IV_SIZE;
	unsigned char buf[s];
	memcpy(buf, this->_key.buffer(), CIPHER_SYMMETRIC_KEY_SIZE);
	memcpy(buf + CIPHER_SYMMETRIC_KEY_SIZE, this->_iv.buffer(), CIPHER_SYMMETRIC_IV_SIZE);
	LOG_DEBUG("getkey key: %s", this->_key.hex().cString());
	LOG_DEBUG("getkey iv: %s", this->_iv.hex().cString());
	return key.alloc(s, buf); 
}

int CipherSymmetric::setkey(Data & key) {
	const size_t s = CIPHER_SYMMETRIC_KEY_SIZE + CIPHER_SYMMETRIC_IV_SIZE;
	if (key.size() != s) {
		LOG_DEBUG("size %ld != %ld", key.size(), s);
		return 10;
	} else if (key.buffer() == NULL) {
		LOG_DEBUG("key is null!");
		return 11;
	}

	unsigned char * buf = (unsigned char *) key.buffer();
	
	this->_key.alloc(CIPHER_SYMMETRIC_KEY_SIZE, buf);
	this->_iv.alloc(CIPHER_SYMMETRIC_IV_SIZE, buf + CIPHER_SYMMETRIC_KEY_SIZE);

	LOG_DEBUG("setkey key: %s", this->_key.hex().cString());
	LOG_DEBUG("setkey iv: %s", this->_iv.hex().cString());

	return 0;
}

int CipherSymmetric::encrypt(Data & in, Data & out) const {
	return _encrypt(in, (unsigned char *) this->_key.buffer(), (unsigned char *) this->_iv.buffer(), out);
}

int _encrypt(Data & _in, const unsigned char *key,
            const unsigned char *iv, Data & out) {
	int result = 0;
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
	const size_t blocksize = kCipherBlocksize;

	// keeping a copy
	Data in = _in;

	// make sure the input buffer length is a multiple of the blocksize
	// 
	// we need this since we are turning off padding	
	size_t newsize = ((in.size() / blocksize) + 1) * blocksize;
	in.alloc(newsize);
	
	// figure out cipher buffer length
	//
	// making sure it's a little bit more than the input size
	newsize = ((in.size() / blocksize) + 1) * (2 * blocksize);
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
		EVP_CIPHER_CTX_set_padding(ctx, 0);
		if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		}
	}

	if (!result) {
		/*
		 * Provide the message to be encrypted, and obtain the encrypted output.
		 * EVP_EncryptUpdate can be called multiple times if necessary
		 */
		if(1 != EVP_EncryptUpdate(ctx, (unsigned char *) out.buffer(), &len, (unsigned char *) in.buffer(), in.size())) {
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
			//ciphertext_len = ((ciphertext_len / blocksize) + 1) * blocksize;
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

int CipherSymmetric::decrypt(Data & in, Data & out) const {
	return _decrypt(in, (unsigned char *) this->_key.buffer(), (unsigned char *) this->_iv.buffer(), out);
}

int _decrypt(Data & in, const unsigned char *key,
            const unsigned char *iv, Data & out) {
	int result = 0;
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
	const size_t blocksize = kCipherBlocksize;
	
	/* 
	 * figure out cipher buffer length
	 * out text should be as long as in text
	 */
	size_t newsize = ((in.size() / blocksize) + 1) * blocksize;
	out.alloc(newsize);

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
		EVP_CIPHER_CTX_set_padding(ctx, 0);
		if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		}
	}

	if (!result) {
		/*
		 * Provide the message to be decrypted, and obtain the plaintext output.
		 * EVP_DecryptUpdate can be called multiple times if necessary.
		 */
		if(1 != EVP_DecryptUpdate(ctx, (unsigned char *) out.buffer(), &len, (unsigned char *) in.buffer(), in.size())) {
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
			LOG_DEBUG("current decrypted buffer: %s", (char *) out.buffer());
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			LOG_DEBUG("openssl error: %s", ERR_error_string(ERR_get_error(), NULL));
			result = -1;
		} else {
			plaintext_len += len;
		}
	}

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    
	return result;
}

