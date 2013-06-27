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

#include "com_android_cts_verifier_camera_analyzer_ColorChecker.h"

#include "utils/Log.h"
#include "android/bitmap.h"
#include "colorchecker.h"

jboolean Java_com_android_cts_verifier_camera_analyzer_ColorChecker_findNative(
    JNIEnv*      env,
    jobject      thiz,
    jobject      inputBitmap) {

    // Verify that we can handle the input bitmap
    AndroidBitmapInfo inputInfo;
    AndroidBitmap_getInfo(env, inputBitmap, &inputInfo);
    if (inputInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888 &&
        inputInfo.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Only RGBA_8888 and RGB_565 bitmaps are supported, was given type %d.", inputInfo.format);
        return JNI_FALSE;
    }

    // Get some references to the fields and class type of ColorChecker
    jclass thizCls = env->GetObjectClass(thiz);
    jfieldID patchId = env->GetFieldID(thizCls, "mPatchValues", "[F");
    jfieldID outputId = env->GetFieldID(thizCls, "mDebugOutput", "Landroid/graphics/Bitmap;");
    jfloatArray patchValues = (jfloatArray)env->GetObjectField(thiz, patchId);

    // Get raw inputs and outputs ready

    uint8_t *inputBuffer;
    int result = AndroidBitmap_lockPixels(
        env,
        inputBitmap,
        reinterpret_cast<void**>(&inputBuffer) );

    if (result != ANDROID_BITMAP_RESUT_SUCCESS) {
        LOGE("Unable to lock input bitmap");
        return JNI_FALSE;
    }

    float *patchRawArray = env->GetFloatArrayElements(patchValues,
                                                      NULL);

    uint8_t *outputImage = NULL;
    int outputWidth, outputHeight;

    // Find the color checker
    bool success;
    uint8_t *inputBufferRGBA = NULL;
    int inputStride;
    bool freeInputRGBA = false;
    switch (inputInfo.format) {
        case ANDROID_BITMAP_FORMAT_RGB_565: {
            // First convert to RGBA_8888
            inputBufferRGBA = new uint8_t[inputInfo.width *
                                          inputInfo.height *
                                          4];
            inputStride = inputInfo.width * 4;
            uint8_t *outP = inputBufferRGBA;
            for (int y = 0; y < inputInfo.height; y++ ) {
                uint16_t *inP = (uint16_t*)(&inputBuffer[y * inputInfo.stride]);
                for (int x = 0; x < inputInfo.width; x++) {
                    *(outP++) = ( ((*inP) >> 0) & 0x001F) << 3;
                    *(outP++) = ( ((*inP) >> 5) & 0x003F) << 2;
                    *(outP++) = ( ((*inP) >> 11) & 0x001F) << 3;
                    outP++;
                    inP++;
                }
            }
            freeInputRGBA = true;
            break;
        }
        case ANDROID_BITMAP_FORMAT_RGBA_8888:
            // Already in RGBA
            inputBufferRGBA = inputBuffer;
            inputStride = inputInfo.stride;
            break;
    }

    success = findColorChecker(inputBufferRGBA,
                               inputInfo.width,
                               inputStride,
                               inputInfo.height,
                               patchRawArray,
                               &outputImage,
                               &outputWidth,
                               &outputHeight);

    // Clean up raw inputs/outputs
    env->ReleaseFloatArrayElements(patchValues, patchRawArray, 0);
    result = AndroidBitmap_unlockPixels(env, inputBitmap);
    if (result != ANDROID_BITMAP_RESUT_SUCCESS) {
        LOGE("Unable to unlock input bitmap");
        return JNI_FALSE;
    }

    if (freeInputRGBA) {
        delete inputBufferRGBA;
    }

    // Create debug bitmap from output image data
    if (outputImage != NULL) {
        // Get method handles, create inputs to createBitmap
        jclass bitmapClass =
                env->FindClass("android/graphics/Bitmap");
        jclass bitmapConfigClass =
                env->FindClass("android/graphics/Bitmap$Config");

        jmethodID createBitmap = env->GetStaticMethodID(
            bitmapClass, "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

        jmethodID getConfig = env->GetStaticMethodID(
            bitmapConfigClass, "valueOf",
            "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");

        // Create bitmap config and bitmap
        jstring bitmapConfigValue = env->NewStringUTF("ARGB_8888");
        jobject rgbaConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                         getConfig,
                                                         bitmapConfigValue);
        jobject outputBitmap = env->CallStaticObjectMethod(bitmapClass,
                                                           createBitmap,
                                                           outputWidth,
                                                           outputHeight,
                                                           rgbaConfig);
        // Copy output image into it
        uint8_t *outputBuffer;
        int result = AndroidBitmap_lockPixels(
            env,
            outputBitmap,
            reinterpret_cast<void**>(&outputBuffer) );

        if (result != ANDROID_BITMAP_RESUT_SUCCESS) {
            LOGE("Unable to lock output bitmap");
            return JNI_FALSE;
        }

        memcpy(outputBuffer, outputImage, outputWidth * outputHeight * 4);

        result = AndroidBitmap_unlockPixels(env, outputBitmap);
        if (result != ANDROID_BITMAP_RESUT_SUCCESS) {
            LOGE("Unable to unlock output bitmap");
            return JNI_FALSE;
        }

        // Write new Bitmap reference into mDebugOutput class member
        env->SetObjectField(thiz, outputId, outputBitmap);

        delete outputImage;
    }
    if (!success) return JNI_FALSE;

    return JNI_TRUE;
}
