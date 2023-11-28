#imports necessarios para o codigo
import serial
import io
from datetime import datetime

#Inicializacao do Serial
ser = serial.Serial('COM5', 9600)
try:    
    ser.close()
except:
    pass
ser.open()

#Definicao dos arquivos de saida
path = "./logs/client/"
arqs= [ "07-5.csv","07-8.csv",
        "08-5.csv","08-8.csv",
        "09-5.csv","09-8.csv",
        "10-5.csv","10-8.csv",
        "11-5.csv","11-8.csv",
        "12-5.csv","12-8.csv"]


#Loop para os experimentos
i = 0
while i < len(args):
    cont = 0
    while cont < 100:
        #Leitura de serial
        
        s=str(ser.readline())[2:].replace("\'","")\
              .replace("\\r","").replace("\\n","")
        
        #Formatacao dos dados
        if "---" in s:
            now = datetime.now()
            current_time = now.strftime("%H:%M:%S.%f")
            s = s.replace("---",current_time)
            
        if '+' in s:
            s=s.replace('+',"")
            line = str(ser.readline())[2:].replace("\'","")\
              .replace("\\r","").replace("\\n","")
            now = datetime.now()
            current_time = now.strftime("%H:%M:%S.%f")
            s+= line.replace("---",current_time)
        
        print(s)
        #Fim da transmissao, incremento do passo
        if "finalizada" in s:
            i += 1
        #Escrita no arquivo de saida
        elif ';' in s:
            try:
                f = open(path+arqs[i],'a')
            except:
                f = open(path+arqs[i],'w')
            f.write(s+'\n')
            f.close()
            cont += 1
              
        
#Fechamento do serial
ser.close()
