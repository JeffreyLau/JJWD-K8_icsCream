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

package com.android.cts.verifier.camera.analyzer;

import android.graphics.Bitmap;

/** The ColorChecker class is used to locate a Xrite Classic Color Checker grid
 * pattern in an image, and return the average value of each color patch.
 *
 * The pattern is a 6x4 grid of square color patches. The detection routine
 * assumes the pattern is placed roughly facing the camera, with the long size
 * roughly horizontal. It doesn't matter whether the pattern is upside down or
 * not - the returned array will have the grayscale squares of the pattern on
 * the bottom row.
 */
class ColorChecker {

    private float[] mPatchValues;
    private Bitmap mDebugOutput;
    private boolean mSuccess = false;

    /** Construct a ColorChecker from a source Bitmap.
     *
     * @param source The source image to scan through for the 6x4 Xrite Classic
     *        Color Checker pattern.
     */
    public ColorChecker(Bitmap source) {
        mPatchValues = new float[6 * 4 * 3];
        mSuccess =  findNative(source);
     }

    /** Returns whether the ColorChecker pattern was found in the source bitmap
     *
     * @return true if the pattern was found in the source Bitmap. false
     *         otherwise.
     */
    public boolean isValid() {
        return mSuccess;
    }

    /** Returns patch RGB triplet for patch (x, y), or null if the pattern wasn't
     * found.
     *
     * @param x Column of the patch
     * @param y Row of the patch
     * @return A 3-entry float array representing the R, G, and B values of the
     *         patch in roughly linear luminance, mapped to the 0-1 range.
     */
    public float[] getPatchRGB(int x, int y) {
        if (!mSuccess) throw new RuntimeException("No color checker found!");
        float[] rgb = {
            mPatchValues[(y * 6 + x) * 3 + 0],
            mPatchValues[(y * 6 + x) * 3 + 1],
            mPatchValues[(y * 6 + x) * 3 + 2]
        };
        return rgb;
    }

    /** Returns patch (x, y) color channel c.
     *
     * @param x Column of the patch
     * @param y Row of the patch
     * @param c Color channel of the patch (0 = R, 1 = G, 2 = B)
     * @return The float value for that color channel in roughly linear
     *         luminance, mapped to the 0-1 range.
     */
    public float getPatchValue(int x, int y, int c) {
        if (!mSuccess) throw new RuntimeException("No color checker found!");
        return mPatchValues[(y * 6 + x) * 3 + c];
    }

    /** Returns debug Bitmap image showing detected candidate patches and the
     * grid if it was found. Valid even if the pattern wasn't found. Candiate
     * patches will have red bounding boxes. In addition, patches that are part
     * of the color checker pattern are have a green diagonal, and all their
     * member pixels are colored in solid blue.
     *
     * @return A low-resolution version of the source Bitmap with overlaid
     *         detection results.
     */
    public Bitmap getDebugOutput() {
        return mDebugOutput;
    }

    native boolean findNative(Bitmap input);

    static {
        System.loadLibrary("cameraanalyzer");
    }
}