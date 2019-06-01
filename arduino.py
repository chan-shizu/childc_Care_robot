import serial
import time
import os
os.system('sudo chnod 777 /dev/ttyAM0')
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)
 
#gameA(色遊び)をarduinoに実行させる
def gameA():
    ser.write('gameA_start'.encode())
    flag = 'none'

    while(flag != 'gameA_over'.encode()):
        flag = ser.readline()
        time.sleep(2)
    
    ser.close()
    return

def gameB():
    ser.write('gameB_start'.encode())
    flag = 'none'

    while(flag != 'gameB_over'.encode()):
        flag = ser.readline()
        time.sleep(2)

    ser.close()
    return

def gameC():
    ser.write('gameC_start'.encode())
    flag = 'none'

    while(flag != 'gameC_over'.encode()):
        flag = ser.readline()
        time.sleep(2)
    
    ser.close()
    return
   