#main Python script

import GPUtil
import psutil
import wmi
import math
import serial
import time
from datetime import datetime

app_start_time = time.time()

arduino = serial.Serial(port='COM3', baudrate=115200, timeout=.1) #open serial port

def write_read(x): #function to send data to the arduino board
    arduino.write(bytes(x,'utf-8'))


while 1:
    current_time = datetime.now()
    now_time = time.time()

    run_time = now_time - app_start_time
    # Convert seconds to hours, minutes, and seconds
    hours, remainder = divmod(run_time, 3600)
    minutes, seconds = divmod(remainder, 60)

    running = f"{int(hours):02}:{int(minutes):02}:{int(seconds):02}"

    print(f"Runtime: {running}")

    w = wmi.WMI(namespace="root\\OpenHardwareMonitor") #open Hardware Monitor
    temperature_infos = w.Sensor()
    value=0

    for sensor in temperature_infos: #Search for the CPU temperature. Due to lack of OpenHardwareMonitor limitation with 11th Intel Family I only get the core 0 temp.
    #It is posible to do the mean of all your processor temps. 
        if sensor.SensorType==u'Temperature' and sensor.Parent==u'/lpc/nct6798d' and sensor.Name==u'Temperature #1':
            value=sensor.Value

    gpu = GPUtil.getGPUs()[0]
    
    A = f"{round(value,2):04}"
    B = f"{round(psutil.cpu_percent())}"
    C = f"{round(gpu.temperature,2):04}"
    D = f"{round(gpu.load*100)}"
    E = f"{current_time.strftime("%H:%M:%S")}"
    finalString = A + "," + B + "," + C + "," + D + "," + running + "," + E + "\n" #Create the String to send to arduino
    print(finalString)
    time.sleep(2)
    write_read(finalString) #calling the send function
