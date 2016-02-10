#!/usr/bin/env python
"""
   publish_plan2_tree - send (bidir) planning tree  from cmd line
   
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
from plan import plan2_entry_t
from plan import plan2_table_t
from plan import plan2_tree_t
import sys
import time
from optparse import OptionParser

# the routing tree
plTree = []

def readFile(fname):
    global plTree
    f = open(fname)
    for line in f.readlines():
        s=line.split()
        if len(s)%2==0:
            print "error reading file: line has length ",len(s)
            sys.exit(1)
        node = s[0]
        succ = [s[n:n+2] for n in range(1,len(s),2)]
        succl = [(i,int(j)) for [i,j] in succ]
        plTree.append((node,succl))


def readFromCmdline(args):
    global plTree
    i=0
    while i < len(args):
        node = args[i]
        rt=list()
        i+=1
        m = int(args[i])
        i+=1
        while m:
           latitude = args[i]
	   i+=1
	   longitude = args[i]
	   i+=1
	   altitude = args[i]
	   i+=1
	   action = args[i]
	   i+=1
	   option = args[i]
	   i+=1
	   timestamp = args[i]
	   i+=1

		# dest=args[i]		
      		# i+=1
          	#  w = int(args[i])		
          	#  i+=1
           rt.append( (latitude,longitude, altitude, action, option , timestamp) )	
           m-=1
        plTree.append((node, rt))	

parser = OptionParser()
parser.add_option("-f", "--file", dest="filename",
                  help="read data from  FILE", 
                  metavar="FILE")

parser.add_option("-c", "--channel", dest="channel",
                  help="LCM channel",
                  default="PLAN",
                  metavar="CHANNEL")

(options, args) = parser.parse_args()

if options.filename:
    readFile(options.filename)

if len(args):
    readFromCmdline(args)

lcm = lcm.LCM("udpm://239.255.76.67:7667?ttl=1")
msg = plan2_tree_t()
msg.timestamp = int(time.time() * 1000000)

print plTree
msg.n = len(plTree)
rtables=list()
for (node, rtable) in plTree:
    rt = plan2_table_t()
    rt.node = node		
    rt.n = len(rtable)
    entries=list()
    for (latitude,longitude, altitude, action, option , timestamp) in rtable:
        re = plan2_entry_t()
        re.latitude = latitude		
	re.longitude = longitude
	re.altitude = altitude		
        re.action = action		       
	re.option = option		
        re.timestamp = timestamp
	entries.append(re)	
    rt.entries = entries	
    rtables.append(rt)		

msg.rtable = rtables
#print msg
print "publishing msg to ",options.channel
lcm.publish(options.channel, msg.encode())

