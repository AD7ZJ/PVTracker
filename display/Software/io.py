import serial
import re
import math

ser = serial.Serial('/dev/ttyUSB3')  # open serial port

while True:
	try:
		line = ser.readline().decode("utf-8")
		print(line)
		result = re.search('^X:(-?\d+)\s+Y:(-?\d+)\s+Z:(-?\d+)\s+S:(\d+)\s*$', line) #\s+Y:(d+)\s+Z:(d+)\s+S:(d+)', line)
		x = float(result.group(1)) / 255.5
		y = float(result.group(2)) / 255.5
		z = float(result.group(3)) / 255.5
		status = bool(result.group(4))

		sign = 1
		if (z < 0):
			sign = -1;
		theta = (math.atan2(-x, math.sqrt((y*y)+(z*z))) * 180) / 3.14159f;
		psi = (math.atan2(y, sign*math.sqrt((z*z)+0.1f*(x*x))) * 180) / 3.14159f;
		print("%f %f %f %s\r\n" % (x, y, z, status))
	except:
		raise
		#pass