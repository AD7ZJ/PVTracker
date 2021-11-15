import math
import datetime
import pysolar
import time
import threading

class Tracker:
    def __init__(self, lattitude, longitude, iface):
        """Constructor"""
        self.lat = lattitude
        self.lon = longitude
        self.keepGoing = False
        self.pumpRunning = False
        self.manualMode = False
        self.iface = iface
        self.iface.btnIncreaseCb = self.BtnIncreasePressed
        self.iface.btnDecreaseCb = self.BtnDecreasePressed
        self.iface.btnMenuCb = self.BtnMenuPressed
        self.iface.btnTrackCb = self.BtnTrackPressed

    def MoveArray(self, dirWest, enable):
        if (enable and not(self.pumpRunning)):
            self.iface.Pump(True)
            self.pumpRunning = True

        if (dirWest):
            self.iface.SolenoidWest(enable)
            self.iface.SolenoidEast(False)
        else:
            self.iface.SolenoidWest(False)
            self.iface.SolenoidEast(enable)

    def BtnIncreasePressed(self):
        print("Increased!")

    def BtnDecreasePressed(self):
        print("Decreased!")

    def BtnMenuPressed(self):
        print("Menu!")

    def BtnTrackPressed(self):
        print("Track!")

    def WorkerThread(self):

        while (self.keepGoing):
            try:
                #if (self.manualMode):

                #else:
                self.iface.Display("Tracker test", 1)
                time.sleep(1)

                # if it's time to update the position
                if (False):
                    date = datetime.datetime.now(datetime.timezone.utc)
                    el = pysolar.solar.get_altitude(self.lat, self.lon, date)
                    az = pysolar.solar.get_azimuth(self.lat, self.lon, date)
                    print("Az: %f El: %f\r\n" % (az, el))
                    # convert this somehow to pitch and roll
                    self.desiredTrackerRoll = 10
                    self.desiredTrackerPitch = 45
                    time.sleep(1)
            except:
                raise
                #pass

    def Start(self):
        """Start the thread that will update the panel location"""
        self.keepGoing = True
        self.thread = threading.Thread(target=self.WorkerThread)
        #self.thread.daemon = True
        self.thread.start()

    def Stop(self):
        self.keepGoing = False
        self.thread.join()



if __name__ == '__main__':
    tracker = Tracker(lattitude=33.00, longitude=-112.00)
    tracker.Start()

    while True:
        inChar = input()
        if (inChar == 'q'):
            break

    tracker.Stop()

