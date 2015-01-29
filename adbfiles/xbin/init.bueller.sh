#!/system/bin/sh
#
#
#
# if mntdata.sh exists, swap data/ext4 drive

if [ -e /system/xbin/mntdata.sh ]
  then
   /system/xbin/mntdata.sh > /storage/usb/.mntdata.log 2>&1
fi;

# get the wifi MAC address from /proc/mac_addr
#
#
MAC_STRING=`cat /proc/mac_addr`
log "Set WIFI MAC address:" $MAC_STRING
#
# format the wifi address string in the appropriate format:
# echo -e \\xE0\\xCB\\x1D\\xA3\\x06\\xF2 > /system/etc/firmware/ath6k/AR6004/hw1.2/softmac.bin
#
mac1=${MAC_STRING:0:2}
mac2=${MAC_STRING:2:2}
mac3=${MAC_STRING:4:2}
mac4=${MAC_STRING:6:2}
mac5=${MAC_STRING:8:2}
mac6=${MAC_STRING:10:2}
wifi_addr_str="\\x${mac1}\\x${mac2}\\x${mac3}\\x${mac4}\\x${mac5}\\x${mac6}"
#
# write the mac address to the file: softmac.bin  
#
echo -e "$wifi_addr_str" > /system/etc/firmware/ath6k/AR6004/hw1.2/softmac.bin
echo -e "$wifi_addr_str" > /system/etc/firmware/ath6k/AR6004/hw1.3/softmac.bin
echo -e "$wifi_addr_str" > /system/etc/firmware/ath6k/AR6004/hw3.0/softmac.bin

#
# Setting the BT mac address file 
#
MAC_STRING=`cat /proc/bt_mac_addr`
log "Set BT MAC address:" $MAC_STRING
mac1=${MAC_STRING:0:2}
mac2=${MAC_STRING:2:2}
mac3=${MAC_STRING:4:2}
mac4=${MAC_STRING:6:2}
mac5=${MAC_STRING:8:2}
mac6=${MAC_STRING:10:2}
bt_addr_str="${mac1}:${mac2}:${mac3}:${mac4}:${mac5}:${mac6}"
echo "$bt_addr_str" > /system/etc/firmware/ar3k/1020201/ar3kbdaddr.pst
