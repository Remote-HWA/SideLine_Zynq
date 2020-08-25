#### Attack Example Script ####
from scipy import signal
import numpy as np
import os
import matplotlib.pyplot as plt
import time


class RSA_viewer(object):

    def __init__(self):

        ##### Data files #####
        self.inFolder = os.path.join(os.getcwd(),"Database") #path that contain the csv files
        self.dataFileBase = "RSA1024_Naive_t10000_s100000" # base name of the data files
        self.outFolder = os.path.join(os.getcwd(),"DLL_RSA_results")
        self.nFile = 1
        self.nTracePerFile = 1400
        ##### 
        self.nTrace =1400 # number of DLL traces to process
        self.nSample = 100000 # number of sample per DLL trace
        self.moduloval = 100 # Refresh Rate (processed traces)
        self.startTime = time.time() # Get time at the begining of treatment

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


    def Data_Acquisition(self):

        globalDataArray = np.zeros(self.nSample)
        globalFilteredDataArray = np.zeros(self.nSample)
        iFile = 0

        for iTrace in range(0, self.nTrace):

            if iTrace % self.nTracePerFile == 0:
                dataFile = open(os.path.join(self.inFolder,self.dataFileBase+"_%02d"%iFile),'rb') #contain the DLL samples in csv format
                iFile += 1


            #print treatment progression
            if (iTrace%self.moduloval)==0 and iTrace != 0:
                print("Processed traces: %d - Time: %.2f/%.2fmin - Progression: %.2f%%"%(iTrace,(time.time() - self.startTime)/60,((time.time() - self.startTime)/(iTrace/self.nTrace))/60,(iTrace/self.nTrace)*100),end="\r")

            dataArray = np.frombuffer(dataFile.readline(),dtype=np.uint8)[0:self.nSample]

            globalDataArray += dataArray

            dataArray = signal.filtfilt(self.b,self.a, dataArray)

            globalFilteredDataArray += dataArray

        #Close data files
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
        plt.savefig(self.outFolder+"\\filtered_avg.pdf",dpi=600,transparent=True,bbox_inches='tight')
        plt.close('all')

        binfile = open(os.path.join(self.outFolder, "filtered.avg"), 'wb') # create bin file under current directory
        binfile.write(bytearray(globalFilteredDataArray)) # fill bin file with samples
        binfile.close()  

        print("\nEnd Acquisition\n")





#---------------- MAIN ---------------#
rs = RSA_viewer()

##### Acquire Data #####
rs.Data_Acquisition()
