import gokko, riddle, yomikikase, line_camera, arduino, camera_csv
import serial
import time, os, random

os.system('sudo chnod 777 /dev/ttyAM0')
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)
flag = 'none'

def main():
    camera_csv.csv_setup()
    flag = 'none'
    while (flag != 'set_up'):
        flag = ser.readline()
        time.sleep(3)

h = random.randint(0, 6)
if (h == 0):
    gokko.main()

elif (h == 1):
    riddle.main()

elif (h == 2):
    yomikikase.main()

elif (h == 3):
    arduino.gameA()  

elif (h == 4):
    arduino.gameB()

elif (h == 5):
    arduino.gameC()   

if __name__ == "__main__":
    main()
