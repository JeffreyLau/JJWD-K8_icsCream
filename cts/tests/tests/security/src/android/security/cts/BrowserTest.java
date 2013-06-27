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

package android.security.cts;

import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;
import android.test.AndroidTestCase;
import android.webkit.cts.CtsTestServer;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Test for browsers which share state across multiple javascript intents.
 * Such browsers may be vulnerable to a data stealing attack.
 *
 * In particular, this test detects CVE-2011-2357.  Patches for CVE-2011-2357
 * are available at:
 *
 * http://android.git.kernel.org/?p=platform/packages/apps/Browser.git;a=commit;h=afa4ab1e4c1d645e34bd408ce04cadfd2e5dae1e
 * http://android.git.kernel.org/?p=platform/packages/apps/Browser.git;a=commit;h=096bae248453abe83cbb2e5a2c744bd62cdb620b
 *
 * See also: http://web.nvd.nist.gov/view/vuln/detail?vulnId=CVE-2011-2357
 */
public class BrowserTest extends AndroidTestCase {
    private CtsTestServer mWebServer;

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mWebServer = new CtsTestServer(mContext);
    }

    @Override
    protected void tearDown() throws Exception {
        mWebServer.shutdown();
        super.tearDown();
    }

    /**
     * Verify that no state is preserved across multiple intents sent
     * to the browser when we reuse a browser tab.
     *
     * In this test, we send two intents to the Android browser. The first
     * intent sets document.b2 to 1.  The second intent attempts to read
     * document.b2.  If the read is successful, then state was preserved
     * across the two intents.
     *
     * If state is preserved across browser tabs, we ask
     * the browser to send an HTTP request to our local server.
     */
    public void testTabReuse() throws InterruptedException {
        List<Intent> intents = getAllJavascriptIntents();
        for (Intent i : intents) {
            mContext.startActivity(i);
            mContext.startActivity(i);

            /*
             * Wait 5 seconds for the browser to contact the server, but
             * fail fast if we detect the bug
             */
            for (int j = 0; j < 5; j++) {
                assertEquals("javascript handler preserves state across "
                        + "multiple intents. Vulnerable to CVE-2011-2357?",
                        0, mWebServer.getRequestCount());
                Thread.sleep(1000);
            }
        }
    }

    /**
     * Verify that no state is preserved across multiple intents sent
     * to the browser when we run out of usable browser tabs.
     *
     * In this test, we send 20 intents to the Android browser.  Each
     * intent sets the variable "document.b1" equal to 1.  If we are able
     * read document.b1 in subsequent invocations of the intent, then
     * we know state was preserved.  In that case, we send a message
     * to the local server, recording this fact.
     *
     * Our test fails if the local server ever receives an HTTP request.
     */
    public void testTabExhaustion() throws InterruptedException {
        List<Intent> intents = getAllJavascriptIntents();
        for (Intent i : intents) {
            i.addFlags(Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT);

            /*
             * Send 20 intents.  20 is greater than the maximum number
             * of tabs allowed by the Android browser.
             */
            for (int j = 0; j < 20; j++) {
                mContext.startActivity(i);
            }

            /*
             * Wait 5 seconds for the browser to contact the server, but
             * fail fast if we detect the bug
             */
            for (int j = 0; j < 5; j++) {
                assertEquals("javascript handler preserves state across "
                        + "multiple intents. Vulnerable to CVE-2011-2357?",
                        0, mWebServer.getRequestCount());
                Thread.sleep(1000);
            }
        }
    }

    /**
     * This method returns a List of explicit Intents for all programs
     * which handle javascript URIs.
     */
    private List<Intent> getAllJavascriptIntents() {
        String localServerUri = mWebServer.getBaseUri();
        String varName = "document.b" + System.currentTimeMillis();

        /*
         * Build a javascript URL containing the following (without spaces and newlines)
         * <code>
         *    if (document.b12345 == 1) {
         *       document.location = "http://localhost:1234/";
         *    }
         *    document.b12345 = 1;
         * </code>
         */
        String javascript = "javascript:if(" + varName + "==1){"
                + "document.location=\"" + localServerUri + "\""
                + "};"
                + varName + "=1";
        Uri uri = Uri.parse(javascript);

        Intent implicit = new Intent(Intent.ACTION_VIEW);
        implicit.setData(uri);

        /* convert our implicit Intent into multiple explicit Intents */
        List<Intent> retval = new ArrayList<Intent>();
        PackageManager pm = mContext.getPackageManager();
        List<ResolveInfo> list = pm.queryIntentActivities(implicit, PackageManager.GET_META_DATA);
        for (ResolveInfo i : list) {
            Intent explicit = new Intent(Intent.ACTION_VIEW);
            explicit.setClassName(i.activityInfo.packageName, i.activityInfo.name);
            explicit.setData(uri);
            explicit.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            retval.add(explicit);
        }

        return retval;
    }
}
