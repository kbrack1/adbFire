#!/system/bin/sh

#
#  FireTV USB Drive Mounter
#
#  REQUIRED: Busybox with support for run-parts, blkid, mount-helper
#  and stuff that's probably slipped my mind required.
#  AFAIK, NONE of the "free" Play Store busyboxes have all of the
#  supplied options. I suggest you use mine or compile your own.
#
#  This script has been tested with 4 drives
#  mounted (vfat,exfat,ntfs,ext4)
#
#  This script mounts your drives in the order
#  they are encountered at mountpoints
#  /storage/usb/drive1,drive2,etc.
#  if you want to control the mountpoint for
#  your drives mount via UUID with mntdrives2.sh
#


sleep 20

i=0

if [[ $(ls /dev/block | $busybox grep -c 'sd') -eq 0 ]]; then 
	exit 0
fi

for f in /dev/block/sd??; do 
        drive=`echo $f | sed -r 's/^.{11}//'`
	mountType=$(busybox blkid $f | busybox sed -n -e 's_^.*TYPE="\([^\"]*\)".*_\1_p');
	mountUUID=$(busybox blkid $f | busybox sed -n -e 's_^.*UUID="\([^\"]*\)".*_\1_p');

i=$((i+1))

  mkdir /storage/usb/drive$i;
  chmod 0777 /storage/usb/drive$i;
  chown shell.shell /storage/usb/drive$i

  if [[ $mountType == "exfat" ]]; then
     mount.exfat-fuse -o rw $f /storage/usb/drive$i
  fi

  if [[ $mountType == "ntfs" ]]; then
  	ntfs-3g $f /storage/usb/drive$i -o rw,sync
  fi

  if [[ $mountType == "vfat" ]]; then
    mount -t vfat -o umask=000 $f /storage/usb/drive$i 
  fi


  if [[ $mountType == "ext4" ]] || [[ $mountType == "ext3" ]] || [[ $mountType == "ext2" ]]; then
    busybox mount UUID=$mountUUID /storage/usb/drive$i 
  fi

done  

# mount -o remount,ro / 


if [ $# -ge 1 ];
then 
   sleep 20
   /data/data/com.funkyfresh.samba/files/samba-rc start
fi

