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
6) Launch SDK

## XILINX SDK (VITIS)

1) Verify that the HW platform project exists in the project explorer 
2) Create a BSP project for the attack program 
-
3) Create a BSP project for the attack program 


Two C projects should be created 


