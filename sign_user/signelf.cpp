#include <iostream>
#include <iomanip>
#include <iterator>

#include "readelf.h"
#include "signelf.h"

namespace signelf
{
	yourVector hashSave(const char *binFile)
	{
		yourVector retVector;	//返回vector

		// 创建SHA哈希内容
		SHA_CTX sha;
		SHA1_Init(&sha);
		readelf::CReadElf elf(binFile);	// 实例化一个CReadElf类出elf
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

	void hashSection(readelf::CReadElf *Elf, const char *sectionName, SHA_CTX *SHA)	//取出相应的section然后hash
	{
		readelf::yourVector section = Elf->getSection(sectionName);
		if(!section.empty())
		{
			SHA1_Update(SHA, &section[0], section.size());				// 每次都Update一下
		}
	}

	yourVector signHash(const unsigned char *hashBuf, unsigned int hashSize, unsigned char *keyBuf, unsigned int keySize)
	{
		
		RSA *pKey = NULL;
		BIO *pBio = NULL;
		yourVector retVector;
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

	bool verifyLib(unsigned char *keyBuf, unsigned int keySize, const char *binFile)
	{
		bool result = false;
		RSA *pKey = NULL;
		BIO *pBio;
		pBio = BIO_new_mem_buf(keyBuf, keySize);
		if(pBio)
		{
			d2i_RSA_PUBKEY_bio(pBio, &pKey);
			BIO_free(pBio);
		}
		if(pKey)
		{
			readelf::CReadElf elf(binFile);
			readelf::yourVector yourSig = elf.getSection(".lsesig");
			if(!yourSig.empty())
			{
				yourVector szHash = hashSave(binFile);		//提取
				result = (0 != RSA_verify(NID_sha1, &szHash[0], szHash.size(), &yourSig[0], yourSig.size(), pKey));	//验证
				RSA_free(pKey);
			} else {
				return true;
			}
		}
		return result;
	}

	void hexPrint(const char *Name, const char *Buf, const size_t Length)
	{
		std::cout << "hash (" << Length << ") ---\n" << std::endl;
		for(unsigned int i=0 ; i < Length ; ++i)
		{
			std::cout << std::hex << std::setw(2) << std::setfill('0') << Buf[i];
		}
		std::cout << "\n---" << std::endl;
	}

}
