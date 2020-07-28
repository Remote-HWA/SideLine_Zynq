# AES OpenSSL attack on Xilinx Zynq SoC

This folder contains scripts and samples to perform the attack on a OpenSSL AES implementation running in the victim core.

## Content 

The folder [**Database**](https://github.com/Remote-HWA/SideLine/tree/master/attack_results/AES/Database) contains 10M compressed traces in .csv format. The total size is 428 MB and 6.16 GB when extracted.
Each trace consists in 200 samples that contains 2 contiguous AES encryptions. The attack is thus conducted on a total of 20M AES encryptions.

The python script [**CPA_script.py**](https://github.com/Remote-HWA/SideLine/blob/master/attack_results/AES/CPA_script.py) contains a working example of CPA attack using the **Database** files. It reads the data files, applies post-treatment on the traces (butterworth filtering) and computes CPA for each key bytes. File reading, filtering and CPA takes approximately 70 min for the total number of traces. Tested with a standard computer setup: i5-6440HQ CPU with 16 GB RAM.

The folder [**CPA_results**](https://github.com/Remote-HWA/SideLine/tree/master/attack_results/AES/CPA_results) contains the CPA results obtained for 10M traces. It provides additionnal results as concord correlation plots, FFT, and khi2 statistical tests.

## How to use the python CPA script

Python requirement: python3, scipy, numpy, matplotlib 

1) Clone the git folder.
2) Go the database folder location.
3) Extract the 5 zip files located within the [**Database**] folder.
4) Double click on the CPA_script.py file.
5) Wait until the end of processing and go to the newly created  DLL_CPA_results folder to observe the results.


## How to modify the CPA parameters

User can modify the CPA parameters such as number of traces, samples, filtering, etc. To do so, modify the _init_ method in the CPA_DLL class.
- self.nTrace = 10000000   #number of DLL traces to process
- self.nSample = 200       #number of sample per DLL trace
- self.nByte = 16          #number of key bytes considered for the CPA
- self.nClass = 256        #number of classes (8bit = 256 classes)
- self.nHyp = 256          #number of key hypotheses (8bit = 256 hyp)
- etc.



