#ifndef MAIN_H_
#define MAIN_H_


#define READ_REG(DATA,MASK,OFFSET) (((DATA)&(MASK<<OFFSET))>>OFFSET)
#define MODIFY_REG(OLDDATA,MASK,OFFSET,NEWDATA) ((OLDDATA)&(~(MASK<<OFFSET)))|(NEWDATA<<OFFSET)

void Command_Helper(void);
int StateMachine(void);
int SetupInterruptSystem(XScuGic *GicPtr, XDmaPs *DmaPtr);
void DmaDoneHandler(unsigned int Channel, XDmaPs_Cmd *DmaCmd,void *CallbackRef);
void set_DmaCmd (XDmaPs_Cmd *DmaCmd, int BurstSize, u32 *Source, u32 *Destination, u32 DMA_WORDS);
void set_DmaCmdComplex (XDmaPs_Cmd *DmaCmd, int BurstSize, u32 *Source, u32 *Destination, u32 DMA_WORDS,u32 SourceInc, u32 DestInc);
void wait_doneint(volatile int *Checked, unsigned int Channel);


#endif /* MAIN_H_ */

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


