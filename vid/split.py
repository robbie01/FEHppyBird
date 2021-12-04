import numpy as np
import cv2
import sys

#Convert to 320x240 using https://www.onlineconverter.com/resize-video before running

folder = sys.argv[1]
sourcevid = cv2.VideoCapture(f"{folder}/source.mp4")
frameNr = 0
while (True):
    success, frame = sourcevid.read()
    if success:
        cv2.imwrite(f'{folder}/frames/{frameNr}.jpg', frame)
    else:
        break
    print("Processed frame #" + str(frameNr))
    frameNr = frameNr+1
sourcevid.release()


