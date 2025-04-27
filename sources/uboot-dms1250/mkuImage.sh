/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/rsdk-linux-objcopy -O binary -R .note -R .comment -S ~/120319/scripts/uClinux-dist/linux-2.6.x/vmlinux linux.bin
gzip -9 linux.bin
tools/mkimage -A mips -O linux -T kernel -C gzip -a 80000000 -e 80000000 -n "Linux Kernel Image" -d linux.bin.gz uImage
cp uImage ~

