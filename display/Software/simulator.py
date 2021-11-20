import tkinter as tk
from tracker import *

class TrackerSim:
    def __init__(self):
        self.pumpRunning = False
        self.eastSol = False
        self.westSol = False
        self.eastLimit = False
        self.westLimit = False
        self.lcdText = ""
        self.lcdLine1 = ""
        self.lcdLine2 = ""
        self.btnIncreaseCb = None
        self.btnDecreaseCb = None
        self.btnMenuCb = None
        self.btnTrackCb = None
        self.SetupUI()

    def SetupUI(self):
        self.window = tk.Tk()
        imgFrame = tk.Frame(self.window)
        imgFrame.grid(row=1, column=1)

        self.lcdText = tk.StringVar()
        self.img = tk.PhotoImage(file="lcd.gif") 
        label = tk.Label(imgFrame, 
                         textvariable=self.lcdText,
                         font=("Courier", 35),
                         image=self.img, compound='center')
        label.pack()

        rightBar = tk.Frame(self.window)
        rightBar.grid(row=1, column=2)

        bar1 = tk.Frame(self.window)
        bar1.grid(row=2, column=1, columnspan=2)

        bar2 = tk.Frame(self.window)
        bar2.grid(row=3, column=1, columnspan=2)

        incBtn = tk.Button(
            rightBar,
            text="Increase",
            width=6,
            height=3,
            fg="black",
        )
        incBtn.bind('<ButtonPress-1>', self.BtnDnInc)
        incBtn.bind('<ButtonRelease-1>', self.BtnUpInc)
        incBtn.pack()

        decBtn = tk.Button(
            rightBar,
            text="Decrease",
            width=6,
            height=3,
            fg="black",
        )
        decBtn.bind('<ButtonPress-1>', self.BtnDnDec)
        decBtn.bind('<ButtonRelease-1>', self.BtnUpDec)
        decBtn.pack()

        menuBtn = tk.Button(
            bar1,
            text="Menu",
            width=5,
            height=3,
            fg="black",
            command=lambda: self.btnMenuCb()
        )
        menuBtn.pack(side=tk.LEFT)

        scrollBtn = tk.Button(
            bar1,
            text="Track",
            width=5,
            height=3,
            fg="black",
            command=lambda: self.btnTrackCb()
        )
        scrollBtn.pack()

        self.manAutoTxt = tk.StringVar()
        manAutoBtn = tk.Button(
            bar2,
            textvariable=self.manAutoTxt,
            width=5,
            height=3,
            fg="black",
            command=self.BtnManAutoPressed
        )
        manAutoBtn.pack(side=tk.LEFT)
        self.manAutoTxt.set("Manual")

        eastBtn = tk.Button(
            bar2,
            text="East",
            width=5,
            height=3,
            fg="black",
            command=lambda: self.btnEastCb()
        )
        eastBtn.pack(side=tk.LEFT)

        westBtn = tk.Button(
            bar2,
            text="West",
            width=5,
            height=3,
            fg="black",
            command=lambda: self.btnWestCb()
        )
        westBtn.pack(side=tk.LEFT)



    def Start(self):
        self.window.mainloop()

    def Pump(self, enable):
        self.pumpRunning = enable

    def SolenoidWest(self, enable):
        self.westSol = enable

    def SolenoidEast(self, enable):
        self.eastSol = enable

    def Display(self, string, line):
        # trim string to 16 chars
        string = string[:16]
        if (line == 1):
            self.lcdLine1 = string.ljust(16)
        if (line == 2):
            self.lcdLine2 = string.ljust(16)
        self.lcdText.set(self.lcdLine1 + "\n" + self.lcdLine2)

    def EastLimit(self):
        return self.eastLimit

    def WestLimit(self):
        return self.westLimit



    def BtnManAutoPressed(self):
        if (self.manAutoTxt.get() == "Manual"):
            self.manAutoTxt.set("Auto")
        else:
            self.manAutoTxt.set("Manual")
        self.btnManAutoCb()

    def BtnDnInc(self, event):
        self.btnDnIncCbJob = self.window.after(1000, self.BtnHeldInc)
        self.btnIncreaseCb()

    def BtnHeldInc(self):
        self.btnIncreaseCb()
        # repeat after 100ms
        self.btnDnIncCbJob = self.window.after(100, self.BtnHeldInc)

    def BtnUpInc(self, event):
        self.window.after_cancel(self.btnDnIncCbJob)

    def BtnDnDec(self, event):
        self.btnDnDecCbJob = self.window.after(1000, self.BtnHeldDec)
        self.btnDecreaseCb()

    def BtnHeldDec(self):
        self.btnDecreaseCb()
        self.btnDnDecCbJob = self.window.after(100, self.BtnHeldDec)

    def BtnUpDec(self, event):
        self.window.after_cancel(self.btnDnDecCbJob)


def PrintPressed(event):
    print("Pressed")

def PrintReleased(event):
    print("released")

if __name__ == '__main__':
    hwSim = TrackerSim()
    tracker = Tracker(lattitude=33.00, longitude=-112.00, iface=hwSim)
    tracker.Start()
    hwSim.Start()  # runs till window is closed
    tracker.Stop()
