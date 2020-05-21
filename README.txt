A c routine to read and write linux i2c device.

Note:
	* if the c-periphery is empty, you may need to:
		$ git submodule init
		$ git submodule update

	* the file: __gpio.h is a candidate of linux/gpio.h if case of some
	  compiler doesn't has it.
