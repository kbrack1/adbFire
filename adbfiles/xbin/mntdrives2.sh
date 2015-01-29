#!/system/bin/sh

sleep 20

#
#  FireTV USB/UUID Drive Mounter
#
#  REQUIRED: Busybox with suuport for run-parts, blkid, mount-helper
#  mount-helper and stuff that's probably slipped my mind required.
#  AFAIK, NONE of the "free" Play Store busyboxes have all of the
#  supplied options. I suggest you use mine or compile your own.

#  Replace the UUIDs below with the UUIDs
#  from your drives. This script has been tested
#  with 4 drives mounted (vfat,exfat,ntfs,ext4)
#
#  cp this script to mntdrives.sh. The FireTV will then
#  use it to mount drives at boot and when the mount button
#  is pressed using adbFire.

export uuid1="544D-B43D"
export uuid2="138FD3254AEAB65E"
export uuid3="94f2a28c-caac-4eb7-a544-84a650c0b809"
export uuid4="0577-07FC"

i=0

if [[ $(ls /dev/block | $busybox grep -c 'sd') -eq 0 ]]; then 
	exit 0
fi

mount -o remount,rw / ;


for f in /dev/block/sd??; do 
        drive=`echo $f | sed -r 's/^.{11}//'`
	mountType=$(busybox blkid $f | busybox sed -n -e 's_^.*TYPE="\([^\"]*\)".*_\1_p');
	mountUUID=$(busybox blkid $f | busybox sed -n -e 's_^.*UUID="\([^\"]*\)".*_\1_p');

i=$((i+1))


  mkdir /storage/usb/drive$i;

  if [[ $mountUUID == $uuid1 ]]; then
     mount.exfat-fuse -o rw $f /storage/usb/drive$i
  fi

  if [[ $mountUUID == $uuid2 ]]; then
  	ntfs-3g $f  /storage/usb/drive$i -o rw,sync
  fi

  if [[ $mountUUID == $uuid3 ]]; then
  	busybox mount UUID=$uuid3 /storage/usb/drive$i
  fi

  if [[ $mountUUID == $uuid4 ]]; then
  	busybox mount UUID=$uuid4 /storage/usb/drive$i
  fi

done  

mount -o remount,ro / 

if [ $# -ge 1 ];
then 
   sleep 20
   /data/data/com.funkyfresh.samba/files/samba-rc start
fi
