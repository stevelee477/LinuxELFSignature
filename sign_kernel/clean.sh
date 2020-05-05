#!/bin/bash

# If this is executed with /bin/sh, $UID isn't populated so we will use
# id -u to get the current uid
L_UID=$(id -u)

if [ "$L_UID" -ne "0" ]; then
    echo "You must be uid=0"
    exit 1
fi

rmmod ./sign.ko