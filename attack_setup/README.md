# SideLine - How Delay-Lines (May) Leak Secrets from your SoC

## About SideLine

SideLine is a novel side-channel vector based on delay-line components widely implemented in high-end SoCs. In the associated paper [paper ref], we provide a detailed method on how to access and convert delay-line data into power consumption information and we demonstrate that these entities can be used to perform remote power side-channel attacks. We report experiments carried out on two SoCs from distinct vendors and we recount several core-vs-core attack scenarios in which an adversary process located in one processor core aims at eavesdropping the activity of a victim process located in another core. For each scenario, we demonstrate the adversary ability to fully recover the secret key of an OpenSSL AES running in the victim core. Even more detrimental, we show that these attacks are still practicable if the victim or the attacker program runs over an operating system.

<p align="center">
<img src="https://user-images.githubusercontent.com/67143135/85726797-bac67600-b6f6-11ea-9162-8daf8975c3bd.png" width="700" height="250">
</p>
<p align="center"> Figure 1: The three attack scenarios presented in SideLine<p align="center">
  
## Content

This repository contains:
- A detailed tutorial on how to build the first SideLine attack scenario as illustrated in Figure 1.a). Source codes, tutorials and other resources are located in the folder [**attack_setup**](https://github.com/Remote-HWA/SideLine/tree/master/attack_setup).

- DLL datasets for AES and RSA side-channel attacks and Python scripts for data treatment, Correlation Power Analysis and Simple Power Analysis. These resources are located in the folder [**attack_results**](https://github.com/Remote-HWA/SideLine/tree/master/attack_results). 

## Attack Principle 

The attack is conducted on a Xilinx Zynq7000 SoC in baremetal mode. Two C programs (attacker and victim) are implemented in two different physical cores (AP#0 and AP#1).
- The [**victim source code**](https://github.com/Remote-HWA/SideLine/tree/master/attack_setup/CPU1_victim) contains several software crypto implementations such as OpenSSL AES and custom RSA (**new!**). It is used as an encryption service by the attack program.
- The [**attack source code**](https://github.com/Remote-HWA/SideLine/tree/master/attack_setup/CPU0_attack) contain several methods which configure the encryption mode and uses DMA and DLL to perform SCA on the algorithms running in the victim program.

## Requirements

- A Xilinx Zynq7000 based development board (eg: Zybo, zedboard, etc.)
- Working installations of Xilinx Vivavo, Xilinx SDK (Vitis) and python3










