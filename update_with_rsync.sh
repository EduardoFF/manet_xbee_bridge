IP=$1
rsync -avz . --exclude=build odroid@${IP}:manet_src/manet_xbee_bridge/
