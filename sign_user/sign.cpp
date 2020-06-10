#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <iterator>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include "signelf.cpp"
#include "signelf.h"
#include "privkey.h"	
int main(int argc, char **argv)
{
	int ret=0;
	signelf::yourVector hashVal;
	hashVal = signelf::hashSave(argv[1]);
	if(!hashVal.empty())
	{
		// 生成签名
		signelf::yourVector yourSig;
		yourSig = signelf::signHash(reinterpret_cast<const unsigned char*>(&hashVal[0]), hashVal.size(), keyBuf, sizeof(keyBuf));
		if(!yourSig.empty())
		{
			// 输出到sig文件
			std::ofstream out("object.sig");
			if(out)
			{
				out.write(reinterpret_cast<char*>(&yourSig[0]), yourSig.size());
				out.close();
				std::string Cmd("/usr/bin/objcopy --add-section .lsesig=object.sig ");
				Cmd += argv[1];
				// 把生成的拷贝进去
				ret = system(Cmd.c_str());
				std::remove("object.sig");
			}
			else
			{
				// 错误代码：无法写入
				ret=3;
			}
		}
		else
		{
			// 错误代码：无法生成
			ret=2;
		}

	}
	else
	{
		// 错误代码：无法生成哈希
		ret=1;
	}
	return ret;
}

