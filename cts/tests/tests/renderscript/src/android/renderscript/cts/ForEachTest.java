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

package android.renderscript.cts;

import android.renderscript.Allocation;

import android.renderscript.Byte2;
import android.renderscript.Byte3;
import android.renderscript.Byte4;

import android.renderscript.Double2;
import android.renderscript.Double3;
import android.renderscript.Double4;

import android.renderscript.Element;

import android.renderscript.Float2;
import android.renderscript.Float3;
import android.renderscript.Float4;

import android.renderscript.Int2;
import android.renderscript.Int3;
import android.renderscript.Int4;

import android.renderscript.Long2;
import android.renderscript.Long3;
import android.renderscript.Long4;

import android.renderscript.RSRuntimeException;

import android.renderscript.Short2;
import android.renderscript.Short3;
import android.renderscript.Short4;

import android.renderscript.Type;

import com.android.cts.stub.R;

public class ForEachTest extends RSBaseCompute {
    /**
     * Test support for reflected forEach() as well as validation of parameters.
     */
    public void testForEach() {
        int x = 7;

        // badOut is always I8, so it is always an invalid type
        Type t = new Type.Builder(mRS, Element.I8(mRS)).setX(x).create();
        Allocation badOut = Allocation.createTyped(mRS, t);

        // I8
        ScriptC_fe_i8 fe_i8 = new ScriptC_fe_i8(mRS, mRes, R.raw.fe_i8);
        Allocation in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U8(mRS)).setX(x).create();
        Allocation out = Allocation.createTyped(mRS, t);
        fe_i8.forEach_root(in, out);
        try {
            fe_i8.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I8_2
        ScriptC_fe_i8_2 fe_i8_2 = new ScriptC_fe_i8_2(mRS,
                                                      mRes,
                                                      R.raw.fe_i8_2);
        t = new Type.Builder(mRS, Element.I8_2(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U8_2(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i8_2.forEach_root(in, out);
        try {
            fe_i8_2.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I8_3
        ScriptC_fe_i8_3 fe_i8_3 = new ScriptC_fe_i8_3(mRS,
                                                      mRes,
                                                      R.raw.fe_i8_3);
        t = new Type.Builder(mRS, Element.I8_3(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U8_3(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i8_3.forEach_root(in, out);
        try {
            fe_i8_3.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I8_4
        ScriptC_fe_i8_4 fe_i8_4 = new ScriptC_fe_i8_4(mRS,
                                                      mRes,
                                                      R.raw.fe_i8_4);
        t = new Type.Builder(mRS, Element.I8_4(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U8_4(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i8_4.forEach_root(in, out);
        try {
            fe_i8_4.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I16
        ScriptC_fe_i16 fe_i16 = new ScriptC_fe_i16(mRS, mRes, R.raw.fe_i16);
        t = new Type.Builder(mRS, Element.I16(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U16(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i16.forEach_root(in, out);
        try {
            fe_i16.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I16_2
        ScriptC_fe_i16_2 fe_i16_2 = new ScriptC_fe_i16_2(mRS,
                                                         mRes,
                                                         R.raw.fe_i16_2);
        t = new Type.Builder(mRS, Element.I16_2(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U16_2(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i16_2.forEach_root(in, out);
        try {
            fe_i16_2.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I16_3
        ScriptC_fe_i16_3 fe_i16_3 = new ScriptC_fe_i16_3(mRS,
                                                         mRes,
                                                         R.raw.fe_i16_3);
        t = new Type.Builder(mRS, Element.I16_3(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U16_3(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i16_3.forEach_root(in, out);
        try {
            fe_i16_3.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I16_4
        ScriptC_fe_i16_4 fe_i16_4 = new ScriptC_fe_i16_4(mRS,
                                                         mRes,
                                                         R.raw.fe_i16_4);
        t = new Type.Builder(mRS, Element.I16_4(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U16_4(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i16_4.forEach_root(in, out);
        try {
            fe_i16_4.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I32
        ScriptC_fe_i32 fe_i32 = new ScriptC_fe_i32(mRS, mRes, R.raw.fe_i32);
        t = new Type.Builder(mRS, Element.I32(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U32(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i32.forEach_root(in, out);
        try {
            fe_i32.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I32_2
        ScriptC_fe_i32_2 fe_i32_2 = new ScriptC_fe_i32_2(mRS,
                                                         mRes,
                                                         R.raw.fe_i32_2);
        t = new Type.Builder(mRS, Element.I32_2(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U32_2(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i32_2.forEach_root(in, out);
        try {
            fe_i32_2.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I32_3
        ScriptC_fe_i32_3 fe_i32_3 = new ScriptC_fe_i32_3(mRS,
                                                         mRes,
                                                         R.raw.fe_i32_3);
        t = new Type.Builder(mRS, Element.I32_3(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U32_3(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i32_3.forEach_root(in, out);
        try {
            fe_i32_3.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I32_4
        ScriptC_fe_i32_4 fe_i32_4 = new ScriptC_fe_i32_4(mRS,
                                                         mRes,
                                                         R.raw.fe_i32_4);
        t = new Type.Builder(mRS, Element.I32_4(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U32_4(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i32_4.forEach_root(in, out);
        try {
            fe_i32_4.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I64
        ScriptC_fe_i64 fe_i64 = new ScriptC_fe_i64(mRS, mRes, R.raw.fe_i64);
        t = new Type.Builder(mRS, Element.I64(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U64(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i64.forEach_root(in, out);
        try {
            fe_i64.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I64_2
        ScriptC_fe_i64_2 fe_i64_2 = new ScriptC_fe_i64_2(mRS,
                                                         mRes,
                                                         R.raw.fe_i64_2);
        t = new Type.Builder(mRS, Element.I64_2(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U64_2(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i64_2.forEach_root(in, out);
        try {
            fe_i64_2.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I64_3
        ScriptC_fe_i64_3 fe_i64_3 = new ScriptC_fe_i64_3(mRS,
                                                         mRes,
                                                         R.raw.fe_i64_3);
        t = new Type.Builder(mRS, Element.I64_3(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U64_3(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i64_3.forEach_root(in, out);
        try {
            fe_i64_3.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // I64_4
        ScriptC_fe_i64_4 fe_i64_4 = new ScriptC_fe_i64_4(mRS,
                                                         mRes,
                                                         R.raw.fe_i64_4);
        t = new Type.Builder(mRS, Element.I64_4(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.U64_4(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i64_4.forEach_root(in, out);
        try {
            fe_i64_4.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // F32
        ScriptC_fe_f32 fe_f32 = new ScriptC_fe_f32(mRS, mRes, R.raw.fe_f32);
        t = new Type.Builder(mRS, Element.F32(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        out = Allocation.createTyped(mRS, t);
        fe_f32.forEach_root(in, out);
        try {
            fe_f32.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // F32_2
        ScriptC_fe_f32_2 fe_f32_2 = new ScriptC_fe_f32_2(mRS,
                                                         mRes,
                                                         R.raw.fe_f32_2);
        t = new Type.Builder(mRS, Element.F32_2(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.F32_2(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_f32_2.forEach_root(in, out);
        try {
            fe_f32_2.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // F32_3
        ScriptC_fe_f32_3 fe_f32_3 = new ScriptC_fe_f32_3(mRS,
                                                         mRes,
                                                         R.raw.fe_f32_3);
        t = new Type.Builder(mRS, Element.F32_3(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        out = Allocation.createTyped(mRS, t);
        fe_f32_3.forEach_root(in, out);
        try {
            fe_f32_3.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // F32_4
        ScriptC_fe_f32_4 fe_f32_4 = new ScriptC_fe_f32_4(mRS,
                                                         mRes,
                                                         R.raw.fe_f32_4);
        t = new Type.Builder(mRS, Element.F32_4(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        out = Allocation.createTyped(mRS, t);
        fe_f32_4.forEach_root(in, out);
        try {
            fe_f32_4.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // F64
        ScriptC_fe_f64 fe_f64 = new ScriptC_fe_f64(mRS, mRes, R.raw.fe_f64);
        t = new Type.Builder(mRS, Element.F64(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        out = Allocation.createTyped(mRS, t);
        fe_f64.forEach_root(in, out);
        try {
            fe_f64.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // F64_2
        ScriptC_fe_f64_2 fe_f64_2 = new ScriptC_fe_f64_2(mRS,
                                                         mRes,
                                                         R.raw.fe_f64_2);
        t = new Type.Builder(mRS, Element.F64_2(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        out = Allocation.createTyped(mRS, t);
        fe_f64_2.forEach_root(in, out);
        try {
            fe_f64_2.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // F64_3
        ScriptC_fe_f64_3 fe_f64_3 = new ScriptC_fe_f64_3(mRS,
                                                         mRes,
                                                         R.raw.fe_f64_3);
        t = new Type.Builder(mRS, Element.F64_3(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        out = Allocation.createTyped(mRS, t);
        fe_f64_3.forEach_root(in, out);
        try {
            fe_f64_3.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // F64_4
        ScriptC_fe_f64_4 fe_f64_4 = new ScriptC_fe_f64_4(mRS,
                                                         mRes,
                                                         R.raw.fe_f64_4);
        t = new Type.Builder(mRS, Element.F64_4(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        out = Allocation.createTyped(mRS, t);
        fe_f64_4.forEach_root(in, out);
        try {
            fe_f64_4.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // fe_test (struct)
        ScriptC_fe_struct fe_struct = new ScriptC_fe_struct(mRS,
                                                            mRes,
                                                            R.raw.fe_struct);
        in = new ScriptField_fe_test(mRS, x).getAllocation();
        out = new ScriptField_fe_test(mRS, x).getAllocation();
        fe_struct.forEach_root(in, out);
        try {
            fe_struct.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // BOOLEAN
        ScriptC_fe_bool fe_bool = new ScriptC_fe_bool(mRS, mRes, R.raw.fe_bool);
        t = new Type.Builder(mRS, Element.BOOLEAN(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        out = Allocation.createTyped(mRS, t);
        fe_bool.forEach_root(in, out);
        try {
            fe_bool.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // A_8
        t = new Type.Builder(mRS, Element.I8(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.A_8(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i8.forEach_root(in, out);
        try {
            fe_i8.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // RGBA_8888
        t = new Type.Builder(mRS, Element.I8_4(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.RGBA_8888(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i8_4.forEach_root(in, out);
        try {
            fe_i8_4.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }

        // RGB_888
        t = new Type.Builder(mRS, Element.I8_3(mRS)).setX(x).create();
        in = Allocation.createTyped(mRS, t);
        t = new Type.Builder(mRS, Element.RGB_888(mRS)).setX(x).create();
        out = Allocation.createTyped(mRS, t);
        fe_i8_3.forEach_root(in, out);
        try {
            fe_i8_3.forEach_root(in, badOut);
            fail("should throw RSRuntimeException");
        } catch (RSRuntimeException e) {
        }
    }
}
