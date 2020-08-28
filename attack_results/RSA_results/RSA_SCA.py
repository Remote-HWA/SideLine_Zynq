#### Attack Example Script ####
from scipy import signal
import numpy as np
import os
import matplotlib.pyplot as plt
import time


class RSA_SCA(object):

    def __init__(self):

        ##### User main parameters #####
        self.inPath = "your_path\\naive_sideline" # path of the file which contains the DLL traces
        self.nSample = 30000 # number of sample per DLL trace
        self.nTrace = 2000 # number of DLL traces

        ##### Advanced parameters #####
        self.outFolder = os.path.join(os.path.dirname(self.inPath),"DLL_RSA_results")
        self.moduloval = 100 # Refresh Rate (processed traces)
        self.startTime = time.time() # Get time at the begining of treatment
        self.lineKey = b""
        self.linePlain = b""
        self.lineSample = b""
        self.lineCipher = b""
        self.lineCounter = 0

        ##### Butterworth Filter #####
        fs = 16000 # DLL Sampling frequency (Khz)
        fc1 = 20 # Filter CutOff 1 frequency (KHz)
        fc2 = 2000 # Filter CutOff 2 frequency (KHz)
        order = 4 # Filter order
        w1 = fc1 / (fs / 2) # Normalize the frequency
        w2 = fc2 / (fs / 2) # Normalize the frequency
        self.b,self.a = signal.butter(order, [w1, w2] , btype='bandpass',output='ba') # HP butterworth filter

        ##### Create a folder to save RSA results #####
        counterlog = 0
        try:
            os.mkdir(self.outFolder)
        except OSError:  
            while(1):
                if(os.path.isdir(self.outFolder+"_%02d"%(counterlog))):
                    counterlog = counterlog + 1
                else:
                    os.mkdir(self.outFolder+"_%02d"%(counterlog))
                    self.outFolder = self.outFolder+"_%02d"%(counterlog)
                    break  


    def Check_Data_Integrity(self,dataFile): # check if the uart log file data is corrupted 

        valid = 1
        state = -1

        while state == -1:
            
            self.linePlain = dataFile.readline().decode("utf-8")
            state = self.linePlain.find("plaintext : ")
            self.lineCounter += 1

            if not self.linePlain:
                print("no more line")
                return 2

            if self.linePlain == "":
                print("empty line")
                return 2


        if len(self.linePlain) != 45:
            print("plain line error: %d: line %d"%(len(self.linePlain),self.lineCounter))
            valid = 0

        self.lineSample = dataFile.readline() 
        self.lineCounter += 1

        if len(self.lineSample) < self.nSample:
            print("sample line error: %d > %d: line %d"%(self.nSample,len(self.lineSample),self.lineCounter))
            valid = 0

        self.lineCipher = dataFile.readline() 
        self.lineCounter += 1

        if len(self.lineCipher) != 46:
            print("cipher line error: %d: line %d"%(len(self.lineCipher),self.lineCounter))
            valid = 0  

        return valid


    def Data_Acquisition(self):

        iTrace = 0
        counterOK = 0
        counterERR = 0
        globalDataArray = np.zeros(self.nSample)
        globalFilteredDataArray = np.zeros(self.nSample)
        dataFile = open(self.inPath,'rb') # file that contains the DLL samples 

        while self.lineKey.decode("utf-8").find("key : ") == -1:
            self.lineKey = dataFile.readline() 
            self.lineCounter += 1

        while iTrace < self.nTrace :

            #print treatment progression
            if (iTrace%self.moduloval)==0 and iTrace != 0:
                print("Processed traces: %d - Time: %.2f/%.2fmin - Progression: %.2f%%"%(iTrace,(time.time() - self.startTime)/60,((time.time() - self.startTime)/(iTrace/self.nTrace))/60,(iTrace/self.nTrace)*100),end="\r")
            
            state = self.Check_Data_Integrity(dataFile)

            if state == 1:

                dataArray = np.frombuffer(self.lineSample,dtype=np.uint8)[0:self.nSample]
                globalDataArray += dataArray
                dataArray = signal.filtfilt(self.b,self.a, dataArray)
                globalFilteredDataArray += dataArray
                counterOK += 1
                iTrace += 1

            elif state == 2:

                print("No more line in file: exiting with %d traces"%iTrace)
                break

            else:
                counterERR += 1
                
        #Close data file
        print("\n\n%d values removed"%(counterERR))
        print("%d values validated"%(counterOK))
        self.nTrace = counterOK
        dataFile.close()

        ####### SAVE AVERAGE VALUE PDF AND BIN #######
        globalDataArray = np.divide(globalDataArray,self.nTrace)
        globalFilteredDataArray = np.divide(globalFilteredDataArray,self.nTrace)

        fig = plt.figure(1)
        ax = fig.add_subplot(111)
        plt.rc('font', size=7)
        plt.title("Averaged value for %d traces"%(self.nTrace))
        plt.text(1.02,0.7,"Nb Traces: %d\nNb Samples: %d\n"%(self.nTrace, self.nSample),horizontalalignment='left',verticalalignment='center', transform = ax.transAxes)   
        plt.xlabel("Time (samples)")
        plt.ylabel("Quantification")
        plt.plot(globalDataArray,linewidth=0.7, linestyle="-")
        plt.savefig(self.outFolder+"\\unfiltered_avg.png",dpi=600,transparent=True,bbox_inches='tight')
        plt.savefig(self.outFolder+"\\unfiltered_avg.pdf",dpi=600,transparent=True,bbox_inches='tight')
        plt.close('all')

        binfile = open(os.path.join(self.outFolder, "unfiltered.avg"), 'wb') # create bin file under current directory
        binfile.write(bytearray(globalDataArray)) # fill bin file with samples
        binfile.close()  

        fig = plt.figure(1)
        ax = fig.add_subplot(111)
        plt.rc('font', size=7)
        plt.title("Filtered averaged value for %d traces"%(self.nTrace))
        plt.text(1.02,0.7,"Nb Traces: %d\nNb Samples: %d\n"%(self.nTrace, self.nSample),horizontalalignment='left',verticalalignment='center', transform = ax.transAxes)   
        plt.xlabel("Time (samples)")
        plt.ylabel("Quantification")
        plt.plot(globalFilteredDataArray,linewidth=0.7, linestyle="-")
        plt.savefig(self.outFolder+"\\filtered_avg.png",dpi=600,transparent=True,bbox_inches='tight')
        plt.savefig(self.outFolder+"\\filtered_avg.pdf",dpi=600,transparent=True,bbox_inches='tight')
        plt.close('all')

        binfile = open(os.path.join(self.outFolder, "filtered.avg"), 'wb') # create bin file under current directory
        binfile.write(bytearray(globalFilteredDataArray)) # fill bin file with samples
        binfile.close()  

        print("\nEnd Acquisition\n")



#---------------- MAIN ---------------#
rs = RSA_SCA()

##### Acquire Data #####
rs.Data_Acquisition()
