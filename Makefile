TARGET = i2ctrl.elf
LIB = c-periphery/periphery.a
CROSS_COMPILE = arm-linux-gnueabihf-
SRC = i2ctrl.c
INC = -I. -I./c-periphery/src

$(TARGET): $(SRC) $(LIB)
	$(CROSS_COMPILE)gcc -Wall -O0 -g $(INC) $(SRC) $(LIB) -o $@

$(LIB):
	make -C c-periphery CROSS_COMPILE=$(CROSS_COMPILE)

clean:
	rm -f $(TARGET)
	
distclean:
	rm -f $(TARGET)
	make -C c-periphery clean

.PHONY: clean distclean
