import cv2
import mediapipe as mp
import serial
import time


def turn_on():
    number = 2
    ser.write(bytes(str(number), 'utf-8'))
    response = ser.readline().decode('utf-8').strip()
    while response != "complete":  
        response = ser.readline().decode('utf-8').strip()
        print(response)
    time.sleep(1)
def receive():
    number = 1
    ser.write(bytes(str(number), 'utf-8')) 
    response = ser.readline().decode('utf-8').strip()
    
    while response != "complete": 
        response = ser.readline().decode('utf-8').strip()
        print(response)
    time.sleep(1)  # 每次迴圈間隔一秒

ser = serial.Serial('COM6', 9600, timeout=1) 
# 等待一些時間，確保串口已經建立連接
time.sleep(2)


# 初始化 MediaPipe Hands 模型
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=1, min_detection_confidence=0.5, min_tracking_confidence=0.5)

# 设置手掌节点索引
PALM_INDEX = mp_hands.HandLandmark.WRIST

# 打开视频流
cap = cv2.VideoCapture(0)

count = 0

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # 转换为 RGB 格式
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # 检测手部姿势
    results = hands.process(frame_rgb)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            palm = hand_landmarks.landmark[PALM_INDEX]

            # 获取手掌中心点的 Y 坐标
            palm_y = palm.y * frame.shape[0]

            # 如果手掌位置处于屏幕底部，关闭摄像头
            if palm_y > frame.shape[0] * 0.9: # 手掌在屏幕底部附近
                turn_on()
            
    # 显示视频流
    cv2.imshow('Hand Gesture Camera Control', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 释放资源
cap.release()
cv2.destroyAllWindows()
ser.close()
