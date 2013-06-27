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

import android.content.Context;
import android.content.res.Resources;
import android.renderscript.RenderScript.RSMessageHandler;
import android.test.AndroidTestCase;

/**
 * Base RenderScript test class. This class provides a message handler and a
 * convenient way to wait for compute scripts to complete their execution.
 */
class RSBase extends AndroidTestCase {

    Context mCtx;
    Resources mRes;

    public int result;
    private boolean msgHandled;

    public static final int RS_MSG_TEST_PASSED = 100;
    public static final int RS_MSG_TEST_FAILED = 101;

    RSMessageHandler mRsMessage = new RSMessageHandler() {
        public void run() {
            if (result == 0) {
                switch (mID) {
                    case RS_MSG_TEST_PASSED:
                    case RS_MSG_TEST_FAILED:
                        result = mID;
                        break;
                    default:
                        fail("Got unexpected RS message");
                        return;
                }
            }
            msgHandled = true;
        }
    };

    protected void waitForMessage() {
        while (!msgHandled) {
            Thread.yield();
        }
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        result = 0;
        msgHandled = false;
        mCtx = getContext();
        mRes = mCtx.getResources();
    }
}
