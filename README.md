- Getting the system working indoor with mocap:


optitrack_lcm_bridge:
use channel: OPTITRACK, ttl=0, rate=1


> rosrun optitrack_lcm_bridge optitrack_lcm_bridge.py \
_publish_markers:=false _rate:=1 _channel:=OPTITRACK \
_ttl:=0

->

OPTITRACK [pose_list_t] -> bridge_poselist.py

> python bridge_poselist.py

use channel: POSEGPS, ttl=1

->
POSEGPS -> goes to WLAN


**** make diagnostics at end_node
- issue warning if not getting GPS stream
- issue warning if not stream rate is not constant

POSEGPS[pos_gps_t] -> end_node (GPSDriver)

-> XBee

-> ctrl_node

**** make diagnostics at ctrl_node
- issue warning when getting GPS (0,0,0)
- issue warning when rate of EndNodeInfo msgs is not constant

-> RCVPOSEGPS [pose_gps_t] -> bridge_gps.py

> python bridge_gps.py --local

use_channel: TRACK, ttl=0






