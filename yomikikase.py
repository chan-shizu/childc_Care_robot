#!/usr/bin/python
 
#coding: utf-8
import subprocess
from datetime import datetime
import random
import serial
import os
#import threading
import camera_csv
#os.system('sudo chnod 777 /dev/ttyAM0')
#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)

stories  = ['kaguyahime2.txt', 'momotarou.txt'] #'turu_no_ongaesi.txt'


def jtalk(t):
    fout = open("/home/pi/ai/yomikikase/temp_text.txt", "w")
    fout.write(t)
    fout.close()
    open_jtalk=['open_jtalk']
    mech=['-x','/var/lib/mecab/dic/open-jtalk/naist-jdic']
    htsvoice=['-m','/home/pi/ai/mei/mei_happy.htsvoice']
    speed=['-r','1.0']
    outwav=['-ow','/home/pi/ai/yomikikase/temp_voice.wav']
    cmd=open_jtalk+mech+htsvoice+speed+outwav
    fin = open("/home/pi/ai/yomikikase/temp_text.txt", "r")
    d = fin.read()
    c = subprocess.Popen(cmd,stdin=subprocess.PIPE)
    c.stdin.write(d.encode('utf-8'))
    c.stdin.close()
    c.wait()
    #subprocess.call(cmd)
    aplay = ['aplay','-q','/home/pi/ai/yomikikase/temp_voice.wav']
    wr = subprocess.Popen(aplay)
    wr.wait()

def main():
    #thread_1 = threading.Thread(target=camera_csv.main('a'))
    #thread_1.start()
    #ser.write('gameF_start'.encode())
    i = random.randint(0,1)
    a = stories[i]
    asobi = "yomikikase"
    for line in open("/home/pi/ai/yomikikase/fairy-tales/{}".format(a),'r'):
        print(line)
        jtalk(line)

    camera_csv.main(asobi)

    #ser.write('gameF_over'.encode())

if __name__ == "__main__":
    main()
