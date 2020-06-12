#!/bin/bash

L_UID=$(id -u)

if [ "$L_UID" -ne "0" ]; then
    echo "You must be uid=0"
    exit 1
fi

apt-get update
apt-get install g++ libssl-dev -y