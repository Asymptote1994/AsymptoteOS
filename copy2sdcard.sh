sleep 2
ls /dev/sd*
sudo dd if=/home/zhangxu/study/AsymptoteOS/kernel.bin of=/dev/sdb bs=512 seek=4
sync
