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

/system/xbin/stopssh

rm /system/etc/install-recovery-2.sh

rm  /system/xbin/ssh
rm  /system/xbin/scp
rm  /system/xbin/sshstatus
rm  /system/xbin/sshstart
rm  /system/xbin/sshstop
rm  /system/xbin/rsync
rm /system/xbin/busybox
rm /system/xbin/buninstall.sh
rm /system/xbin/ntfs-3g
rm /system/xbin/mount.exfat-fuse
rm /system/xbin/mntdrives.sh
rm dropbear*
rm rsync
rm dbclient
rm -r /system/etc/init.d
rm -r /data/jocala

mount -o remount,ro /system

