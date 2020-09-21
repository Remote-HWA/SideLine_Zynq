#ifndef __MAIN_CPU1_H_

#define __MAIN_CPU1_H_

	/***** Communication channel with CPU0 *****/
	#define SHARED_BASE_ADDR  0x7000000

	#define MODE_ADDR (SHARED_BASE_ADDR+0x0)

		#define IDLE	  		0
		#define AES_SSL  		1

	#define CONFIG_ADDR (SHARED_BASE_ADDR+0x4)

		#define CRYPT_WAIT  	1
		#define CRYPT_READY 	2
		#define CRYPT_START 	3
		#define CRYPT_END		4
		#define CRYPT_STOP  	5
		#define PLAIN_READY 	6
		#define CIPHER_READY 	7

	#define KEY_ADDR (SHARED_BASE_ADDR+0x100)
	#define PLAIN_ADDR (SHARED_BASE_ADDR+0x200)
	#define CIPHER_ADDR (SHARED_BASE_ADDR+0x300)

	/***** Function declaration *****/
	int AES128_OpenSSL_Encrypt(void);



#endif


