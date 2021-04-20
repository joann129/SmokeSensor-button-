from time import sleep
import time
import serial

def formatStrToInt(target):
    kit = ""
    for i in range(len(target)):
        temp=ord(target[i])
        temp=hex(temp)[2:]
        kit=kit+str(temp)+" "
        #print(temp,)
    return kit

device_id = "24790262077"

connect_pack = "10 3A 00 04 4D 51 54 54 04 C2 00 3C 00 06 41 42 43 44 45 46 00 12 50 4b 53 54 31 43 54 5a 34 31 50 57 46 37 43 47 57 31 00 12 50 4b 53 54 31 43 54 5a 34 31 50 57 46 37 43 47 57 31"

prifix = "/v1/device/" + device_id + "/rawdata"
#publish_pack = "[{\"id\":\"sensor01\",\"value\":[\"0.0\"]}]"



ser = serial.Serial("/dev/ttyS0", 9600, timeout=0.5)
serbee = serial.Serial("/dev/ttyUSB0", 9600)



ser.write("AT\r\n".encode())
sleep(1)
data = ser.readline()
print(data)
data = ser.readline()
print(data)

def send(publish_pack):
    payload_len = len(prifix+publish_pack)
    payload_len = payload_len + 2

    if(payload_len<128):
        payload_len_hex = hex(payload_len).split('x')[-1]
    else:
        a = payload_len % 128
        b = payload_len // 128
        a = hex(a+128).split('x')[-1]
        b = hex(b).split('x')[-1]
        b = b.zfill(2)
        payload_len_hex = str(a) + " " +  str(b)
        #print(payload_len_hex)

    a = formatStrToInt(prifix+publish_pack)

    add_on = "30 " + str(payload_len_hex.upper()) +" 00 1E "

    end_line = "1A"

    message_package = add_on + a + end_line
    print(connect_pack)
    print("==========================")
    print(message_package.upper())
            
    ser.write("AT+CIPCLOSE=1\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)

    ser.write("AT+CIPSHUT\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)
            
    ser.write("AT+CIPSENDHEX=1\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)

    ser.write("AT+CSTT=\"internet.iot\"\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)

    ser.write("AT+CIICR\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)
                       
    ser.write("AT+CIFSR\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)

    ser.write("AT+CIPSTART=\"TCP\",\"iot.cht.com.tw\",1883\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)
            
    ser.write("AT+CIPSEND\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)

    ser.write(connect_pack.encode())
    #ser.write("\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)

    ser.write(message_package.upper().encode())
    #ser.write("\r\n".encode())
    sleep(1)
    data = ser.readline()
    print(data)
    data = ser.readline()
    print(data)

start_time = time.time()
flag = 0.0
try:
    while True:
        while serbee.in_waiting:#wait data
            data_raw = serbee.readline() #readdata
            start_time = time.time() # 重新計算時間
            print(data_raw)
            sensordata = data_raw.decode()
            sensordata = sensordata.strip('\n')
            sensordata = sensordata.strip('\r')
            publish_pack = sensordata
            print(publish_pack)
            # find = publish_pack.find(',')
            # id = publish_pack[8:find-1]
            value = publish_pack[-7:-4] # 尋找value的值 
            # if id == 'sensor01': # https://www.itread01.com/content/1543896186.html 尋找逗點位置，再去判斷id跟value，目前一對一，先不處理..
            if float(value) != flag:
                flag = float(value)
                send(publish_pack) # 當得到的value與之前的狀態不一樣的時候就要send資料到大平台
        

        end_time = time.time()
        if (end_time - start_time > 20):
            publish_pack = "[{\"id\":\"sensor01\",\"value\":[\"-1.0\"]}]"
            send(publish_pack)

            
except KeyboardInterrupt:
    
    
    ser.close()
    serbee.close()
    


