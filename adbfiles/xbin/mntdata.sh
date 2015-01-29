#!/system/bin/sh

# this mkdata.orig bash script 
# is by jocala@jocala.com. Based
# in part on work by jmandawg@xda
# and croniccorey@xda.


sleep 20
mount -o remount,rw / ;

export UUID=3fca3dfe-b63d-4882-ac5f-da09911f6764

if [[ $(ls /dev/block | $busybox grep -c 'sd') -eq 0 ]]; then 
	exit 0
fi


        mkdir /olddata
        busybox umount /data;
        busybox mount -t ext4 /dev/block/mmcblk0p20 /olddata;
        busybox mount UUID=$UUID /data
        chown 1000:1000 /data;
        chmod 771 /data;
        if [ ! -e /data/data ]
          then
           cp -av /olddata/. /data
        fi;
        for i in misc;
         do
          if [ -e /data/$i ]
           then
             mount -o bind /olddata/$i /data/$i;
           fi;
         done;

mount -o remount,ro / 

sync;
