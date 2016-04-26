#!/usr/bin/env python
"""
   publish_route2_tree - send (bidir) routing tree  from cmd line
   
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
import sys
import time
from optparse import OptionParser


paths=[]
def readFile(fname):
    global paths
    f = open(fname)
    for line in f.readlines():
        s=line.split()
        paths.append(s)





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

lcm = lcm.LCM("udpm://239.255.76.67:7667?ttl=1")


print paths





tables={}


print "---------- TABLES ------------"
# one dir

for p in paths:
    dest = p[-1]
    for j in range(0, len(p)-1):
        n = p[j]
        tab = {}
        if n in tables:
            tab=tables[n]
        if dest not in tab:
            nh = p[j+1]
            tab[dest]=nh
        tables[n] = tab

print tables

print "-------- REVERSE -----------"

for p in paths:
    p.reverse()
    dest = p[-1]
    for j in range(0, len(p)-1):
        n = p[j]
        tab = {}
        if n in tables:
            tab=tables[n]
        if dest not in tab:
            nh = p[j+1]
            tab[dest]=nh
        tables[n] = tab
print tables


msg = route2_tree_t()
msg.timestamp = int(time.time() * 1000000)

msg.n = len(tables)
            

rtables=list()
for (node, rtable) in tables.items():
    rt = route2_table_t()
    rt.node = node
    rt.n = len(rtable)
    entries=list()
    for (dest,nh) in rtable.items():
        re = route2_entry_t()
        re.dest = dest
        re.node = nh
        re.weight = 1
        entries.append(re)
    rt.entries = entries
    rtables.append(rt)

msg.rtable = rtables
print rtables
#print msg
print "publishing msg to ",options.channel
lcm.publish(options.channel, msg.encode())

