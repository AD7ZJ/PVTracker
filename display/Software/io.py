import serial
import re
import math
import time
import pickle
import threading

class Calibration:
    def __init__(self):
        self.maxX = 0
        self.maxY = 0
        self.maxZ = 0
        self.minX = 0
        self.minY = 0
        self.minZ = 0

class Sensor:
    def __init__(self, serialPort=""):
        """Constructor"""
        self.serialPort = serialPort
        self.port = serial.Serial(self.serialPort)  # open serial port
        self.keepGoing = False
        self.calibrate = False
        self.maxX = 0
        self.maxY = 0
        self.maxZ = 0
        self.minX = 0
        self.minY = 0
        self.minZ = 0
        self.cal = Calibration()
        self.LoadCalibration()

    def SaveCalibration(self):
        self.cal.maxX = self.maxX
        self.cal.maxY = self.maxY
        self.cal.maxZ = self.maxZ
        self.cal.minX = self.minX
        self.cal.minY = self.minY
        self.cal.minZ = self.minZ
        print("X: %f, %f" % (self.cal.minX, self.cal.maxX))
        print("Y: %f, %f" % (self.cal.minY, self.cal.maxY))
        print("Z: %f, %f" % (self.cal.minZ, self.cal.maxZ))
        self.cal.minZ
        f = open('cal.txt', 'wb')
        pickle.dump(self.cal, f)
        f.close()

    def LoadCalibration(self):
        f = open('cal.txt', 'rb')
        self.cal = pickle.load(f)
        f.close()

    def Start(self):
        """Start the thread that will read the tilt sensor"""
        self.keepGoing = True
        self.thread = threading.Thread(target=self.MonitorThread)
        #self.thread.daemon = True
        self.thread.start()

    def Stop(self):
        self.keepGoing = False
        self.thread.join()

    def LinearMap(self, x, inMin, inMax, outMin, outMax):
        return (float(x) - float(inMin)) * (float(outMax) - float(outMin)) / (float(inMax) - float(inMin)) + float(outMin);

    def MonitorThread(self):
        while (self.keepGoing):
            try:
                line = self.port.readline().decode("utf-8")
                #line = "X:6 Y:5 Z:249 S:1"
                #time.sleep(1)
                result = re.search('^X:(-?\d+)\s+Y:(-?\d+)\s+Z:(-?\d+)\s+S:(\d+)\s*$', line) #\s+Y:(d+)\s+Z:(d+)\s+S:(d+)', line)
                xRaw = int(result.group(1))
                yRaw = int(result.group(2))
                zRaw = int(result.group(3))
                status = bool(result.group(4))

                if (xRaw > self.maxX):
                    self.maxX = xRaw
                if (yRaw > self.maxY):
                    self.maxY = yRaw
                if (zRaw > self.maxZ):
                    self.maxZ = zRaw
                if (xRaw < self.minX):
                    self.minX = xRaw
                if (yRaw < self.minY):
                    self.minY = yRaw
                if (zRaw < self.minZ):
                    self.minZ = zRaw

                # apply calibration to normalize values to 1g
                x = self.LinearMap(xRaw, self.cal.minX, self.cal.maxX, -1, 1)
                y = self.LinearMap(yRaw, self.cal.minY, self.cal.maxY, -1, 1)
                z = self.LinearMap(zRaw, self.cal.minZ, self.cal.maxZ, -1, 1)

                #print("%f %f %f %s   %f %f\r\n" % (x, y, z, status, self.cal.minX, self.cal.maxX))

                sign = 1
                if (z < 0):
                    sign = -1;
                theta = (math.atan2(-x, math.sqrt((y*y)+(z*z))) * 180) / 3.14159;
                psi = (math.atan2(y, sign*math.sqrt((z*z)+0.1*(x*x))) * 180) / 3.14159;
                print("theta %f psi %f\r\n" % (theta, psi))
            except:
                #raise
                pass


if __name__ == '__main__':
    sensor = Sensor(serialPort="/dev/ttyUSB3")
    sensor.Start()

    while True:
        inChar = input()
        if (inChar == 'c'):
            sensor.SaveCalibration()
        elif (inChar == 'q'):
            break

    sensor.Stop()

