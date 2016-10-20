SRC1="udpsrc port=5021 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec"
#SRC1="filesrc location=/home/eduardo/p22.ogg ! oggdemux ! theoradec"
gst-launch -e videomixer name=mix ! ffmpegcolorspace ! xvimagesink \
	   ${SRC1}  ! textoverlay font-desc="Sans 24" text="CAM1" valign=top halign=left shaded-background=true ! \
	   videobox border-alpha=0 top=-200 left=-50 ! mix. \
   multifilesrc location="control.jpg" caps="image/jpeg,framerate=1/1" ! jpegdec ! \
     textoverlay font-desc="Sans 26" text="Control Center" halign=left shaded-background=true auto-resize=false ! \
     ffmpegcolorspace ! video/x-raw-yuv,format=\(fourcc\)AYUV ! mix.



#	   udpsrc port=5021 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec  ! textoverlay font-desc="Sans 24" text="CAM2" valign=top halign=left shaded-background=true ! \
#	   videobox border-alpha=0 top=-200 left=-450 ! mix. \
#	   udpsrc port=5021 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! textoverlay font-desc="Sans 24" text="CAM3" valign=top halign=left shaded-background=true ! \
 #    videobox border-alpha=0 top=-200 left=-850 ! mix. \
