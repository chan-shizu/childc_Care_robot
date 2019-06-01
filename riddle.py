import random, time
import yomikikase
import serial
import os
os.system('sudo chnod 777 /dev/ttyAM0')
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)

conversation = ["なぞなぞを はじめるよ！", "ようく 考えてね", "こたえを いうよ。こたえは", "つぎの もんだいを いうよ"]

question = ["つめたい いす ってなあに?", "みんな すきに なってしまう スポーツって なあに?", "ねずみが かよっている がっこうって なあに?", \
            "おさらから ころっところがっちゃいそうな おかずはなに?", "てぶくろが かくしている いろはなに?" ,"ぼうしのなかに かくれている どうぶつってなに?", \
            "そらからふってくる あまいおかしってなに?", "あかいかおした ふたごのくだものってなに?" ,"「き」は「き」でも ごみをたべてくれる きってなに?"]

answer = ["あいす だよ", "すきー だよ", "ちゅうがっこう だよ", "ころっけ だよ", "くろ だよ", "うし だよ", "あめ だよ", "さくらんぼ だよ", "そうじき だよ"]

def main():
    ser.write('gameE_start'.encode())
    
    for riddle_num in range(3):
        i = random.randrange(10)
    
        if riddle_num ==0:
            yomikikase.jtalk(conversation[0])
        else :
            yomikikase.jtalk(conversation[3])
        
        time.sleep(2)
        yomikikase.jtalk(question[i])
        time.sleep(5)
        yomikikase.jtalk(conversation[1])
        time.sleep(4)
        yomikikase.jtalk(conversation[2])
        time.sleep(1)
        yomikikase.jtalk(answer[i])

    ser.write('gameE_over')

if __name__ == "__main__":
    main()




