#!/usr/bin/python
"""
This program is demonstration for face and object detection using haar-like features.
The program finds faces in a camera image or video stream and displays a red box around them,
then centers the webcam via two servos so the face is at the center of the screen
Based on facedetect.py in the OpenCV samples directory
"""
#-----------------------Includes----------------------
import sys
import bluetooth
from optparse import OptionParser
import cv2.cv as cv
import glob
import serial
import os
import time

# Parameters for haar detection
# From the API:
# The default parameters (scale_factor=2, min_neighbors=3, flags=0) are tuned
# for accurate yet slow object detection. For a faster operation on real video
# images the settings are:
# scale_factor=1.2, min_neighbors=2, flags=CV_HAAR_DO_CANNY_PRUNING,
# min_size=<minimum possible face size

#-----------------------Variables----------------------

min_size = (20, 20)
image_scale = 2
haar_scale = 1.2
min_neighbors = 2
haar_flags = cv.CV_HAAR_DO_CANNY_PRUNING
'''   --- IMPORTANT VARIABLES b---  '''
max_pwm = 180
min_pwm = 2
'''   --- IMPORTANT VARIABLES e---  '''
midScreenWindow = 40  # acceptable 'error' for the center of the screen.
'''   --- IMPORTANT VARIABLES b---  '''
panStepSize = 2 # degree of change for each pan update
tiltStepSize = -2 # degree of change for each tilt update
servoPanPosition = 90 # initial pan position
servoTiltPosition = 100 # initial tilt position

panGpioPin = 2  # servoblaster pin 2 : gpio pin 18
tiltGpioPin = 5  # servoblaster pin 5 : gpio pin 23
'''   --- IMPORTANT VARIABLES e---  '''
BTConencted = False
sock = 0
ser = 0
ser1 = 0
timeout = 0
resethight = 0

#-----------------------BT Function----------------------
def BTconnect (message):
    global BTConencted
    global sock
    if BTConencted == False:
        bd_addr = "98:D3:31:B4:34:1B"
        port = 1
        sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        sock.connect((bd_addr, port))
        if message == 'Z':
            sock.send("hello!!")
        else:
            sock.send(message)
        BTConencted = True
    else:
        sock.send(message)
        #sock.close()

#-----------------------Detect and draw Function----------------------

def detect_and_draw(img, cascade):
    gray = cv.CreateImage((img.width,img.height), 8, 1)
    small_img = cv.CreateImage((cv.Round(img.width / image_scale),cv.Round (img.height / image_scale)), 8, 1)

    # convert color input image to grayscale
    cv.CvtColor(img, gray, cv.CV_BGR2GRAY)

    # scale input image for faster processing
    cv.Resize(gray, small_img, cv.CV_INTER_LINEAR)

    cv.EqualizeHist(small_img, small_img)

    midFace = None

    if(cascade):
        t = cv.GetTickCount()
        # HaarDetectObjects takes 0.02s
        faces = cv.HaarDetectObjects(small_img, cascade, cv.CreateMemStorage(0),
                                     haar_scale, min_neighbors, haar_flags, min_size)
        t = cv.GetTickCount() - t
        if faces:
            for ((x, y, w, h), n) in faces:
                # the input to cv.HaarDetectObjects was resized, so scale the
                # bounding box of each face and convert it to two CvPoints
                pt1 = (int(x * image_scale), int(y * image_scale))
                pt2 = (int((x + w) * image_scale), int((y + h) * image_scale))
                cv.Rectangle(img, pt1, pt2, cv.RGB(255, 0, 0), 3, 8, 0)
                # get the xy corner co-ords, calc the midFace location
                x1 = pt1[0]
                x2 = pt2[0]
                y1 = pt1[1]
                y2 = pt2[1]
                midFaceX = x1+((x2-x1)/2)
                midFaceY = y1+((y2-y1)/2)
                midFace = (midFaceX, midFaceY)
		
    cv.ShowImage("result", img)
    #time.sleep(0.2)
    return midFace

#-----------------------servo command Function----------------------
def move(servo, angle, ports):
    '''Moves the specified servo to the supplied angle.

    Arguments:
        servo
          the servo number to command, an integer from 0-7
        angle
          the desired pulse width for servoblaster, an integer from 0 to 249

    (e.g.) >>> servo.move(2, 90)
           ... # "move servo #2 to 90 degrees"
           '''

    if (min_pwm <= angle <= max_pwm):
        #command = 'echo %s=%s > /dev/servoblaster' % (str(servo), str(angle))
        #os.system(command)
        #print command

        position = int('%d' % (angle))
        if servo == 2:
        #pan
            if position >= 10 and position < 100:
                output = "#0"+str(position)+"b,"
		print "-----------------------------B 10-100--------------------"
            elif position < 10:
                output = "#00"+str(position)+"b,"
		print "-----------------------------B 0-10--------------------"
            elif position >= 100:
                output = "#"+str(position)+"b,"
		print "-----------------------------B 100+--------------------"
            print output
	    if ports != '2':
                global ser
                ser.write(output)
            elif ports == '2':
                BTconnect(output)
	      
       
        #tilt        
        if servo == 5:
	    
            if position >= 10 and  position < 100:
                output = "#0"+str(position)+"a,"
		print "-----------------------------A 10-100--------------------"
            elif position < 10:
                output = "#00"+str(position)+"a,"
		print "-----------------------------A 0-10--------------------"
            elif position >= 100:
                output = "#"+str(position)+"a,"
		print "-----------------------------A 100+--------------------"
            print output
            if ports != '2':
                global ser
                ser.write(output)
            else:
                BTconnect(output)
	    output2 = "noinput"
	    print output2
            if position <= 80 or position >= 105:
	        if position <= 80:
		    print "------------------------------UP--------------------------------"
		    output2 = "c,p,"
		    print output2
	        elif position >=105:
		    print "-----------------------------DOWN-------------------------------"
		    output2 = "d,p,"
		    print output2
		global ser1
	        ser1.write(output2)	  
	        print "****************************************************************"
	   
	    



	    
	    

               
           
        #time.sleep(0.2)
        global timeout
        #timeout voor geval gezicht buiten programma komt
        timeout = time.time()
	#time.sleep(0.2) 
        print timeout
	global resethight
        resethight = 0
    else:
        print "Servo angle must be an integer between 0 and 249.\n"
#-----------------------servo command RESET Function----------------------
def reset(servo, angle, ports):
    
    if (min_pwm <= angle <= max_pwm):
        #command = 'echo %s=%s > /dev/servoblaster' % (str(servo), str(angle))
        #os.system(command)
        #print command

        position = int('%d' % (angle))
        if servo == 2:
        #pan
            if position >= 10 and position < 100:
                output = "#0"+str(position)+"b,"
		print "-----------------------------B 10-100--------------------"
            elif position < 10:
                output = "#00"+str(position)+"b,"
		print "-----------------------------B 0-10--------------------"
            elif position >= 100:
                output = "#"+str(position)+"b,"
		print "-----------------------------B 100+--------------------"
            print output
	    if ports != '2':
                global ser
                ser.write(output)
            elif ports == '2':
                BTconnect(output)
	      
        #tilt        
        if servo == 5:

	    
            if position >= 10 and  position < 100:
                output = "#0"+str(position)+"a,"
		print "-----------------------------A 10-100--------------------"
            elif position < 10:
                output = "#00"+str(position)+"a,"
		print "-----------------------------A 0-10--------------------"
            elif position >= 100:
                output = "#"+str(position)+"a,"
		print "-----------------------------A 100+--------------------"
            print output
            if ports != '2':
                global ser
                ser.write(output)
            else:
                BTconnect(output)
	    output2 = "noinput"
	    print output2
            if position <= 80 or position >= 105:
	        if position <= 80:
		    print "------------------------------UP--------------------------------"
		    output2 = "c,p,"
		    print output2
	        elif position >=105:
		    print "-----------------------------DOWN-------------------------------"
		    output2 = "d,p,"
		    print output2
		global ser1
	        ser1.write(output2)	  
	        print "****************************************************************"
	   
	         
           
        #time.sleep(0.2)
        global timeout
        #timeout voor geval gezicht buiten programma komt
        timeout = time.time()
	#time.sleep(0.2) 
        print timeout
	global resethight
	resethight = resethight +1
	print "reset "+str(resethight/2)+"/5"
    else:
        print "Servo angle must be an integer between 0 and 249.\n"


#-----------------------The Main----------------------
if __name__ == '__main__':
    
    #READ input to get port for servo
    port = raw_input('What USB port to use for servos? 0 for ttyACM0 , 1 for ttyACM1 :: ' ''', 2 for Bluetooth [warning use only when your sure coded BT adress is corect]:: ''')
    if port == '0':
        global ser
        ser = serial.Serial('/dev/ttyUSB0', 9600)
	print "servo's connected to usb0"
    if port == '1':
        global ser
        ser = serial.Serial('/dev/ttyUSB1', 9600)
	print "servo's connected to usb1"
    if port == '2':
        BTconnect ('Z')
    
    time.sleep(0.2)
    
    #read input for port DMQ stepper controller
   
    if port == '0':
        global ser1
        ser1 = serial.Serial('/dev/ttyUSB1', 9600)
	print "stepper connected to usb1"
    if port == '1':
        global ser1
        ser1 = serial.Serial('/dev/ttyUSB0', 9600)   
	print "stepper connected to usb0"
    
        
    # parse cmd line options, setup Haar classifier
    parser = OptionParser(usage = "usage: %prog [options] [camera_index]")
    parser.add_option("-c", "--cascade", action="store", dest="cascade", type="str", help="Haar cascade file, default %default", default = "./haarcascade_frontalface_alt.xml")#cascade file can be changed
    (options, args) = parser.parse_args()

    cascade = cv.Load(options.cascade)

    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    input_name = args[0]
    if input_name.isdigit():
        capture = cv.CreateCameraCapture(int(input_name))
    else:
        print "We need a camera input! Specify camera index e.g. 0"
        sys.exit(0)


    answer = raw_input('do you need boot time? yes[1] or no[0] or (yes with) reset[3]')
    if answer == '1' or answer =='3':
	if answer =='3':
	    ser1.write("r")
        print "booting:"    
	time.sleep(8)
    	print "1/3 boot time"
    	time.sleep(8)
    	print "2/3 boot time"
    	time.sleep(6)
    	print "3/3 boot time finished"
    else : 
	print "boot skipped"


    cv.NamedWindow("result", 1)

    if capture:
        frame_copy = None

        move(panGpioPin, 90, port)
        move(tiltGpioPin, 100, port)

        while True:
            frame = cv.QueryFrame(capture)
            if not frame:
                cv.WaitKey(0)
                break
            if not frame_copy:
                frame_copy = cv.CreateImage((frame.width,frame.height),
                                            cv.IPL_DEPTH_8U, frame.nChannels)
            if frame.origin == cv.IPL_ORIGIN_TL:
                cv.Copy(frame, frame_copy)
            else:
                cv.Flip(frame, frame_copy, 0)

            midScreenX = (frame.width/2)
            midScreenY = (frame.height/2)

            midFace = detect_and_draw(frame_copy, cascade)

            if midFace is not None:
                midFaceX = midFace[0]
                midFaceY = midFace[1]

                #Find out if the X component of the face is to the left of the middle of the screen.
                if(midFaceX < (midScreenX - midScreenWindow)):
                    #Update the pan position variable to move the servo to the right.
                    servoPanPosition += panStepSize
                    print str(midFaceX) + " > " + str(midScreenX) + " : Pan Right : " + str(servoPanPosition)
                #Find out if the X component of the face is to the right of the middle of the screen.
                elif(midFaceX > (midScreenX + midScreenWindow)):
                    #Update the pan position variable to move the servo to the left.
                    servoPanPosition -= panStepSize
                    print str(midFaceX) + " < " + str(midScreenX) + " : Pan Left : " + str(servoPanPosition)
                else:
                    print str(midFaceX) + " ~ " + str(midScreenX) + " : " + str(servoPanPosition)

                servoPanPosition = min(servoPanPosition, max_pwm)
                servoPanPosition = max(servoPanPosition, min_pwm)              
                move(panGpioPin, servoPanPosition, port)

                #Find out if the Y component of the face is below the middle of the screen.
                if(midFaceY < (midScreenY - midScreenWindow)):
                    if(servoTiltPosition >= 1):
                        #Update the tilt position variable to raise the tilt servo.
                        servoTiltPosition += tiltStepSize
                        print str(midFaceY) + " < " + str(midScreenY) + " : Tilt Up : " + str(servoTiltPosition)
                #Find out if the Y component of the face is above the middle of the screen.
                elif(midFaceY > (midScreenY + midScreenWindow)):
                    if(servoTiltPosition <= max_pwm):
                        #Update the tilt position variable to lower the tilt servo.
                        servoTiltPosition -= tiltStepSize
                        print str(midFaceY) + " > " + str(midScreenY) + " : Tilt Down : " + str(servoTiltPosition)
		else:
		    print str(midFaceY) + " ~ " + str(midScreenY) + " : " + str(servoTiltPosition)

                servoTiltPosition = min(servoTiltPosition, max_pwm)
                servoTiltPosition = max(servoTiltPosition, min_pwm) 
                move(tiltGpioPin, servoTiltPosition, port)
            elif midFace is None:
                global timeout
		global resethight
		if resethight >= 10 :
		    ser1.write("M")
		    print "set camera to middle of frame"
                    global resethight
                    resethight = 0
                if(time.time() - timeout >= 20 ):
            	    print "reset position"
            	    reset(panGpioPin, 90, port)
		    time.sleep(0.1)
            	    reset(tiltGpioPin, 100, port)
		    servoPanPosition=90
		    servoTiltPosition=100
	    if cv.WaitKey(10) >= 0: # 10ms delay
		break
                
                
        

    cv.DestroyWindow("result")
