import lcm
import sys
from rnp import flow_list_t, flow_entry_t
from optparse import OptionParser
import time

def my_handler(channel, data):
    msg = flow_list_t.decode(data)
    print("Received message on channel \"%s\"" % channel)
    print("   timestamp   = %d" % msg.timestamp)
    print("   addr    = %s" % str(msg.addr))
    print("   n    = %d" % msg.n)
    for i in range(msg.n):
        print "flow_%d: "%(i),
        print "addr: %s -> %s "%(msg.flows[i].src_addr, msg.flows[i].dst_addr),
        print "pkt_cnt %d "%(msg.flows[i].pkt_count),
        print "byte_cnt %d "%(msg.flows[i].byte_count),
        print "rate %f "%(msg.flows[i].data_rate),
        print "last_act %d "%(msg.flows[i].last_activity),
    print("")


flist = flow_list_t()
    
def readFromCmdline(args):
    global flist
    flist.addr = args[0]
    i=1
    while i + 6 <= len(args):
        fe               = flow_entry_t()
        fe.src_addr      = args[i]
        fe.dst_addr      = args[i+1]
        fe.pkt_count     = int(args[i+2])
        fe.byte_count    = int(args[i+3])
        fe.data_rate     = float(args[i+4])
        fe.last_activity = int(args[i+5])
        flist.flows.append(fe)
        i+=6
    flist.n = len(flist.flows)

    
if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="filename",
                      help="read data from  FILE", 
                      metavar="FILE")

    parser.add_option("-c", "--channel", dest="channel",
                      help="LCM channel",
                      default="RNP",
                      metavar="CHANNEL")

    (options, args) = parser.parse_args()

    if options.filename:
        readFile(options.filename)

    if len(args):
        readFromCmdline(args)

    flist.timestamp = int(time.time() * 1000000)
    lc = lcm.LCM()
    lc.publish(options.channel, flist.encode())
    
