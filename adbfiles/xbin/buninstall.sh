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
rm /system/xbin/01mntdrives
rm /system/xbin/02sshd
rm /system/xbin/busybox
rm /system/xbin/aftv-unlock
rm /system/xbin/buninstall.sh
rm /system/xbin/init.bueller.sh
rm /system/xbin/ntfs-3g
rm /system/xbin/mount.exfat-fuse
rm /system/xbin/mntdrives*.sh
rm /system/xbin/mntdata*
rm dropbear*
rm ssh*
rm rmssh
rm rsync
rm dbclient
rm scp
rm samba
rm -r /system/etc/init.d
rm -r /data/jocala
mv /system/etc/init.bueller.sh.old /system/etc/init.bueller.sh

mount -o remount,ro /system
