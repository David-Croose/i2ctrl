#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "i2c.h"

/**
 * change string into raw data
 * @param s: the string to be changed. it can only contain "0-9", "a-f" and "A-F", and must
 *           terminated with 0
 * @param hex: the memory stores the raw data changed from @s
 * @param hexlen: the length(in bytes) of @hex
 * @param reallen: the real length(in bytes) of @hex
 * @return: 0 --- successfully
 *          others --- failed
 */
// TODO  "1" --> 0x10, that's right?
static uint32_t str2hex(const char *s, uint8_t *hex, uint32_t hexlen, uint32_t *reallen) {
/**
 * turn string into number.
 * e.g:
 *     const char *p = "123456";
 *     char (*q)[2] = (char (*)[2])p;
 *     STR2NUM(q) == 12
 * @param x: a two character array pointer, like: char (*q)[2];
 * @param t: temporary variable
 * @return: the number
 */
#define STR2NUM(x, t) \
    ((t) = ((x)[0][0] <= '9' ? (x)[0][0] - '0' : ((x)[0][0] <= 'F' ? (x)[0][0] - 'A' + 0xA : (x)[0][0] - 'a' + 0xA)), \
    (t) <<= 4, (t) |= ((x)[0][1] == 0 ? 0 : ((x)[0][1] <= '9' ? (x)[0][1] - '0' : ((x)[0][1] <= 'F' ? (x)[0][1] - 'A' + 0xA : (x)[0][1] - 'a' + 0xA))), (t))

    uint32_t i;
    uint32_t slen = strlen(s);
    char (*p)[2];
    char (*q)[2];
    uint8_t t;

    if (!s || !hex || !hexlen || !reallen || !slen || hexlen < (slen + 1) / 2) {
        return 1;
    }

    for (i = 0; i < slen && ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'A' && s[i] <= 'F') || (s[i] >= 'a' && s[i] <= 'f')); i++);
    if (i < slen) {
        return 2;
    }

    for (p = (char (*)[2])(s + slen), i = 0, q = (char (*)[2])s; q < p; q++, i++) {
        hex[i] = STR2NUM(q, t);
    }
    *reallen = i;
    return 0;
}

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
    unsigned int reallen;
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
		len /= 2;
	}

    msg_data = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (!msg_data) {
    	printf("error: no enough memory for %dbytes\n", len);
    	return -5;
    }
    if (rw == WRITE) {
		str2hex(argv[4], msg_data, len, &reallen);
		if (reallen != len) {
			printf("error: the data you want to write is wrong\n");
			free(msg_data);
			return -8;
		}
		printf("write:\n");
		for (i = 0; i < len; i++)
			printf("%x ", msg_data[i]);
		printf("\n");
	}

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

    if (i2c_transfer(i2c, &msgs, 1) < 0)
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
    i2c_close(i2c);
    i2c_free(i2c);
    free(msg_data);
    return 0;
}
