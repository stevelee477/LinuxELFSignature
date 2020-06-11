#include <iostream>

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include "signelf.h"

const char pub[] = "public.pem";

int main(int argc, char *argv[]) {
	BIO *bio = BIO_new_file(pub, "r");
	char *nm = NULL;
	unsigned char *keyBuf = NULL;
	long len = 0;

	if (!bio) {
		std::cerr << "Read public key failed" << std::endl;
	}
	PEM_bytes_read_bio(&keyBuf, &len, &nm, PEM_STRING_PUBLIC, bio, NULL, NULL);
	if (!keyBuf) {
		std::cerr << "Read public key failed" << std::endl;
	}

	bool bVerify = signelf::verify(keyBuf, len, argv[1]);
	std::cout << "Result: " << bVerify << std::endl;;

	return bVerify ? 0 : 1;
}

