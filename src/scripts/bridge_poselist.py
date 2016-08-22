import lcm
import sys
from pos_gps_t import pos_gps_t
from pose.pose_list_t import pose_list_t
from optparse import OptionParser

lc = None
output_channel="POSEGPS"

def send_pose_gps(rid, x,y,z):
    msg = pos_gps_t()
    msg.robotid = rid
    msg.latitude =  y
    msg.longitude = x
    msg.altitude = z
    lc.publish("POSEGPS", msg.encode())

def my_handler(channel, data):
    msg = pose_list_t.decode(data)
    t = int(msg.timestamp)
    print "got msg with ",len(msg.poses)
    for pose in msg.poses:
        [x,y,z] = pose.position
        rid = pose.robotid
        send_pose_gps(rid, x,y,z)
#    print("   n    = %d" % msg.n)

def main():
    global output_channel, ttl,lc
    parser = OptionParser()
    parser.add_option("-c", "--channel",
                      dest="channel",
                      help="LCM channel",
                      default="OPTITRACK")

    parser.add_option("-o", "--output-channel",
                      dest="output_channel",
                      help="LCM output channel",
                      default=output_channel)

    parser.add_option("-l", "--local",
                          action="store_true",
                          help="publish local")

    (options, args) = parser.parse_args()
    print options
    output_channel = options.output_channel
    if options.local:
        ttl=0
    else:
        ttl=1
    lc = lcm.LCM("udpm://239.255.76.67:7667?ttl=%d"%(ttl)) 
    subscription = lc.subscribe(options.channel, my_handler)
    try:
        while True:
            lc.handle()
    except KeyboardInterrupt:
        pass

    lc.unsubscribe(subscription)

if __name__ == "__main__":
    main()

