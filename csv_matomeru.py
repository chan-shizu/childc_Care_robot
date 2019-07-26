import pandas as pd
import sys, csv
import numpy as np

path1 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\0511camera.csv"
path2 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\0511camera_matome.csv"
key = ["anger", "disgust", "contempt", "neutral", "fear", "sadness", "happiness", "surprise"]
syurui = ["riddle", "gokko", "yomikikase", "iro", "oto", "dance"]


df = pd.read_csv(path1) #csvデータを読み込み
df = df.dropna(how="any") #nullのデータを削除

with open(path2, "w", newline="") as f:
    writer = csv.writer(f)
    total_score = ["test"]#id名, total_score[0]に代入
    for d in syurui:
        df_exact = df[df["asobi"] == d]#元のcsvから遊びの特定の遊びの行だけ抽出
        print(d)
        feelings = [d]#feelings[0]に遊びの名前を代入

        for feeling in key: 
            d = "df_exact.{}.values.tolist()".format(feeling)#感情ごとにlistに変換
            data_list = eval(d)

            if len(data_list) != 0:
                average = round((sum(data_list) / len(data_list)), 5)

            else:
                average = 0

            feelings.append(average)
            print(feeling + str(average)) #デバッグ用

        total_score.append(-feelings[1] - feelings[2]  - feelings[3] - feelings[5] - feelings[6] + feelings[7] + feelings[8])
        print(total_score) #デバッグ用

    print(total_score) #デバッグ用
    writer.writerow(total_score)

    
    