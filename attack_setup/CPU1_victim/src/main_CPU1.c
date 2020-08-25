#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "xil_io.h"
#include "aes.h"
#include "main_CPU1.h"
#include "xil_io.h"
#include "utility.h"
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/sp.h>

// AES DECLARATION
AES_KEY key;
uint8_t ptArray[16];
uint8_t ctArray[16];
uint8_t keyArray[16] = {0x7d,0xc4,0x04,0x63,0xee,0x8d,0x0d,0x11,0x56,0xda,0x16,0x8a,0x71,0x1d,0xef,0xba};

// RSA
RsaKey      rsaPrivateKey1024;
RsaKey*     privRsaKey1024 = NULL;
byte plain1024[128];


int main(void)
{
	uint8_t mode;

	xil_printf("CPU#1: Welcome! I'm ready to cipher data!\n\r");

	//RSA INIT
	wc_InitRsaKey(&rsaPrivateKey1024, NULL);
	privRsaKey1024 = &rsaPrivateKey1024;
	mp_read_unsigned_bin(&rsaPrivateKey1024.n, n1024, sizeof(n1024));
	mp_read_unsigned_bin(&rsaPrivateKey1024.d, d1024, sizeof(d1024));
	rsaPrivateKey1024.type = RSA_PRIVATE_DECRYPT;

	// Perform the AES key expension
	AES_set_encrypt_key(keyArray,128,&key);

	while(1)
	{
		mode = Xil_In32(MODE_ADDR);

		switch(mode)
		{
			case AES_SSL:
				AES128_OpenSSL_Encrypt();
				break;

			case RSA_SSL:
				//not implemented yet
				break;

			case RSA_NAIVE:
				RSA1024_Naive_Private_Decrypt();
				break;

			case RSA_CONSTANT:
				//not implemented yet
				break;

			case RSA_LADDER:
				//not implemented yet
				break;

			default:
				break;

		}
	}
	return 1;
}



int RSA1024_Naive_Private_Decrypt(void)
{
	//SEND READY TO CPU 0
	Xil_Out32(CONFIG_ADDR,CRYPT_READY);

	//WAIT CPU0 SYNCHRO
	while(Xil_In32(CONFIG_ADDR) != CRYPT_START){}

	//OPENSSL ENCRYPT
	sp_RsaPrivate_1024_custom_naive(enc1024, sizeof(enc1024), &rsaPrivateKey1024.d, &rsaPrivateKey1024.n, plain1024, sizeof(plain1024));

	/*xil_printf(("\n\rplain naive 1024\n\r"));
	for(int i = 0 ; i < 128; i++)
	{
	xil_printf("%02x ",plain1024[i]);
	}
	xil_printf("\n\r");*/

	//SEND SYNCHRO TO CPU#0
	Xil_Out32(CONFIG_ADDR,CRYPT_END);

	//WAIT CPU0 END
	while(Xil_In32(CONFIG_ADDR) != CRYPT_STOP){}

	return 1;
}

int AES128_OpenSSL_Encrypt(void)
{
	uint8_t c_i, p_i;

	// Wait for CPU#0 plaintext
	while(Xil_In32(CONFIG_ADDR) != PLAIN_READY){}

	// When the plaintext is ready, read it
	for(p_i = 0 ; p_i < 4 ; p_i++)
	{
		ptArray[p_i*4+3] = (uint8_t)((Xil_In32(PLAIN_ADDR+p_i*4) & 0x000000ff)>>0);
		ptArray[p_i*4+2] = (uint8_t)((Xil_In32(PLAIN_ADDR+p_i*4) & 0x0000ff00)>>8);
		ptArray[p_i*4+1] = (uint8_t)((Xil_In32(PLAIN_ADDR+p_i*4) & 0x00ff0000)>>16);
		ptArray[p_i*4+0] = (uint8_t)((Xil_In32(PLAIN_ADDR+p_i*4) & 0xff000000)>>24);
	}

	//SEND READY TO CPU 0
	Xil_Out32(CONFIG_ADDR,CRYPT_READY);

	//WAIT CPU0 SYNCHRO
	while(Xil_In32(CONFIG_ADDR) != CRYPT_START){}

	//OPENSSL ENCRYPT
	AES_encrypt(ptArray, ctArray, &key);
	AES_encrypt(ptArray, ctArray, &key);

	//SEND SYNCHRO TO CPU#0
	Xil_Out32(CONFIG_ADDR,CRYPT_END);

	//SEND CIPHERTEXT TO CPU0
	for(c_i = 0; c_i < 4 ; c_i++){
		Xil_Out32(CIPHER_ADDR+c_i*4,((ctArray[c_i*4+0]<<24) & 0xff000000) | ((ctArray[c_i*4+1]<<16) & 0x00ff0000) | ((ctArray[c_i*4+2]<<8) & 0x00ff00) | ((ctArray[c_i*4+3]<<0) & 0x000000ff));
	}

	//WAIT CPU0 END
	while(Xil_In32(CONFIG_ADDR) != CRYPT_STOP){}

	// Send ciphertext ready to CPU#0
	Xil_Out32(CONFIG_ADDR,CIPHER_READY);

	return 1;
}

