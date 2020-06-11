#ifndef __signelf_h__
#define __signelf_h__

#include <vector>

#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/x509.h>

#include "readelf.h"

namespace signelf {
	typedef std::vector<unsigned char> SigVec;
	void hashSection(readelf::ReadElf *Elf, const char *sectionName, SHA_CTX *SHA);
	SigVec hashSave(const char *binFile); //生成.text 和 .data段的哈希储存在vector里
	SigVec signHash(const unsigned char *hashBuf, unsigned int hashSize, unsigned char *keyBuf, unsigned int keySize); //签名给定的hash并储存
	bool verify(unsigned char *keyBuf, unsigned int keySize, const char *binFile);
} // namespace signelf

#endif