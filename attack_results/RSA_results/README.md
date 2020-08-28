# Custom RSA attack on Xilinx Zynq SoC

This folder contains example results and a python script to perform the SPA attack on several custom RSA implementations.

## Content 

The folder **Database** available in the latest SideLine [**release**] contains the several RSA datafiles. These files contain the entire teraterm session output for several thoudand RSA acquisitions. 

The python script [**RSA_SCA.py**](https://github.com/Remote-HWA/SideLine/blob/master/attack_results/RSA_Results/RSA_SCA.py) allows the user to visualize the RSA power consumption. It reads the datafiles, applies post-treatment on the traces and save RSA power consumption in png and binay and pdf files. 

The folders ConstantTime_RSA, MontgomeryLadder_RSA and Naive_RSA contain example results obtained with the Zybo board.

## How to use the python SPA script

Python requirement: python3, scipy, numpy, matplotlib 

1) Download the latest SideLine [**release**]
2) Extract the database folder.
3) Open the RSA_SCA.py file and specify the RSA datafile path. *self.inPath = "your_path/naive_sideline"*
4) Open a terminal at the RSA_SCA.py location and start the SPA. *python3 RSA_SCA.py*
5) Wait until the end of processing and go to the newly created  DLL_RSA_results folder to observe the results.

## How to modify the SPA parameters

User can modify the SPA parameters such as number of traces, samples, filtering, etc. To do so, modify the _init_ method in the RSA_SCA class.
- self.nTrace = 10000000&nbsp;&nbsp;&nbsp;#number of DLL traces to process
- self.nSample = 200&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;#number of sample per DLL trace



