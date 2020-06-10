#ifndef __LibSign_h__
#define __LibSign_h__

#include <vector>

#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/x509.h>

#include "readelf.h"

namespace signelf
{
	typedef std::vector<unsigned char> yourVector;

	void hashSection(readelf::CReadElf *Elf, const char *sectionName, SHA_CTX *SHA);

	yourVector hashSave(const char *binFile);			//生成.text 和 .data段的哈希储存在vector里

	yourVector signHash(const unsigned char *hashBuf, unsigned int hashSize, unsigned char *keyBuf, unsigned int keySize);	//签名给定的hash并储存

	bool verifyLib(unsigned char *keyBuf, unsigned int keySize, const char *binFile);

	void hexPrint(const char *Name, const char *Buf, const unsigned int Length);
}

#endif
