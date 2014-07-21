#!/system/bin/sh

mount -o remount,rw /system

cd /system/xbin
for f in $(busybox find /system/xbin -type l) ; 
do 
 bblnk=$(busybox readlink $f) ; 
  if echo "$bblnk" | busybox grep -q busybox ;
  then
   busybox rm $f
  fi ;
done

rm /system/xbin/busybox
rm /system/xbin/buninstall.sh
rm /system/xbin/ntfs-3g
rm /system/xbin/mount.exfat-fuse
rm /system/xbin/mntdrives.sh

mount -o remount,ro /system

