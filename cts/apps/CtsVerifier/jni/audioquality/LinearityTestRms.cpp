/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

/* This test accepts a collection of N speech waveforms collected as
   part of N recognition attempts.  The waveforms are ordered by
   increasing presentation level.  The test determines the extent to
   which the peak amplitudes in the waveforms track the change in
   presentation level.  Failure to track the presentation level within
   some reasonable margin is an indication of clipping or of automatic
   gain control in the signal path.

   RMS of each level is used as a parameter for deciding lienairy.
   For each level, RMS is calculated, and a line fitting into RMS vs level
   will be calculated. Then for each level, residual error of measurement
   vs line fitting will be calculated, and the residual error is normalized
   with each measurement. The test failes if residual error is bigger than
   2dB.

   This test will be robust to background noise as long as it is persistent.
   But background noise which appears shortly with enough strength can
   mess up the result.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cutils/log.h>
#include "LinearityTest.h"

#define LOG_TAG "LinearityTestRms"

// vectorDot, vectorNorm, and solveLeastSquares
// copied from frameworks/base/libs/ui/Input.cpp

static inline float vectorDot(const float* a, const float* b, uint32_t m) {
    float r = 0;
    while (m--) {
        r += *(a++) * *(b++);
    }
    return r;
}

static inline float vectorNorm(const float* a, uint32_t m) {
    float r = 0;
    while (m--) {
        float t = *(a++);
        r += t * t;
    }
    return sqrtf(r);
}

/**
 * Solves a linear least squares problem to obtain a N degree polynomial that fits
 * the specified input data as nearly as possible.
 *
 * Returns true if a solution is found, false otherwise.
 *
 * The input consists of two vectors of data points X and Y with indices 0..m-1.
 * The output is a vector B with indices 0..n-1 that describes a polynomial
 * that fits the data, such the sum of abs(Y[i] - (B[0] + B[1] X[i] + B[2] X[i]^2 ... B[n] X[i]^n))
 * for all i between 0 and m-1 is minimized.
 *
 * That is to say, the function that generated the input data can be approximated
 * by y(x) ~= B[0] + B[1] x + B[2] x^2 + ... + B[n] x^n.
 *
 * The coefficient of determination (R^2) is also returned to describe the goodness
 * of fit of the model for the given data.  It is a value between 0 and 1, where 1
 * indicates perfect correspondence.
 *
 * This function first expands the X vector to a m by n matrix A such that
 * A[i][0] = 1, A[i][1] = X[i], A[i][2] = X[i]^2, ..., A[i][n] = X[i]^n.
 *
 * Then it calculates the QR decomposition of A yielding an m by m orthonormal matrix Q
 * and an m by n upper triangular matrix R.  Because R is upper triangular (lower
 * part is all zeroes), we can simplify the decomposition into an m by n matrix
 * Q1 and a n by n matrix R1 such that A = Q1 R1.
 *
 * Finally we solve the system of linear equations given by R1 B = (Qtranspose Y)
 * to find B.
 *
 * For efficiency, we lay out A and Q column-wise in memory because we frequently
 * operate on the column vectors.  Conversely, we lay out R row-wise.
 *
 * http://en.wikipedia.org/wiki/Numerical_methods_for_linear_least_squares
 * http://en.wikipedia.org/wiki/Gram-Schmidt
 */
static bool solveLeastSquares(const float* x, const float* y, uint32_t m, uint32_t n,
        float* outB, float* outDet) {
#if DEBUG_LEAST_SQUARES
    LOGD("solveLeastSquares: m=%d, n=%d, x=%s, y=%s", int(m), int(n),
            vectorToString(x, m).string(), vectorToString(y, m).string());
#endif

    // Expand the X vector to a matrix A.
    float a[n][m]; // column-major order
    for (uint32_t h = 0; h < m; h++) {
        a[0][h] = 1;
        for (uint32_t i = 1; i < n; i++) {
            a[i][h] = a[i - 1][h] * x[h];
        }
    }
#if DEBUG_LEAST_SQUARES
    LOGD("  - a=%s", matrixToString(&a[0][0], m, n, false /*rowMajor*/).string());
#endif

    // Apply the Gram-Schmidt process to A to obtain its QR decomposition.
    float q[n][m]; // orthonormal basis, column-major order
    float r[n][n]; // upper triangular matrix, row-major order
    for (uint32_t j = 0; j < n; j++) {
        for (uint32_t h = 0; h < m; h++) {
            q[j][h] = a[j][h];
        }
        for (uint32_t i = 0; i < j; i++) {
            float dot = vectorDot(&q[j][0], &q[i][0], m);
            for (uint32_t h = 0; h < m; h++) {
                q[j][h] -= dot * q[i][h];
            }
        }

        float norm = vectorNorm(&q[j][0], m);
        if (norm < 0.000001f) {
            // vectors are linearly dependent or zero so no solution
#if DEBUG_LEAST_SQUARES
            LOGD("  - no solution, norm=%f", norm);
#endif
            return false;
        }

        float invNorm = 1.0f / norm;
        for (uint32_t h = 0; h < m; h++) {
            q[j][h] *= invNorm;
        }
        for (uint32_t i = 0; i < n; i++) {
            r[j][i] = i < j ? 0 : vectorDot(&q[j][0], &a[i][0], m);
        }
    }
#if DEBUG_LEAST_SQUARES
    LOGD("  - q=%s", matrixToString(&q[0][0], m, n, false /*rowMajor*/).string());
    LOGD("  - r=%s", matrixToString(&r[0][0], n, n, true /*rowMajor*/).string());

    // calculate QR, if we factored A correctly then QR should equal A
    float qr[n][m];
    for (uint32_t h = 0; h < m; h++) {
        for (uint32_t i = 0; i < n; i++) {
            qr[i][h] = 0;
            for (uint32_t j = 0; j < n; j++) {
                qr[i][h] += q[j][h] * r[j][i];
            }
        }
    }
    LOGD("  - qr=%s", matrixToString(&qr[0][0], m, n, false /*rowMajor*/).string());
#endif

    // Solve R B = Qt Y to find B.  This is easy because R is upper triangular.
    // We just work from bottom-right to top-left calculating B's coefficients.
    for (uint32_t i = n; i-- != 0; ) {
        outB[i] = vectorDot(&q[i][0], y, m);
        for (uint32_t j = n - 1; j > i; j--) {
            outB[i] -= r[i][j] * outB[j];
        }
        outB[i] /= r[i][i];
    }
#if DEBUG_LEAST_SQUARES
    LOGD("  - b=%s", vectorToString(outB, n).string());
#endif

    // Calculate the coefficient of determination as 1 - (SSerr / SStot) where
    // SSerr is the residual sum of squares (squared variance of the error),
    // and SStot is the total sum of squares (squared variance of the data).
    float ymean = 0;
    for (uint32_t h = 0; h < m; h++) {
        ymean += y[h];
    }
    ymean /= m;

    float sserr = 0;
    float sstot = 0;
    for (uint32_t h = 0; h < m; h++) {
        float err = y[h] - outB[0];
        float term = 1;
        for (uint32_t i = 1; i < n; i++) {
            term *= x[h];
            err -= term * outB[i];
        }
        sserr += err * err;
        float var = y[h] - ymean;
        sstot += var * var;
    }
    *outDet = sstot > 0.000001f ? 1.0f - (sserr / sstot) : 1;
#if DEBUG_LEAST_SQUARES
    LOGD("  - sserr=%f", sserr);
    LOGD("  - sstot=%f", sstot);
    LOGD("  - det=%f", *outDet);
#endif
    return true;
}

/* calculate RMS of given sample with numSamples of length */
float calcRms(short* pcm, int numSamples)
{
    float energy = 0.0f;
    for(int i = 0; i < numSamples; i++) {
        float sample = (float)pcm[i];
        energy += (sample * sample);
    }
    return sqrtf(energy);
}

/* There are numSignals int16 signals in pcms.  sampleCounts is an
   integer array of length numSignals containing their respective
   lengths in samples.  They are all sampled at sampleRate.  The pcms
   are ordered by increasing stimulus level.  The level steps between
   successive stimuli were of size dbStepSize dB.
   The maximum deviation in linearity found
   (in dB) is returned in maxDeviation.  The function returns 1 if
   the measurements could be made, or a negative number that
   indicates the error, as defined in LinearityTest.h */
int linearityTestRms(short** pcms, int* sampleCounts, int numSignals,
                  float sampleRate, float dbStepSize,
                  float* maxDeviation) {
    if (!(pcms && sampleCounts)) {
        return ERROR_INPUT_SIGNAL_MISSING;
    }
    if (numSignals < 2) {
      return ERROR_INPUT_SIGNAL_NUMBERS;
    }
    if (sampleRate <= 4000.0) {
      return ERROR_SAMPLE_RATE_TOO_LOW;
    }
    if (dbStepSize <= 0.0) {
        return ERROR_NEGATIVE_STEP_SIZE;
    }

    float* levels = new float[numSignals];
    levels[0] = 1.0f;
    float stepInMag = powf(10.0f, dbStepSize/20.0f);
    for(int i = 1; i < numSignals; i++) {
        levels[i] = levels[i - 1] * stepInMag;
    }

    float* rmsValues = new float[numSignals];
    for (int i = 0; i < numSignals; i++) {
        rmsValues[i] = calcRms(pcms[i], sampleCounts[i]);
    }
    const int NO_COEFFS = 2; // only line fitting
    float coeffs[NO_COEFFS];
    float outDet;
    if(!solveLeastSquares(levels, rmsValues, numSignals, NO_COEFFS,
                          coeffs, &outDet)) {
        LOGI(" solveLeastSquares fails with det %f", outDet);
        return ERROR_LINEAR_FITTING;
    }
    LOGI(" coeffs offset %f linear %f", coeffs[0], coeffs[1]);
    float maxDev = 0.0f;
    for(int i = 0; i < numSignals; i++) {
        float residue = coeffs[0] + coeffs[1] * levels[i] - rmsValues[i];
        // to make devInDb positive, add measured value itself
        // then normalize
        float devInDb = 20.0f * log10f((fabs(residue) + rmsValues[i])
                                       / rmsValues[i]);
        LOGI(" %d-th residue %f dB", i, devInDb);
        if (devInDb > maxDev) {
            maxDev = devInDb;
        }
    }
    *maxDeviation = maxDev;

    delete[] levels;
    delete[] rmsValues;

    return 1;
}
