import cv2
import socket
import sys
import numpy
import time

video = cv2.VideoCapture(1)
while True:
    success, image = video.read()
    height, width = image.shape[:2]
    print "w %d h %d" % (width,height)
#    resized_image = cv2.resize(image, (320, 240)) 
#    ret, jpeg = cv2.imencode('.jpg', resized_image)
    ret, jpeg = cv2.imencode('.jpg', image)
    data = numpy.array(jpeg)
    stringData = data.tostring()
    frame = numpy.fromstring (stringData,dtype=numpy.uint8)
    img_data_ndarray = cv2.imdecode(frame, cv2.CV_LOAD_IMAGE_UNCHANGED)
    cv2.imshow('frame',img_data_ndarray)
    cv2.waitKey(10)
 #   time.sleep(1)

