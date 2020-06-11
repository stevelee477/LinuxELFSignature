#include <iostream>
#include <iomanip>
#include <iterator>

#include "readelf.h"
#include "signelf.h"

namespace signelf {
	SigVec hashSave(const char *binFile) {
		SigVec retVector;	//返回vector

		// 创建SHA哈希内容
		SHA_CTX sha;
		SHA1_Init(&sha);
		readelf::ReadElf elf(binFile);	// 实例化一个ReadElf类出elf
		hashSection(&elf, ".data", &sha);
		hashSection(&elf, ".text", &sha);
		hashSection(&elf, ".rodata", &sha);
		hashSection(&elf, ".init", &sha);
		hashSection(&elf, ".fini", &sha);
		hashSection(&elf, ".ctors", &sha);
		hashSection(&elf, ".dtors", &sha);
		hashSection(&elf, ".dynamic", &sha);
		hashSection(&elf, ".dynsym", &sha);
		hashSection(&elf, ".dynstr", &sha);
		retVector.resize(SHA_DIGEST_LENGTH);
		SHA1_Final(&retVector[0], &sha);		//拿到最终的散列
		return retVector;
	}

	void hashSection(readelf::ReadElf *Elf, const char *sectionName, SHA_CTX *SHA) { //取出相应的section然后hash
		readelf::SectionVec section = Elf->getSection(sectionName);
		if(!section.empty()) {
			SHA1_Update(SHA, &section[0], section.size());				// 每次都Update一下
		}
	}

	SigVec signHash(const unsigned char *hashBuf, unsigned int hashSize, unsigned char *keyBuf, unsigned int keySize) {
		RSA *pKey = NULL;
		BIO *pBio = NULL;
		SigVec retVector;
		pBio = BIO_new_mem_buf(keyBuf, keySize);
		if(pBio)
		{
			d2i_RSAPrivateKey_bio(pBio, &pKey);
		}

		if(pKey)
		{
			unsigned int nSigLen;
			retVector.resize(RSA_size(pKey));
			RSA_sign(NID_sha1, hashBuf, hashSize, &retVector[0], &nSigLen, pKey);
		}

		return retVector;
	}

	bool verify(unsigned char *keyBuf, unsigned int keySize, const char *binFile) { //验证签名
		bool result = false;
		RSA *pKey = NULL;
		BIO *pBio;
		pBio = BIO_new_mem_buf(keyBuf, keySize);
		if(pBio) {
			d2i_RSA_PUBKEY_bio(pBio, &pKey);
			BIO_free(pBio);
		}
		if(pKey) {
			readelf::ReadElf elf(binFile);
			readelf::SectionVec yourSig = elf.getSection(".sig");
			if(!yourSig.empty())
			{
				SigVec szHash = hashSave(binFile);		//提取
				result = (0 != RSA_verify(NID_sha1, &szHash[0], szHash.size(), &yourSig[0], yourSig.size(), pKey));	//验证
				RSA_free(pKey);
			} else {
				std::cout << "No sig found" << std::endl;
				return true;
			}
		}
		return result;
	}
}
