import random, time
import yomikikase
import subprocess
import serial
import os
import gazou_fullscreen

#os.system('sudo chnod 777 /dev/ttyAM0')
#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)

conversation = ["ごっこ遊びを はじめるよ！", "はずかしがらないで", "僕だったら こんなふうに まねするよ", "つぎは なにの まねを しようかな"]

sound_path = ["elephant1", "dog2", "cat2", "aburazemi-cry1", "horse-dash1", "passer-montanus-cry1", "chicken-cry1", "helicopter-flight1", \
              "train-pass2", "rain1", "thunder3"]

suggestion = ["ゾウさんの まねを してみよう", "いぬの まねを してみよう", "ねこの まねを してみよう", "せみの まねを してみよう", \
            "うまの まねを してみよう", "とりの まねを してみよう", "にわとりの まねを してみよう", "ヘリコプターの まねを してみよう", \
            "電車の まねを してみよう", "あめの まねを してみよう", "かみなりの まねを してみよう"]

def main():
    #ser.write('gameD_start'.encode())
    #gazou_fullscreen.main("T:\ai\yomikikase\gakki.jpg", 10000)
    
    for gokko_num in range(3):
        i = random.randrange(10)
    
        if gokko_num ==0:
            yomikikase.jtalk(conversation[0])
        else :
            yomikikase.jtalk(conversation[3])
        
        time.sleep(2)
        yomikikase.jtalk(suggestion[i])
        time.sleep(5)
        yomikikase.jtalk(conversation[1])
        time.sleep(4)
        yomikikase.jtalk(conversation[2])
        time.sleep(1)
        a = sound_path[i]
        aplay = ['aplay','-q','/home/pi/ai/yomikikase/sound/{}.wav'.format(a)]
        wr = subprocess.Popen(aplay)
        time.sleep(5)
        wr.kill()

    #ser.write('gameD_over')

if __name__ == "__main__":
    main()






