#!/bin/bash


# some default parameters #
NODEID=1
TXPOWER=20
CHANNEL=1
WLAN=wlan19

# modifying parameters from cmd line
source $1

IPPREFIX=10.42.43
NETMASK=24

IPADDR=${IPPREFIX}.${NODEID}
WLANM=`cat /sys/class/net/${WLAN}/address`
#echo ${WLANM}


sudo ./setup_wlan.sh ${WLAN} ${IPADDR}/${NETMASK} ${CHANNEL} ${TXPOWER} 

sudo ip route del to ${IPPREFIX}.0/${NETMASK}


export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib
RNP_PKGclick WLANMAC=${WLANM} WLANINTERFACE=${WLAN} ADHOCIP=${IPADDR}/${NETMASK} ../../click/scripts/rnp_scripts/rnp_linux_args.click
