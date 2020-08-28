# AES OpenSSL attack on Xilinx Zynq SoC

This folder contains example results and a python script to perform the CPA attack on the OpenSSL AES implementation running within the victim core.

## Content 

The folder **Database** available in the latest SideLine [**release**] contains the "aes_sideline" file. This file contains the entire teraterm session output for 10M AES trace acquisitions. The total size is 428 MB and 6.16 GB when extracted.
Each trace consists in 200 samples that contains 2 contiguous AES encryptions. The attack is thus conducted on a total of 20M AES encryptions.

The python script [**AES_SCA.py**](https://github.com/Remote-HWA/SideLine/blob/master/attack_results/AES_Results/AES_SCA.py) contains a working example of CPA attack using the **Database** files. It reads the data files, applies post-treatment on the traces and computes CPA for each key bytes. File reading, filtering and CPA takes approximately 70 min for the total number of traces. Tested with a standard computer setup: i5-6440HQ CPU with 16 GB RAM.

The folder [**CPA_results**](https://github.com/Remote-HWA/SideLine/tree/master/attack_results/AES_Results/CPA_results) contains the CPA results obtained for 10M traces. It provides additionnal results as concord correlation plots, FFT, and khi2 statistical tests.

## How to use the python CPA script

Python requirement: python3, scipy, numpy, matplotlib 

1) Download the latest SideLine [**release**]
2) Extract the database folder.
3) Open the AES_SCA.py file and specify the AES datafile path. *self.inPath = "your_path/aes_sideline"*
4) Open a terminal at the AES_SCA.py location and start the CPA. *python3 AES_SCA.py*
5) Wait until the end of processing and go to the newly created  DLL_AES_results folder to observe the results.

## How to modify the CPA parameters

User can modify the CPA parameters such as number of traces, samples, filtering, etc. To do so, modify the _init_ method in the AES_SCA class.
- self.nTrace = 10000000&nbsp;&nbsp;&nbsp;#number of DLL traces to process
- self.nSample = 200&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;#number of sample per DLL trace
- self.nByte = 16&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;#number of key bytes considered for the CPA
- self.nClass = 256&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;#number of classes (8bit = 256 classes)
- self.nHyp = 256&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;#number of key hypotheses (8bit = 256 hyp)
- etc.



