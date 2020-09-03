# Tutorial: Building SideLine (RSA) on *******

Source code is not available right now, the responsible disclosure process is still ongoing.

## Experimental Setup / Software Version

**Board**: *******  
**Processor**: *******  
**Design suite**: *******  
**SDK**: *******  

## ******* SDK 

**1) Verify that the HW platform project exists in the project explorer** (if not add it)

**2) Create the RSA project**
  - File -> New -> Application Project
  - Project Name -> RSA_viewer_project
  - Processor -> *******
  - Click Next 
  - Select "Empty Application"
  - Click Finish

You should have 3 projects

<!---IMAGE-->


**3) Import GIT Source Code (RSA_setup)**
  - First, delete the existing "src" folder in the RSA_viewer_project arborescence
  - Then, drag and drop git folders [**src**](https://github.com/Remote-HWA/SideLine/tree/master/attack_setup/RSA_setup/RSA_sources/src), [**inc**](https://github.com/Remote-HWA/SideLine/tree/master/attack_setup/RSA_setup/RSA_sources/inc), wolfcrypt and wolfssl into RSA_viewer_project (copy files and folders)  
 Project arborescence should look like that: 
 
<!---IMAGE-->
 
 - We need to add "inc" folder in the project include directories:
 - Right click on RSA_viewer_project -> Properties -> C/C++ General -> Paths and Symbols -> Add -> Workspace -> RSA_viewer_project/inc
 - Now all the errors should have disapeared
 
**4) Run Configuration**

- Connect your board in JTAG mode (PROG UART)
- Open an uart terminal (e.g. teraterm) to test the program
- Select the appropriate COM port & baud rate (115200 default)
- When the terminal is ready, right click RSA_viewer_project -> Run as -> Launch on hardware (System Debugger)
- Press enter in the terminal, you should see:

![teraterm1](https://user-images.githubusercontent.com/67143135/91533438-294ecc80-e910-11ea-9cd5-e62caad463d8.PNG)


The attack is ready.

# Program User Manual 

### Command table
| cmd | Parameters | Description |
| --- | --- | --- |
| naive | <sample_min> <sample_max> <n_Trace> | Start Naive RSA SCA acquisition using DLL |
| const | <sample_min> <sample_max> <n_Trace> | Start Constant Time RSA SCA acquisition using DLL |
| ladder | <sample_min> <sample_max> <n_Trace> | Start Montgomery Ladder RSA SCA acquisition using DLL |
| dll | <n_Sample> | Print current DLL value in decimal |
  
# Save the serial output and use it for SCA

- Once the attack program is up and working, save the serial log using logging in Putty or "File->log..." in teraterm. 
- Then start the RSA SCA acquisition using the command RSA -> ex: naive 0 50000 1000
- Once the acquisition has finished, use the python script available in [**RSA_results**](https://github.com/Remote-HWA/SideLine/tree/master/attack_results/RSA_results) to parse the datafile and conduct the SPA. Follow the guidelines given in the README file associated to the script.







  



