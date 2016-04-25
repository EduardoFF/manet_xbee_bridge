import lcm
import sys
from pos_gps_t import pos_gps_t

def my_handler(channel, data):
    msg = pos_gps_t.decode(data)
    print("Received message on channel \"%s\"" % channel)
    print("")
    print "robotid: ", msg.robotid
    print "longitude: ", msg.longitude
    print "latitude: ", msg.latitude
    print "altitude: ", msg.altitude

lc = lcm.LCM()
subscription = lc.subscribe(sys.argv[1], my_handler)

try:
    while True:
        lc.handle()
except KeyboardInterrupt:
    pass

lc.unsubscribe(subscription)
