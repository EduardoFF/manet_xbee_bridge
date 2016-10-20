import cv2
import socket
import sys
import numpy
import time
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = ('10.42.43.1', 12345)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)

while True:
    print >>sys.stderr, '\nwaiting to receive message'
    data, address = sock.recvfrom(2**16)
    
    print >>sys.stderr, 'received %s bytes from %s' % (len(data), address)
    
    if data:
        frame = numpy.fromstring (data,dtype=numpy.uint8)
        img_data_ndarray = cv2.imdecode(frame, cv2.CV_LOAD_IMAGE_UNCHANGED)
        
#        frame = frame.reshape (240,320,3)
        cv2.imshow('frame',img_data_ndarray)
        cv2.waitKey(10)

