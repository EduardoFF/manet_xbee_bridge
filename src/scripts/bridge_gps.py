import lcm
import sys
from pos_gps_t import pos_gps_t
from pose.pose_list_t import pose_list_t
from pose.pose_t import pose_t
from optparse import OptionParser


lc=None
ttl=1
output_channel="TRACK"

def send_poselist(rid, x,y,z):
    msg = pose_list_t()
    msg.timestamp = 0
    p = pose_t()
    msg.n = 1
    p.robotid = rid
    p.position=[x,y,z]
    p.orientation=[0,0,0,10000]
    p.velocity = 0
    msg.poses = [p]

    lc.publish(output_channel, msg.encode())

def my_handler(channel, data):
    msg = pos_gps_t.decode(data)
    send_poselist(msg.robotid, msg.longitude, msg.latitude, msg.altitude)
    
#    print("   n    = %d" % msg.n)


def main():
    global output_channel, ttl,lc
    parser = OptionParser()
    parser.add_option("-c", "--channel",
                      dest="channel",
                      help="LCM channel",
                      default="RCVPOSEGPS",
                      metavar="CHANNEL")

    parser.add_option("-o", "--output-channel",
                      help="LCM output channel",
                      default=output_channel)

    parser.add_option("-l", "--local",
                          action="store_true",
                          help="publish local")

    (options, args) = parser.parse_args()
    output_channel = options.output_channel
    print options
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
