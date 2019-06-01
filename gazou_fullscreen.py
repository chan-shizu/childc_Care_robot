import cv2
import base64

def main(img, time):
    winname = 'screen'
    img = gazou_henkan(img)
    cv2.namedWindow(winname, cv2.WINDOW_NORMAL)
    cv2.setWindowProperty(winname, cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
    cv2.imshow(winname, img)
    cv2.waitKey(time)

def gazou_henkan(img):
    img = cv2.imread(img)
    # raw image -> jpg
    ret, jpg = cv2.imencode('.jpg', img)

    with open('test.txt', 'wb') as f:
    # jpg -> base64
        encoded = base64.b64encode(jpg.tostring())
        f.write(encoded)

    return img

if __name__ == "__main__":
    img = 'C:\\Users\\bubbl\\OneDrive\\Desktop\\gakki.jpg'
    time = 10000
    main(img, time)
