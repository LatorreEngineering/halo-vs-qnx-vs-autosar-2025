```makefile
CC_HALO = arm-none-eabi-gcc
CC_QNX = qcc -Vgcc_ntoaarch64le
CC_AUTOSAR = tricore-gcc

CFLAGS = -O2 -Wall -g
HALO_LIBS = -lvcos -lvbslite
QNX_LIBS = -lc

all: cyclictest_halo cyclictest_qnx cyclictest_autosar

cyclictest_halo: cyclictest_halo.c
	$(CC_HALO) $(CFLAGS) $< -o $@ $(HALO_LIBS)

cyclictest_qnx: cyclictest_qnx.c
	$(CC_QNX) $(CFLAGS) $< -o $@ $(QNX_LIBS)

cyclictest_autosar: cyclictest_autosar.c
	$(CC_AUTOSAR) $(CFLAGS) $< -o $@ -lOs

clean:
	rm -f cyclictest_* *.o *.elf

.PHONY: all clean
```
