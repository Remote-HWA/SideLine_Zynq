#ifndef __MAIN_CPU0_H_

	#define __MAIN_CPU0_H_

	/***** Communication channel with CPU1 *****/
	#define SHARED_BASE_ADDR  0x7000000

	#define MODE_ADDR (SHARED_BASE_ADDR+0x0)

		#define IDLE	  		0
		#define AES_SSL  		1
		#define RSA_SSL 		2
		#define RSA_NAIVE 		3
		#define RSA_CONSTANT	4
		#define RSA_LADDER  	5

	#define CONFIG_ADDR (SHARED_BASE_ADDR+0x4)

		#define CRYPT_WAIT  1
		#define CRYPT_READY 2
		#define CRYPT_START 3
		#define CRYPT_END	4
		#define CRYPT_STOP  5
		#define PLAIN_READY 	6
		#define CIPHER_READY 	7

	#define KEY_ADDR (SHARED_BASE_ADDR+0x100)
	#define PLAIN_ADDR (SHARED_BASE_ADDR+0x200)
	#define CIPHER_ADDR (SHARED_BASE_ADDR+0x300)



	/***** DMA Configuration *****/
	#define DMA_DEVICE_ID2 			XPAR_XDMAPS_0_DEVICE_ID
	#define DMA_DEVICE_ID 			XPAR_XDMAPS_1_DEVICE_ID
	#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID

	#define DMA_DONE_INTR_0			XPAR_XDMAPS_0_DONE_INTR_0
	#define DMA_DONE_INTR_1			XPAR_XDMAPS_0_DONE_INTR_1
	#define DMA_DONE_INTR_2			XPAR_XDMAPS_0_DONE_INTR_2
	#define DMA_DONE_INTR_3			XPAR_XDMAPS_0_DONE_INTR_3
	#define DMA_DONE_INTR_4			XPAR_XDMAPS_0_DONE_INTR_4
	#define DMA_DONE_INTR_5			XPAR_XDMAPS_0_DONE_INTR_5
	#define DMA_DONE_INTR_6			XPAR_XDMAPS_0_DONE_INTR_6
	#define DMA_DONE_INTR_7			XPAR_XDMAPS_0_DONE_INTR_7
	#define DMA_FAULT_INTR			XPAR_XDMAPS_0_FAULT_INTR

	/***** DLL Configuration *****/
	#define DLL_addr  0xF80061E0

		// MASTER DLL 1
		#define coarsemask1  0x7f
		#define coarsepos1  13
		#define finemask1  0x03
		#define finepos1  11
		// MASTER DLL 0
		#define coarsemask0  0x7f
		#define coarsepos0  4
		#define finemask0  0x03
		#define finepos0  2


	/***** Function declaration *****/
	void Command_Helper(void);
	int StateMachine(void);
	int SetupInterruptSystem(XScuGic *GicPtr, XDmaPs *DmaPtr);
	void DmaDoneHandler(unsigned int Channel, XDmaPs_Cmd *DmaCmd,void *CallbackRef);
	void set_DmaCmd (XDmaPs_Cmd *DmaCmd, int BurstSize, u32 *Source, u32 *Destination, u32 DMA_WORDS);
	int AES128_OpenSSL_Attack(char * user_input);
	int RSA1024_Naive_Attack(char * user_input);
	void Print_DLL_State(char * user_input);



#endif




