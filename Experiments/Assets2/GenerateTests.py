#!/usr/bin/env python
import os
import zipfile


def GenOutput(a_ip, a_port, a_numberOfClients, a_renderingTech, a_xDim, a_yDim, a_testIndex, a_outputFileName, a_numberOfLights, a_compressImage):
  l_output = "# server IP\n"
  l_output += a_ip + "\n"
  l_output += "# port\n"
  l_output += a_port + "\n"
  l_output += "# number of clients\n"
  l_output += str(a_numberOfClients) + "\n"
  l_output += "# Rendering Tech (OPENGL OR VULKAN) \n"
  l_output += a_renderingTech + "\n"
  l_output += "# window dimensions (e.g. 680 420)( not applicable on clients) \n"
  l_output += str(a_xDim) + " " + str(a_yDim) + "\n"
  l_output += "# test index (0 OR 1)( not applicable on clients) \n"
  l_output += str(a_testIndex) + "\n"
  l_output += "# output file \n"
  l_output += a_outputFileName + "\n"
  l_output += "# number of lights ( not applicable on clients) \n"
  l_output += str(a_numberOfLights) + "\n"
  l_output += "#compress image ( 0 false, 1 true) ( not applicable on servers ) \n"
  l_output += str(a_compressImage) + "\n"
  return l_output

def GenerateFiles(a_ip, a_port, a_numberOfClients, a_renderingTech, a_xDim, a_yDim, a_testIndex, a_outputFileName, a_numberOfLights, a_compressImage):
  
  
  l_output = GenOutput(a_ip, a_port, a_numberOfClients, a_renderingTech, a_xDim, a_yDim, a_testIndex, "Server_" + a_outputFileName + ".txt", a_numberOfLights, a_compressImage)
  text_file = open("Scripts/ServerConfiguration.config", "w")
  text_file.write(l_output)
  text_file.close()
  
  l_output = GenOutput(a_ip, a_port, a_numberOfClients, a_renderingTech, a_xDim, a_yDim, a_testIndex, "Client_" + a_outputFileName + ".txt", a_numberOfLights, a_compressImage)
  text_file = open("Scripts/ClientConfiguration.config", "w")
  text_file.write(l_output)
  text_file.close()
  
def zipdir(path, ziph):
    # ziph is zipfile handle
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file))

if __name__ == '__main__':

    # l_ip = "192.168.1.2"
    l_ip = "localhost"
    l_port = "50000"
    l_numbersOfClients = [1,2,3,4]
    l_implementationTechs = ["OPENGL", "VULKAN"]
    l_resolutions = [ [640,480], [1280,720], [1920,1080] ]
    l_testIndices = [0, 1]
    l_numberOfLights = [ [10, 100, 200], [100, 500, 1000] ]
    l_compress = [0, 1]
    
    
    
    for l_clientsNumber in l_numbersOfClients:
      for l_tech in l_implementationTechs:
        for l_resolution in l_resolutions:
          for l_testIndex in l_testIndices:
            for l_lightsNum in l_numberOfLights[l_testIndex]:
              for l_willCompress in l_compress:
                l_outputFileName = "T" + str(l_testIndex) + "_" + str(l_clientsNumber) + "_" + l_tech + "_" + str(l_resolution[0]) + "x" + str(l_resolution[1]) + "_L" + str(l_lightsNum) + "_C" + str(l_willCompress)
                GenerateFiles(l_ip, l_port, l_clientsNumber, l_tech, l_resolution[0], l_resolution[1], l_testIndex, l_outputFileName, l_lightsNum, l_willCompress)

                zipf = zipfile.ZipFile( l_outputFileName +'.zip', 'w', zipfile.ZIP_DEFLATED)
                zipdir('Scripts/', zipf)
                zipf.close()