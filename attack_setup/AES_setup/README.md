# Building SideLine (AES) on Xilinx Zynq-7000

## Experimental Setup / Software Version

**Board**: Zybo PB200-279 REV-B  
**Processor**: Zynq XC7Z010CLG400  
**Design suite**: Vivado 2018.1  
**SDK**: Xilinx SDK 2018.1  

## Prelude: Vivado

If you already have a working Hardware Platform Specification File for your board, you can skip this step and import it directly in the SDK.
For those who don't have this file, Vivado allows us to create and customize it. 

**1) Create New Vivado Project**
  - Select your board (Here Zybo)
  - No constraint file
  - No source file
  
**2) Create a simple block design**
  - Add "Zynq7 processing system"
  - Run block automation
  - Check apply board preset then press "OK"
  - (optional) Modify UART1 baud rate to 921600 bauds (increases attack speed)
  - Connect "M_AXI_GP0_ACLK" to "FCLK_CLK0"
  - Create HDL wrapper
  
**3) Generate Block Design (Global)**

**4) Generate Bitstream**

**5) Export HW**
- File -> Export -> Export HW (check include bitstream)
- File -> Launch SDK

## Xilinx SDK (Vitis)

**1) Verify that the HW platform project exists in the project explorer** (if not add it)

**2) Create the attack project**
  - File -> New -> Application Project
  - Project Name -> CPU0_attack_project
  - Processor CPU0 -> ps7_cortexa9_0
  - Click Next 
  - Select "Empty Application"
  - Click Finish

**3) Create the victim project**
  - File -> New -> Application Project
  - Project Name -> CPU1_victim_project
  - Processor CPU0 -> ps7_cortexa9_1
  - Click Next 
  - Select "Empty Application"
  - Click Finish

You should have 5 projects

![projects](https://user-images.githubusercontent.com/67143135/85715121-65389c00-b6eb-11ea-9028-1e2ac7de4e1d.PNG)


**4) Import GIT Source Code (CPU0_attack_project)**
  - First, delete the existing "src" folder in the CPU0_attack_project arborescence
  - Then, drag and drop git folders  "SideLine/attack_script/CPU0/src" and "SideLine/attack_script/CPU0/inc" into CPU0_attack_project (copy files and folders)  
 Project arborescence should look like that: 
 
 ![attack_arbo](https://user-images.githubusercontent.com/67143135/85715557-df692080-b6eb-11ea-8354-8745e1370017.PNG)
 
 - We need to add "inc" folder in the project include directories:
 - Right click on CPU0_attack_project -> Properties -> C/C++ General -> Paths and Symbols -> Add -> Workspace -> CPU0_attack_project/inc
 - Now all the errors should have disapeared
 
 **5) Import GIT Source Code (CPU0_victim_project)**
  - Reproduce step 4) with the CPU0_victim_project
  
Screenshot of the global arborescence: 

![global_arbo](https://user-images.githubusercontent.com/67143135/85715495-ce201400-b6eb-11ea-8279-ebc533fdf40d.PNG)

**6) Run Configuration**

- Connect your board in JTAG mode (PROG UART)
- Open an uart terminal (e.g. teraterm) to test the program
- Select the appropriate COM port & baud rate (115200 default)
- When the terminal is ready, right click CPU0_attack_project -> Run as -> Launch on hardware (System Debugger)
- Press enter in the terminal, you should see:

![teraterm1](https://user-images.githubusercontent.com/67143135/85725080-142da580-b6f5-11ea-9455-689780df846a.PNG)

**7) Dual Core Configuration**

To set up the attack we need to modify the run configuration. 

- Select RUN -> Run Configurations... -> Select the saved configuration
- Select application window -> Check ps7_cortexa9_1 -> press Apply

Now victim and attack programs shouldn parralel in differents CPU cores.
- To verify, select Run
This time, CPU#0 and CPU#1 messages should appear in the console.

![teraterm2](https://user-images.githubusercontent.com/67143135/85725245-3aebdc00-b6f5-11ea-8a32-45431654139a.PNG)


The attack is ready.


# Program User Manual 

### Command table
| cmd  |              Parameters            |    Description   |
| ---- | ---------------------------------- | ---------------- |
| aes  | <sample_min> <sample_max> <nTrace> |   AES attack     |
| dll  | <nSample>                          | Print DLL value  |
                                                                      	|









  



