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

#include <vector>
#include <utility>

#ifndef FILTERPACK_CALIBRATION_GROUPING_H
#define FILTERPACK_CALIBRATION_GROUPING_H

// To use the Grouping function, derive one class from Field.
// Field class provides the interface for the Grouping function.
// FF_ID is the pixel class used to compare values,
//  != operator must be defined in this class
//  region number of the pixel

typedef std::vector <std::vector<int> > MASK;
typedef std::pair<int, int> POS;
// FF_ID needs to implement the operator !=
// bool  operator != (const FF_ID &id)
template <class FF_ID>
class Field {
  public:
    int id_no;
    MASK mask;
    virtual FF_ID operator () (int y, int x) const =0 ;
    virtual int getWidth()  const = 0;
    virtual int getHeight()  const= 0;
    virtual ~Field() {}
};

template < class FF_ID>
void FloodFill(int sx,
               int sy,
               int id_no,
               const FF_ID &id,
               Field<FF_ID> *pField,
               POS *new_pos) {
    std::vector<POS> stack;
    stack.push_back(POS(sx,sy));
    while (stack.size() > 0) {
        sx = stack.back().first;
        sy = stack.back().second;
        stack.pop_back();

        // fill the current line
        int x;
        for (x = sx-1; x >= 0; x--)
        {
            if (pField->mask[sy][x]!=0) break;
            if (id != (*pField)(sy,x)) {
                new_pos->first = x;
                new_pos->second =sy;
                break;
            }
            pField->mask[sy][x] = id_no;
        }
        int startx = x;
        for (x = sx;x < pField->getWidth(); x++) {
            if (pField->mask[sy][x]!=0) break;
            if (id != (*pField)(sy,x)) {
                new_pos->first = x;
                new_pos->second =sy;
                break;
            }
            pField->mask[sy][x] = id_no;
        }
        int endx = x;
        if (endx >= pField->getWidth()) endx = pField->getWidth() - 1;
        if (startx < 0) startx = 0;
        // push the adjacent spans to the stack
        if (sy>0) {
            int bNew = true;
            for (x = endx; x >= startx; x--) {
                if (pField->mask[sy-1][x] != 0 || id != (*pField)(sy-1,x) ) {
                    bNew = true;
                    continue;
                }
                if (bNew) {
                    stack.push_back( POS(x, sy-1));
                    bNew = false;
                }
            }
        }
        if (sy < (pField->getHeight() - 1)) {
            int bNew = true;
            for (x = endx; x >= startx; x--) {
                if (pField->mask[sy+1][x]!=0 || id != (*pField)(sy+1,x)) {
                    bNew = true;
                    continue;
                }
                if (bNew) {
                    stack.push_back( POS(x, sy+1));
                    bNew = false;
                }
            }
        }
    }
}

// Group the pixels in Field based on the FF_ID != operator.
// All pixels will be labeled from 1. The total number of unique groups(regions)
// is (pField->id_no - 1) after the call
// The labeasl of the pixels are stored in the mask member of Field.

template <class FF_ID>
void Grouping(Field <FF_ID> *pField) {
    int width = pField->getWidth();
    int height = pField->getHeight();
    pField->mask =  MASK(height, std::vector<int> (width, 0) );

    FF_ID id;
    pField->id_no = 1;
    int sx = width / 2, sy = height / 2;
    POS new_pos(-1,-1);
    while (1) {
        id = (*pField)(sy,sx);
        int id_no = pField->id_no;
        new_pos.first = -1;
        new_pos.second = -1;
        FloodFill(sx, sy, id_no, id, pField, &new_pos);
        if (new_pos.first < 0) // no new position found, during the flood fill
        {
            const int kNumOfRetries = 10;
            // try 10 times for the new unfilled position
            for (int i = 0; i < kNumOfRetries; i++) {
                sx = rand() % width;
                sy = rand() % height;
                if (pField->mask[sy][sx] == 0) {
                    new_pos.first = sx;
                    new_pos.second = sy;
                    break;
                }
            }
            if (new_pos.first < 0) { // still failed, search the whole image
                for (int y = 0; y < height && new_pos.first < 0; y++)
                    for (int x = 0; x < width; x++) {
                        if (pField->mask[y][x] == 0) {
                            new_pos.first = x;
                            new_pos.second = y;
                            break;
                        }
                    }
            }
            if (new_pos.first < 0) break; // finished
        }
        sx = new_pos.first;
        sy = new_pos.second;
        pField->id_no++;
    }
}

#endif
