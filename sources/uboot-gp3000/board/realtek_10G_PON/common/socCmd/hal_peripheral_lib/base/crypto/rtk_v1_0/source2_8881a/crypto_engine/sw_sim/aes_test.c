#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
	char *key = "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
		  "\x51\x2e\x03\xd5\x34\x12\x00\x06";
	int klen = 16;
	char *iv = "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30"
		  "\xb4\x22\xda\x80\x2c\x9f\xac\x41";
	char *input = "\xe3\x53\x77\x9c\x10\x79\xae\xb8"
		  "\x27\x08\x94\x2d\xbe\x77\x18\x1a";
	int ilen = 16;
	char *result = "Single block msg";
	int retval;
	static unsigned char sw_orig[1024];
	static unsigned char sw_dec[1024];

	memcpy(sw_orig, input, ilen);
	retval = aesSim_aes(0x20, sw_orig, sw_dec, ilen,
		klen, key, iv);
	
	if (retval != 0)
	{	
		printf("aesSim_aes failed\n");
		return -1;
	}

	if (memcmp(sw_dec, result, ilen) == 0)
	{
		printf("aes test vector success\n");
	}
	else
	{
		printf("aes test vector failed!\n");
	}

	return 0;
}

