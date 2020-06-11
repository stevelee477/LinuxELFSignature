#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include "signelf.h"

const char priv[] = "private.pem";

int main(int argc, char *argv[]) {
    BIO *bio = BIO_new_file(priv, "r");
    char *nm = NULL;
    unsigned char *keyBuf = NULL;
    long len = 0;

    if (!bio) {
        std::cerr << "Read private key failed" << std::endl;
        return 0;
    }
    PEM_bytes_read_bio(&keyBuf, &len, &nm, PEM_STRING_EVP_PKEY, bio, NULL, NULL);
    if (!keyBuf) {
        std::cerr << "Read private key failed" << std::endl;
        return 0;
    }
    std::cout << "Read public key success" << std::endl;

    int ret = 0;
    signelf::SigVec hashVal;
    hashVal = signelf::hashSave(argv[1]);
    if (!hashVal.empty()) {
        // 生成签名
        signelf::SigVec sig;
        sig = signelf::signHash(reinterpret_cast<const unsigned char *>(&hashVal[0]), hashVal.size(), keyBuf, len);
        if (!sig.empty()) {
            // 输出到sig文件
            std::ofstream out("object.sig");
            if (out) {
                out.write(reinterpret_cast<char *>(&sig[0]), sig.size());
                out.close();
                std::string Cmd("/usr/bin/objcopy --add-section .sig=object.sig ");
                Cmd += argv[1];
                // 把生成的拷贝进去
                ret = system(Cmd.c_str());
                std::remove("object.sig");
            }
        }
    }
    return ret;
}
