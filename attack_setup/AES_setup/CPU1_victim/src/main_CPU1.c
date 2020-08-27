#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "xil_io.h"
#include "aes.h"
#include "main_CPU1.h"
#include "xil_io.h"
#include "utility.h"

// AES DECLARATION
AES_KEY key;
uint8_t ptArray[16];
uint8_t ctArray[16];
uint8_t keyArray[16] = {0x7d,0xc4,0x04,0x63,0xee,0x8d,0x0d,0x11,0x56,0xda,0x16,0x8a,0x71,0x1d,0xef,0xba};

int main(void)
{
	xil_printf("CPU#1: Welcome! I'm ready to cipher data!\n\r");

	// Perform the AES key expension
	AES_set_encrypt_key(keyArray,128,&key);

	while(1)
	{
		AES128_OpenSSL_Encrypt();
	}
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

