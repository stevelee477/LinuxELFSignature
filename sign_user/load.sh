L_UID=$(id -u)

if [ "$L_UID" -ne "0" ]; then
    echo "You must be uid=0"
    exit 1
fi

cp user /usr/bin/
cp sign.service /etc/systemd/system/

systemctl start sign