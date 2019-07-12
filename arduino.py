import serial
import time
import os
import subprocess

os.system('sudo chnod 777 /dev/ttyAM0')
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)
 
#gameA(色遊び)をarduinoに実行させる
def gameA():
    ser.write('gameA_start'.encode())
    flag = 'none'
    cmd = "python3 camera_csv2.py gameA"
    p = subprocess.Popen(cmd.split())

    while(flag != 'gameA_over'.encode()):
        flag = ser.readline()
        time.sleep(2)
    
    ser.close()
    p.kill()
    return

def gameB():
    ser.write('gameB_start'.encode())
    flag = 'none'
    cmd = "python3 camera_csv2.py gameB"
    p = subprocess.Popen(cmd.split())

    while(flag != 'gameB_over'.encode()):
        flag = ser.readline()
        time.sleep(2)

    ser.close()
    p.kill()
    return

def gameC():
    ser.write('gameC_start'.encode())
    flag = 'none'
    cmd = "python3 camera_csv2.py gameC"
    p = subprocess.Popen(cmd.split())

    while(flag != 'gameC_over'.encode()):
        flag = ser.readline()
        time.sleep(2)
    
    ser.close()
    p.kill()
    return

def gameD():
    ser.write('gameD_start'.encode())
    flag = 'none'
    cmd = "python3 camera_csv2.py gameD"
    p = subprocess.Popen(cmd.split())

    while(flag != 'gameD_over'.encode()):
        flag = ser.readline()
        time.sleep(2)
    
    ser.close()
    p.kill()
    return

def gameE():
    ser.write('gameE_start'.encode())
    flag = 'none'
    cmd = "python3 camera_csv2.py gameE"
    p = subprocess.Popen(cmd.split())

    while(flag != 'gameE_over'.encode()):
        flag = ser.readline()
        time.sleep(2)

    ser.close()
    p.kill()
    return

def gameF():
    ser.write('gameF_start'.encode())
    flag = 'none'
    cmd = "python3 camera_csv2.py gameF"
    p = subprocess.Popen(cmd.split())

    while(flag != 'gameF_over'.encode()):
        flag = ser.readline()
        time.sleep(2)
    
    ser.close()
    p.kill()
    return