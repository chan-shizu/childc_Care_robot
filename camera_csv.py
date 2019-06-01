from time import sleep
from datetime import datetime
import subprocess
import requests
import json
import csv
import line_camera_faceAPI

#Face-APIのAPIキー
face_api_url = 'https://japaneast.api.cognitive.microsoft.com/face/v1.0/detect?returnFaceId=true&returnFaceLandmarks=false&recognitionModel=recognition_01&returnRecognitionModel=false HTTP/1.1'
subscription_key = "d9d37d228bbb4daaabc51348ba0113c1"
headers = {'Content-Type': 'application/octet-stream', 'Ocp-Apim-Subscription-Key':  subscription_key }
assert subscription_key
image_url = 'https://japaneast.api.cognitive.microsoft.com/face/v1.0/detect'

time = datetime.now().strftime("%m%d")

def face_API_csv(address):
    data = open(address, 'rb')
    img = data.read()
    
    params = {
            'returnFaceId': 'true',\
            'returnFaceLandmarks': 'false',\
            'returnFaceAttributes': 'age,emotion'\
    }

    response = requests.post(image_url, params=params, headers=headers, data=img)   
    i = 0
    age_dic = {}
    emotional_dic = {}

    for i in range(len(response.json())):
        age_dic = response.json()[i]["faceAttributes"]["age"]
        emotional_dic = response.json()[i]["faceAttributes"]["emotion"]

    return age_dic,emotional_dic

def csv_hozon(asobi):
    global time
    fname = 'camera_csv.jpg'
    line_camera_faceAPI.exec("fswebcam "+fname)
    age, emotion = face_API_csv(fname)#画像からマイクロソフトfaceAPIで年齢と表情情報取得
    emotion_list = list(emotion.values())
    emotion_list.insert(0, asobi)

    with open(time + 'camera.csv','a') as f: #表情認識のデータをcsvで保存
        writer = csv.writer(f)
        writer.writerow(emotion_list)

def csv_setup():
    key = ["asobi", "anger", "disgust", "contempt", "neutral", "fear", "sadness", "happiness", "surprise"]
    global time
    with open(time +'camera.csv','a') as f: #表情認識のデータをcsvで保存
        writer = csv.writer(f)
        writer.writerow(key)

def main(asobi):
    try:
        while True:
    	#カメラで10秒ごとに撮影
            csv_hozon(asobi)
            sleep(10)

    except KeyboardInterrupt:
        pass
            
if __name__ == "__main__":
    word = "test"
    main(word)
