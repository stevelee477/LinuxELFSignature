# Linux ELF 签名

使用Kernel Module劫持了Execve系统调用来实现对可执行文件的签名验证

## 安装方式

```Shell
cd sign_kernel
make
sudo sh load.sh
cd ../sign_user
make
sudo sh load.sh
```

## 卸载方式

```Shell
cd sign_kernel
sudo sh clean.sh
make clean
cd ../sign_user
sudo sh clean.sh
make clean
```