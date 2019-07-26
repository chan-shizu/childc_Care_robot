import numpy as np
from sklearn.linear_model import LinearRegression
import csv

print("start")

path1 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\過去論文遊びの好み.xlsx"
path2 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\0511camera_matome.csv"
path3 = "C:\\Users\\bubbl\\OneDrive\\Desktop\\multiple_regression_analysis.csv"

analysis_score = []
#with open(path3, "w", newline="") as f:
    #writer = csv.writer(f)
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
    model = LinearRegression()

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

print(analysis_score)
