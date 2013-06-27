#
# Copyright (C) 2011 Telechips, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

$(call inherit-product, hardware/telechips/common/tcc_common.mk)
$(call inherit-product, hardware/telechips/omx/tcc_omx.mk)
$(call inherit-product-if-exists, hardware/telechips/dxb/tcc_dxb.mk)
$(call inherit-product-if-exists, hardware/telechips/common/remote_player/remote_player.mk)

PRODUCT_PACKAGES += \
	busybox

PRODUCT_PROPERTY_OVERRIDES += tcc.all.hdmi.720p.fixed = 0
PRODUCT_PROPERTY_OVERRIDES += tcc.all.video.call.enable = 0
PRODUCT_PROPERTY_OVERRIDES += tcc.all.camera.no.display = 0
PRODUCT_PROPERTY_OVERRIDES += tcc.video.lplayback.mode = 0
PRODUCT_PROPERTY_OVERRIDES += tcc.all.web.full.video = 0
PRODUCT_PROPERTY_OVERRIDES += tcc.show.video.fps = 0
PRODUCT_PROPERTY_OVERRIDES += tcc.hwc.disable = 0
PRODUCT_PROPERTY_OVERRIDES += tcc.all.output.support.camera = 0
PRODUCT_PROPERTY_OVERRIDES += tcc.media.rtsp.cusset = 1
PRODUCT_PROPERTY_OVERRIDES += tcc.media.rtsp.starttime = 10
PRODUCT_PROPERTY_OVERRIDES += tcc.media.rtsp.autimeout = 10
PRODUCT_PROPERTY_OVERRIDES += tcc.media.rtsp.eos = 1
PRODUCT_PROPERTY_OVERRIDES += tcc.media.rtsp.seekend = 3
#
# tcc.indicate.buff.percentage.at
# running : normal indication of buffering percentage.
# underrun: notifying buffering percentage
#           from 0% buffering at underrun-start
#           until 100% buffering at high water mark.
#PRODUCT_PROPERTY_OVERRIDES += tcc.indicate.buff.percentage.at = underrun
#
#PRODUCT_PROPERTY_OVERRIDES += tcc.parser.sequential = 3

# 
# http media streaming reconnection property
# - if disalbed, default is as below :
#
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.conn.timeout.sec = 5
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.recv.timeout.sec = 6
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.reconn.trial.num = 100
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.disconn.at.full = 0

# 
# http media streaming extra option property
# - if disalbed, default is as below :
#
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.flv.opt = 0

# http media streaming buffering watermark property
# - if disalbed, default is as below :
#
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.preroll.buffering.msec = 2000
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.pause.buffering.msec = 500
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.resume.buffering.msec = 5000
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.resume.rtsp.msec = 1000
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.resume.hlslive.msec = 3000
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.resume.dlna.msec = 1000
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.low.buffering.bytes = 4000
#PRODUCT_PROPERTY_OVERRIDES += tcc.http.high.buffering.bytes = 200000

# 
# http live streaming property
# - if disalbed, default is as below :
#
#PRODUCT_PROPERTY_OVERRIDES += tcc.hls.adaptive.play = 0

# 
# http progressive streaming property
# - if disalbed, default is as below :
#
#PRODUCT_PROPERTY_OVERRIDES += tcc.demuxer.max.frame.num = 10240
#PRODUCT_PROPERTY_OVERRIDES += tcc.demuxer.video.bufsize = 33554432
#PRODUCT_PROPERTY_OVERRIDES += tcc.demuxer.audio.bufsize = 4194304

# 
# http live streaming property
# - if disalbed, default is as below :
#
#PRODUCT_PROPERTY_OVERRIDES += tcc.hls.init-ts.num = 10000
#
## Interoperability in the Octoshape apk
#PRODUCT_PROPERTY_OVERRIDES += tcc.hls.prepare.opt = 1 

PRODUCT_COPY_FILES := \
	device/telechips/common/factory/init.factory.rc:root/init.factory.rc	\

PRODUCT_COPY_FILES += \
	device/telechips/common/factory/res/images/test_image0.png:root/res/images/test_image0.png	\
	device/telechips/common/factory/res/images/test_image1.png:root/res/images/test_image1.png	\
	device/telechips/common/factory/res/images/test_image2.png:root/res/images/test_image2.png	\
	device/telechips/common/factory/res/images/test_image3.png:root/res/images/test_image3.png	\
	device/telechips/common/factory/res/images/test_image4.png:root/res/images/test_image4.png	\
	device/telechips/common/factory/res/images/test_image5.png:root/res/images/test_image5.png	\
	device/telechips/common/factory/res/images/test_image6.png:root/res/images/test_image6.png


