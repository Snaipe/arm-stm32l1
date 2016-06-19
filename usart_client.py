#!/usr/bin/env python

import serial
import time
import struct

ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=38400,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS
        )


if ser.isOpen():
    try:
        print ("Starting Up USART with STM32L")
        ser.flushInput()
        ser.flushOutput()

        response = ser.read(5)
        print (response)

        if reponse != b"stm\r\n":
            exit(1)

        while True:
            response = ser.read(4)
            print (response)
            temperature = struct.unpack('>f', response)
            response = ser.read(4)
            print (response)
            ser.read(2)
            humidity = struct.unpack('>f', response)
            print("Temperature: " + temperature)
            print("Humidity: " + humidity)

        ser.close()

    except Exception as e:
        print ("Error:" + str(e))

else:
    print ("Error: cannot open serial port.")
