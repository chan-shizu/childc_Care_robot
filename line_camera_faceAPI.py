from time import sleep
from datetime import datetime
import subprocess
import requests
import json

#LINENotifyのAPIキー
#TOKEN = 'ERssGmSvLcMC14PRenIpEJ0SrriPtQi42Nx2GvykwjQ' #練習用グループ「しずやの実験」?
TOKEN = 'Fx0612QKWOHUnKSTo8eX5coViqzhU1s07Ko31Dj7oSY' #練習用俺のライン
API = 'https://notify-api.line.me/api/notify'

#Face-APIのAPIキー
face_api_url = 'https://japaneast.api.cognitive.microsoft.com/face/v1.0/detect?returnFaceId=true&returnFaceLandmarks=false&recognitionModel=recognition_01&returnRecognitionModel=false HTTP/1.1'
subscription_key = "d9d37d228bbb4daaabc51348ba0113c1"
headers = {'Content-Type': 'application/octet-stream', 'Ocp-Apim-Subscription-Key':  subscription_key }
assert subscription_key
image_url = 'https://japaneast.api.cognitive.microsoft.com/face/v1.0/detect'

#コマンドの実行
def exec(cmd):
    r = subprocess.check_output(cmd, shell = True)
    return r.decode("UTF8").strip()

def face_API(address):
    data = open(address, 'rb')
    img = data.read()
    
    params = {
            'returnFaceId': 'true',\
            'returnFaceLandmarks': 'false',\
            'returnFaceAttributes': 'age,emotion'\
    }

    response = requests.post(image_url, params=params, headers=headers, data=img)   
    i = 0
    age_list = []
    emotional_list = []

    for i in range(len(response.json())):
        age_list.append(json.dumps(response.json()[i]["faceAttributes"]["age"]))
        emotional_list.append(json.dumps(response.json()[i]["faceAttributes"]["emotion"]))

    return age_list,emotional_list

#写真の撮影コマンドを実行(ファイル名を日時に)
last_post = datetime(2000, 1, 1)#適当に初期化

def line_photo():
    global last_post
    #写真を撮影
    #now = datetime.now()
    #fname = now.strftime('%Y-%m-%d_%H-%M-%S') + ".jpg"
    fname = 'line_camera.jpg'
    exec("fswebcam "+fname)
    age, emotion = face_API(fname)
    age_mojiretu = ','.join(age)
    emotion_mojiretu = ','.join(emotion)	
    
	#通知をLINEに挿入
    post_data = {'message': '侵入者アリ \n'+ '彼らの年齢は{}\n'.format(age_mojiretu) + '彼らの感情は{}'.format(emotion_mojiretu)}
    headers = {'Authorization': 'Bearer ' + TOKEN}
    files = {'imageFile':open(fname, 'rb')}
    res = requests.post(API, data = post_data, headers = headers, files = files)
    print(res.text)

def main():
	try:
		while True:
    	#カメラで10秒ごとに撮影
			line_photo()
			sleep(10)

	except KeyboardInterrupt:
			pass
            
if __name__ == "__main__":
    main()
