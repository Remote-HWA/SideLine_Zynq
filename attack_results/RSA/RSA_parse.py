import os
import sys

print("Welcome!")

try: 
    i = sys.argv[1]
except:
    print("Help : ")
    print("-in_path   <filepath> ../samples/data_in")
    print("-out_path  <filepath> ../samples/data_out")
    print("-nb_trace  limit the number of trace")
    quit()
else:
    stringData = ""
    for i in range(1, len(sys.argv)):
        stringData += sys.argv[i] + " "

    command =  stringData.split(" ") 

    print("")

    try:
        in_path = command[command.index("-in_path")+1]
    except:
        print("Error        :               no -in_path")
        in_path = ""
    else:
        print("-in_path     :  %s"%in_path)       

    try:
        out_path = command[command.index("-out_path")+1]
    except:
        print("Error        :               no -out_path")
        out_path = ""
    else:
        print("-out_path    :  %s"%out_path)  

    try:
        nb_trace = int(command[command.index("-nb_trace")+1])
    except:
        nb_trace = 0xFFFFFFFF
    else:
        print("-nb_trace     :  %s"%nb_trace)  

print("\n")

counterERR = 0
counterOK = 0
line=" "
fichier_raw = open(in_path, 'r',encoding='utf-8',errors='replace') #error replace removes codec errors
fichier_csv = open(out_path, 'wb')

# MESURE TAILLE SAMPLE
while True:
    line = fichier_raw.readline()
    index = line.find("WolfSSL RSA")
    if index > -1:
        line = fichier_raw.readline()
        while not line.strip():
            line = fichier_raw.readline()
        nbSamples = int(len(line)) 
        print("nSample per trace: %d"%nbSamples)
        break

fichier_raw.close()


fichier_raw = open(in_path, 'rb') #error replace removes codec errors4

for iTrace in range(nb_trace):

    line = fichier_raw.readline() 

    if not line: #exit if the end of file has been reached 
        break

    if(iTrace%100 == 0): #print progression
        print("Progression: %d traces\r"%(iTrace),end="\r")

            
    if len(line) != nbSamples:
        counterERR += 1
    else:
        counterOK += 1
        #intList = [str(ord(i)) for i in line]
        #line = ",".join(intList)
        fichier_csv.write(line)


print("\n\n%d values removed"%(counterERR))
print("%d values validated"%(counterOK))
fichier_csv.close()
fichier_raw.close()