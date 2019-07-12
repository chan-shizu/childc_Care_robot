#import gokko, riddle, yomikikase, line_camera, arduino, camera_csv
#import serial
import time, os, random

#os.system('sudo chnod 777 /dev/ttyAM0')
#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)
#flag = 'none'

def main(number):
    #camera_csv.csv_setup()
    #flag = 'none'
    #while (flag != 'set_up'):
        #flag = ser.readline()
        #time.sleep(3)

    if (number == 0):
        #arduino.gameA  
        print("遊びは0")

    elif (number == 1):
        #arduino.gameB 
        print("遊びは1")

    elif (number == 2):
        #arduino.gameC 
        print("遊びは2")

    elif (number == 3):
        #arduino.gameD() #gokko 
        print("遊びは3")

    elif (number == 4):
        #arduino.gameE() #riddle
        print("遊びは4")

    elif (number == 5):
        #arduino.gameF() #yomikikase  
        print("遊びは5")

if __name__ == "__main__":
    main()