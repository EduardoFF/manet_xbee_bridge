import lcm
import sys
from rnp import flow_list_t, flow_entry_t


ochannel = "iflowip"
myip = '10.42.43.1'
mymac = '60-E3-27-10-76-98'
abook = {}
lc = lcm.LCM()

def my_handler(channel, data):
    msg = flow_list_t.decode(data)
    print("Received message on channel \"%s\"" % channel)
    print("   timestamp   = %d" % msg.timestamp)
    print("   addr    = %s" % str(msg.addr))
    print("   n    = %d" % msg.n)
    msg_n = flow_list_t()
    msg_n.timestamp = msg.timestamp
    msg_n.addr = msg.addr
    msg_n.n = 0
    for i in range(msg.n):
        print "flow_%d: "%(i),
        print "addr: %s -> %s "%(msg.flows[i].src_addr, msg.flows[i].dst_addr),
        print "pkt_cnt %d "%(msg.flows[i].pkt_count),
        print "byte_cnt %d "%(msg.flows[i].byte_count),
        print "rate %f "%(msg.flows[i].data_rate),
        print "last_act %d "%(msg.flows[i].last_activity)
        src_addr = msg.flows[i].src_addr
        dst_addr = msg.flows[i].dst_addr
        if dst_addr == mymac:
            print "found ",dst_addr
            if src_addr in abook:
                print abook[src_addr],msg.flows[i].data_rate
                fentry = flow_entry_t()
                fentry.dst_addr = myip
                fentry.src_addr = abook[src_addr]
                fentry.data_rate = msg.flows[i].data_rate
                msg_n.flows.append(fentry)
            else:
                print "not found",src_addr
        msg_n.n = len(msg_n.flows)
        if msg_n > 0:
            lc.publish(ochannel, msg_n.encode())

        
    print("")

subscription = lc.subscribe(sys.argv[1], my_handler)




""" read the abook """
f = open(sys.argv[2])
for line in f.readlines():
    s = line.split()
    nid = int(s[0])
    ip = s[1]
    mac = s[2]
    mmac = mac.upper().replace(':','-')
    abook[mmac] = ip
print abook

            
try:
    while True:
        lc.handle()
except KeyboardInterrupt:
    pass

lc.unsubscribe(subscription)
