#!/usr/bin/env python
"""
   rnp_bidir_bridge - makes rnp output  bidirectional 
   
    Copyright (C) 2016 Eduardo Feo
     
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import lcm 
from rnp import route2_entry_t
from rnp import route2_table_t
from rnp import route2_tree_t

from rnp import route_entry_t
from rnp import route_table_t
from rnp import route_tree_t

import sys
import time
from optparse import OptionParser
import networkx as nx

from prettytable import PrettyTable

LCM = None
output_channel = None
no_sink = False
def find_paths(rtree):
    """ find sinks """
    sinks=set()
    nodes=set()
    G=nx.DiGraph()
    for rtable in rtree.rtable:
        for rentry in rtable.entries:
            is_sink=False
            is_dump = False
            if rentry.node == 'SINK':
                sinks.add(rtable.node)
                is_sink=True
            elif rentry.node == 'DUMP':
                is_dump=True
            else:
                G.add_edge(rentry.node, rtable.node)
            if not is_sink and not is_dump:
                nodes.add(rtable.node)
                
    print "sinks ", sinks
    print list(G.edges())
    paths=[]
    for sink in sinks:
        for node in nodes:
            path=nx.shortest_path(G,sink,node)
            paths.append(path)
    return paths

def route_to_route2(rtree):
    global no_sink
    rtree2 = route2_tree_t()
    rtree2.timestamp = rtree.timestamp

    rtables2={}
    """ copy the table """
    for rtable in rtree.rtable:
        rtable2 = route2_table_t()
        rtable2.node = rtable.node
        for rentry in rtable.entries:
            rentry2 = route2_entry_t()
            rentry2.dest = 'SINK'
            rentry2.node = rentry.node
            rentry2.weight = rentry.weight
            if rentry.node == 'SINK' and no_sink:
                print "WARNING: omiting SINK by request"
                continue
            rtable2.entries.append(rentry2)
        rtable2.n = len(rtable2.entries)
        rtables2[rtable.node] = rtable2

    newtables={}
    paths=find_paths(rtree)
    print "Paths",paths
    for p in paths:
        dest = p[-1]
        for j in range(0, len(p)-1):
            n = p[j]
            tab = {}
            if n in newtables:
                tab=newtables[n]
            if dest not in tab:
                nh = p[j+1]
                tab[dest]=nh
            newtables[n] = tab
    print newtables

    for (node, rtable) in newtables.items():
        rt = route2_table_t()
        rt.node = node
        if node in rtables2:
            rt = rtables2[node]
        print node,
        entries=list()
        for (dest,nh) in rtable.items():
            re = route2_entry_t()
            re.dest = dest
            re.node = nh
            re.weight = 1
            print dest,nh,
            entries.append(re)
        print ""
        rt.entries += entries
        rt.n = len(rt.entries)
        rtables2[node]=rt
    
    rtables=[rtable for (node,rtable) in rtables2.items()]

    rtree2.rtable = rtables
    rtree2.n = len(rtables)
    return rtree2

def print_route2_tree(rtree2):
    print "Converted Table:"
    print "timestamp: ", rtree2.timestamp
    print "n: " , rtree2.n
    x = PrettyTable(["Node", "Destination", "NextHop", "Weight"])
    for rtable2 in rtree2.rtable:        
        for rentry2 in rtable2.entries:
            x.add_row([rtable2.node,rentry2.dest, rentry2.node, rentry2.weight])

    print x
            
    
def print_route_tree(rtree):
    print "Received Table:"
    print "timestamp: ", rtree.timestamp
    print "n: " , rtree.n
    x = PrettyTable(["Node", "NextHop", "Weight"])
    for rtable in rtree.rtable:        
        for rentry in rtable.entries:
            x.add_row([rtable.node, rentry.node, rentry.weight])

    print x

    
        


def my_handler(channel, data):
    global output_channel
    rtree = route_tree_t.decode(data)
    print("Received message on channel \"%s\"" % channel)
    print_route_tree(rtree)
    rt2 = route_to_route2(rtree)
    print "publishing msg to ",output_channel
    print_route2_tree(rt2)
    LCM.publish(output_channel, rt2.encode())


def main():
    global output_channel, LCM, no_sink
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="filename",
                      help="read data from  FILE", 
                      metavar="FILE")

    parser.add_option("-i", "--input-channel", dest="input_channel",
                      help="LCM input channel",
                      default="RNP",
                      metavar="INPUT_CHANNEL")

    parser.add_option("-o", "--output-channel", dest="output_channel",
                      help="LCM output channel",
                      default="RNP2",
                      metavar="OUTPUT_CHANNEL")
    parser.add_option("--omit-sink",
                      action="store_true",
                      help="do not send sink (bug when rnp bidir)")

    
    parser.add_option("-l", "--local",
                          action="store_true",
                          help="publish local")

    (options, args) = parser.parse_args()

    if options.filename:
        readFile(options.filename)

    ttl = 1
    if options.local:
        ttl=0
    LCM = lcm.LCM("udpm://239.255.76.67:7667?ttl=%d"%(ttl))
    no_sink = options.omit_sink
    subscription = LCM.subscribe(options.input_channel, my_handler)
    output_channel = options.output_channel


    try:
        while True:
            LCM.handle()
    except KeyboardInterrupt:
        pass

    LCM.unsubscribe(subscription)


if __name__ == "__main__":
    main()
