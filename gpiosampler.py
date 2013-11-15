#!/usr/bin/env python

import RPi.GPIO as GPIO
import pygame
import glob

###################################################

def buttonDown(i):
    return not GPIO.input(i)


###################################################

class SoundTrigger:
    buttons = [17,18,27,22,23,24,25]

    def __init__(self):
        soundbank = []
        buttonStates = []
        self.fillSoundbank()
        self.buttonStates = [False] * len(self.buttons)
        self.currentSet = 0


    #load all files into the soundbank
    def fillSoundbank(self):
        self.soundbank = []
        paths = sorted(glob.glob("soundbank/*"))
        paths.reverse()
        while paths:
            thisSet = []
            for i in range(len(self.buttons)):
                path = paths.pop()
                thisSet.append(pygame.mixer.Sound(path))
                print "add " + path
            self.soundbank.append(thisSet)
            print "to soundbank"
                

    def nextBank(self):
        self.stopEverything()
        self.currentSet = min(self.currentSet + 1, len(self.soundbank) - 1)
        print "use bank " + str(self.currentSet) + " out of " + str(len(self.soundbank))


    def prevBank(self):
        self.stopEverything()
        self.currentSet = max(self.currentSet - 1, 0)
        print "use bank " + str(self.currentSet) + " out of " + str(len(self.soundbank))


    #read gpio values and start/stop audio accordingly
    def process(self):
        for i in range(len(self.buttons)):
            if self.buttonStates[i] != buttonDown(self.buttons[i]):
                self.buttonStates[i] = not self.buttonStates[i]
                if self.buttonStates[i]:
                    print "play sound " + str(i)
                    self.soundbank[self.currentSet][i].play()
                else:
                    print "stop sound " + str(i)
                    self.soundbank[self.currentSet][i].stop()


    #stop every sound in the soundbank
    def stopEverything(self):
        print "stopping everything"
        for thisSet in self.soundbank:
            for sound in thisSet:
                sound.stop()



#########################################

class BankSelector:
    activate = [17,25]
    prev = 18
    next = 27

    def __init__(self):
        self.isActive = False
        self.activateIsPressed = False
        self.prevIsPressed = False
        self.nextIsPressed = False

    def process(self):
        #update the isActive value
        if self.activateIsPressed != ([buttonDown(b) for b in self.activate].count(False) == 0):
            self.activateIsPressed = not self.activateIsPressed
            if self.activateIsPressed:
                self.isActive = not self.isActive
                print "activate/deactivate bank selector"

        if self.isActive:

            if self.prevIsPressed != buttonDown(self.prev):
                self.prevIsPressed = not self.prevIsPressed
                if self.prevIsPressed:
                    st.prevBank()

            if self.nextIsPressed != buttonDown(self.next):
                self.nextIsPressed = not self.nextIsPressed
                if self.nextIsPressed:
                    st.nextBank()


#########################################

#set up audio
pygame.mixer.init(frequency=22050, size=-16, channels=2, buffer=64)

#set up pins
GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(27, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(22, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(23, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(24, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(25, GPIO.IN, pull_up_down=GPIO.PUD_UP)


st = SoundTrigger()
bs = BankSelector()


while True:
    bs.process()
    if not bs.isActive:
        st.process()
