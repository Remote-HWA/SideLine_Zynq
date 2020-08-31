/*******************************************************************************
*               SideLine 2020 - https://github.com/Remote-HWA/SideLine/
*
* Name        : main_RSA.c
*
* Description : Perform RSA encryption while collecting DLL state using DMA.
*               Data is exported through Uart.
*
* Author      : Remote-HWA
*
* Toolchain   : Xilinx ARM v7 GNU Toolchain
*
* Host        : Zynq XC7Z010-CLG400
*
*
*******************************************************************************
*
* Version :     1.0
*               31/08/2020 : 1.0 - Initial version
*******************************************************************************/

#include <main_RSA.h>

// Number of measurements between each interrupt
#define DMA_LENGTH	100000
// Number of sample that will be saved per interrupt
#define NB_SAMPLE_PER_LOOP 50000
// Total number of samples (set the number of interrupt)
#define NB_SAMPLE_TOTAL 50000
// Number of interrupts = NB_SAMPLE_TOTAL / NB_SAMPLE_PER_LOOP

static int Dst[DMA_LENGTH] __attribute__ ((aligned (32)));
static int sampleArray[NB_SAMPLE_TOTAL] __attribute__ ((aligned (32)));

XDmaPs DmaInstance;
XScuGic GicInstance;
XDmaPs *DmaInst = &DmaInstance;
int intStatus = 0;
int offsetLoop = 0;

int main(int argc, char* argv[])
{
	char * command;
	char * user_input;

	// Init performance counter
	init_perfcounters (1,0);

	// Print Hello Banner
	// http://patorjk.com/software/taag/ Font Name: Small Slant
	xil_printf("\n\r\n\r\n\r\n\r");
	xil_printf(
	"   _____    __    ___                   \n\r"
	"  / __(_)__/ /__ / (_)__  ___           \n\r"
	" _\\ \\/ / _  / -_) / / _ \\/ -_)       \n\r"
	"/___/_/\\_,_/\\__/_/_/_//_/\\__/  on RSA\n\r");



	xil_printf("\n\rEnter \"help\" to display the command helper\n\r");


	do{
		xil_printf("SideLine>");
		command = GetString();
		user_input = strtok(command," ");
		xil_printf("\n\r");

		/********RSA 2048 WOLFSSL*********/
		if((strcmp(user_input,"wssl")==0) || (strcmp(user_input,"WSSL")==0))
		{
			//to do
		}
		/********RSA 1024 NAIVE IMPLEM*********/
		else if(strstr(user_input,"naive") || strstr(user_input,"NAIVE"))
		{
			Custom_RSA_Attack(user_input,NAIVE);
		}
		/********RSA 1024 CONSTANT TIME IMPLEM*********/
		else if(strstr(user_input,"const") || strstr(user_input,"CONST"))
		{
			Custom_RSA_Attack(user_input,CONSTANT_TIME);
		}
		/********RSA 1024 MONTGOMERY LADDER IMPLEM*********/
		else if(strstr(user_input,"ladder") || strstr(user_input,"LADDER"))
		{
			Custom_RSA_Attack(user_input,LADDER);
		}
		/*******PRINT DLL STATE*********/
		else if((strcmp(user_input,"dll")==0) || (strcmp(user_input,"DLL")==0))
		{
			Print_DLL_State(user_input);
		}
		/********HELP*****/
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

}

int Custom_RSA_Attack(char * user_input, uint8_t mode)
{
	// RSA variables
	RsaKey         	rsaPrivateKey;
	RsaKey*     	privRsaKey = NULL;
	byte plain1024[128];

	// DMA and interrupt variables
	XDmaPs_Config 	*DmaCfg;
	XDmaPs_Cmd 		DmaCmd;
	XDmaPs 			DmaInstance;
	XScuGic 		GicInstance;
	XDmaPs 			*DmaInst = &DmaInstance;
	unsigned int 	Channel = 0;
	volatile int 	Checked[XDMAPS_CHANNELS_PER_DEV];
	u16 			DeviceId = 	DMA_DEVICE_ID;

	// Acquisition variables
	int Status = 0;
	int nTrace 	   = 0;
	int nSample  = 0;
	int minSample = 0;
	int sample_max = 0;
	uint8_t DLL0_cmd = 0;
	uint8_t DLL1_cmd = 0;

	/************ Initialisation *************/

	//The index of the first sample to be displayed (default 0)
	user_input = strtok(NULL," ");
	minSample = (user_input == NULL)?0:int2int(user_input);

	//The index of the last sample to be displayed (default 100)
	user_input = strtok(NULL," ");
	sample_max = (user_input == NULL)?100:int2int(user_input);

	//The number of traces to acquire (default 10
	user_input = strtok(NULL," ");
	nTrace = (user_input == NULL)?10:int2int(user_input);

	//Total number of sample acquired during the DMA transfer
	nSample = sample_max - 0;

	/************ Print Acquisition Info *************/

	switch(mode)
	{
		case NAIVE:
			xil_printf("\n\r\n\rSimple Core Attack on Naive RSA:");
			break;
		case CONSTANT_TIME:
			xil_printf("\n\r\n\rSimple Core Attack on Constant Time RSA:");
			break;
		case LADDER:
			xil_printf("\n\r\n\rSimple Core Attack on Ladder RSA:");
			break;
		default:
			xil_printf("\n\r\n\rError in RSA selection");
			return 0;
			break;
	}
	xil_printf("\n\rnTrace: %d",nTrace);
	xil_printf("\n\rnSample: %d (%d:%d)\n\r",sample_max-minSample,minSample,sample_max-1);

	/************  DMA Configuration  *************/
	DmaCfg = XDmaPs_LookupConfig(DeviceId); if (DmaCfg == NULL) { return XST_FAILURE;}
	Status = XDmaPs_CfgInitialize(DmaInst, DmaCfg, DmaCfg->BaseAddress); if (Status != XST_SUCCESS) { return XST_FAILURE; }
	Status = SetupInterruptSystem(&GicInstance, DmaInst);
	set_DmaCmd (&DmaCmd, 4, (u32*) DLL_addr, (u32*) Dst,DMA_LENGTH);
	XDmaPs_SetDoneHandler(DmaInst, Channel, DmaDoneHandler, (void *)Checked);

	/************ Init RSA *************/
	wc_InitRsaKey(&rsaPrivateKey, NULL);
	privRsaKey = &rsaPrivateKey;
	mp_read_unsigned_bin(&rsaPrivateKey.n, n1024, sizeof(n1024));
	mp_read_unsigned_bin(&rsaPrivateKey.d, d1024, sizeof(d1024));
	rsaPrivateKey.type = RSA_PRIVATE_DECRYPT;

	/************ Start Acquisition *************/
	xil_printf("\n\rStart Acquisition:");

	/************ Prepare for post treatment (python) *************/
	xil_printf("\n\rkey : (dummy)0%08x%08x%08x",0,0,0);

	for(int iTrace = 0 ; iTrace < nTrace ; iTrace++)
	{
		//Reset DMA
		Checked[Channel] = 0;

		xil_printf("\n\rplaintext : (counter)%07x%08x%08d\n\r",0,0,iTrace);

		//Start DLL acquisition
		Status = XDmaPs_Start(DmaInst, Channel, &DmaCmd, 0); if (Status != XST_SUCCESS) { xil_printf ("Error!\n\r"); return XST_FAILURE; }

		// Apply delay before decryption
		//for(uint32_t i = 0 ; i < 100000 ; i++);

		//Launch custom RSA implem
		switch(mode)
		{
			case NAIVE:
				sp_RsaPrivate_1024_custom_naive(enc1024,sizeof(enc1024),&rsaPrivateKey.d,&rsaPrivateKey.n,plain1024,sizeof(plain1024));
				break;
			case CONSTANT_TIME:
				sp_RsaPrivate_1024_custom_constanttime(enc1024,sizeof(enc1024),&rsaPrivateKey.d,&rsaPrivateKey.n,plain1024,sizeof(plain1024));
				break;
			case LADDER:
				sp_RsaPrivate_1024_custom_montgomeryladder(enc1024,sizeof(enc1024),&rsaPrivateKey.d,&rsaPrivateKey.n,plain1024,sizeof(plain1024));
				break;
			default:
				break;
		}

		/*xil_printf(("\n\rplain\n\r"));
		for(int i = 0 ; i < 128 ; i++)
		{
			xil_printf("%02x ",plain1024[i]);
		}
		xil_printf("\n\r");*/

		//Print samples
		for (int iSample = minSample; iSample < nSample; iSample++)
		{
			DLL1_cmd = (uint8_t)((sampleArray[iSample]>>finepos1)&finemask1) +  ((uint8_t)((sampleArray[iSample]>>coarsepos1)&coarsemask1)*4) -120 + 45;
			DLL0_cmd = (uint8_t)((sampleArray[iSample]>>finepos0)&finemask0) + ((uint8_t)((sampleArray[iSample]>>coarsepos0)&coarsemask0)*4) -120 + 45;
			xil_printf("%c",DLL1_cmd+DLL0_cmd-45);
		}

		xil_printf("\n\rciphertext : (counter)%07x%08x%08d",0,0,iTrace);

	}

	xil_printf("\n\rEnd Acquisition.\n\r\n\r");

	// Free RSA key
	if (privRsaKey != NULL)
		wc_FreeRsaKey(privRsaKey);

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


	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	/*
	 * Enable the interrupts for the device
	 */
	XScuGic_Enable(GicPtr, DMA_DONE_INTR_0);
	XScuGic_Enable(GicPtr, DMA_FAULT_INTR);

	Xil_ExceptionEnable();

	return XST_SUCCESS;

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

void DmaDoneHandler(unsigned int Channel, XDmaPs_Cmd *DmaCmd, void *CallbackRef)
{
	volatile int *Checked = (volatile int *)CallbackRef;

	int step = (int)(DMA_LENGTH / NB_SAMPLE_PER_LOOP);
	int sampleCounter = 0;

	for(int dmaCounter = 0 ; dmaCounter < DMA_LENGTH ; dmaCounter = dmaCounter + step)
	{
		sampleArray[sampleCounter+(offsetLoop*NB_SAMPLE_PER_LOOP)] = Dst[dmaCounter];
		sampleCounter++;
	}

	Checked[Channel] = 1;

	if(offsetLoop < (int)(NB_SAMPLE_TOTAL / NB_SAMPLE_PER_LOOP))
	{

		//RESTART DMA
		if(offsetLoop == (int)(NB_SAMPLE_TOTAL / NB_SAMPLE_PER_LOOP)-1)
		{
			offsetLoop = 0;
		}
		else
		{
			Checked[Channel] = 0;
			intStatus = XDmaPs_Start(DmaInst, Channel, DmaCmd, 0); if (intStatus != 0) { xil_printf ("Error!\n\r"); }
			offsetLoop++;
		}

	}
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
	xil_printf("\n\r|naive | <sample_min> <sample_max> <nTrace> | Naive S&M RSA    |");
	xil_printf("\n\r|const | <sample_min> <sample_max> <nTrace> | Constant time RSA|");
	xil_printf("\n\r|ladder| <sample_min> <sample_max> <nTrace> | Montladder RSA   |");
	xil_printf("\n\r|dll   | <nSample>                          | Print DLL value  |");
	xil_printf("\n\r----------------------------------------------------------------");
	xil_printf("\n\r\n\rexample : \"naive 0 100 10\" = 10 RSA traces x 100 samples\n\r\n\r");
}
