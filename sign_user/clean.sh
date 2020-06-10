L_UID=$(id -u)

if [ "$L_UID" -ne "0" ]; then
    echo "You must be uid=0"
    exit 1
fi

systemctl stop sign

rm /usr/bin/user
rm /etc/systemd/system/sign.service