#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "i2c.h"

static void show_usage(void)
{
	printf("usage: ./thisprog <i2cdev> <addr> <r|w> "
			"[<readlen>|writedata...]\n");
	printf("example:\n");
	printf("    ./thisprog /dev/i2c-0 0x35 r 5\n");
	printf("    ./thisprog /dev/i2c-0 0x35 w \\x01\\x02\\xA1\\xB2\n");
}

int main(int argc, char *argv[])
{
    i2c_t *i2c;
    char *name;
    unsigned int addr;
    enum {WRITE, READ} rw;
    unsigned int len;
    unsigned char *msg_data;
    struct i2c_msg msgs;
    int i;

    /*
     * [1] check parameter
     */
    if (argc == 2 && (!strcmp(argv[1], "help") || !strcmp(argv[1], "?"))) {
    	show_usage();
    	return 0;
    }
    else if (argc < 4) {
    	show_usage();
    	return 0;
    }

    addr = strtoul(argv[2], NULL, 16);
    if (addr > 0x7f) {
    	printf("error: i2caddr(%#x) > 0x7f\n", addr);
    	return -1;
    }

    if (!strcmp(argv[3], "r"))
        rw = READ;
    else if (!strcmp(argv[3], "w"))
    	rw = WRITE;
    else {
    	printf("error: argv[3] can only be 'r' or 'w'\n");
    	return -2;
    }

    if (rw == READ && argc != 5) {
    	printf("error: parameters 'readlen' is wrong\n");
    	return -3;
    }
	if (rw == WRITE && argc < 5) {
		printf("error: 'writedata' is absent\n");
		return -4;
	}

    if (rw == READ)
    	len = strtoul(argv[4], NULL, 10);
    else {
		len = strlen(argv[4]);
	}

    msg_data = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (!msg_data) {
    	printf("error: no enough memory for %dbytes\n", len);
    	return -5;
    }
    if (rw == WRITE)
    	strncpy(msg_data, argv[4], sizeof(msg_data));

    name = argv[1];

    /*
     * [2] process
     */
	msgs.addr = addr;
	msgs.flags = (rw == READ ? I2C_M_RD : 0);
	msgs.len = len;
	msgs.buf = msg_data;

    i2c = i2c_new();
    if (i2c_open(i2c, name) < 0) {
        printf("error: i2c_open(%s): %s\n", name, i2c_errmsg(i2c));
        return -4;
    }

    if (i2c_transfer(i2c, msgs, 1) < 0)
        printf("error: i2c_transfer(): %s\n", i2c_errmsg(i2c));
    else {
    	if (rw == READ) {
    		printf("read:\n");
    		for (i = 0; i < len; i++)
    			printf("%x ", msg_data[i]);
    		printf("\n");
    	}
    	printf("done\n");
    }

    /*
     * [2] end
     */
    free(msg_data);
    i2c_close(i2c);
    i2c_free(i2c);
    return 0;
}
