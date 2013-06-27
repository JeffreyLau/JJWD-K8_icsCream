/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef ANDROID_AUDIO_EFFECTS_CONF_H
#define ANDROID_AUDIO_EFFECTS_CONF_H


/////////////////////////////////////////////////
//      Definitions for effects configuration file (audio_effects.conf)
/////////////////////////////////////////////////

#define AUDIO_EFFECT_DEFAULT_CONFIG_FILE "/system/etc/audio_effects.conf"
#define AUDIO_EFFECT_VENDOR_CONFIG_FILE "/vendor/etc/audio_effects.conf"
#define LIBRARIES_TAG "libraries"
#define PATH_TAG "path"

#define EFFECTS_TAG "effects"
#define LIBRARY_TAG "library"
#define UUID_TAG "uuid"

#define PREPROCESSING_TAG "pre_processing"
#define PARAM_TAG "param"
#define VALUE_TAG "value"
#define INT_TAG "int"
#define SHORT_TAG "short"
#define FLOAT_TAG "float"
#define BOOL_TAG "bool"
#define STRING_TAG "string"
#define MIC_SRC_TAG "mic"
#define VOICE_UL_SRC_TAG "voice_uplink"
#define VOICE_DL_SRC_TAG "voice_downlink"
#define VOICE_CALL_SRC_TAG "voice_call"
#define CAMCORDER_SRC_TAG "camcorder"
#define VOICE_REC_SRC_TAG "voice_recognition"
#define VOICE_COMM_SRC_TAG "voice_communication"
#define LINE_IN_SRC_TAG "line-in"


#endif  // ANDROID_AUDIO_EFFECTS_CONF_H