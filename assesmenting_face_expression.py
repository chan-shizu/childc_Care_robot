import pandas as pd
import random

path1 = "C:\\Users\\kou_0\\Desktop\\csv_data\\0511camera_matome.csv"

#csvから都合のいいようにデータ変換
user_similarity = []
df_test = pd.read_csv(path1, sep=',', header=None)#csvファイルを読み込み
df_test = df_test.iloc[0, 1:7]#idを除く
df_test = df_test/sum(df_test)#正規化
df_test = df_test.values.tolist()#listに変換

rand = random.random()
if(rand < df_test[0]):
    print(1)

elif(rand < (df_test[0] + df_test[1])):
    print(2)

elif(rand < (df_test[0] + df_test[1] + df_test[2])):
    print(3)

elif(rand < (df_test[0] + df_test[1] + df_test[2] + df_test[3])):
    print(4)

elif(rand < (df_test[0] + df_test[1] + df_test[2] + df_test[3] + df_test[4])):
    print(5)

elif(rand < (df_test[0] + df_test[1] + df_test[2] + df_test[3] + df_test[4] + df_test[5])):
    print(6)








