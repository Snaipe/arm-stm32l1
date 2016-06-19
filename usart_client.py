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

        response = ser.read(6)

        if response[1:] != b"stm\r\n":
            exit(1)

        while True:
            response = ser.read(4)
            temperature = struct.unpack('<f', response)[0]
            response = ser.read(4)
            ser.read(2)
            humidity = struct.unpack('<f', response)[0]
            print("Temperature:", temperature)
            print("Humidity:", humidity)

        ser.close()
    except KeyboardInterrupt:
        exit()
else:
    print ("Error: cannot open serial port.")
