#!/system/bin/sh

mount -o remount,rw /system
cp /sdcard/busybox /system/xbin/busybox
chmod 0755 /system/xbin/busybox
/system/xbin/busybox --install -s /system/xbin
rm /system/xbin/binstall.sh
rm /sdcard/binstall.sh
rm /sdcard/buninstall.sh
rm /sdcard/busybox
rm /sdcard/ntfs-3g
rm /sdcard/mount.exfat-fuse

mount -o remount,ro /system
