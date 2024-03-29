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
    def __init__(self, tracker):
        self.tracker = tracker
        self.iface = tracker.iface

    def BtnIncPressed(self):
        print("Inc!")

    def BtnDecPressed(self):
        print("Dec!")

    def BtnTrackPressed(self):
        print("Track!")

class MenuHome(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("%d/%d/%d %d:%d" % (tracker.day, tracker.month, tracker.year, tracker.hour, tracker.minute), 1)
        if (tracker.manualMode):
            self.iface.Display("Manual", 2)
        else:
            self.iface.Display("Auto", 2)


class MenuMoveSafe(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Press Track to", 1)
        self.iface.Display("move safe.", 2)

class MenuRuntime(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Accum Runtime:", 1)
        self.iface.Display("10:05:31", 2)

class MenuSetupMode(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Setup Menus?", 1)
        self.DisplayEnabled()

    def DisplayEnabled(self):
        if (self.tracker.setupMode):
            self.iface.Display("Yes", 2)
        else:
            self.iface.Display("No", 2)

    def BtnIncPressed(self):
        self.tracker.setupMode = not(self.tracker.setupMode)
        self.DisplayEnabled()

    def BtnDecPressed(self):
        self.tracker.setupMode = not(self.tracker.setupMode)
        self.DisplayEnabled()

class MenuDateSetYear(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Set Year:", 1)
        self.DisplayYear()

    def DisplayYear(self):
        self.iface.Display("%d" % (self.tracker.year), 2)

    def BtnIncPressed(self):
        self.tracker.year += 1
        self.DisplayYear()

    def BtnDecPressed(self):
        self.tracker.year -= 1
        self.DisplayYear()

class MenuDateSetMonth(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Set Month:", 1)
        self.DisplayMonth()

    def DisplayMonth(self):
        self.iface.Display("%d" % (self.tracker.month), 2)

    def BtnIncPressed(self):
        self.tracker.month += 1
        self.DisplayMonth()

    def BtnDecPressed(self):
        self.tracker.month -= 1
        self.DisplayMonth()

class MenuDateSetDay(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Set Day:", 1)
        self.DisplayDay()

    def DisplayDay(self):
        self.iface.Display("%d" % (self.tracker.day), 2)

    def BtnIncPressed(self):
        self.tracker.day += 1
        if (self.tracker.day > 31):
            self.tracker.day = 1
        self.DisplayDay()

    def BtnDecPressed(self):
        self.tracker.day -= 1
        if (self.tracker.day < 1):
            self.tracker.day = 31
        self.DisplayDay()

class MenuDateSetHour(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Set Hour:", 1)
        self.DisplayHour()

    def DisplayHour(self):
        self.iface.Display("%d" % (self.tracker.hour), 2)

    def BtnIncPressed(self):
        self.tracker.hour += 1
        if (self.tracker.hour > 23):
            self.tracker.hour = 0
        self.DisplayHour()

    def BtnDecPressed(self):
        self.tracker.hour -= 1
        if (self.tracker.hour < 0):
            self.tracker.hour = 23
        self.DisplayHour()

class MenuDateSetMin(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Set Minute:", 1)
        self.DisplayMin()

    def DisplayMin(self):
        self.iface.Display("%d" % (self.tracker.minute), 2)

    def BtnIncPressed(self):
        self.tracker.minute += 1
        if (self.tracker.minute > 59):
            self.tracker.minute = 0
        self.DisplayMin()

    def BtnDecPressed(self):
        self.tracker.minute -= 1
        if (self.tracker.minute < 0):
            self.tracker.minute = 59
        self.DisplayMin()

class MenuSetLat(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Set Lattitude:", 1)
        self.DisplayLat()

    def DisplayLat(self):
        self.iface.Display("%.2f" % (self.tracker.lat), 2)

    def BtnIncPressed(self):
        self.tracker.lat += .01
        self.DisplayLat()

    def BtnDecPressed(self):
        self.tracker.lat -= .01
        self.DisplayLat()

class MenuSetLon(MenuBase):
    def __init__(self, tracker):
        super().__init__(tracker)
        self.iface.Display("Set Longitude:", 1)
        self.DisplayLon()

    def DisplayLon(self):
        self.iface.Display("%.2f" % (self.tracker.lon), 2)

    def BtnIncPressed(self):
        self.tracker.lon += .01
        self.DisplayLon()

    def BtnDecPressed(self):
        self.tracker.lon -= .01
        self.DisplayLon()

class Tracker:
    def __init__(self, lattitude, longitude, iface):
        """Constructor"""
        self.lat = lattitude
        self.lon = longitude
        self.keepGoing = False
        self.pumpRunning = False
        self.manualMode = False
        self.iface = iface
        self.iface.btnIncreaseCb = lambda: self.menu.BtnIncPressed()
        self.iface.btnDecreaseCb = lambda: self.menu.BtnDecPressed()
        self.iface.btnTrackCb = lambda: self.menu.BtnTrackPressed()
        self.iface.btnMenuCb = self.BtnMenuPressed
        self.iface.btnManAutoCb = self.BtnManAutoPressed
        self.menus = [globals()["MenuHome"], 
                      globals()["MenuMoveSafe"],
                      globals()["MenuRuntime"],
                      globals()["MenuSetupMode"],
                      globals()["MenuDateSetYear"],
                      globals()["MenuDateSetMonth"],
                      globals()["MenuDateSetDay"],
                      globals()["MenuDateSetHour"],
                      globals()["MenuDateSetMin"],
                      globals()["MenuSetLat"],
                      globals()["MenuSetLon"],
                     ]
        self.menuItem = 0
        self.setupMode = False
        self.year = 2021
        self.month = 10
        self.day = 10
        self.hour = 0
        self.minute = 0

        self.MenuRefresh()


    def MenuRefresh(self):
        # create the first menu
        self.menu = self.menus[self.menuItem](self)

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
        if (self.menuItem >= len(self.menus) or (not(self.setupMode) and self.menuItem >= 4)):
            self.menuItem = 0
        self.MenuRefresh()

    def BtnManAutoPressed(self):
        self.manualMode = not(self.manualMode)
        self.MenuRefresh()

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

