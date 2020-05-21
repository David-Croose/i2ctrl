TARGET = i2ctrl.elf
LIB = c-periphery/periphery.a
CROSS_COMPILE = arm-linux-gnueabihf-
SRC = i2ctrl.c
INC = -I. -I./c-periphery/src

# compile this header file instead of linux/gpio.h
# in case of some compilers doesn't has linux/gpio.h
BACKUP = __gpio.h

SUBMODULE = c-periphery/Makefile
PATCH = sed -i -e 's/#include <linux\/gpio.h>/#include "..\/..\/__gpio.h"/g' c-periphery/src/gpio.c

$(TARGET): $(SRC) $(LIB) $(SUBMODULE)
	$(CROSS_COMPILE)gcc -Wall -O0 -g $(INC) $(SRC) $(LIB) -o $@

$(SUBMODULE):
	ifeq ($(wildcard $(SUBMODULE)),)
		@echo 'the c-periphery is empty, you may need to:'
		@echo '$ git submodule init'
		@echo '$ git submodule update'
	endif

$(LIB):
	$(shell $(PATCH))
	make -C c-periphery CROSS_COMPILE=$(CROSS_COMPILE)

clean:
	rm -f $(TARGET)
	
distclean:
	rm -f $(TARGET)
	make -C c-periphery clean

.PHONY: clean distclean
