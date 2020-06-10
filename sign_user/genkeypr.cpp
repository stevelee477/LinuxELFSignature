#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>
int main(int argc, char **argv)
{
	if(argc == 3)
	{
		std::cerr << "loading keyfile" << std::endl;
		BIO *bio;
		bio = BIO_new_file(argv[2], "r");
		if(bio)
		{
			char *nm = NULL;
			unsigned char *data = NULL;
			long len = 0;
			std::string sKeyType(argv[1]);
			if(sKeyType == "priv")
			{
				std::cerr << "Reading private key" << std::endl;
				PEM_bytes_read_bio(&data, &len, &nm, PEM_STRING_EVP_PKEY, bio, NULL, NULL);
			}
			else if(sKeyType == "pub")
			{
				std::cerr << "Reading public key" << std::endl;
				PEM_bytes_read_bio(&data, &len, &nm, PEM_STRING_PUBLIC, bio, NULL, NULL);
			}
			else
			{
				std::cerr << "Unknown keytype" << std::endl;
			}
			if(data)
			{
				std::cout << "unsigned char szKeyBuf[] = {" << std::endl;
				std::cout << "\t0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(data[0]);
				for(long i = 1; i < len; ++i)
				{
					std::cout << ", 0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(data[i]);
					if((i % 14) == 0)
					{
						std::cout << "\n\t";
					}
				}
				std::cout << "};" << std::endl;
				OPENSSL_free(data);
			}
		}
		std::cerr << "Done reading key" << std::endl;
	}
	else
	{
		std::cerr << "usage: " << argv[0] << " <priv|pub> <keyfile>" << std::endl
				  << "where priv and pub indicate the type of key in" << std::endl
				  << "keyfile. Keyfile is a PEM encoded key generated from" << std::endl
				  << "openssl genrsa or openssl rsa (See openssl(1) for details)" << std::endl;
	}
	return 0;
}

