dtb-y += ms_rtl9310.dtb
dtb-y += ms_rtl9300.dtb
dtb-y += ms_rtl8390.dtb
dtb-y += ms_rtl8380.dtb

obj-y				+= $(patsubst %.dtb, %.dtb.o, $(dtb-y))

# Force kbuild to make empty built-in.o if necessary
obj-				+= dummy.o

always				:= $(dtb-y)
clean-files			:= *.dtb *.dtb.S

$(shell cd formosa/bsp/dts; \
	rm -f *.dtb)
