#!/usr/bin/env python2

import pygame
import glob

pygame.mixer.init()


#simulate gpio on keyboard
pygame.display.init()
pygame.display.set_mode((100,100), pygame.RESIZABLE)
def readGPIO(index):
    keys = [pygame.K_a, pygame.K_s, pygame.K_d, pygame.K_f,
            pygame.K_h, pygame.K_j, pygame.K_k, pygame.K_l, pygame.K_q]
    return pygame.key.get_pressed()[keys[index]]


#################################################

class SoundTrigger:
    buttons = [4,5,6,7]


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
            if self.buttonStates[i] != readGPIO(self.buttons[i]):
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
    activate = [0,3]
    prev = 1
    next = 2

    def __init__(self):
        self.isActive = False
        self.activateIsPressed = False
        self.prevIsPressed = False
        self.nextIsPressed = False

    def process(self):
        #update the isActive value
        if self.activateIsPressed != ([readGPIO(b) for b in self.activate].count(False) == 0):
            self.activateIsPressed = not self.activateIsPressed
            if self.activateIsPressed:
                self.isActive = not self.isActive
                print "activate/deactivate bank selector"

        if self.isActive:

            if self.prevIsPressed != readGPIO(self.prev):
                self.prevIsPressed = not self.prevIsPressed
                if self.prevIsPressed:
                    st.prevBank()

            if self.nextIsPressed != readGPIO(self.next):
                self.nextIsPressed = not self.nextIsPressed
                if self.nextIsPressed:
                    st.nextBank()


#########################################

st = SoundTrigger()
bs = BankSelector()

while True:
    pygame.event.pump()

    bs.process()
    if not bs.isActive:
        st.process()

    if readGPIO(8):
        exit()
