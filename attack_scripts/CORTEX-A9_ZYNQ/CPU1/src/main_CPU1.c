#include "xadcps.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xdmaps.h"
#include "xscugic.h"
#include <aes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "math.h"
#include "xparameters.h"
#include "xil_io.h"
#include "main.h"
#include "utility.h"
#include "sleep.h"
#include "xil_cache.h"

#define KEY_READY 0x4
#define KEY_ACK 0x8
#define AES_START 0xc
#define AES_ACK 0x10
#define AES_END 0x14
#define PLAIN_READY 0x18
#define PLAIN_ACK 0x1c
#define AES_PLAIN 0x20
#define AES_CIPHER 0x30
#define AES_SKEY 0x40
#define TRANSFER_END 0x44

const uint32_t sharedAddr = 0x7000000;

int main(void)
{
	xil_printf("CPU#1: Welcome! I'm ready to cipher data!\n\r");
	StateMachine();
	return XST_SUCCESS;
}

int StateMachine(void)
{

	uint8_t k_i, c_i, p_i;

	//AES DECLARATION
	AES_KEY key;
	uint8_t ptArray[16];
	uint8_t ctArray[16];
	uint8_t keyArray[16] = {0x7d,0xc4,0x04,0x63,0xee,0x8d,0x0d,0x11,0x56,0xda,0x16,0x8a,0x71,0x1d,0xef,0xba};

	// Print the AES key for experimental purposes
	/*xil_printf("\n\rkey : ");
	for(k_i = 0 ; k_i < 16 ; k_i++){ xil_printf("%02x",keyArray[k_i]);}
	xil_printf("\n\r");*/

	// Perform the AES key expension
	AES_set_encrypt_key(keyArray,128,&key);

	while(1)
	{
		// Wait for CPU#0 plaintext
		while(Xil_In32(sharedAddr+PLAIN_READY)!=1){};

		// When the plaintext is ready, read it and reset PLAIN_READY
		//xil_printf("CPU0: the plaintext has been received!\n\rplaintext : ");
		for(p_i = 0 ; p_i < 4 ; p_i++)
		{
			ptArray[p_i*4] = (uint8_t)((Xil_In32(sharedAddr+AES_PLAIN+p_i*4) & 0x000000ff)>>0);
			ptArray[p_i*4+1] = (uint8_t)((Xil_In32(sharedAddr+AES_PLAIN+p_i*4) & 0x0000ff00)>>8);
			ptArray[p_i*4+2] = (uint8_t)((Xil_In32(sharedAddr+AES_PLAIN+p_i*4) & 0x00ff0000)>>16);
			ptArray[p_i*4+3] = (uint8_t)((Xil_In32(sharedAddr+AES_PLAIN+p_i*4) & 0xff000000)>>24);
			//xil_printf("%02x%02x%02x%02x",ptArray[p_i*4+3],ptArray[p_i*4+2],ptArray[p_i*4+1],ptArray[p_i*4+0]);
		}
		//xil_printf("\n\r");
		Xil_Out32(sharedAddr+PLAIN_READY,0);

		//SEND READY TO CPU 0
		Xil_Out32(sharedAddr+PLAIN_ACK,0x1);

		//WAIT CPU0 SYNCHRO
		while(Xil_In32(sharedAddr+AES_START) != 0x1){}

		//OPENSSL ENCRYPT
		AES_encrypt(ptArray, ctArray, &key);

		//SEND SYNCHRO TO CPU#0 and reset AES_START
		Xil_Out32(sharedAddr+AES_END,0x1);
		Xil_Out32(sharedAddr+AES_START,0x0);


		//SEND CIPHERTEXT TO CPU0
		for(c_i = 0; c_i < 4 ; c_i++){
			Xil_Out32(sharedAddr+AES_CIPHER+c_i*4,((ctArray[c_i*4+0]<<24) & 0xff000000) | ((ctArray[c_i*4+1]<<16) & 0x00ff0000) | ((ctArray[c_i*4+2]<<8) & 0x00ff00) | ((ctArray[c_i*4+3]<<0) & 0x000000ff));
		}

		//WAIT CPU0 END and reset TRANSFER_END
		while(Xil_In32(sharedAddr+TRANSFER_END) != 0x1){}
		Xil_Out32(sharedAddr+TRANSFER_END,0x0);

	}
}


