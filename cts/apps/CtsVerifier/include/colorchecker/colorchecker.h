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

#ifndef COLORCHECKER_H
#define COLORCHECKER_H

/** Detects the 6x4 Xrite ColorChecker Classic pattern in the input image,
 *   and calculates the average color value per patch.
 *
 *  All squares in the colorchecker pattern have to be fully visible,
 *  and the whole pattern should fill at least 1/3 of the image
 *  width. The pattern cannot be facing away from the camera at a very
 *  large angle (>45 degrees). If multiple 6x4 grids can be found, the
 *  one that is most front-facing will be returned.
 *
 *  The average color is returned as a floating-point value per
 *  channel, linearized by an inverse gamma transform and normalized
 *  to 0-1 (255 = 1). The linearization is only approximate.
 *
 *  @param inputImage Source image to detect the pattern in. Assumed
 *                    to be a 3-channel interleaved image. Row-major
 *  @param width Width of input image
 *  @param height Height of input image
 *  @param patchColors Output 6x4 3-channel image of average patch
 *                     values.  Allocated by caller. Pass in NULL if
 *                     the average values aren't needed.
 *  @param outputImage Resized inputImage with overlaid grid detection
 *                     diagnostics. Image width is approximately 160
 *                     pixels. Pass in NULL if the diagnostic image
 *                     isn't needed.
 *  @param outputWidth Actual width of outputImage.
 *  @param outputHeight Actual height of outputImage.
 *
 *  @return true if a grid was found, false otherwise. If false, the
 *          input variables are unchanged.
 */
bool findColorChecker(const unsigned char *image,
                      int width,
                      int rowSpan,
                      int height,
                      float *patchColors,
                      unsigned char **outputImage,
                      int *outputWidth,
                      int *outputHeight);


#endif
