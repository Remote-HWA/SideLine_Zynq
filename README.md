# SideLine - Abstract

To meet the ever-growing need for performance in silicon devices, SoC providers have been increasingly relying on software-hardware cooperation. By controlling hardware resources such as power or clock management from the software, developers earn the possibility to build more flexible and power efficient applications. Despite the benefits, these hardware components are now exposed to software code and can potentially be misused as open-doors to jeopardize trusted environments, perform privilege escalation or steal cryptographic secrets. In this work, we introduce SideLine, a novel side-channel vector based on delay-line components widely implemented in high-end SoCs. After providing a detailed method on how to access and convert delay-line data into power consumption information, we demonstrate that these entities can be used to perform remote power side-channel attacks. We report experiments carried out on two SoCs from distinct vendors and we recount several core-vs-core attack scenarios in which an adversary process located in one processor core aims at eavesdropping the activity of a victim process located in another core. For each scenario, we demonstrate the adversary ability to fully recover the secret key of an OpenSSL AES running in the victim core. Even more detrimental, we show that these attacks are still practicable if the victim or the attacker program runs over an operating system.

![image](https://user-images.githubusercontent.com/67143135/85726797-bac67600-b6f6-11ea-9162-8daf8975c3bd.png)

# What's inside this GitHub repository ?

This repository provides a detailed tutorial on how to conduct SideLine DLL attack scenario (a). The attack is conducted on a Xilinx Zynq7010 SoC in baremetal mode. Two C programs (attacker and victim) are implemented in two different physical cores (AP#0 and AP#1). We provide all the source code in the folder **attack_script** associated to a detailed README helper file. The attack proposed in the paper was conducted on a Zybo board but should be easily portable on others devices that use Zynq-70xx family. Vivado and Xilinx SDK (Vitis) working installations are required to reproduce the attack.
Several numpy arrays which contain 20M separated traces and their associated plaintexts are provided in the folder **attack_samples**. Filtered data arrays are also given, they provide better CPA results with a reduced noise. 
The SCA model leveraged in the paper is close to the first round sbox model but leverages the special t-table output used in OpenSSL AES. The following repository provides a working ttable version of the CPA program https://github.com/cryptolu/aes-cpa.




