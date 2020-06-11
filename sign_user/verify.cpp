#include <iostream>

#include "pubkey.h"
#include "signelf.h"

int main(int argc, char **argv) {
	bool bVerify = signelf::verify(keyBuf, sizeof(keyBuf), argv[1]);
	std::cout << "Result: " << bVerify << std::endl;;

	return 0;
}