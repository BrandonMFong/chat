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

int do_crypt(Data & in, Data & out, unsigned char * key, unsigned char * iv, int do_encrypt);

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
	//return do_crypt(in, out, this->_key, this->_iv, 1);
	return _encrypt((unsigned char *) in.buffer(), in.size(), this->_key, this->_iv, out);
}

int _encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, Data & out) {
	int result = 0;
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

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
		/* 
		 * figure out cipher buffer length
		 */
		const size_t blocksize = 16;
		const size_t cs = ((plaintext_len % blocksize) + 1) * blocksize;
		out.alloc(cs);

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

		out.resize(out.size() + len);
		//out.resize(ciphertext_len);

		/*
		 * Finalise the encryption. Further ciphertext bytes may be written at
		 * this stage.
		 */
		if(1 != EVP_EncryptFinal_ex(ctx, (unsigned char *) out.buffer() + len, &len)) {
			LOG_DEBUG("%s:%d", __FILE__, __LINE__);
			result = -1;
		} else {
			ciphertext_len += len;

			out.resize(ciphertext_len);
		}
	}

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return result;
}

int CipherSymmetric::decrypt(BF::Data & in, BF::Data & out) {
	//return do_crypt(in, out, this->_key, this->_iv, 0);
	return _decrypt((unsigned char *) in.buffer(), in.size(), this->_key, this->_iv, out);
}

int _decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, Data & out) {
	int result = 0;
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

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
		 * out text should be as long as in text
		 */
		out.alloc(ciphertext_len);

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
			out.resize(plaintext_len);
		}
	}

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    
	return result;
}

int do_crypt(Data & in, Data & out, unsigned char * key, unsigned char * iv, int do_encrypt)
 {
     /* Allow enough space in output buffer for additional block */
     unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
     int inlen, outlen;
     EVP_CIPHER_CTX *ctx;
     /*
      * Bogus key and IV: we'd normally set these from
      * another source.
      */
     //unsigned char key[] = "0123456789abcdeF";
     //unsigned char iv[] = "1234567887654321";

     /* Don't set key or IV right away; we want to check lengths */
     ctx = EVP_CIPHER_CTX_new();
     EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
                       do_encrypt);
     OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 16);
     OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

     /* Now we can set key and IV */
     EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, do_encrypt);
	EVP_CIPHER_CTX_set_padding(ctx, 0);

	 /*
     for (;;) {
         inlen = fread(inbuf, 1, 1024, in);
         if (inlen <= 0)
             break;
         if (!EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, inlen)) {
             EVP_CIPHER_CTX_free(ctx);
             return 0;
         }
         fwrite(outbuf, 1, outlen, out);
     }
 */
	 out.alloc(1024 + EVP_MAX_BLOCK_LENGTH);
	 if (!EVP_CipherUpdate(ctx, (unsigned char *) out.buffer(), &outlen, (unsigned char *) in.buffer(), in.size())) {
		 EVP_CIPHER_CTX_free(ctx);
		 return 1;
	 }

     if (!EVP_CipherFinal_ex(ctx, outbuf, &outlen)) {
         /* Error */
         EVP_CIPHER_CTX_free(ctx);
         return 1;
     }
     //fwrite(outbuf, 1, outlen, out);

     EVP_CIPHER_CTX_free(ctx);
     return 0;
 }
