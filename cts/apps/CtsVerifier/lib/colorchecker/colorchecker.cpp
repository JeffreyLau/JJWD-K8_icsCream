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

//#define LOG_NDEBUG 0
#include <utils/Log.h>
#include <utils/Timers.h>

#include "colorchecker.h"
#include "grouping.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>

const int totalChannels = 4; // Input image channel count
const int colorChannels = 3; // Input image color channel count
const float gammaCorrection = 2.2; // Assumed gamma curve on input
const int thresholdSq = 675; // Threshold on pixel difference to be considered
                             // part of the same patch

class PixelId {
  public:
    int id;
    const unsigned char *p;

    PixelId(): id(0), p(NULL) {}

    bool operator!=(const PixelId &other) const {
        int dR = ((int)p[0] - other.p[0]) * ((int)p[0] - other.p[0]);
        int dG = ((int)p[1] - other.p[1]) * ((int)p[1] - other.p[1]);
        int dB = ((int)p[2] - other.p[2]) * ((int)p[2] - other.p[2]);
        int distSq = dR + dG + dB;
        if (distSq > thresholdSq) return true;
        else return false;
    }
};

class ImageField: public Field<PixelId> {
  public:
    ImageField(int width, int height, const unsigned char *data):
            mWidth(width), mHeight(height), pData(data) {
    }

    PixelId operator() (int y, int x) const {
        PixelId pId;
        pId.p = pData + (y * mWidth + x ) * totalChannels;
        if (mask.size() != 0) {
            pId.id = mask[y][x];
        }
        return pId;
    }

    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
  private:
    int mWidth;
    int mHeight;
    const unsigned char *pData;
};

class PixelGroup {
  public:
    PixelGroup(int id, const ImageField *src):
        mId(id),
        mMinX(1e7),
        mMinY(1e7),
        mMaxX(0),
        mMaxY(0),
        mArea(0),
        mSrc(src),
        mRNeighbor(NULL),
        mDNeighbor(NULL),
        mLNeighbor(NULL),
        mUNeighbor(NULL) {
        mSum[0] = 0;
        mSum[1] = 0;
        mSum[2] = 0;
    }

    struct IdCompare {
        bool operator() (const PixelGroup* l, const PixelGroup* r) const {
            return l->getId() < r->getId();
        }
    };

    void growGroup(int x, int y) {
        if (x < mMinX) mMinX = x;
        if (x > mMaxX) mMaxX = x;
        if (y < mMinY) mMinY = y;
        if (y > mMaxY) mMaxY = y;
        mArea++;
        const unsigned char *p = (*mSrc)(y,x).p;
        mSum[0] += p[0];
        mSum[1] += p[1];
        mSum[2] += p[2];
    }

    int getId() const {
        return mId;
    }

    int getArea() const {
        return mArea;
    }

    int getBoundArea() const {
        return (mMaxX - mMinX) * (mMaxY - mMinY);
    }

    float getApproxAspectRatio() const {
        return ((float)(mMaxY - mMinY)) / (mMaxX - mMinX);
    }

    void getApproxCenter(int *x, int *y) const {
        *x = (mMaxX + mMinX)/2;
        *y = (mMaxY + mMinY)/2;
    }

    void getBoundingBox(int *x1, int *y1, int *x2, int *y2) const {
        *x1 = mMinX;
        *x2 = mMaxX;
        *y1 = mMinY;
        *y2 = mMaxY;
    }

    void getAvgValue(unsigned char *p) const {
        p[0] = mSum[0] / mArea;
        p[1] = mSum[1] / mArea;
        p[2] = mSum[2] / mArea;
    }

    bool operator<(const PixelGroup &other) const {
        return mArea < other.getArea();
    }

    typedef std::set<PixelGroup*, PixelGroup::IdCompare> IdSet;
    typedef std::set<PixelGroup*, PixelGroup::IdCompare>::iterator IdSetIter;

    void findNeighbors(IdSet &candidates) {
        int cX, cY;
        getApproxCenter(&cX, &cY);
        int rDistSq = 1e9; // Larger than any reasonable image distance
        int dDistSq = rDistSq;

        for (IdSetIter neighbor = candidates.begin();
             neighbor != candidates.end();
             neighbor++) {
            if (*neighbor == this) continue;
            int nX, nY;
            (*neighbor)->getApproxCenter(&nX, &nY);
            // 'right' means slope between (-1/3, 1/3), positive X change
            if ( (nX - cX) > 0 ) {
                float slope = ((float)(nY - cY)) / (nX - cX);
                if (slope > -0.33 && slope < 0.33) {
                    int distSq = (nX - cX) * (nX - cX) + (nY - cY) * (nY - cY);
                    if (distSq < rDistSq) {
                        setRNeighbor(*neighbor);
                        rDistSq = distSq;
                    }
                }
            }
            // 'down' means inv slope between (-1/3, 1/3), positive Y change
            if ( (nY - cY) > 0) {
                float invSlope = ((float)(nX - cX)) / (nY - cY);
                if (invSlope > -0.33 && invSlope < 0.33) {
                    int distSq = (nX - cX) * (nX - cX) + (nY - cY) * (nY - cY);
                    if (distSq < dDistSq) {
                        setDNeighbor(*neighbor);
                        dDistSq = distSq;
                    }
                }
            }
        }
        // Do reverse links if possible
        if (getRNeighbor() != NULL) {
            getRNeighbor()->setLNeighbor(this);
        }
        if (getDNeighbor() != NULL) {
            getDNeighbor()->setUNeighbor(this);
        }

    }

    void setRNeighbor(PixelGroup *rNeighbor) {
        mRNeighbor = rNeighbor;
    }

    PixelGroup* getRNeighbor(int distance = 1)  {
        PixelGroup *current = this;
        for (int i=0; i < distance; i++) {
            if (current != NULL) {
                current = current->mRNeighbor;
            } else break;
        }
        return current;
    }

    void setDNeighbor(PixelGroup *dNeighbor) {
        mDNeighbor = dNeighbor;
    }

    PixelGroup* getDNeighbor(int distance = 1) {
        PixelGroup *current = this;
        for (int i=0; i < distance; i++) {
            if (current != NULL) {
                current = current->mDNeighbor;
            } else break;
        }
        return current;
    }

    void setLNeighbor(PixelGroup *lNeighbor) {
        mLNeighbor = lNeighbor;
    }

    PixelGroup* getLNeighbor(int distance = 1) {
        PixelGroup *current = this;
        for (int i=0; i < distance; i++) {
            if (current != NULL) {
                current = current->mLNeighbor;
            } else break;
        }
        return current;
    }

    void setUNeighbor(PixelGroup *uNeighbor) {
        mUNeighbor = uNeighbor;
    }

    PixelGroup* getUNeighbor(int distance = 1) {
        PixelGroup *current = this;
        for (int i=0; i < distance; i++) {
            if (current != NULL) {
                current = current->mUNeighbor;
            } else break;
        }
        return current;
    }

    float distanceSqTo(const PixelGroup* other) {
        int mX, mY;
        getApproxCenter(&mX, &mY);
        int oX, oY;
        other->getApproxCenter(&oX, &oY);
        int dx = (oX - mX);
        int dy = (oY - mY);
        return dx * dx + dy * dy;
    }

    float distanceTo(const PixelGroup* other) {
        return sqrt( distanceSqTo(other) );
    }

  private:
    int mId;
    int mMinX, mMinY;
    int mMaxX, mMaxY;
    int mArea;
    int mSum[3];
    const ImageField *mSrc;

    PixelGroup *mRNeighbor;
    PixelGroup *mDNeighbor;
    PixelGroup *mLNeighbor;
    PixelGroup *mUNeighbor;
};

/* Scales input down by factor of outScale to output. Assumes input size is
 * exactly output size times scale */
void downsample(const unsigned char *input,
                unsigned char *output,
                int rowSpan,
                int outWidth,
                int outHeight,
                int outScale) {
    for (int oY = 0, iY = 0; oY < outHeight; oY++, iY += outScale) {
        for (int oX = 0, iX = 0; oX < outWidth; oX++, iX += outScale) {
            short out[3] = {0,0,0};
            const unsigned char *in = input + iY * rowSpan + iX * totalChannels;
            for (int j = 0; j < outScale; j++) {
                for (int k = 0; k < outScale; k++) {
                    for (int i = 0; i < colorChannels; i++) {
                        out[i] += in[i];
                    }
                    in += totalChannels;
                }
                in += rowSpan - outScale * totalChannels;
            }
            output[0] = out[0] / (outScale * outScale);
            output[1] = out[1] / (outScale * outScale);
            output[2] = out[2] / (outScale * outScale);
            output += totalChannels;
        }
    }
}

void drawLine(unsigned char *image,
              int rowSpan,
              int x0, int y0,
              int x1, int y1,
              int r, int g, int b) {
    if ((x0 == x1) && (y0 == y1)) {
        unsigned char *p = &image[(y0 * rowSpan + x0) * totalChannels];
        if (r != -1) p[0] = r;
        if (g != -1) p[1] = g;
        if (b != -1) p[2] = b;
        return;
    }
    if ( std::abs(x1-x0) > std::abs(y1-y0) ) {
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        float slope = (float)(y1 - y0) / (x1 - x0);
        for (int x = x0; x <= x1; x++) {
            int y = y0 + slope * (x - x0);
            unsigned char *p = &image[(y * rowSpan + x) * totalChannels];
            if (r != -1) p[0] = r;
            if (g != -1) p[1] = g;
            if (b != -1) p[2] = b;
        }
    } else {
        if (y0 > y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        float invSlope = (float)(x1 - x0) / (y1 - y0);
        for (int y = y0; y <= y1; y++) {
            int x = x0 + invSlope * (y - y0);
            unsigned char *p = &image[(y*rowSpan + x) * totalChannels];
            if (r != -1) p[0] = r;
            if (g != -1) p[1] = g;
            if (b != -1) p[2] = b;
        }
    }

}
bool findColorChecker(const unsigned char *image,
                      int width,
                      int rowSpan,
                      int height,
                      float *patchColors,
                      unsigned char **outputImage,
                      int *outputWidth,
                      int *outputHeight) {
    int64_t startTime = systemTime();

    const int outTargetWidth = 160;
    const int outScale = width / outTargetWidth;
    const int outWidth = width / outScale;
    const int outHeight = height / outScale;
    LOGV("Debug image dimensions: %d, %d", outWidth, outHeight);

    unsigned char *output = new unsigned char[outWidth * outHeight * totalChannels];

    unsigned char *outP;
    unsigned char *inP;

    // First step, downsample for speed/noise reduction
    downsample(image, output, rowSpan, outWidth, outHeight, outScale);

    // Find connected components (groups)
    ImageField outField(outWidth, outHeight, output);
    Grouping(&outField);

    // Calculate component bounds and areas
    std::vector<PixelGroup> groups;
    groups.reserve(outField.id_no);
    for (int i = 0; i < outField.id_no; i++) {
        groups.push_back(PixelGroup(i + 1, &outField));
    }

    inP = output;
    for (int y = 0; y < outHeight; y++) {
        for (int x = 0; x < outWidth; x++) {
            groups[ outField(y, x).id - 1].growGroup(x, y);
        }
    }

    // Filter out groups that are too small, too large, or have too
    // non-square aspect ratio
    PixelGroup::IdSet candidateGroups;

    // Maximum/minimum width assuming pattern is fully visible and >
    // 1/3 the image in width
    const int maxPatchWidth = outWidth / 6;
    const int minPatchWidth = outWidth / 3 / 7;
    const int maxPatchArea = maxPatchWidth * maxPatchWidth;
    const int minPatchArea = minPatchWidth * minPatchWidth;
    // Assuming nearly front-on view of target, so aspect ratio should
    // be quite close to square
    const float maxAspectRatio = 5.f / 4.f;
    const float minAspectRatio = 4.f / 5.f;
    for (int i = 0; i < (int)groups.size(); i++) {
        float aspect = groups[i].getApproxAspectRatio();
        if (aspect < minAspectRatio || aspect > maxAspectRatio) continue;
        // Check both boundary box area, and actual pixel count - they
        // should both be within bounds for a roughly square patch.
        int boundArea = groups[i].getBoundArea();
        if (boundArea < minPatchArea || boundArea > maxPatchArea) continue;
        int area = groups[i].getArea();
        if (area < minPatchArea || area > maxPatchArea) continue;
        candidateGroups.insert(&groups[i]);
    }

    // Find neighbors for candidate groups. O(n^2), but not many
    // candidates to go through
    for (PixelGroup::IdSetIter group = candidateGroups.begin();
         group != candidateGroups.end();
         group++) {
        (*group)->findNeighbors(candidateGroups);
    }

    // Try to find a plausible 6x4 grid by taking each pixel group as
    // the candidate top-left corner and try to build a grid from
    // it. Assumes no missing patches.
    float bestError = -1;
    std::vector<int> bestGrid(6 * 4,0);
    for (PixelGroup::IdSetIter group = candidateGroups.begin();
         group != candidateGroups.end();
         group++) {
        int dex, dey; (*group)->getApproxCenter(&dex, &dey);
        std::vector<int> grid(6 * 4, 0);
        PixelGroup *tl = *group;
        PixelGroup *bl, *tr, *br;

        // Find the bottom-left and top-right corners
        if ( (bl = tl->getDNeighbor(3)) == NULL ||
             (tr = tl->getRNeighbor(5)) == NULL ) continue;
        LOGV("Candidate at %d, %d", dex, dey);
        LOGV("  Got BL and TR");

        // Find the bottom-right corner
        if ( tr->getDNeighbor(3) == NULL ) {
            LOGV("  No BR from TR");
            continue;
        }
        br = tr->getDNeighbor(3);
        if ( br != bl->getRNeighbor(5) ) {
            LOGV("  BR from TR and from BL don't agree");
            continue;
        }
        br->getApproxCenter(&dex, &dey);
        LOGV("  Got BR corner at %d, %d", dex, dey);

        // Check that matching grid edge lengths are about the same
        float gridTopWidth = tl->distanceTo(tr);
        float gridBotWidth = bl->distanceTo(br);

        if (gridTopWidth / gridBotWidth < minAspectRatio ||
            gridTopWidth / gridBotWidth > maxAspectRatio) continue;
        LOGV("  Got reasonable widths: %f %f", gridTopWidth, gridBotWidth);

        float gridLeftWidth = tl->distanceTo(bl);
        float gridRightWidth = tr->distanceTo(br);

        if (gridLeftWidth / gridRightWidth < minAspectRatio ||
            gridLeftWidth / gridRightWidth > maxAspectRatio) continue;
        LOGV("  Got reasonable heights: %f %f", gridLeftWidth, gridRightWidth);

        // Calculate average grid spacing
        float gridAvgXGap = (gridTopWidth + gridBotWidth) / 2 / 5;
        float gridAvgYGap = (gridLeftWidth + gridRightWidth) / 2 / 3;

        // Calculate total error between average grid spacing and
        // actual spacing Uses difference in expected squared distance
        // and actual squared distance
        float error = 0;
        for (int x = 0; x < 6; x++) {
            for (int y = 0; y < 4; y++) {
                PixelGroup *node;
                node = tl->getRNeighbor(x)->getDNeighbor(y);
                if (node == NULL) {
                    error += outWidth * outWidth;
                    grid[y * 6 + x] = 0;
                } else {
                    grid[y * 6 + x] = node->getId();
                    if (node == tl) continue;
                    float dist = tl->distanceSqTo(node);
                    float expXDist = (gridAvgXGap * x);
                    float expYDist = (gridAvgYGap * y);
                    float expDist =  expXDist * expXDist + expYDist * expYDist;
                    error += fabs(dist - expDist);
                }
            }
        }
        if (bestError == -1 ||
            bestError > error) {
            bestGrid = grid;
            bestError = error;
            LOGV("  Best candidate, error %f", error);
        }
    }

    // Check if a grid wasn't found
    if (bestError == -1) {
        LOGV("No color checker found!");
    }

    // Make sure black square is in bottom-right corner
    if (bestError != -1) {
        unsigned char tlValues[3];
        unsigned char brValues[3];
        int tlId = bestGrid[0];
        int brId = bestGrid[23];

        groups[tlId - 1].getAvgValue(tlValues);
        groups[brId - 1].getAvgValue(brValues);

        int tlSum = tlValues[0] + tlValues[1] + tlValues[2];
        int brSum = brValues[0] + brValues[1] + brValues[2];
        if (brSum > tlSum) {
            // Grid is upside down, need to flip!
            LOGV("Flipping grid to put grayscale ramp at bottom");
            bestGrid = std::vector<int>(bestGrid.rbegin(), bestGrid.rend());
        }
    }

    // Output average patch colors if requested
    if (bestError != -1 && patchColors != NULL) {
        for (int n = 0; n < 6 * 4 * colorChannels; n++) patchColors[n] = -1.f;

        // Scan over original input image for grid regions, degamma, average
        for (int px = 0; px < 6; px++) {
            for (int py = 0; py < 4; py++) {
                int id = bestGrid[py * 6 + px];
                if (id == 0) continue;

                PixelGroup &patch = groups[id - 1];
                int startX, startY;
                int endX, endY;
                patch.getBoundingBox(&startX, &startY, &endX, &endY);

                float sum[colorChannels] = {0.f};
                int count = 0;
                for (int y = startY; y <= endY; y++) {
                    for (int x = startX; x < endX; x++) {
                        if (outField(y,x).id != id) continue;
                        for (int iY = y * outScale;
                             iY < (y + 1) * outScale;
                             iY++) {
                            const unsigned char *inP = image +
                                    (iY * rowSpan)
                                    + (x * outScale * totalChannels);
                            for (int iX = 0; iX < outScale; iX++) {
                                for (int c = 0; c < colorChannels; c++) {
                                    // Convert to float and normalize
                                    float v = inP[c] / 255.f;
                                    // Gamma correct to get back to
                                    // roughly linear data
                                    v = pow(v, gammaCorrection);
                                    // Sum it up
                                    sum[c] += v;
                                }
                                count++;
                                inP += totalChannels;
                            }
                        }
                    }
                }
                for (int c = 0 ; c < colorChannels; c++) {
                    patchColors[ (py * 6  + px) * colorChannels + c ] =
                            sum[c] / count;
                }
            }
        }
        // Print out patch colors
        IF_LOGV() {
            for (int y = 0; y < 4; y++) {
                char tmpMsg[256];
                int cnt = 0;
                cnt = snprintf(tmpMsg, 256, "%02d:", y + 1);
                for (int x = 0; x < 6; x++) {
                    int id = bestGrid[y * 6 + x];
                    if (id != 0) {
                        float *p = &patchColors[ (y * 6 + x) * colorChannels];
                        cnt += snprintf(tmpMsg + cnt, 256 - cnt,
                                        "\t(%.3f,%.3f,%.3f)", p[0], p[1], p[2]);
                    } else {
                        cnt += snprintf(tmpMsg + cnt, 256 - cnt,
                                        "\t(xxx,xxx,xxx)");
                    }
                }
                LOGV("%s", tmpMsg);
            }
        }
    }

    // Draw output if requested
    if (outputImage != NULL) {
        *outputImage = output;
        *outputWidth = outWidth;
        *outputHeight = outHeight;

        // Draw all candidate group bounds
        for (PixelGroup::IdSetIter group = candidateGroups.begin();
             group != candidateGroups.end();
             group++) {

            int x,y;
            (*group)->getApproxCenter(&x, &y);

            // Draw candidate bounding box
            int x0, y0, x1, y1;
            (*group)->getBoundingBox(&x0, &y0, &x1, &y1);
            drawLine(output, outWidth,
                     x0, y0, x1, y0,
                     255, 0, 0);
            drawLine(output, outWidth,
                     x1, y0, x1, y1,
                     255, 0, 0);
            drawLine(output, outWidth,
                     x1, y1, x0, y1,
                     255, 0, 0);
            drawLine(output, outWidth,
                     x0, y1, x0, y0,
                     255, 0, 0);

            // Draw lines between neighbors
            // Red for to-right and to-below of me connections
            const PixelGroup *neighbor;
            if ( (neighbor = (*group)->getRNeighbor()) != NULL) {
                int nX, nY;
                neighbor->getApproxCenter(&nX, &nY);
                drawLine(output,
                         outWidth,
                         x, y, nX, nY,
                         255, -1, -1);
            }
            if ( (neighbor = (*group)->getDNeighbor()) != NULL) {
                int nX, nY;
                neighbor->getApproxCenter(&nX, &nY);
                drawLine(output,
                         outWidth,
                         x, y, nX, nY,
                         255, -1, -1);
            }
            // Blue for to-left or to-above of me connections
            if ( (neighbor = (*group)->getLNeighbor()) != NULL) {
                int nX, nY;
                neighbor->getApproxCenter(&nX, &nY);
                drawLine(output,
                         outWidth,
                         x, y, nX, nY,
                         -1, -1, 255);
            }
            if ( (neighbor = (*group)->getUNeighbor()) != NULL) {
                int nX, nY;
                neighbor->getApproxCenter(&nX, &nY);
                drawLine(output,
                         outWidth,
                         x, y, nX, nY,
                         -1, -1, 255);
            }
        }

        // Mark found grid patch pixels
        if (bestError != -1) {
            for (int x=0; x < 6; x++) {
                for (int y =0; y < 4; y++) {
                    int id = bestGrid[y * 6 + x];
                    if (id != 0) {
                        int x0, y0, x1, y1;
                        groups[id - 1].getBoundingBox(&x0, &y0, &x1, &y1);
                        // Fill patch pixels with blue
                        for (int px = x0; px < x1; px++) {
                            for (int py = y0; py < y1; py++) {
                                if (outField(py,px).id != id) continue;
                                unsigned char *p =
                                        &output[(py * outWidth + px)
                                                * totalChannels];
                                p[0] = 0;
                                p[1] = 0;
                                p[2] = 255;

                            }
                        }
                        drawLine(output, outWidth,
                                 x0, y0, x1, y1,
                                 0, 255, 0);
                        drawLine(output, outWidth,
                                 x0, y1, x0, y1,
                                 0, 255, 0);
                    }
                }
            }
        }

    } else {
        delete output;
    }

    int64_t endTime = systemTime();
    LOGV("Process time: %f ms",
         (endTime - startTime) / 1000000.);

    if (bestError == -1) return false;

    return true;
}
