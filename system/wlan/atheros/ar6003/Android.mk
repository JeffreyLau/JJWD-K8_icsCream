#------------------------------------------------------------------------------
#    Copyright (c) 2005-2010 Atheros Corporation.  All rights reserved.
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
#------------------------------------------------------------------------------
#==============================================================================
# Author(s): ="Atheros"
#==============================================================================


ifneq ($(TARGET_SIMULATOR),true)
ifeq ($(BOARD_USES_ATH_WIFI),true)
ifneq ($(filter $(BOARD_WIFI_MODULE),ar6003 ar6103 ar6302),)
    include $(CURDIR)/system/wlan/atheros/ar6003/AR6kSDK.build_3.1_RC.573/host/Android.mk
    #include $(CURDIR)/system/wlan/atheros/ar6003/AR6kSDK.build_3.1_RC.573-proprietary/host/Android.mk
  endif
endif
endif

