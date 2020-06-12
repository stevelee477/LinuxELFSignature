#!/bin/bash

L_UID=$(id -u)

if [ "$L_UID" -ne "0" ]; then
    echo "You must be uid=0"
    exit 1
fi

cd sign_kernel
make clean
make
sh load.sh
cd ../sign_user
make clean
make
sh load.sh
