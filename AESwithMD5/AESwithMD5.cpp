#include <stdio.h>
#include <aes.h>
#include <md5.h>
#include "transcoding.h"
int main(int argc, char *argv[]) {
	int i,len;
	char in[256];
	char out[64];
	char key[16];
	uint8_t *ek = 0;
	uint8_t cipher[32]; // 128
	uint8_t encrypt[64];
	while (1)
	{
		printf("������Ҫ����MD5ֵ���ַ���:\n");
		gets_s(in);
		md5_str(in, key);
		//printf("MD5:");
		//for (i = 0; i<16; i++) //16λ�޷�������
		//	printf("%02x", (unsigned char)key[i]);
		//printf("\n");
		hex_to_str((unsigned char*)key, out);
		printf("MD5:%s\n", out);
		printf("��MD5ֵΪ��Կ\n");
		len=key_len((uint8_t *)key);
		ek = (uint8_t*)malloc(len);
		key_exp((uint8_t*)key, ek);
		hex_to_str((unsigned char*)key, out);
		printf("����:%s\n", out);
		en_cipher((uint8_t*)key, cipher, ek);
		hex_to_str(cipher, out);
		printf("����:%s\n", out);
		inv_cipher(cipher, encrypt, ek);
		hex_to_str(encrypt, out);
		printf("����:%s\n", out);
		//getchar();
		printf("\n");
	}
	//exit(0);
}
