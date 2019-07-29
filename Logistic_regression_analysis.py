import numpy as np
from sklearn.linear_model import LogisticRegression
from sklearn.externals import joblib
import csv
import matplotlib.pyplot as plt
import line_camera_faceAPI

print("start")

# for note pc(dynabook)
'''
path1 =   
path2 = 
path3 = 
'''
# for desktop pc(university)
path1 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\facial_expression_data\\過去論文遊びの好み.csv"    
path2 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\facial_expression_data\\0511camera_matome.csv"
path3 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\facial_expression_data\\logistic_regression_analysis.csv"
#path4 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\facial_expression_data\\clf_data.pkl"
path5 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\facial_expression_data\\logistic_analysis_result.csv"

def main():
    analysis_score = []
    with open(path3, "w", newline="") as f:
        writer = csv.writer(f)
        for i in [7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]:
            training_personality = np.loadtxt(path1,\
                            delimiter=",",\
                            skiprows=1,\
                            max_rows=12,\
                            usecols=(i),\
                            encoding="utf-8"\
                            )

            training_preference = np.loadtxt(path1,\
                            delimiter=",",\
                            skiprows=1,\
                            max_rows=12,\
                            usecols=(1, 2, 3, 4, 5),\
                            encoding="utf-8"\
                            )

            test_preference = np.empty((1, 5))
            test_preference[0] = np.loadtxt(path2,
                            delimiter=",",\
                            skiprows=0,\
                            max_rows=1,\
                            usecols=(1, 2, 3, 4, 5),\
                            encoding="utf-8"\
                            )
            np.reshape(test_preference, (1, 5))

            #重回帰分析モデルの生成
            model = LogisticRegression()

            #モデルの学習(最適なパラメータを求める)
            model.fit(training_preference, training_personality)
                #LinearRegression(copy_X=True, fit_intercept=True, n_jobs=None,normalize=False)

            #調整されたパラメータを見る
            print(i)
            print("model.coef_")
            print(model.coef_)
            print("model.intercept_")
            print(model.intercept_)
            print("score(0.5以上が目安)")
            print(model.score(training_preference, training_personality))

            #予測
            print(model.predict(test_preference))
            print(model.score(training_preference, training_personality))
            #np.savetxt(path3,model.score(training_preference, training_personality))
            analysis_score.append(model.score(training_preference, training_personality))
        
            path4 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\facial_expression_data\\logistic_clf_data{}.pkl".format(i)
            joblib.dump(model, path4) #学習データを保存

        writer.writerow(analysis_score)


    print(analysis_score)

def fit_analysis():
    analysis_score = []
    personality_number = []
    for i in [7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]:
        df = np.loadtxt(path2,\
                        delimiter=",",\
                        skiprows=0,\
                        max_rows=1,\
                        usecols=(1, 2, 3, 4, 5),\
                        encoding="utf-8"\
                        )
        print(df)
        df = df.reshape(1,-1)
        print(df)
        path4 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\facial_expression_data\\logistic_clf_data{}.pkl".format(i)
        model = joblib.load(path4) #過去の学習データ(遊びの好みと性格の関係)をロードする
        predict = model.predict(df) #学習データから数値を予測する
        print(predict)
        analysis_score.append(predict[0])
        name = "p{}".format(i-6)
        personality_number.append(name)

    with open(path5, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(personality_number)
        writer.writerow(analysis_score)
    
    plt.bar(personality_number, analysis_score, tick_label=personality_number, align="center") #横軸, 縦軸の配列を指定, 
    plt.title("child's personality")
    plt.xlabel("p") #x軸のラベル
    plt.ylabel("figures") #y軸のラベル
    plt.grid(True) #格子表示
    #plt.show() #画像表示
    plt.savefig("C:\\Users\\bubbl\\OneDrive\\Desktop\\logistic_chart.jpg") #画像保存
    line_camera_faceAPI.line_photo_picture("C:\\Users\\bubbl\\OneDrive\\Desktop\\logistic_chart.jpg") #ラインにグラフの画像を送信

if __name__ == "__main__":
    main()
    fit_analysis()
