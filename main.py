import raspy_main, csv_matomeru, assesmenting_face_expression
#import line_camera_faceAPI

while True:
    #line_camera_faceAPI.main()
    csv_matomeru.main() #csvのデータを整理する!!
    raspy_main.main(assesmenting_face_expression.main())


