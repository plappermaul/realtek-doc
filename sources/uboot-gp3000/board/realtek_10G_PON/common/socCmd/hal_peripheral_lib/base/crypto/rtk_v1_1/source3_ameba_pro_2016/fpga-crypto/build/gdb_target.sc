file target.axf
target remote 127.0.0.1:5181
lo
#set remotetimeout 100000
#compare-sections
#p /x $sr=0
p /x $sp=0x1ffffffc
p /x $pc=0x100
c
