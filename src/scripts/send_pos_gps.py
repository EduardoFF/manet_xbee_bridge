#!/usr/bin/env python
"""
   (C) 2014 Eduardo Feo
     
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
from pos_gps_t import pos_gps_t
import sys
import time
import optparse

parser = optparse.OptionParser(usage="usage: %prog [options] id lon lat altitude",
                               version="%prog 1.0")
parser.add_option("-c", "--channel",
                  default="POSEGPS",
                  help="lcm channel")


(options, args) = parser.parse_args()
if len(args) != 4:
    print "mandatory arguments missing (",len(args),")"
    parser.print_help()
    exit(-1)


robotid = int(args[0])
lon = float(args[1])
lat = float(args[2])
alt = float(args[3])
"""  --- no orientaion --- """

channel = options.channel
lc = lcm.LCM("udpm://239.255.76.67:7667?ttl=1") 

msg = pos_gps_t()
msg.robotid = robotid
msg.latitude =  lat
msg.longitude = lon
msg.altitude = alt

lc.publish(options.channel, msg.encode())
