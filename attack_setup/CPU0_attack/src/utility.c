#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "xil_io.h"


void Write_Register(int adress,int data)
{
	int tempAdress = 4*adress;
	Xil_Out32(tempAdress,  data);
}

int Read_Register(int adress)
{
	int data;
	int tempAdress =  4*adress;
	data =  Xil_In32(tempAdress);
	return data;
}

void Modify_Register(int adress,int newdata, int cleardata)
{
	int value = Read_Register(adress);
	value = (value & (~cleardata)) | newdata;
	Write_Register(adress,value);
}


/*convert hex char to int*/
uint32_t hex2int(char *hex) {
    uint32_t val = 0;
    removeChar(hex,'x');
    while (*hex) {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

/* char array to 32 bit data*/
uint32_t int2int(char *intchar)
{
    return atoi(intchar);
}

uint32_t float2float(char *floatchar)
{
    return atof(floatchar);
}

/*remove char from string*/
void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

int hammingDistance(int x, int y) {
        int z  = x ^ y;
        int r = 0;
        for (; z > 0; z >>= 1) {
            r += z & 1;
        }
        return r;
    }

int hammingWeight(int n)
{
  int r = 0;
  while(n)
  {
      r++;
      n &=n-1;
  }
  return r;
}


int GetInt(void)
{
	//Set initial array length reasonably. size_t is used due to it's ability
	//to allow an array to grow as large as it needs.
	size_t strLength = 32;
	char *stringPtr = malloc(strLength);
	if (stringPtr == NULL)
	{
		fprintf(stderr, "Unable to allocate memory to hold char array. Exiting!\n");
		return 1;
	}
	//printf("Enter some input: ");
	int c = EOF;
	unsigned int i = 0;
	//Checks the value of c (user character input) to see if RETURN or CTRL+C/Z was entered
	while ((c = getchar()) != '\r' && c != EOF)
	{
		//Adds the character entered into the next index of the char array
		stringPtr[i++] = (char) c;
		//Check if we have reached the end of the allocated memory for the char array
		if (i == strLength)
		{
			//multiply the current amount of memory allocated by 2.
			strLength *= 2;
			if ((stringPtr = realloc(stringPtr, strLength)) == NULL)
			{
				fprintf(stderr, "Unable to expand memory to hold char array. Exiting!\n");
				return 2;
			}
		}
	}
	//End of input. This adds the null terminator to terminate the char array
	stringPtr[i] = '\0';
	//Check if we have any unused memory allocated for the array left. If so, we
	//shrink it down to be the size of the input including the null terminator
	if (sizeof(stringPtr) < strLength)
	{
		stringPtr = realloc(stringPtr, i);
	}
	int data = hex2int(stringPtr);
	//printf("data is: %d",data);
	//Memory cleanup time
	return data;
}

/* Function to calculate x raised to the power y */
int power(int x, unsigned int y)
{
    if (y == 0)
        return 1;
    else if (y%2 == 0)
        return power(x, y/2)*power(x, y/2);
    else
        return x*power(x, y/2)*power(x, y/2);
}


char * GetString(void)
{
	//Set initial array length reasonably. size_t is used due to it's ability
	//to allow an array to grow as large as it needs.
	size_t strLength = 32;
	char *stringPtr = malloc(strLength);
	if (stringPtr == NULL)
	{
		fprintf(stderr, "Unable to allocate memory to hold char array. Exiting!\n");
		return '\0';
	}
	//printf("Enter some input: ");
	int c = EOF;
	unsigned int i = 0;
	//Checks the value of c (user character input) to see if RETURN or CTRL+C/Z was entered
	while ((c = getchar()) != ('\r') && c != EOF)
	{
		//Adds the character entered into the next index of the char array
		stringPtr[i++] = (char) c;
		//Check if we have reached the end of the allocated memory for the char array
		if (i == strLength)
		{
			//multiply the current amount of memory allocated by 2.
			strLength *= 2;
			if ((stringPtr = realloc(stringPtr, strLength)) == NULL)
			{
				fprintf(stderr, "Unable to expand memory to hold char array. Exiting!\n");
				return '\0';
			}
		}
	}
	//End of input. This adds the null terminator to terminate the char array
	stringPtr[i] = '\0';
	//Check if we have any unused memory allocated for the array left. If so, we
	//shrink it down to be the size of the input including the null terminator
	if (sizeof(stringPtr) < strLength)
	{
		stringPtr = realloc(stringPtr, i);
	}
	//printf("data is: %d",data);
	//Memory cleanup time
	return stringPtr;
}

void init_perfcounters (int do_reset, int enable_divider)
{
    // in general enable all counters (including cycle counter)
    int value = 1;

    // peform reset:
    if (do_reset)
    {
        value |= 2;     // reset all counters to zero.
        value |= 4;     // reset cycle counter to zero.
    }

    if (enable_divider)
        value |= 8;     // enable "by 64" divider for CCNT.

    value |= 16;

    // program the performance-counter control-register:
    asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));

    // enable all counters:
    asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));

    // clear overflows:
    asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));

    // program the performance-counter control-register:
    asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));
}



void HW_uDelay(int usecond)
{
	int val;

    // program the performance-counter control-register:
    asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(0x17));

	while(val < (usecond*666))
	{
		// Read performance counter -> val
		asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(val));
	}

	//xil_printf("\n\r delay val = %d",val);

}






