import lcm
import sys
from rnp import route2_entry_t
from rnp import route2_table_t
from rnp import route2_tree_t
import hashlib

md5print = True
verbose= False
def my_handler(channel, data):
    msg = route2_tree_t.decode(data)
    if verbose:
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
            print "last_act %d "%(msg.flows[i].last_activity)
        print("")
    if md5print:
        print hashlib.md5(msg.encode()).hexdigest()
#        print hash(msg)

lc = lcm.LCM("udpm://239.255.76.67:7667?ttl=0")
subscription = lc.subscribe(sys.argv[1], my_handler)

try:
    while True:
        lc.handle()
except KeyboardInterrupt:
    pass

lc.unsubscribe(subscription)
