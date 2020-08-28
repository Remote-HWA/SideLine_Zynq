#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xil_io.h"
#include "xdmaps.h"
#include "xscugic.h"
#include "main_CPU0.h"
#include "utility.h"

#define DMA_LENGTH	100000 /* Length of the Dma Transfers */
#define Dst (*(volatile unsigned int (*)[DMA_LENGTH])(0x08000000))

int main(void)
{
	char * user_input;
	char * command = malloc(sizeof(char) * 32);
	init_perfcounters (1,0);

	// Print Hello Banner
	xil_printf("\n\r\n\r\n\r\n\r");
	xil_printf(
	"   _____    __    ___                   \n\r"
	"  / __(_)__/ /__ / (_)__  ___           \n\r"
	" _\\ \\/ / _  / -_) / / _ \\/ -_)	     \n\r"
	"/___/_/\\_,_/\\__/_/_/_//_/\\__/  on AES\n\r");

	xil_printf("\n\rCPU#0: Welcome! I'm ready to attack!\n\r");

	// Wait for CPU1 ready
	HW_uDelay(100000);

	xil_printf("\n\rEnter \"help\" to display the command helper\n\r");

	do{

		xil_printf("SideLine>");
		command = GetString();
		user_input = strtok(command," ");
		xil_printf("\n\r");

		if((strcmp(user_input,"aes")==0) || (strcmp(user_input,"AES")==0))
		{
			AES128_OpenSSL_Attack(user_input);
		}
		else if((strcmp(user_input,"dll")==0) || (strcmp(user_input,"DLL")==0))
		{
			Print_DLL_State(user_input);
		}
		else if((strcmp(user_input,"?")==0) || (strcmp(user_input,"help")==0))
		{
			Command_Helper();
		}
		else
		{
			if(strcmp(command,"")!=0){
				xil_printf("Unknown Command %s\n\r",command);
			}
		}
	}
	while(1);

	return 1;
}


int SetupInterruptSystem(XScuGic *GicPtr, XDmaPs *DmaPtr)
{
	int Status;
#ifndef TESTAPP_GEN
	XScuGic_Config *GicConfig;


	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	GicConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(GicPtr, GicConfig,
				       GicConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			     (Xil_ExceptionHandler)XScuGic_InterruptHandler,
			     GicPtr);
#endif
	/*
	 * Connect the device driver handlers that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the specific
	 * interrupt processing for the device
	 */

	/*
	 * Connect the Fault ISR
	 */
	Status = XScuGic_Connect(GicPtr,
				 DMA_FAULT_INTR,
				 (Xil_InterruptHandler)XDmaPs_FaultISR,
				 (void *)DmaPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the Done ISR for all 8 channels of DMA 0
	 */
	Status = XScuGic_Connect(GicPtr,
				 DMA_DONE_INTR_0,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_0,
				 (void *)DmaPtr);
	Status |= XScuGic_Connect(GicPtr,
				 DMA_DONE_INTR_1,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_1,
				 (void *)DmaPtr);
	Status |= XScuGic_Connect(GicPtr,
				 DMA_DONE_INTR_2,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_2,
				 (void *)DmaPtr);
	Status |= XScuGic_Connect(GicPtr,
				 DMA_DONE_INTR_3,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_3,
				 (void *)DmaPtr);
	Status |= XScuGic_Connect(GicPtr,
				 DMA_DONE_INTR_4,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_4,
				 (void *)DmaPtr);
	Status |= XScuGic_Connect(GicPtr,
				 DMA_DONE_INTR_5,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_5,
				 (void *)DmaPtr);
	Status |= XScuGic_Connect(GicPtr,
				 DMA_DONE_INTR_6,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_6,
				 (void *)DmaPtr);
	Status |= XScuGic_Connect(GicPtr,
				 DMA_DONE_INTR_7,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_7,
				 (void *)DmaPtr);

	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	/*
	 * Enable the interrupts for the device
	 */
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_0);
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_1);
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_2);
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_3);
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_4);
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_5);
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_6);
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_7);
	XScuGic_Enable(GicPtr, DMA_FAULT_INTR);

	Xil_ExceptionEnable();

	return XST_SUCCESS;

}

void DmaDoneHandler(unsigned int Channel, XDmaPs_Cmd *DmaCmd, void *CallbackRef)
{
	volatile int *Checked = (volatile int *)CallbackRef;
	Checked[Channel] = 1;
}

void set_DmaCmd (XDmaPs_Cmd *DmaCmd, int BurstSize, u32 *Source, u32 *Destination, u32 DMA_WORDS)
{
	memset(DmaCmd, 0, sizeof(XDmaPs_Cmd)); // Clearing DmaCmd: explicitly resets all fields of the DMA command!

	// Setting up Dma parameters for a certain Channel
	DmaCmd->ChanCtrl.SrcBurstSize = BurstSize;
	DmaCmd->ChanCtrl.SrcBurstLen = 4;
	DmaCmd->ChanCtrl.SrcInc = 0;
	DmaCmd->ChanCtrl.DstBurstSize = BurstSize;
	DmaCmd->ChanCtrl.DstBurstLen = 4;
	DmaCmd->ChanCtrl.DstInc = 1;
	DmaCmd->BD.SrcAddr = (u32) Source;
	DmaCmd->BD.DstAddr = (u32) Destination;
	DmaCmd->BD.Length = DMA_WORDS * sizeof(int); // Length of data transmission
}

int AES128_OpenSSL_Attack(char * user_input)
{
	// DMA variables
	XDmaPs_Config *DmaCfg;
	u16 DeviceId = DMA_DEVICE_ID;
	XDmaPs_Cmd DmaCmd;
	XDmaPs DmaInstance;
	XScuGic GicInstance;
	XDmaPs *DmaInst = &DmaInstance;
	unsigned int Channel = 0;
	volatile int Checked[XDMAPS_CHANNELS_PER_DEV];
	int Status = 0;
	int val1,val2,val3,val4,val5;

	// Acquisition variables
	int nb_acq 	   = 0;
	int true_nb_sample  = 0;
	int sample_min = 0;
	int sample_max = 0;
	uint8_t DLL0_cmd = 0;
	uint8_t DLL1_cmd = 0;
	uint8_t k_i, p_i;
	uint32_t iDelay,iSample;
	uint32_t plainArray[4];
	uint8_t keyArray[16] = {0x7d,0xc4,0x04,0x63,0xee,0x8d,0x0d,0x11,0x56,0xda,0x16,0x8a,0x71,0x1d,0xef,0xba};

	/************ Acquisition initialize *************/

	//The index of the first sample to be displayed (default 0)
	user_input = strtok(NULL," ");
	sample_min = (user_input == NULL)?0:int2int(user_input);

	//The index of the last sample to be displayed (default 100)
	user_input = strtok(NULL," ");
	sample_max = (user_input == NULL)?100:int2int(user_input);

	//The number of traces to acquire (default 10
	user_input = strtok(NULL," ");
	nb_acq = (user_input == NULL)?10:int2int(user_input);

	//Total number of sample acquired during the DMA transfer
	true_nb_sample = sample_max - 0;

	// Select AES Mode
	Xil_Out32(MODE_ADDR,AES_SSL);

	// Print Mode
	xil_printf("\n\rStart Dual Core Attack on OpenSSL AES\n\r");
	xil_printf("\n\rnTrace: %d",nb_acq);
	xil_printf("\n\rnSample: %d (%d:%d)",sample_max-sample_min,sample_min,sample_max-1);


	/********* DMA Configuration *********/

	// Configure DMA source, destination and size
	DmaCfg = XDmaPs_LookupConfig(DeviceId); if (DmaCfg == NULL) { return XST_FAILURE;}
	Status = XDmaPs_CfgInitialize(DmaInst, DmaCfg, DmaCfg->BaseAddress); if (Status != XST_SUCCESS) { return XST_FAILURE; }
	Status = SetupInterruptSystem(&GicInstance, DmaInst);
	set_DmaCmd (&DmaCmd, 4, (u32*) DLL_addr, (u32*) Dst, true_nb_sample);
	XDmaPs_SetDoneHandler(DmaInst, Channel, DmaDoneHandler, (void *)Checked);


	// Print the AES key for experimental purposes
	xil_printf("\n\rkey : ");
	for(k_i = 0 ; k_i < 16 ; k_i++){ xil_printf("%02x",keyArray[k_i]);}
	xil_printf("\n\r");

	/********* Acquisition Start *********/

	xil_printf("\n\rStart Acquisition:");
	// Loop over the number of traces
	for(uint32_t iTrace = 0 ; iTrace < nb_acq ; iTrace++)
	{

		// Prepare plaintext for transfer to CPU#1
		for(p_i = 0; p_i < 4 ; p_i++){
			plainArray[p_i] = (uint32_t)rand();
			Xil_Out32(PLAIN_ADDR+p_i*4,plainArray[p_i]);
		}

		// Send plain ready to CPU#1
		Xil_Out32(CONFIG_ADDR,PLAIN_READY);

		// Reset DMA status
		Checked[Channel] = 0;

		// Reset performance counters
		init_perfcounters (1,0);

		// Wait for CPU1 encryption ready
		while(Xil_In32(CONFIG_ADDR) != CRYPT_READY);

		// Start DMA transfer (DLL sampling)
		Status = XDmaPs_Start(DmaInst, Channel, &DmaCmd, 0); if (Status != XST_SUCCESS) { xil_printf ("Error!\n\r"); return XST_FAILURE; }

		// Read performance counter -> val1
		asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val1));

		// For loop delay
		for(iDelay = 0 ; iDelay < 100 ; iDelay++){}

		// Read performance counter -> val2
		asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val2));

		// Launch CPU1 encryption
		Xil_Out32(CONFIG_ADDR,CRYPT_START);

		// Wait for CPU1 end encryption
		while(Xil_In32(CONFIG_ADDR) != CRYPT_END){}

		// Read performance counter -> val3
		asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val3));

		// For loop delay
		for(iDelay = 0 ; iDelay < 100 ; iDelay++){}

		// Read performance counter -> val4
		asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val4));

		// Wait for DMA end transfer
		while (!Checked[Channel]) {}

		// Read performance counter -> val5
		asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val5));

		/*if((val3-val2) < min_cycle)
		{
			min_cycle = val3-val2;
		}*/

		//if((val2-val1) < 104030) //2 AES
		//if((val2-val1) < 104030) //2 AES
		//if((val2-val1) < 3160) //1 AES 2 for loop 60 optimisation 0 both
		//if((val2-val1) < 2130)//1 AES 2 for loop 70 optimisation 02(core1) 00(core0
		//if((val3-val1) < 201390))
		//{

		xil_printf("\n\rplaintext : %08x%08x%08x%08x\n\r",plainArray[0],plainArray[1],plainArray[2],plainArray[3]);

		for (iSample = sample_min; iSample < true_nb_sample; iSample++)
		{
			DLL1_cmd = (uint8_t)((Dst[iSample]>>finepos1)&finemask1) +  ((uint8_t)((Dst[iSample]>>coarsepos1)&coarsemask1)*4) -120 + 45;
			DLL0_cmd = (uint8_t)((Dst[iSample]>>finepos0)&finemask0) + ((uint8_t)((Dst[iSample]>>coarsepos0)&coarsemask0)*4) -120 + 45;
			//MasterDLL_0
			//xil_printf("%c",DLL0_cmd);
			//MasterDLL_1
			//xil_printf("%c",DLL1_cmd);
			//MasterDLL_0 + MasterDLL_1
			xil_printf("%c",DLL1_cmd+DLL0_cmd-45);
		}

		//}


		//xil_printf("\n\rnb cycles FOR : %d", val2-val1);
		//xil_printf("\n\rnb cycles AES : %d", val3-val2);
		//xil_printf("\n\rnb cycles FOR  : %d", val4-val3);
		//xil_printf("\n\rnb cycles DMA  : %d", val5-val4);
		//xil_printf("\n\rmin cycle: %d",min_cycle);

		//Pre
		if(iTrace == nb_acq - 1)
		{
			//SELECT IDLE MODE
			Xil_Out32(MODE_ADDR,IDLE);
		}

		//WAIT CPU0 END and reset TRANSFER_END
		Xil_Out32(CONFIG_ADDR,CRYPT_STOP);

		// Wait for CPU#1 ciphertext
		while(Xil_In32(CONFIG_ADDR) != CIPHER_READY){}

		// Print ciphertext
		xil_printf("\n\rciphertext : %08x%08x%08x%08x",Xil_In32(CIPHER_ADDR),Xil_In32(CIPHER_ADDR+4),Xil_In32(CIPHER_ADDR+8),Xil_In32(CIPHER_ADDR+12));


	}
	xil_printf("\n\rEnd Acquisition.\n\r\n\r");

	//SELECT IDLE CONFIG
	Xil_Out32(CONFIG_ADDR,IDLE);

	return 1;
}

void Print_DLL_State(char * user_input)
{
	/************ INITIALISATION *************/
	uint32_t nSample;
	uint32_t DLL_reg;
	uint8_t DLL0_cmd = 0;
	uint8_t DLL1_cmd = 0;

	user_input = strtok(NULL," ");
	nSample = (user_input == NULL)?10:int2int(user_input);

	xil_printf("DLL value: \n\r");

	for (int iSample = 0; iSample < nSample; iSample++)
	{
		DLL_reg = Xil_In32(DLL_addr);
		DLL1_cmd = (uint8_t)((DLL_reg>>finepos1)&finemask1) +  ((uint8_t)((DLL_reg>>coarsepos1)&coarsemask1)*4) -100;
		DLL0_cmd = (uint8_t)((DLL_reg>>finepos0)&finemask0) + ((uint8_t)((DLL_reg>>coarsepos0)&coarsemask0)*4) -100;
		//MasterDLL_0
		//xil_printf("%c",DLL0_cmd);
		//MasterDLL_1
		//xil_printf("%c",DLL1_cmd);
		//MasterDLL_0 + MasterDLL_1
		xil_printf("%02d ",DLL1_cmd+DLL0_cmd);
	}

	xil_printf("\n\r");
}

void Command_Helper(void)
{
	xil_printf("\n\rCommand Helper:");
	xil_printf("\n\r----------------------------------------------------------------");
	xil_printf("\n\r| cmd  |              Parameters            |    Description   |");
	xil_printf("\n\r|--------------------------------------------------------------|");
	xil_printf("\n\r| aes  | <sample_min> <sample_max> <nTrace> |   AES attack     |");
	xil_printf("\n\r| dll  | <nSample>                          | Print DLL value  |");
	xil_printf("\n\r----------------------------------------------------------------");
	xil_printf("\n\r\n\rexample : \"aes 0 100 10\" = 10 AES traces x 100 samples\n\r\n\r");
}
