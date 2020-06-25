#Building the Attack on Xilinx Zynq-7000


## Experimental Setup 

Zybo PB200-279 REV-B
Zynq XC7Z010CLG400

MASTER DLL ADDR: 0xF80061E0

## VIVADO

1) Create a simple block design
2) Select the preset for your board
3) Autoconnection
3) Modify UART1 baud rate to 921600 bauds (increases attack speed)
4) Generate Bitstream
5) Export HW
6) Launch Vitis

## VITIS SDK

1) Verify that the HW platform project exists in the project explorer 
2) Create a BSP project for the attack program 
-
3) Create a BSP project for the attack program 


Two C projects should be created 


