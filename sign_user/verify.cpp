#include <iostream>

#include "pubkey.h" 
#include "signelf.h"

int main(int argc, char **argv)
{

	bool result = signelf::verifyLib(keyBuf, sizeof(keyBuf), argv[1]);
	std::cout << "verify result is : " << result << std::endl;;

	return result ? 0 : 1;
}

