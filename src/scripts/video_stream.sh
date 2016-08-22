ID=$1
gst-launch-1.0  v4l2src ! video/x-raw, width=640, height=480 ! videorate !  \
		"video/x-raw,framerate=2/1" ! jpegenc ! rtpjpegpay ! \
		udpsink host=10.42.43.1 port=50${ID}
