#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "xdmaps.h"
#include "xscugic.h"
#include "xparameters.h"
#include "xil_io.h"
#include "utility.h"
#include "main.h"
#include "xil_cache.h"

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

#define TEST_ROUNDS	1				/* Number of loops that the Dma transfers run.*/
#define DMA_LENGTH	10000			/* Length of the Dma Transfers */
#define TIMEOUT_LIMIT 	0x10000		/* Loop count for timeout */

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

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Macro Definitions *****************************/

/************************** Variable Definitions *****************************/
//static int Src[DMA_LENGTH] __attribute__ ((aligned (32)));
static int Dst[DMA_LENGTH] __attribute__ ((aligned (32)));


XDmaPs DmaInstance;
XScuGic GicInstance;
int val1,val2,val3,val4,val5;
const uint32_t sharedAddr = 0x7000000;

int main(void)
{
	xil_printf("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\rCPU#0: Welcome! I'm ready to attack!\n\r");

	//wait for CPU#1 to be ready
	//usleep(100000);

	StateMachine();

	return 1;
}


int StateMachine(void)
{
	/*******MASTER DLL 1**********/
	int baseaddr = 0xF80061E0;
	uint8_t coarsemask1 = 0x7f;
	uint8_t coarsepos1 = 13;
	uint8_t finemask1 = 0x03;
	uint8_t finepos1 = 11;

	/*******MASTER DLL 0**********/
	uint8_t coarsemask0 = 0x7f;
	uint8_t coarsepos0 = 4;
	uint8_t finemask0 = 0x03;
	uint8_t finepos0 = 2;

	/*******TO MASTER DLL OUTPUTFILTER 0**********/
	/*int baseaddr = 0xF80061E4;
	uint8_t coarsemask = 0xFF;
	uint8_t coarsepos = 2;
	uint8_t finemask = 0x03;
	uint8_t finepos = 0;*/

	/*******FROM MASTER DLL OUTPUTFILTER 0**********/
	/*int baseaddr = 0xF80061E4;
	uint8_t coarsemask = 0X7F;
	uint8_t coarsepos = 12;
	uint8_t finemask = 0x03;
	uint8_t finepos = 10;*/

	/*******DLL_WDQS**********/
	/*int baseaddr = 0xF80061CC;
	uint8_t coarsemask = 0X7F;
	uint8_t coarsepos = 20;
	uint8_t finemask = 0x03;
	uint8_t finepos = 18;*/


	int Status;
	uint32_t min_cycle=0xffffffff;
	unsigned int Channel = 0;
	volatile int Checked[XDMAPS_CHANNELS_PER_DEV];
	XDmaPs *DmaInst = &DmaInstance;
	XDmaPs_Config *DmaCfg;
	XDmaPs_Cmd DmaCmd;
	char * command;
	char * value;
	u16 DeviceId = DMA_DEVICE_ID;
	uint8_t DLL0_cmd = 0;
	uint8_t DLL1_cmd = 0;
	int nb_acq 	   = 0;
	int true_nb_sample  = 0;
	int sample_min = 0;
	int sample_max = 0;
	uint8_t p_i = 0;
	uint32_t plainArray[4];
	uint32_t DLL_reg = 0;

	DmaCfg = XDmaPs_LookupConfig(DeviceId); if (DmaCfg == NULL) { return XST_FAILURE;}
	Status = XDmaPs_CfgInitialize(DmaInst, DmaCfg, DmaCfg->BaseAddress); if (Status != XST_SUCCESS) { return XST_FAILURE; }
	Status = SetupInterruptSystem(&GicInstance, DmaInst);

  do{
	   usleep(200000);
	   xil_printf("TEST>");
	   command = GetString();
	   value = strtok(command," ");
	   xil_printf("\n\r");


		if((strcmp(value,"ssl")==0) || (strcmp(value,"SSL")==0))
		{
			xil_printf("Start Dual Core Attack on OpenSSL AES\n\r");


			/************ INITIALISATION *************/

			//The index of the first sample to be displayed (default 0)
			value = strtok(NULL," ");
			sample_min = (value == NULL)?0:int2int(value);

			//The index of the last sample to be displayed (default 100)
			value = strtok(NULL," ");
			sample_max = (value == NULL)?100:int2int(value);

			//The number of traces to acquire (default 10
			value = strtok(NULL," ");
			nb_acq = (value == NULL)?10:int2int(value);

			//Total number of sample acquired during the DMA transfer
			true_nb_sample = sample_max - 0;


			/********* DMA CONFIGURATION *********/

			// Reset the DLL command destination array
			for (int i = 0; i < true_nb_sample; i++) Dst[i] = 0;

			// Configure DMA source, destination and size
			set_DmaCmd (&DmaCmd, 4, (u32*) baseaddr, (u32*) Dst, true_nb_sample);
			XDmaPs_SetDoneHandler(DmaInst, Channel, DmaDoneHandler, (void *)Checked);


			/********  AES CONFIGURATION *********/

			// Loop over the number of traces
			for(uint32_t trace = 0 ; trace < nb_acq ; trace++)
			{

				// Prepare plaintext transfer to CPU#1
				for(p_i = 0; p_i < 4 ; p_i++){
					plainArray[p_i] = (uint32_t)rand();
					Xil_Out32(sharedAddr+AES_PLAIN+p_i*4,plainArray[p_i]);
				}

				// Send plain ready to CPU#1
				Xil_Out32(sharedAddr+PLAIN_READY,1);

				//RESET DMA STATUS
				Checked[Channel] = 0;

				//RESET PERF COUNTERS
				init_perfcounters (1,0);

				//WAIT FOR CPU1 READY and reset PLAIN_ACK
				while(Xil_In32(sharedAddr+PLAIN_ACK)!=1);
				Xil_Out32(sharedAddr+PLAIN_ACK,0);

				//LAUNCH DMA
				Status = XDmaPs_Start(DmaInst, Channel, &DmaCmd, 0); if (Status != XST_SUCCESS) { xil_printf ("Error!\n\r"); return XST_FAILURE; }

				//READ PERF COUNTER -> VAL1
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val1));

				//For loop delay
				for(int i = 0 ; i < 1000 ; i++){}

				//READ PERF COUNTER -> VAL2
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val2));

				//LAUNCH CPU1 ENCRYPTION
				Xil_Out32(sharedAddr+AES_START,0x1);

				//WAIT FOR CPU1 END OF ENCRYPTION and reset AES_END
				while(Xil_In32(sharedAddr+AES_END) != 0x1){}
				Xil_Out32(sharedAddr+AES_END,0x0);

				//READ PERF COUNTER -> VA42
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val3));

				for(int i = 0 ; i < 1000 ; i++){}

				//READ PERF COUNTER -> VA42
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val4));

				//WAIT FOR DMA END
				while (!Checked[Channel]) {}

				//READ PERF COUNTER -> VAL3 DMA
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val5));

				if((val2-val1) < min_cycle)
				{
					min_cycle = val2-val1;
				}

				//if((val2-val1) < 104030) //2 AES
				//if((val2-val1) < 104030) //2 AES
				//if((val2-val1) < 3160) //1 AES 2 for loop 60 optimisation 0 both
				//if((val2-val1) < 2130)	//1 AES 2 for loop 70 optimisation 02(core1) 00(core0
				//if((val3-val1) < 201390))
				//{

				xil_printf("\n\rplaintext : %08x%08x%08x%08x\n\r",plainArray[0],plainArray[1],plainArray[2],plainArray[3]);

				for (int i = sample_min; i < true_nb_sample; i++)
				{

					DLL1_cmd = (uint8_t)((Dst[i]>>finepos1)&finemask1) +  ((uint8_t)((Dst[i]>>coarsepos1)&coarsemask1)*4) -120 + 45;
					DLL0_cmd = (uint8_t)((Dst[i]>>finepos0)&finemask0) + ((uint8_t)((Dst[i]>>coarsepos0)&coarsemask0)*4) -120 + 45;
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
				xil_printf("\n\rciphertext : %08x%08x%08x%08x",Xil_In32(sharedAddr+AES_CIPHER),Xil_In32(sharedAddr+AES_CIPHER+4),Xil_In32(sharedAddr+AES_CIPHER+8),Xil_In32(sharedAddr+AES_CIPHER+12));

				//SEND END TO CPU1
				Xil_Out32(sharedAddr+TRANSFER_END,0x1);

			}
			xil_printf("\n\r");
		}
		else if((strcmp(value,"expdual")==0) || (strcmp(value,"EXPDUAL")==0))
		{
			xil_printf("Start Dual Core Attack on Open SSL\n\r");

			//READ NUMBER OF ACQUISITIONS
			int nb_acq 	   = 0;
			int true_nb_sample  = 0;
			int sample_min = 0;
			int sample_max = 0;

			value = strtok(NULL," "); // getdata
			sample_min = (value == NULL)?0:int2int(value);
			value = strtok(NULL," "); // getdata
			sample_max = (value == NULL)?10:int2int(value);
			value = strtok(NULL," "); // getdata
			nb_acq = (value == NULL)?10:int2int(value);
			true_nb_sample = sample_max - 0;

			//CLR DESTINATION
			for (int i = 0; i < true_nb_sample; i++) Dst[i] = 0;

			//DMA CONFIGURATION
			set_DmaCmd (&DmaCmd, 4, (u32*) baseaddr, (u32*) Dst, true_nb_sample);
			XDmaPs_SetDoneHandler(DmaInst, Channel, DmaDoneHandler, (void *)Checked);

			usleep(1000000);

			//PRINT KEY RECEIVED FROM CPU1
			xil_printf("key : %08x%08x%08x%08x\n\r",Xil_In32(sharedAddr+0x24),Xil_In32(sharedAddr+0x28),Xil_In32(sharedAddr+0x2c),Xil_In32(sharedAddr+0x30));

			//xil_printf("\n\rCPU0: %08x\n\r",Xil_In32(sharedAddr));
			// LOOP OVER TRACE NUMBER
			for(uint32_t trace = 0 ; trace < nb_acq ; trace++)
			{
				//RESET DMA STATUS
				Checked[Channel] = 0;

				//RESET PERF COUNTERS
				init_perfcounters (1,0);

				//WAIT FOR CPU1 READY
				while(Xil_In32(sharedAddr) != 5){
					//xil_printf("stuck 5\n\r");
				}

				//LAUNCH DMA
				Status = XDmaPs_Start(DmaInst, Channel, &DmaCmd, 0); if (Status != XST_SUCCESS) { xil_printf ("Error!\n\r"); return XST_FAILURE; }

				//READ PERF COUNTER -> VAL1
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val1));

				for(int i = 0 ; i < 20000 ; i++){}
				//for(int i = 0 ; i < 1000 ; i++){}

				//READ PERF COUNTER -> VAL2
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val2));

				//LAUNCH CPU1 ENCRYPTION
				Xil_Out32(sharedAddr,0x1);
				//WAIT FOR CPU1 END OF ENCRYPTION
				while(Xil_In32(sharedAddr) != 2){}

				//READ PERF COUNTER -> VA42
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val3));


				for(int i = 0 ; i < 40000 ; i++){}
				//for(int i = 0 ; i < 1000 ; i++){}

				//READ PERF COUNTER -> VA42
				//asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val4));

				//WAIT FOR DMA END
				while (!Checked[Channel]) {}

				//READ PERF COUNTER -> VAL3 DMA
				asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val5));

				if((val2-val1) < min_cycle)
				{
					min_cycle = val2-val1;
				}

				//if((val2-val1) < 104030) //2 AES

				//if((val2-val1) < 104030) //2 AES
				//if((val2-val1) < 3160) //1 AES 2 for loop 60 optimisation 0 both
				//if((val2-val1) < 2130)	//1 AES 2 for loop 70 optimisation 02(core1) 00(core0
			  //if((val3-val1) < 201390))
			  //{
				xil_printf("\n\rplaintext : %08x%08x%08x%08x\n\r",Xil_In32(sharedAddr+0x4),Xil_In32(sharedAddr+0x8),Xil_In32(sharedAddr+0xc),Xil_In32(sharedAddr+0x10));

				for (int i = sample_min; i < true_nb_sample; i++)
				{

					DLL1_cmd = (uint8_t)((Dst[i]>>finepos1)&finemask1) +  ((uint8_t)((Dst[i]>>coarsepos1)&coarsemask1)*4) -120 + 45;
					DLL0_cmd = (uint8_t)((Dst[i]>>finepos0)&finemask0) + ((uint8_t)((Dst[i]>>coarsepos0)&coarsemask0)*4) -120 + 45;
					//MasterDLL_0
					//xil_printf("%c",DLL0_cmd);
					//MasterDLL_1
					//xil_printf("%c",DLL1_cmd);
					//MasterDLL_0 + MasterDLL_1
					xil_printf("%c",DLL1_cmd+DLL0_cmd-45);
				}
				/*xil_printf("\n\rnb cycles FOR : %d", val2-val1);
				xil_printf("\n\rnb cycles AES : %d", val3-val2);
				//xil_printf("\n\rnb cycles FOR  : %d", val4-val3);
				xil_printf("\n\rnb cycles DMA  : %d", val5-val1);*/
					//xil_printf("\n\rmin cycle: %d",min_cycle);
					//xil_printf("\n\rciphertext : %08x%08x%08x%08x",Xil_In32(sharedAddr+0x14),Xil_In32(sharedAddr+0x18),Xil_In32(sharedAddr+0x1c),Xil_In32(sharedAddr+0x20));
			//*}
			/*	else
				{
					xil_printf ("\n\rnb cycles : %d", val2-val1);
				}*/

				//SEND END TO CPU1
				Xil_Out32(sharedAddr,0x3);

			}
			xil_printf("\n\r");
		}
		else if((strcmp(value,"dll")==0) || (strcmp(value,"DLL")==0))
		{

			/************ INITIALISATION *************/

			//The index of the first sample to be displayed (default 0)
			value = strtok(NULL," ");
			true_nb_sample = (value == NULL)?10:int2int(value);


			xil_printf("DLL value: \n\r");

			for (int i = 0; i < true_nb_sample; i++)
			{
				DLL_reg = Xil_In32(baseaddr);
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
		else
		{
			xil_printf("Unknown Command\n\r");
		}
   }
   while(1);


  return 0;

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







