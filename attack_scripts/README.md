# Building the Attack on Xilinx Zynq-7000


## Experimental Setup 

Zybo PB200-279 REV-B
Zynq XC7Z010CLG400

MASTER DLL ADDR: 0xF80061E0

## VIVADO

1) Create New Vivado Project
  - Select your board (Here Zybo)
  - No constraint file
  - No source file
2) Create a simple block design
  - Add "Zynq7 processing system"
  - Run block automation
  - Check apply board preset then press "OK"
  - (optional) Modify UART1 baud rate to 921600 bauds (increases attack speed)
  - Connect "M_AXI_GP0_ACLK" to "FCLK_CLK0"
  - Create HDL wrapper
3) Generate Block Design (Global)
4) Generate Bitstream
5) Export HW
- File -> Export -> Export HW (check include bitstream)
- File -> Launch SDK

## XILINX SDK (VITIS)

1) Verify that the HW platform project exists in the project explorer 

2) Create the attack project
  - File -> New -> Application Project
  - Project Name -> CPU0_attack_project
  - Processor CPU0 -> ps7_cortexa9_0
  - Click Next 
  - Select "Empty Application"
  - Click Finish

3) Create the victim project
  - File -> New -> Application Project
  - Project Name -> CPU1_victim_project
  - Processor CPU0 -> ps7_cortexa9_1
  - Click Next 
  - Select "Empty Application"
  - Click Finish

You should have 5 projects

4) Import Source Code (CPU0_attack_project)
  - Delete src folder in the CPU0_victim_project arborescence
  - Copy & paste git folders  "SideLine/attack_script/CPU0/src" and "SideLine/attack_script/CPU0/inc" into the CPU0_victim_project
  
  
  
  - Copy paste "SideLine/attack_script/CPU0/src" and "SideLine/attack_script/CPU0/inc" in the CPU0_victim_project



Two C projects should be created 


