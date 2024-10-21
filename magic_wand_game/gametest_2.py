import serial  
import time  
import threading  

health1 = 8  
health2 = 8  
blueBar1 = 2  
blueBar2 = 2  

flag1 = False
flag2 = True

action1 = -1
action2 = 3

port1 = 'COM8'  # Nano 33 BLE 1  
port2 = 'COM4'  # Nano 33 BLE 2  

# 初始化串口  
ser1 = serial.Serial(port1, 115200, timeout=0.01)  
ser2 = serial.Serial(port2, 115200, timeout=1)  

# 等待串口连接  
time.sleep(2)  

def read_from_nano(ser, id):  
    action = -1
    global action1,action2  
    global flag1,flag2
    while True:  
        if ser.in_waiting > 0:  
            data = ser.readline().decode('utf-8').strip() 
            print(f"From Nano{id},Received Data: {data}")
            values = data.strip().split(',')
            if len(values) == 4:
                value1, value2,value3,value4 = map(float, values)
                print(f"From Nano{id},Parsed Data: Value1={value1}, Value2={value2},Value3={value3},Value4={value4}")
                if value1 > 0.9:
                    action = 1
                elif value2 > 0.9:
                    action = -1
                elif value3 > 0.9:
                    action = 2
                elif value4 > 0.9:
                    action = 3
            if action >=0:
                print(f"From Nano{id},Action: {action}")
                if id == 1:
                    action1 = action
                    flag1 = True
                elif id == 2:
                    action2 = action
                    flag2 = True
                action = -1

def game_loop():
    global health1,health2,blueBar1,blueBar2
    global action1,action2
    global flag1,flag2
    while True:
        if flag1 and flag2:
            if action1 == 1:
                match action2:
                    case 1:
                        if blueBar1>blueBar2:
                            health2 -= blueBar1-blueBar2
                        elif blueBar2>=blueBar1:
                            health1 -= blueBar2-blueBar1
                        blueBar1 = 0
                        blueBar2 = 0
                    case 2:
                        if blueBar1>2*blueBar2:
                            health2 -= blueBar1-2*blueBar2
                        elif blueBar1<=2*blueBar2:
                            health1 -= blueBar2
                        blueBar1 = 0
                        blueBar2 = round(blueBar2/2)
                    case 3:
                        health2 -= blueBar1
                        blueBar1 = 0
                        blueBar2 += 3
            elif action1 == 2:
                match action2:
                    case 1:
                        if blueBar2>2*blueBar1:
                            health1 -= blueBar2-2*blueBar1
                        elif blueBar2<=2*blueBar1:
                            health2 -= blueBar1
                        blueBar2 = 0
                        blueBar1 = round(blueBar1/2)
                    case 2:
                        blueBar1 = round(blueBar1/2)
                        blueBar2 = round(blueBar2/2)
                    case 3:
                        blueBar1 = round(blueBar1/2)
                        blueBar2 += 3
            elif action1 == 3:
                match action2:
                    case 1:
                        health1 -= blueBar2
                        blueBar2 = 0
                        blueBar1 += 3
                    case 2:
                        blueBar2 = round(blueBar2/2)
                        blueBar1 += 3
                    case 3:
                        blueBar1 += 3
                        blueBar2 += 3
            flag1 = False
            flag2 = False
            action1 = -1
            action2 = -1
            
            blueBar1 += 1
            blueBar2 += 1
            if blueBar1 > 8:
                blueBar1 = 8
            if blueBar2 > 8:
                blueBar2 =8
            print(f"Health1: {health1}, Health2: {health2}, BlueBar1: {blueBar1}, BlueBar2: {blueBar2}")
            if health1 <= 0 or health2 <= 0:
                print("Game Over!")
                break
            ser1.write(str.encode(f"{health1},{blueBar1}\n"))
            ser2.write(str.encode(f"{health2},{blueBar2}\n"))
            
        time.sleep(0.3)



if __name__ == "__main__":  
    # 启动读取线程  
    threading.Thread(target=read_from_nano, args=(ser1, 1)).start()  
    threading.Thread(target=read_from_nano, args=(ser2, 2)).start() 


    while True:
        ser1.write("WAIT\n".encode('utf-8'))
        #ser2.write("WAIT\n".encode('utf-8'))
        state=input("Enter 1 to start the game: ")
        if state == '1':
            print("开始游戏!")
            ser1.write("START\n".encode('utf-8'))
            ser2.write("START\n".encode('utf-8'))
            game_loop()
            state = '0'
            
            if health1 <= 0 and health2 <= 0:
                print("平局!")
            elif health1 <= 0:
                print("玩家2获胜!")
                ser1.write("NO\n".encode('utf-8'))
                ser2.write("YES\n".encode('utf-8'))
            elif health2 <= 0:
                print("玩家1获胜!")
                ser1.write("YES\n".encode('utf-8'))
                ser2.write("NO\n".encode('utf-8'))
            else:
                print("未知错误!")
                ser1.write("WAIT\n".encode('utf-8'))
                ser2.write("NO\n".encode('utf-8'))
                break
            #ser1.write("WAIT\n".encode('utf-8'))
            #ser1.close()
            #ser2.write("WAIT\n".encode('utf-8'))
            break