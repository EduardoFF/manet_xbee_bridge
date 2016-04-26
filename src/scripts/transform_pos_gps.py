import lcm
import sys
from pos_gps_t import pos_gps_t
from pose_t import pose_t
import pyproj



pose_channel = "POSE"

offset_x = -487240
offset_y = -113400
def my_handler(channel, data):
    msg = pos_gps_t.decode(data)
    print("Received message on channel \"%s\"" % channel)
    print("")
    print "robotid: ",   msg.robotid
    print "longitude: ", msg.longitude
    print "latitude: ",  msg.latitude
    print "altitude: ",  msg.altitude

    print "transform"
    p1 = pyproj.Proj(init='epsg:4326')
    p2 = pyproj.Proj(init='epsg:21781')
    mx, my = pyproj.transform(p1, p2, msg.longitude ,msg.latitude)

    print mx, my
    msg_pose = pose_t()
    msg_pose.robotid = msg.robotid
    msg_pose.position =  [mx+offset_x,my+offset_y,msg.altitude]
    print msg_pose.position
    msg_pose.orientation = [0,0,0,0]
    lc.publish(pose_channel , msg_pose.encode())

    

lc = lcm.LCM()
subscription = lc.subscribe(sys.argv[1], my_handler)

try:
    while True:
        lc.handle()
except KeyboardInterrupt:
    pass

lc.unsubscribe(subscription)
