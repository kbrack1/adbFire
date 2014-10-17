#!/system/bin/sh

sleep 15

# v3

storage="/storage/usb"


if [[ $(ls /dev/block | $busybox grep -c 'sd') -eq 0 ]]; then 
	exit 0
fi

mount -o remount,rw / ;


for f in /dev/block/sd??; do 
        drive=`echo $f | sed -r 's/^.{11}//'`
	busybox mkdir -p $storage/$drive;
	busybox chmod 777 $storage/$drive;
	mountType=$(busybox blkid $f | busybox sed -n -e 's_^.*TYPE="\([^\"]*\)".*_\1_p');



  if [[ $mountType == "exfat" ]]; then
        mount.exfat-fuse $f $storage/$drive
  fi

  if [[ $mountType == "vfat" ]]; then
  	busybox mount -t vfat -o rw,nosuid,nodev,fmask=0,dmask=0,umask=0 $f $storage/$drive
  fi

  
  if [[ $mountType == "ntfs" ]]; then
  	ntfs-3g -o rw,allow_other,permissions,fmask=0,dmask=0,umask=0 $f $storage/$drive
  fi
  
  if [[ $mountType == "ext4" ]] || [[ $mountType == "ext3" ]] || [[ $mountType == "ext2" ]]; then
  	busybox mount -t auto -o rw  $f $storage/$drive
        chmod 777 $storage/$drive
  fi

done  

mount -o remount,ro / 

if [ $# -ge 1 ];
then 
   sleep 20
   /data/data/com.funkyfresh.samba/files/samba-rc start
fi


