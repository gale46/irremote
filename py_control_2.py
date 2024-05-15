import cv2
import mediapipe as mp
import serial
import time
                
                    
'''
https://mudgalvaibhav.medium.com/real-time-gesture-recognition-using-googles-mediapipe-hands-add-your-own-gestures-tutorial-1-dd7f14169c19
'''
irr = [0,1,2,3,4,5]
def receive():
    ser.write(bytes(str(irr[0]), 'utf-8')) 
    response = ser.readline().decode('utf-8').strip()
    while response != "complete": 
        response = ser.readline().decode('utf-8').strip()
        print(response)
    
def turn_on_off():
    ser.write(bytes(str(irr[1]), 'utf-8'))
    print("開關")
    response = ser.readline().decode('utf-8').strip()
    while response != "complete": 
        response = ser.readline().decode('utf-8').strip()
        print(response)
def increase_speed():
    ser.write(bytes(str(irr[2]), 'utf-8'))
    print("增加")
    response = ser.readline().decode('utf-8').strip()
    while response != "complete": 
        response = ser.readline().decode('utf-8').strip()
        print(response)
def decrease_speed():
    ser.write(bytes(str(irr[3]), 'utf-8'))
    print("減少")
    response = ser.readline().decode('utf-8').strip()
    while response != "complete": 
        response = ser.readline().decode('utf-8').strip()
        print(response)

def shake():
    ser.write(bytes(str(irr[4]), 'utf-8'))
    print("擺頭")
    response = ser.readline().decode('utf-8').strip()
    while response != "complete": 
        response = ser.readline().decode('utf-8').strip()
        print(response)
def clear():
    ser.write(bytes(str(irr[5]), 'utf-8'))
    print("清除")
try:
    ser = serial.Serial('COM6', 9600, timeout=1) 
    time.sleep(2)
    mp_hands = mp.solutions.hands
    hands = mp_hands.Hands(static_image_mode=False, max_num_hands=1, min_detection_confidence=0.5, min_tracking_confidence=0.5)
    PALM_INDEX = mp_hands.HandLandmark.WRIST
    cap = cv2.VideoCapture(0)

    count = 0

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = hands.process(frame_rgb)
        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                palm_bottom = hand_landmarks.landmark[0]  
                palm_center = hand_landmarks.landmark[9] 
                '''
                landmark 
                  x: 
                  y:
                  z:
                '''
                x0 = palm_bottom.x
                y0 = palm_bottom.y
                        
                x9 = palm_center.x
                y9 = palm_center.y
                
                if abs(x9 - x0) < 0.05:      #since tan(0) --> ∞
                    m = 1000000000
                else:
                    m = abs((y9 - y0)/(x9 - x0))       
                    
                if m>=0 and m<=1:
                    if x9 > x0:
                        increase_speed()
                    else:
                        decrease_speed()
                if m>1:
                    if y9 < y0:       #since, y decreases upwards
                        turn_on_off()
                    else:
                        receive()
                

        cv2.imshow('Hand Gesture Camera Control', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
except Exception as e:
    print("An error occurred:", e)
finally:

    if 'ser' in locals():
        ser.close()
    if 'cap' in locals():
        cap.release()
    cv2.destroyAllWindows()

