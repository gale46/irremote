import serial

#from pyfirmata2 import Arduino

import time

ser = serial.Serial('COM6', 9600, timeout=1) 


# 等待一些時間，確保串口已經建立連接
time.sleep(2)

#向arduino重送接收指令
number = 1
ser.write(bytes(str(number), 'utf-8')) 
response = ser.readline().decode('utf-8').strip()#接收arduino的console
while response != "complete": 
    response = ser.readline().decode('utf-8').strip()
    print(response)
time.sleep(1)  # 每次迴圈間隔一秒

#向arduino重送發射指令3次
number = 2
for i in range(3): 
    ser.write(bytes(str(number), 'utf-8'))
    response = ser.readline().decode('utf-8').strip()
    print(response)
    time.sleep(1)
print("end")

# 關閉串口連接
ser.close()

    
