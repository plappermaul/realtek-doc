#include <stdio.h>
#include "../crypto_types.h"

void usage(char *name)
{
	printf("%s <data_file> <data_len> <key_file> <key_len>\n", name);
}

void crypto_memDump(void *start, uint32 size, int8 * strHeader)
{
	int32 row, column, index, index2, max;
	uint32 buffer[5];
	uint8 *buf, *line, ascii[17];
	int8 empty = ' ';

	if(!start ||(size==0))
		return;

	line = (uint8*)start;

	/*
	16 bytes per line
	*/
	if (strHeader)
		printf ("%s", strHeader);

	column = size % 16;
	row = (size / 16) + 1;
	for (index = 0; index < row; index++, line += 16) 
	{
		buf = (uint8*)line;
		memset (ascii, 0, 17);

		max = (index == row - 1) ? column : 16;
		if ( max==0 ) break; /* If we need not dump this line, break it. */

		printf ("\n%08x ", (uint32) line);
		
		//Hex
		for (index2 = 0; index2 < max; index2++)
		{
			if (index2 == 8)
			printf ("  ");
			printf ("%02x ", (uint8) buf[index2]);
			ascii[index2] = ((uint8) buf[index2] < 32) ? empty : buf[index2];
		}

		if (max != 16)
		{
			if (max < 8)
				printf ("  ");
			for (index2 = 16 - max; index2 > 0; index2--)
				printf ("   ");
		}

		//ASCII
		printf ("  %s", ascii);
	}

	printf ("\n");
	return;
}

int main(int argc, char *argv[])
{
	static char data[16384];
	static char key[1024];
	char digest[20];
	FILE *fp_d, *fp_k;
	int data_len, key_len;
	int ret = 0;

	if (argc != 5)
	{
		usage(argv[0]);
		return -1;
	}

	fp_d = fopen(argv[1], "r");
	if (fp_d == NULL)
	{
		printf("fopen %s failed\n", argv[1]);
		return -2;
	}

	fp_k = fopen(argv[3], "r");
	if (fp_k == NULL)
	{
		printf("fopen %s failed\n", argv[3]);
		fclose(fp_d);
		return -3;
	}

	data_len = 0;
	while ((!feof(fp_d)) && 
		(data_len < sizeof(data)))
	{
		data_len += fread(&data[data_len], 1, sizeof(data) - data_len, fp_d);
	}

	if (!feof(fp_d))
	{
		printf("buf[%d] is smaller than data file\n", sizeof(data));
		ret = -4;
		goto done;
	}

	if (data_len < atoi(argv[2]))
	{
		printf("data file (%d) is smaller than specified data length (%d)\n", data_len, atoi(argv[2]));
		ret = -4;
		goto done;
	}
	else if (data_len != atoi(argv[2]))
	{
		printf("reset data_len %d to %d\n", data_len, atoi(argv[2]));
		data_len = atoi(argv[2]);
	}

	key_len = 0;
	while ((!feof(fp_k)) && 
		(key_len < sizeof(key)))
	{
		key_len += fread(&key[key_len], 1, sizeof(key) - key_len, fp_k);
	}

	if (!feof(fp_k))
	{
		printf("key[%d] is smaller than key file\n", sizeof(key));
		ret = -5;
		goto done;
	}

	if (key_len < atoi(argv[4]))
	{
		printf("key file (%d) is smaller than specified key length (%d)\n", key_len, atoi(argv[4]));
		ret = -4;
		goto done;
	}
	else
	{
		key_len = atoi(argv[4]);
	}

	if (strcmp(basename(argv[0]), "hmac-md5") == 0)
	{
		if (HMACMD5(data, data_len, key, key_len, digest) == SUCCESS)
		{
			crypto_memDump(digest, 16, "hmac-md5 digest");
			crypto_memDump(data, data_len, "hmac-md5 data");
			crypto_memDump(key, key_len, "hmac-md5 key");
			return 0;
		}
		else
		{
			printf("hmac-md5 failed\n");
			ret = -6;
			goto done;
		}
	}
	else if (strcmp(basename(argv[0]), "hmac-sha1") == 0)
	{
		if (HMACSHA1(data, data_len, key, key_len, digest) == SUCCESS)
		{
			crypto_memDump(digest, 20, "hmac-sha1 digest");
			crypto_memDump(data, data_len, "hmac-md5 data");
			crypto_memDump(key, key_len, "hmac-md5 key");
			return 0;
		}
		else
		{
			printf("hmac-sha1 failed\n");
			ret = -7;
			goto done;
		}
	}
	else
	{
		usage(argv[0]);
		ret = -2;
		goto done;
	}

done:
	fclose(fp_d);
	fclose(fp_k);
	return ret;
}

