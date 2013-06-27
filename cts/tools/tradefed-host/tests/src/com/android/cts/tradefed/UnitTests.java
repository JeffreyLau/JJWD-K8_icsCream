/*
 * Copyright (C) 2010 The Android Open Source Project
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
package com.android.cts.tradefed;

import com.android.cts.tradefed.result.CtsXmlResultReporterTest;
import com.android.cts.tradefed.result.TestTest;
import com.android.cts.tradefed.testtype.CtsTestTest;
import com.android.cts.tradefed.testtype.JarHostTestTest;
import com.android.cts.tradefed.testtype.TestPackageDefTest;
import com.android.cts.tradefed.testtype.TestPackageXmlParserTest;
import com.android.cts.tradefed.testtype.TestPlanTest;

import junit.framework.Test;
import junit.framework.TestSuite;

/**
 * A test suite for all cts-tradefed unit tests.
 * <p/>
 * All tests listed here should be self-contained, and do not require any external dependencies
 * (such as a full CTS build with XML etc).
 */
public class UnitTests extends TestSuite {

    public UnitTests() {
        super();
        addTestSuite(CtsXmlResultReporterTest.class);
        addTestSuite(JarHostTestTest.class);
        addTestSuite(CtsTestTest.class);
        addTestSuite(TestPlanTest.class);
        addTestSuite(TestPackageXmlParserTest.class);
        addTestSuite(TestPackageDefTest.class);
        addTestSuite(TestTest.class);
    }

    public static Test suite() {
        return new UnitTests();
    }
}
