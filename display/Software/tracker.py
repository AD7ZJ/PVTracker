import math
import datetime
import pysolar
import time
import threading
import enum

class Menus(enum.Enum):
    Home = 0
    MoveSafe = 1
    RunTime = 2
    Setup = 3
    DateSet = 4
    TimeSet = 5
    LattitudeSet = 6
    LongitudeSet = 7

class MenuBase:
    def __init__(self, iface):
        self.iface = iface

    def BtnDnIncrease(self):
        print("Inc Down!")

    def BtnUpIncrease(self):
        print("Inc Up!")

    def BtnDnDecrease(self):
        print("Dec Down!")

    def BtnUpDecrease(self):
        print("Dec Up!")

    def BtnTrackPressed(self):
        print("Track!")

class MenuHome(MenuBase):
    def __init__(self, iface):
        super().__init__(iface)
        self.iface.Display("Tracking home", 1)
        self.iface.Display("", 2)


class MenuMoveSafe(MenuBase):
    def __init__(self, iface):
        super().__init__(iface)
        self.iface.Display("Press Track to", 1)
        self.iface.Display("move safe.", 2)

class MenuRuntime(MenuBase):
    def __init__(self, iface):
        super().__init__(iface)
        self.iface.Display("Accum Runtime:", 1)
        self.iface.Display("10:05:31", 2)



class Tracker:
    def __init__(self, lattitude, longitude, iface):
        """Constructor"""
        self.lat = lattitude
        self.lon = longitude
        self.keepGoing = False
        self.pumpRunning = False
        self.manualMode = False
        self.iface = iface
        self.iface.btnDnIncreaseCb = lambda: self.menu.BtnDnIncrease()
        self.iface.btnUpIncreaseCb = lambda: self.menu.BtnUpIncrease()
        self.iface.btnDnDecreaseCb = lambda: self.menu.BtnDnDecrease()
        self.iface.btnUpDecreaseCb = lambda: self.menu.BtnUpDecrease()
        self.iface.btnTrackCb = lambda: self.menu.BtnTrackPressed()
        self.iface.btnMenuCb = self.BtnMenuPressed
        self.menus = [globals()["MenuHome"], 
                      globals()["MenuMoveSafe"],
                      globals()["MenuRuntime"]]
        self.menuItem = 0

        # create the first menu
        self.menu = self.menus[0](self.iface)



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

    def BtnMenuPressed(self):
        self.menuItem += 1
        if (self.menuItem >= len(self.menus)):
            self.menuItem = 0
        # create new menu object
        self.menu = self.menus[self.menuItem](self.iface)


    def WorkerThread(self):

        while (self.keepGoing):
            try:
                #if (self.manualMode):

                #else:
                #self.MenuHandler()
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

