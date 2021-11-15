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
        self.btnDnIncreaseCb = None
        self.btnUpIncreaseCb = None
        self.btnDnDecreaseCb = None
        self.btnUpDecreaseCb = None
        self.btnMenuCb = None
        self.btnTrackCb = None

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

    def BtnMenuPressed(self):
        self.btnMenuCb()

    def BtnTrackPressed(self):
        self.btnTrackCb()


def PrintPressed(event):
    print("Pressed")

def PrintReleased(event):
    print("released")

if __name__ == '__main__':
    hwSim = TrackerSim()

    window = tk.Tk()
    imgFrame = tk.Frame(window)
    imgFrame.grid(row=1, column=1)

    hwSim.lcdText = tk.StringVar()
    img = tk.PhotoImage(file="lcd.gif") 
    label = tk.Label(imgFrame, 
                     textvariable=hwSim.lcdText,
                     font=("Courier", 35),
                     image=img, compound='center')
    label.pack()

    rightBar = tk.Frame(window)
    rightBar.grid(row=1, column=2)

    lowerBar = tk.Frame(window)
    lowerBar.grid(row=2, column=1, columnspan=2)

    incBtn = tk.Button(
        rightBar,
        text="Increase",
        width=6,
        height=3,
        fg="black",
    )
    incBtn.bind('<ButtonPress-1>', lambda event: hwSim.btnDnIncreaseCb())
    incBtn.bind('<ButtonRelease-1>', lambda event: hwSim.btnUpIncreaseCb())
    incBtn.pack()

    decBtn = tk.Button(
        rightBar,
        text="Decrease",
        width=6,
        height=3,
        fg="black",
    )
    decBtn.bind('<ButtonPress-1>', lambda event: hwSim.btnDnDecreaseCb())
    decBtn.bind('<ButtonRelease-1>', lambda event: hwSim.btnUpDecreaseCb())
    decBtn.pack()

    menuBtn = tk.Button(
        lowerBar,
        text="Menu",
        width=5,
        height=3,
        fg="black",
        command=hwSim.BtnMenuPressed
    )
    menuBtn.pack(side=tk.LEFT)

    scrollBtn = tk.Button(
        lowerBar,
        text="Track",
        width=5,
        height=3,
        fg="black",
        command=hwSim.BtnTrackPressed
    )
    scrollBtn.pack()

    tracker = Tracker(lattitude=33.00, longitude=-112.00, iface=hwSim)
    tracker.Start()


    window.mainloop()
    tracker.Stop()
