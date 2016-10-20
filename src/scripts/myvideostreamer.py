import cv2
import socket
import sys
import numpy
import time
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = (sys.argv[1], 12345)


video = cv2.VideoCapture(1)
while True:
    success, image = video.read()
    height, width = image.shape[:2]
    print "w %d h %d" % (width,height)
    resized_image = cv2.resize(image, (320, 240)) 
    ret, jpeg = cv2.imencode('.jpg', resized_image)



    data = numpy.array(jpeg)
    stringData = data.tostring()
    print >>sys.stderr, 'sending "%d bytes"' % len(stringData)
    sent = sock.sendto(stringData, server_address)
    time.sleep(1)
