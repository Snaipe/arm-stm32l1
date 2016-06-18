SRCS = main.c

PROJ_NAME=main

TRIPLE=arm-none-eabi

CC := $(TRIPLE)-gcc
OBJCOPY := $(TRIPLE)-objcopy
GDB := $(TRIPLE)-gdb
STUTIL := st-util
STFLASH := st-flash

CFLAGS = -Wall -std=c99 -Os -g -gdwarf-2 \
		 -mthumb \
		 -fno-builtin \
		 -mcpu=cortex-m3 \
		 -ffunction-sections \
		 -fdata-sections \
		 -mabi=aapcs \
		 -fverbose-asm \
		 -DSTM32L1 \
		 -Iopencm3/include

LDFLAGS = -mcpu=cortex-m3 \
		  -mthumb \
		  -nostartfiles \
		  -Wl,-gc-sections \
		  -Tlibopencm3_stm32l1.ld \
		  -Wl,-Map=$(PROJ_NAME).map,--cref,-no-warn-mismatch \

LIBS = -Lopencm3/lib -lopencm3_stm32l1

OBJS = $(SRCS:.c=.o)

all: $(PROJ_NAME).elf

opencm3/Makefile:
	git submodule update --init opencm3

opencm3/lib/libopencm3_stm32l1.a: opencm3/Makefile
	$(MAKE) TARGETS="stm32/l1" $(MFLAGS) -C opencm3

$(PROJ_NAME).elf: $(OBJS) opencm3/lib/libopencm3_stm32l1.a
	$(LINK.o) $(OBJS) -o $@ $(LIBS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

download: $(PROJ_NAME).bin
	$(STFLASH) write main.bin 0x08000000

run:

debug:
	nohup $(STUTIL) $(STFLAGS) -p 4142 </dev/null >/dev/null 2>&1 &
	$(GDB) $(GDBFLAGS) -q -ex "target remote localhost:4142" main.elf

debug-qemu:
	$(GDB) $(GDBFLAGS) -q -ex "target remote localhost:4042" main.elf

qemu: lib proj
	qemu-system-gnuarmeclipse \
		--verbose --verbose \
		--board STM32F429I-Discovery --mcu STM32F429ZI \
		--gdb tcp::4042 \
		-d unimp,guest_errors --nographic \
		--image $(PROJ_NAME).elf \
		--semihosting-config enable=on,target=native \
		--semihosting-cmdline main $(CMDLINE)

clean:
	$(RM) $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin
	$(MAKE) TARGETS="stm32/l1" $(MFLAGS) -C opencm3 clean

.PHONY: lib download
