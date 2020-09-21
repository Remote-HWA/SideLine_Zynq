#include <stdint.h>

void Write_Register(int adress,int data);
int Read_Register(int adress);
void Modify_Register(int adress,int newdata, int cleardata);
uint32_t convertData32(int startIndex, uint8_t *data);

uint32_t hex2int(char *hex);
uint32_t int2int(char *intchar);
uint32_t float2float(char *floatchar);
void removeChar(char *str, char garbage);
int hammingDistance(int x, int y);
int hammingWeight(int n);
int GetInt(void);
int power(int x, unsigned int y);
char * GetString(void);
void init_perfcounters (int do_reset, int enable_divider);
void HW_uDelay(int usecond);
