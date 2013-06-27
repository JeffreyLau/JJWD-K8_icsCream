/*
 * Copyright (C) 2009 The Android Open Source Project
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

package android.webkit.cts;

import dalvik.annotation.BrokenTest;
import dalvik.annotation.TestLevel;
import dalvik.annotation.TestTargetClass;
import dalvik.annotation.TestTargetNew;
import dalvik.annotation.TestTargets;
import dalvik.annotation.ToBeFixed;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Picture;
import android.graphics.Rect;
import android.graphics.Bitmap.Config;
import android.net.Uri;
import android.net.http.SslCertificate;
import android.net.http.SslError;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.test.ActivityInstrumentationTestCase2;
import android.test.UiThreadTest;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.cts.DelayedCheck;
import android.webkit.CacheManager;
import android.webkit.CacheManager.CacheResult;
import android.webkit.ConsoleMessage;
import android.webkit.DownloadListener;
import android.webkit.SslErrorHandler;
import android.webkit.WebBackForwardList;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebViewDatabase;
import android.webkit.WebView.HitTestResult;
import android.webkit.WebView.PictureListener;
import android.widget.AutoCompleteTextView;
import android.widget.LinearLayout;

import java.io.File;
import java.io.FileInputStream;
import java.util.Date;

import junit.framework.Assert;

@TestTargetClass(android.webkit.WebView.class)
public class WebViewTest extends ActivityInstrumentationTestCase2<WebViewStubActivity> {
    private static final String LOGTAG = "WebViewTest";
    private static final int INITIAL_PROGRESS = 100;
    private static long TEST_TIMEOUT = 20000L;
    private static long TIME_FOR_LAYOUT = 1000L;

    private WebView mWebView;
    private CtsTestServer mWebServer;
    private boolean mIsUiThreadDone;

    public WebViewTest() {
        super("com.android.cts.stub", WebViewStubActivity.class);
    }

    @Override
    public void runTestOnUiThread(Runnable runnable) throws Throwable {
        mIsUiThreadDone = false;
        super.runTestOnUiThread(runnable);
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mWebView = getActivity().getWebView();
        File f = getActivity().getFileStreamPath("snapshot");
        if (f.exists()) {
            f.delete();
        }
    }

    @Override
    protected void tearDown() throws Exception {
        try {
            runTestOnUiThread(new Runnable() {
                public void run() {
                    mWebView.clearHistory();
                    mWebView.clearCache(true);
                }
            });
        } catch(Throwable t) {
            Log.w(LOGTAG, "tearDown(): Caught exception when posting Runnable to UI thread");
        }
        if (mWebServer != null) {
            mWebServer.shutdown();
        }
        super.tearDown();
    }

    private void startWebServer(boolean secure) throws Exception {
        assertNull(mWebServer);
        mWebServer = new CtsTestServer(getActivity(), secure);
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "WebView",
            args = {Context.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "WebView",
            args = {Context.class, AttributeSet.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "WebView",
            args = {Context.class, AttributeSet.class, int.class}
        )
    })
    @UiThreadTest
    public void testConstructor() {
        new WebView(getActivity());
        new WebView(getActivity(), null);
        new WebView(getActivity(), null, 0);
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "findAddress",
        args = {String.class}
    )
    @UiThreadTest
    public void testFindAddress() {
        /*
         * Info about USPS
         * http://en.wikipedia.org/wiki/Postal_address#United_States
         * http://www.usps.com/
         */
        // full address
        assertEquals("455 LARKSPUR DRIVE CALIFORNIA SPRINGS CALIFORNIA 92926",
                WebView.findAddress("455 LARKSPUR DRIVE CALIFORNIA SPRINGS CALIFORNIA 92926"));
        // full address ( with abbreviated street type and state)
        assertEquals("455 LARKSPUR DR CALIFORNIA SPRINGS CA 92926",
                WebView.findAddress("455 LARKSPUR DR CALIFORNIA SPRINGS CA 92926"));
        // misspell the state ( CALIFORNIA -> CALIFONIA )
        assertNull(WebView.findAddress("455 LARKSPUR DRIVE CALIFORNIA SPRINGS CALIFONIA 92926"));
        // without optional zip code
        assertEquals("455 LARKSPUR DR CALIFORNIA SPRINGS CA",
                WebView.findAddress("455 LARKSPUR DR CALIFORNIA SPRINGS CA"));
        // house number, street name and street type are missing
        assertNull(WebView.findAddress("CALIFORNIA SPRINGS CA"));
        // city & state are missing
        assertNull(WebView.findAddress("455 LARKSPUR DR"));
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.SUFFICIENT,
            method = "getZoomControls",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getSettings",
            args = {}
        )
    })
    @SuppressWarnings("deprecation")
    @UiThreadTest
    public void testGetZoomControls() {
         WebSettings settings = mWebView.getSettings();
         assertTrue(settings.supportZoom());
         View zoomControls = mWebView.getZoomControls();
         assertNotNull(zoomControls);

         // disable zoom support
         settings.setSupportZoom(false);
         assertFalse(settings.supportZoom());
         assertNull(mWebView.getZoomControls());
    }

    @TestTargetNew(
        level = TestLevel.SUFFICIENT,
        method = "invokeZoomPicker",
        args = {},
        notes = "Cannot test the effect of this method"
    )
    @UiThreadTest
    public void testInvokeZoomPicker() throws Exception {
        WebSettings settings = mWebView.getSettings();
        assertTrue(settings.supportZoom());
        startWebServer(false);
        String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        assertLoadUrlSuccessfully(url);
        mWebView.invokeZoomPicker();
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "zoomIn",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "zoomOut",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getScale",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getSettings",
            args = {}
        )
    })
    @UiThreadTest
    public void testZoom() {
        WebSettings settings = mWebView.getSettings();
        settings.setSupportZoom(false);
        assertFalse(settings.supportZoom());
        float currScale = mWebView.getScale();
        float previousScale = currScale;

        // can zoom in or out although zoom support is disabled in web settings
        assertTrue(mWebView.zoomIn());
        currScale = mWebView.getScale();
        assertTrue(currScale > previousScale);

        // zoom in
        assertTrue(mWebView.zoomOut());
        previousScale = currScale;
        currScale = mWebView.getScale();
        assertTrue(currScale < previousScale);

        // enable zoom support
        settings.setSupportZoom(true);
        assertTrue(settings.supportZoom());
        currScale = mWebView.getScale();

        assertTrue(mWebView.zoomIn());
        previousScale = currScale;
        currScale = mWebView.getScale();
        assertTrue(currScale > previousScale);

        // zoom in until it reaches maximum scale
        while (currScale > previousScale) {
            mWebView.zoomIn();
            previousScale = currScale;
            currScale = mWebView.getScale();
        }

        // can not zoom in further
        assertFalse(mWebView.zoomIn());
        previousScale = currScale;
        currScale = mWebView.getScale();
        assertEquals(currScale, previousScale);

        // zoom out
        assertTrue(mWebView.zoomOut());
        previousScale = currScale;
        currScale = mWebView.getScale();
        assertTrue(currScale < previousScale);

        // zoom out until it reaches minimum scale
        while (currScale < previousScale) {
            mWebView.zoomOut();
            previousScale = currScale;
            currScale = mWebView.getScale();
        }

        // can not zoom out further
        assertFalse(mWebView.zoomOut());
        previousScale = currScale;
        currScale = mWebView.getScale();
        assertEquals(currScale, previousScale);
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setScrollBarStyle",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "overlayHorizontalScrollbar",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "overlayVerticalScrollbar",
            args = {}
        )
    })
    @UiThreadTest
    public void testSetScrollBarStyle() {
        mWebView.setScrollBarStyle(View.SCROLLBARS_INSIDE_INSET);
        assertFalse(mWebView.overlayHorizontalScrollbar());
        assertFalse(mWebView.overlayVerticalScrollbar());

        mWebView.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);
        assertTrue(mWebView.overlayHorizontalScrollbar());
        assertTrue(mWebView.overlayVerticalScrollbar());

        mWebView.setScrollBarStyle(View.SCROLLBARS_OUTSIDE_INSET);
        assertFalse(mWebView.overlayHorizontalScrollbar());
        assertFalse(mWebView.overlayVerticalScrollbar());

        mWebView.setScrollBarStyle(View.SCROLLBARS_OUTSIDE_OVERLAY);
        assertTrue(mWebView.overlayHorizontalScrollbar());
        assertTrue(mWebView.overlayVerticalScrollbar());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setHorizontalScrollbarOverlay",
            args = {boolean.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setVerticalScrollbarOverlay",
            args = {boolean.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "overlayHorizontalScrollbar",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "overlayVerticalScrollbar",
            args = {}
        )
    })
    @UiThreadTest
    public void testScrollBarOverlay() throws Throwable {
        mWebView.setHorizontalScrollbarOverlay(true);
        mWebView.setVerticalScrollbarOverlay(false);
        assertTrue(mWebView.overlayHorizontalScrollbar());
        assertFalse(mWebView.overlayVerticalScrollbar());

        mWebView.setHorizontalScrollbarOverlay(false);
        mWebView.setVerticalScrollbarOverlay(true);
        assertFalse(mWebView.overlayHorizontalScrollbar());
        assertTrue(mWebView.overlayVerticalScrollbar());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "loadUrl",
            args = {String.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getUrl",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getOriginalUrl",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getProgress",
            args = {}
        )
    })
    @UiThreadTest
    public void testLoadUrl() throws Exception {
        assertNull(mWebView.getUrl());
        assertNull(mWebView.getOriginalUrl());
        assertEquals(INITIAL_PROGRESS, mWebView.getProgress());

        startWebServer(false);
        String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        mWebView.loadUrl(url);
        waitForLoadComplete();
        assertEquals(100, mWebView.getProgress());
        assertEquals(url, mWebView.getUrl());
        assertEquals(url, mWebView.getOriginalUrl());
        assertEquals(TestHtmlConstants.HELLO_WORLD_TITLE, mWebView.getTitle());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getUrl",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getOriginalUrl",
            args = {}
        )
    })
    public void testGetOriginalUrl() throws Throwable {
        startWebServer(false);
        final String finalUrl = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        final String redirectUrl =
                mWebServer.getRedirectingAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);

        runTestOnUiThread(new Runnable() {
            public void run() {
                assertNull(mWebView.getUrl());
                assertNull(mWebView.getOriginalUrl());

                // By default, WebView sends an intent to ask the system to
                // handle loading a new URL. We set a WebViewClient as
                // WebViewClient.shouldOverrideUrlLoading() returns false, so
                // the WebView will load the new URL.
                mWebView.setWebViewClient(new WebViewClient());
                mWebView.setWebChromeClient(new LoadCompleteWebChromeClient());
                mWebView.loadUrl(redirectUrl);
            }
        });

        // We need to yield the UI thread to allow the callback to
        // WebViewClient.shouldOverrideUrlLoading() to be made.
        waitForUiThreadDone();

        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals(finalUrl, mWebView.getUrl());
                assertEquals(redirectUrl, mWebView.getOriginalUrl());
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "stopLoading",
        args = {}
    )
    @UiThreadTest
    public void testStopLoading() throws Exception {
        assertNull(mWebView.getUrl());
        assertEquals(INITIAL_PROGRESS, mWebView.getProgress());

        startWebServer(false);
        String url = mWebServer.getDelayedAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        mWebView.loadUrl(url);
        mWebView.stopLoading();
        new DelayedCheck() {
            @Override
            protected boolean check() {
                return 100 == mWebView.getProgress();
            }
        }.run();
        assertNull(mWebView.getUrl());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "canGoBack",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "canGoForward",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "canGoBackOrForward",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "goBack",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "goForward",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "goBackOrForward",
            args = {int.class}
        )
    })
    @UiThreadTest
    public void testGoBackAndForward() throws Exception {
        assertGoBackOrForwardBySteps(false, -1);
        assertGoBackOrForwardBySteps(false, 1);

        startWebServer(false);
        String url1 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL1);
        String url2 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL2);
        String url3 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL3);

        assertLoadUrlSuccessfully(url1);
        delayedCheckWebBackForwardList(url1, 0, 1);
        assertGoBackOrForwardBySteps(false, -1);
        assertGoBackOrForwardBySteps(false, 1);

        assertLoadUrlSuccessfully(url2);
        delayedCheckWebBackForwardList(url2, 1, 2);
        assertGoBackOrForwardBySteps(true, -1);
        assertGoBackOrForwardBySteps(false, 1);

        assertLoadUrlSuccessfully(url3);
        delayedCheckWebBackForwardList(url3, 2, 3);
        assertGoBackOrForwardBySteps(true, -2);
        assertGoBackOrForwardBySteps(false, 1);

        mWebView.goBack();
        delayedCheckWebBackForwardList(url2, 1, 3);
        assertGoBackOrForwardBySteps(true, -1);
        assertGoBackOrForwardBySteps(true, 1);

        mWebView.goForward();
        delayedCheckWebBackForwardList(url3, 2, 3);
        assertGoBackOrForwardBySteps(true, -2);
        assertGoBackOrForwardBySteps(false, 1);

        mWebView.goBackOrForward(-2);
        delayedCheckWebBackForwardList(url1, 0, 3);
        assertGoBackOrForwardBySteps(false, -1);
        assertGoBackOrForwardBySteps(true, 2);

        mWebView.goBackOrForward(2);
        delayedCheckWebBackForwardList(url3, 2, 3);
        assertGoBackOrForwardBySteps(true, -2);
        assertGoBackOrForwardBySteps(false, 1);
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "addJavascriptInterface",
        args = {Object.class, String.class}
    )
    @UiThreadTest
    public void testAddJavascriptInterface() throws Exception {
        WebSettings settings = mWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setJavaScriptCanOpenWindowsAutomatically(true);

        final class DummyJavaScriptInterface {
            private boolean mWasProvideResultCalled;
            private String mResult;

            private synchronized String waitForResult() {
                while (!mWasProvideResultCalled) {
                    try {
                        wait(TEST_TIMEOUT);
                    } catch (InterruptedException e) {
                        continue;
                    }
                    if (!mWasProvideResultCalled) {
                        Assert.fail("Unexpected timeout");
                    }
                }
                return mResult;
            }

            public synchronized boolean wasProvideResultCalled() {
                return mWasProvideResultCalled;
            }

            public synchronized void provideResult(String result) {
                mWasProvideResultCalled = true;
                mResult = result;
                notify();
            }
        }

        final DummyJavaScriptInterface obj = new DummyJavaScriptInterface();
        mWebView.addJavascriptInterface(obj, "dummy");
        assertFalse(obj.wasProvideResultCalled());

        startWebServer(false);
        String url = mWebServer.getAssetUrl(TestHtmlConstants.ADD_JAVA_SCRIPT_INTERFACE_URL);
        assertLoadUrlSuccessfully(url);
        assertEquals("Original title", obj.waitForResult());
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "addJavascriptInterface",
        args = {Object.class, String.class}
    )
    @UiThreadTest
    public void testAddJavascriptInterfaceNullObject() throws Exception {
        WebSettings settings = mWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        String setTitleToPropertyTypeHtml = "<html><head></head>" +
                "<body onload=\"document.title = typeof window.injectedObject;\"></body></html>";

        // Test that the property is initially undefined.
        mWebView.loadData(setTitleToPropertyTypeHtml, "text/html", null);
        waitForLoadComplete();
        assertEquals("undefined", mWebView.getTitle());

        // Test that adding a null object has no effect.
        mWebView.addJavascriptInterface(null, "injectedObject");
        mWebView.loadData(setTitleToPropertyTypeHtml, "text/html", null);
        waitForLoadComplete();
        assertEquals("undefined", mWebView.getTitle());

        // Test that adding an object gives an object type.
        final Object obj = new Object();
        mWebView.addJavascriptInterface(obj, "injectedObject");
        mWebView.loadData(setTitleToPropertyTypeHtml, "text/html", null);
        waitForLoadComplete();
        assertEquals("object", mWebView.getTitle());

        // Test that trying to replace with a null object has no effect.
        mWebView.addJavascriptInterface(null, "injectedObject");
        mWebView.loadData(setTitleToPropertyTypeHtml, "text/html", null);
        waitForLoadComplete();
        assertEquals("object", mWebView.getTitle());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "addJavascriptInterface",
            args = {Object.class, String.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "removeJavascriptInterface",
            args = {String.class}
        )
    })
    @UiThreadTest
    public void testAddJavascriptInterfaceOddName() throws Exception {
        WebSettings settings = mWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        final Object obj = new Object();

        // We should be able to use any character other than a single quote.
        // TODO: We currently fail when the name contains '#', '\', '\n' or '\r'.
        // See b/3279426
        //String oddNames[] = {" x y ", "`!\"$%^&*()-=_+[]{};#:@~\\|,./<>?\n\r ", " ", "\n", ""};
        String oddNames[] = {" x y ", "`!\"$%^&*()-=_+[]{};:@~|,./<>? ", " ", ""};
        for (String name : oddNames) {
            String setTitleToPropertyTypeHtml = "<html><head>" +
                    "<script>function updateTitle() { document.title = typeof window['" +
                    name +
                    "']; }</script>" +
                    "</head><body onload=\"updateTitle();\"></body></html>";

            mWebView.addJavascriptInterface(obj, name);
            mWebView.loadData(Uri.encode(setTitleToPropertyTypeHtml), "text/html", null);
            waitForLoadComplete();
            assertEquals("object", mWebView.getTitle());

            mWebView.removeJavascriptInterface(name);
            mWebView.loadData(Uri.encode(setTitleToPropertyTypeHtml), "text/html", null);
            waitForLoadComplete();
            assertEquals("undefined", mWebView.getTitle());
        }
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "removeJavascriptInterface",
        args = {String.class}
    )
    @UiThreadTest
    public void testRemoveJavascriptInterface() throws Exception {
        WebSettings settings = mWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        String setTitleToPropertyTypeHtml = "<html><head></head>" +
                "<body onload=\"document.title = typeof window.injectedObject;\"></body></html>";

        // Test that adding an object gives an object type.
        mWebView.addJavascriptInterface(new Object(), "injectedObject");
        mWebView.loadData(setTitleToPropertyTypeHtml, "text/html", null);
        waitForLoadComplete();
        assertEquals("object", mWebView.getTitle());

        // Test that reloading the page after removing the object leaves the property undefined.
        mWebView.removeJavascriptInterface("injectedObject");
        mWebView.loadData(setTitleToPropertyTypeHtml, "text/html", null);
        waitForLoadComplete();
        assertEquals("undefined", mWebView.getTitle());
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "removeJavascriptInterface",
        args = {String.class}
    )
    public void testUseRemovedJavascriptInterface() throws Throwable {
        class RemovedObject {
            @Override
            public String toString() {
                return "removedObject";
            }
            public void remove() throws Throwable {
                runTestOnUiThread(new Runnable() {
                    public void run() {
                        mWebView.removeJavascriptInterface("removedObject");
                        System.gc();
                    }
                });
            }
        }
        class ResultObject {
            private String mResult;
            private boolean mIsResultAvailable;
            public synchronized void setResult(String result) {
                mResult = result;
                mIsResultAvailable = true;
                notify();
            }
            public synchronized String getResult() {
                while (!mIsResultAvailable) {
                    try {
                        wait();
                    } catch (InterruptedException e) {
                    }
                }
                return mResult;
            }
        }
        final ResultObject resultObject = new ResultObject();

        // Test that an object is still usable if removed while the page is in use, even if we have
        // no external references to it.
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.getSettings().setJavaScriptEnabled(true);
                mWebView.addJavascriptInterface(new RemovedObject(), "removedObject");
                mWebView.addJavascriptInterface(resultObject, "resultObject");
                mWebView.loadData("<html><head></head>" +
                        "<body onload=\"window.removedObject.remove();" +
                        "resultObject.setResult(removedObject.toString());\"></body></html>",
                        "text/html", null);
            }
        });
        assertEquals("removedObject", resultObject.getResult());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setBackgroundColor",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "capturePicture",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "reload",
            args = {}
        )
    })
    public void testCapturePicture() throws Exception, Throwable {
        startWebServer(false);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.BLANK_PAGE_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                // showing the blank page will make the picture filled with background color
                mWebView.loadUrl(url);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        class PictureRunnable implements Runnable {
            private Picture mPicture;
            public void run() {
                mPicture = mWebView.capturePicture();
                Bitmap b = Bitmap.createBitmap(mPicture.getWidth(), mPicture.getHeight(),
                        Config.ARGB_8888);
                mPicture.draw(new Canvas(b));
                // default color is white
                assertBitmapFillWithColor(b, Color.WHITE);

                mWebView.setBackgroundColor(Color.CYAN);
                mWebView.reload();
                waitForLoadComplete();
            }
            public Picture getPicture() {
                return mPicture;
            }
        }
        PictureRunnable runnable = new PictureRunnable();
        runTestOnUiThread(runnable);
        getInstrumentation().waitForIdleSync();

        // the content of the picture will not be updated automatically
        Picture picture = runnable.getPicture();
        Bitmap b = Bitmap.createBitmap(picture.getWidth(), picture.getHeight(), Config.ARGB_8888);
        picture.draw(new Canvas(b));
        assertBitmapFillWithColor(b, Color.WHITE);

        runTestOnUiThread(new Runnable() {
            public void run() {
                // update the content
                Picture p = mWebView.capturePicture();
                Bitmap b = Bitmap.createBitmap(p.getWidth(), p.getHeight(), Config.ARGB_8888);
                p.draw(new Canvas(b));
                assertBitmapFillWithColor(b, Color.CYAN);
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "setPictureListener",
        args = {PictureListener.class}
    )
    public void testSetPictureListener() throws Exception, Throwable {
        final class MyPictureListener implements PictureListener {
            public int callCount;
            public WebView webView;
            public Picture picture;

            public void onNewPicture(WebView view, Picture picture) {
                this.callCount += 1;
                this.webView = view;
                this.picture = picture;
            }
        }

        final MyPictureListener listener = new MyPictureListener();
        startWebServer(false);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setPictureListener(listener);
                assertLoadUrlSuccessfully(url);
            }
        });
        new DelayedCheck(TEST_TIMEOUT) {
            protected boolean check() {
                return listener.callCount > 0;
            }
        }.run();
        assertEquals(mWebView, listener.webView);
        assertNotNull(listener.picture);

        final int oldCallCount = listener.callCount;
        final String newUrl = mWebServer.getAssetUrl(TestHtmlConstants.SMALL_IMG_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertLoadUrlSuccessfully(newUrl);
            }
        });
        new DelayedCheck(TEST_TIMEOUT) {
            protected boolean check() {
                return listener.callCount > oldCallCount;
            }
        }.run();
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "savePicture",
            args = {Bundle.class, File.class}
        ),
        @TestTargetNew(
            level = TestLevel.SUFFICIENT,
            notes = "Cannot test whether picture has been restored correctly.",
            method = "restorePicture",
            args = {Bundle.class, File.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "reload",
            args = {}
        )
    })
    public void testSaveAndRestorePicture() throws Throwable {
        mWebView.setBackgroundColor(Color.CYAN);
        startWebServer(false);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.BLANK_PAGE_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertLoadUrlSuccessfully(url);
            }
        });
        getInstrumentation().waitForIdleSync();

        final Bundle bundle = new Bundle();
        final File f = getActivity().getFileStreamPath("snapshot");
        if (f.exists()) {
            f.delete();
        }

        try {
            assertTrue(bundle.isEmpty());
            assertEquals(0, f.length());
            runTestOnUiThread(new Runnable() {
                public void run() {
                    assertTrue(mWebView.savePicture(bundle, f));
                }
            });

            // File saving is done in a separate thread.
            new DelayedCheck() {
                @Override
                protected boolean check() {
                    return f.length() > 0;
                }
            }.run();

            assertFalse(bundle.isEmpty());

            Picture p = Picture.createFromStream(new FileInputStream(f));
            Bitmap b = Bitmap.createBitmap(p.getWidth(), p.getHeight(), Config.ARGB_8888);
            p.draw(new Canvas(b));
            assertBitmapFillWithColor(b, Color.CYAN);

            runTestOnUiThread(new Runnable() {
                public void run() {
                    mWebView.setBackgroundColor(Color.WHITE);
                    mWebView.reload();
                    waitForLoadComplete();
                }
            });
            getInstrumentation().waitForIdleSync();

            runTestOnUiThread(new Runnable() {
                public void run() {
                    Bitmap b = Bitmap.createBitmap(mWebView.getWidth(), mWebView.getHeight(),
                            Config.ARGB_8888);
                    mWebView.draw(new Canvas(b));
                    assertBitmapFillWithColor(b, Color.WHITE);

                    // restorePicture is only supported in software rendering
                    mWebView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
                    assertTrue(mWebView.restorePicture(bundle, f));
                }
            });
            getInstrumentation().waitForIdleSync();
            // Cannot test whether the picture has been restored successfully.
            // Drawing the webview into a canvas will draw white, but on the display it is cyan
        } finally {
            if (f.exists()) {
                f.delete();
            }
        }
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setHttpAuthUsernamePassword",
            args = {String.class, String.class, String.class, String.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getHttpAuthUsernamePassword",
            args = {String.class, String.class}
        )
    })
    @UiThreadTest
    public void testAccessHttpAuthUsernamePassword() {
        try {
            WebViewDatabase.getInstance(getActivity()).clearHttpAuthUsernamePassword();

            String host = "http://localhost:8080";
            String realm = "testrealm";
            String userName = "user";
            String password = "password";

            String[] result = mWebView.getHttpAuthUsernamePassword(host, realm);
            assertNull(result);

            mWebView.setHttpAuthUsernamePassword(host, realm, userName, password);
            result = mWebView.getHttpAuthUsernamePassword(host, realm);
            assertNotNull(result);
            assertEquals(userName, result[0]);
            assertEquals(password, result[1]);

            String newPassword = "newpassword";
            mWebView.setHttpAuthUsernamePassword(host, realm, userName, newPassword);
            result = mWebView.getHttpAuthUsernamePassword(host, realm);
            assertNotNull(result);
            assertEquals(userName, result[0]);
            assertEquals(newPassword, result[1]);

            String newUserName = "newuser";
            mWebView.setHttpAuthUsernamePassword(host, realm, newUserName, newPassword);
            result = mWebView.getHttpAuthUsernamePassword(host, realm);
            assertNotNull(result);
            assertEquals(newUserName, result[0]);
            assertEquals(newPassword, result[1]);

            // the user is set to null, can not change any thing in the future
            mWebView.setHttpAuthUsernamePassword(host, realm, null, password);
            result = mWebView.getHttpAuthUsernamePassword(host, realm);
            assertNotNull(result);
            assertNull(result[0]);
            assertEquals(password, result[1]);

            mWebView.setHttpAuthUsernamePassword(host, realm, userName, null);
            result = mWebView.getHttpAuthUsernamePassword(host, realm);
            assertNotNull(result);
            assertEquals(userName, result[0]);
            assertEquals(null, result[1]);

            mWebView.setHttpAuthUsernamePassword(host, realm, null, null);
            result = mWebView.getHttpAuthUsernamePassword(host, realm);
            assertNotNull(result);
            assertNull(result[0]);
            assertNull(result[1]);

            mWebView.setHttpAuthUsernamePassword(host, realm, newUserName, newPassword);
            result = mWebView.getHttpAuthUsernamePassword(host, realm);
            assertNotNull(result);
            assertEquals(newUserName, result[0]);
            assertEquals(newPassword, result[1]);
        } finally {
            WebViewDatabase.getInstance(getActivity()).clearHttpAuthUsernamePassword();
        }
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "savePassword",
        args = {String.class, String.class, String.class}
    )
    @UiThreadTest
    public void testSavePassword() {
        WebViewDatabase db = WebViewDatabase.getInstance(getActivity());
        try {
            db.clearUsernamePassword();

            String host = "http://localhost:8080";
            String userName = "user";
            String password = "password";
            assertFalse(db.hasUsernamePassword());
            mWebView.savePassword(host, userName, password);
            assertTrue(db.hasUsernamePassword());
        } finally {
            db.clearUsernamePassword();
        }
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "loadData",
            args = {String.class, String.class, String.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getTitle",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "capturePicture",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "capturePicture",
            args = {}
        )
    })
    public void testLoadData() throws Throwable {
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertNull(mWebView.getTitle());
                mWebView.loadData("<html><head><title>Hello,World!</title></head><body></body>" +
                        "</html>",
                        "text/html", null);
                waitForLoadComplete();
                assertEquals("Hello,World!", mWebView.getTitle());
            }
        });

        // Test that JavaScript can't access cross-origin content.
        class ConsoleMessageWebChromeClient extends WebChromeClient {
            private boolean mIsMessageLevelAvailable;
            private ConsoleMessage.MessageLevel mMessageLevel;
            @Override
            public synchronized boolean onConsoleMessage(ConsoleMessage message) {
                mMessageLevel = message.messageLevel();
                mIsMessageLevelAvailable = true;
                notify();
                return true;
            }
            public synchronized ConsoleMessage.MessageLevel getMessageLevel() {
                while (!mIsMessageLevelAvailable) {
                    try {
                        wait();
                    } catch (InterruptedException e) {
                    }
                }
                return mMessageLevel;
            }
        }
        startWebServer(false);
        final ConsoleMessageWebChromeClient webChromeClient = new ConsoleMessageWebChromeClient();
        final String crossOriginUrl = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.getSettings().setJavaScriptEnabled(true);
                mWebView.setWebChromeClient(webChromeClient);
                mWebView.loadData("<html><head></head><body onload=\"" +
                        "document.title = " +
                        "document.getElementById('frame').contentWindow.location.href;" +
                        "\"><iframe id=\"frame\" src=\"" + crossOriginUrl + "\"></body></html>",
                        "text/html", null);
            }
        });
        assertEquals(ConsoleMessage.MessageLevel.ERROR, webChromeClient.getMessageLevel());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "loadDataWithBaseURL",
            args = {String.class, String.class, String.class, String.class, String.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getTitle",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getUrl",
            args = {}
        )
    })
    @UiThreadTest
    public void testLoadDataWithBaseUrl() throws Exception {
        assertNull(mWebView.getTitle());
        assertNull(mWebView.getUrl());
        String imgUrl = TestHtmlConstants.SMALL_IMG_URL; // relative

        // Check that we can access relative URLs and that reported URL is supplied history URL.
        startWebServer(false);
        String baseUrl = mWebServer.getAssetUrl("foo.html");
        String historyUrl = "random";
        mWebView.loadDataWithBaseURL(baseUrl,
                "<html><body><img src=\"" + imgUrl + "\"/></body></html>",
                "text/html", "UTF-8", historyUrl);
        waitForLoadComplete();
        assertTrue(mWebServer.getLastRequestUrl().endsWith(imgUrl));
        assertEquals(historyUrl, mWebView.getUrl());

        // Check that reported URL is "about:blank" when supplied history URL
        // is null.
        imgUrl = TestHtmlConstants.LARGE_IMG_URL;
        mWebView.loadDataWithBaseURL(baseUrl,
                "<html><body><img src=\"" + imgUrl + "\"/></body></html>",
                "text/html", "UTF-8", null);
        waitForLoadComplete();
        assertTrue(mWebServer.getLastRequestUrl().endsWith(imgUrl));
        assertEquals("about:blank", mWebView.getUrl());

        // Test that JavaScript can access content from the same origin as the base URL.
        mWebView.getSettings().setJavaScriptEnabled(true);
        final String crossOriginUrl = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        mWebView.loadDataWithBaseURL(baseUrl, "<html><head></head><body onload=\"" +
                "document.title = document.getElementById('frame').contentWindow.location.href;" +
                "\"><iframe id=\"frame\" src=\"" + crossOriginUrl + "\"></body></html>",
                "text/html", "UTF-8", null);
        waitForLoadComplete();
        assertEquals(crossOriginUrl, mWebView.getTitle());

        // Check that when the base URL uses the 'data' scheme, a 'data' scheme URL is used and the
        // history URL is ignored.
        mWebView.loadDataWithBaseURL("data:foo", "<html><body>bar</body></html>",
                "text/html", "UTF-8", historyUrl);
        waitForLoadComplete();
        assertTrue(mWebView.getUrl().indexOf("data:text/html,") == 0);
        assertTrue(mWebView.getUrl().indexOf("bar") > 0);
    }

    @TestTargetNew(
        level = TestLevel.SUFFICIENT,
        method = "findAll",
        args = {String.class},
        notes = "Cannot check highlighting"
    )
    @UiThreadTest
    public void testFindAll() {
        String p = "<p>Find all instances of find on the page and highlight them.</p>";

        mWebView.loadData("<html><body>" + p + "</body></html>", "text/html", null);
        waitForLoadComplete();

        assertEquals(2, mWebView.findAll("find"));
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "findNext",
            args = {boolean.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "findAll",
            args = {String.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "clearMatches",
            args = {}
        )
    })
    public void testFindNext() throws Throwable {
        final ScrollRunnable runnable = new ScrollRunnable();

        final class StopScrollingDelayedCheck extends DelayedCheck {
            private int mPreviousScrollY = -1;
            @Override
            protected boolean check() {
                try {
                    runTestOnUiThread(runnable);
                } catch (Throwable t) {}
                boolean hasStopped =
                    (mPreviousScrollY == -1 ? false : (runnable.getScrollY() == mPreviousScrollY));
                mPreviousScrollY = runnable.getScrollY();
                return hasStopped;
            }
        }

        final class FindNextRunnable implements Runnable {
            private boolean mForward;
            FindNextRunnable(boolean forward) {
                mForward = forward;
            }
            public void run() {
                mWebView.findNext(mForward);
            }
        }

        runTestOnUiThread(new Runnable() {
            public void run() {
                // Reset the scaling so that finding the next "all" text will require scrolling.
                mWebView.setInitialScale(100);

                DisplayMetrics metrics = mWebView.getContext().getResources().getDisplayMetrics();
                int dimension = Math.max(metrics.widthPixels, metrics.heightPixels);
                // create a paragraph high enough to take up the entire screen
                String p = "<p style=\"height:" + dimension + "px;\">" +
                        "Find all instances of a word on the page and highlight them.</p>";

                mWebView.loadData("<html><body>" + p + p + "</body></html>", "text/html", null);
                waitForLoadComplete();

                // highlight all the strings found
                mWebView.findAll("all");
            }
        });
        getInstrumentation().waitForIdleSync();

        runTestOnUiThread(runnable);
        int previousScrollY = runnable.getScrollY();

        // Focus "all" in the second page and assert that the view scrolls.
        runTestOnUiThread(new FindNextRunnable(true));
        new StopScrollingDelayedCheck().run();
        assertTrue(runnable.getScrollY() > previousScrollY);
        previousScrollY = runnable.getScrollY();

        // Focus "all" in the first page and assert that the view scrolls.
        runTestOnUiThread(new FindNextRunnable(true));
        new StopScrollingDelayedCheck().run();
        assertTrue(runnable.getScrollY() < previousScrollY);
        previousScrollY = runnable.getScrollY();

        // Focus "all" in the second page and assert that the view scrolls.
        runTestOnUiThread(new FindNextRunnable(false));
        new StopScrollingDelayedCheck().run();
        assertTrue(runnable.getScrollY() > previousScrollY);
        previousScrollY = runnable.getScrollY();

        // Focus "all" in the first page and assert that the view scrolls.
        runTestOnUiThread(new FindNextRunnable(false));
        new StopScrollingDelayedCheck().run();
        assertTrue(runnable.getScrollY() < previousScrollY);
        previousScrollY = runnable.getScrollY();

        // clear the result
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.clearMatches();
            }
        });
        getInstrumentation().waitForIdleSync();

        // can not scroll any more
        runTestOnUiThread(new FindNextRunnable(false));
        new StopScrollingDelayedCheck().run();
        assertTrue(runnable.getScrollY() == previousScrollY);

        runTestOnUiThread(new FindNextRunnable(true));
        new StopScrollingDelayedCheck().run();
        assertTrue(runnable.getScrollY() == previousScrollY);
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "documentHasImages",
        args = {android.os.Message.class}
    )
    public void testDocumentHasImages() throws Exception, Throwable {
        final class DocumentHasImageCheckHandler extends Handler {
            private boolean mReceived;
            private int mMsgArg1;
            public DocumentHasImageCheckHandler(Looper looper) {
                super(looper);
            }
            @Override
            public void handleMessage(Message msg) {
                synchronized(this) {
                    mReceived = true;
                    mMsgArg1 = msg.arg1;
                }
            }
            public synchronized boolean hasCalledHandleMessage() {
                return mReceived;
            }
            public synchronized int getMsgArg1() {
                return mMsgArg1;
            }
        }

        startWebServer(false);
        final String imgUrl = mWebServer.getAssetUrl(TestHtmlConstants.SMALL_IMG_URL);

        // Create a handler on the UI thread.
        final DocumentHasImageCheckHandler handler =
            new DocumentHasImageCheckHandler(mWebView.getHandler().getLooper());

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadData("<html><body><img src=\"" + imgUrl + "\"/></body></html>",
                        "text/html", null);
                waitForLoadComplete();
                Message response = new Message();
                response.setTarget(handler);
                assertFalse(handler.hasCalledHandleMessage());
                mWebView.documentHasImages(response);
            }
        });
        new DelayedCheck() {
            @Override
            protected boolean check() {
                return handler.hasCalledHandleMessage();
            }
        }.run();
        assertEquals(1, handler.getMsgArg1());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "pageDown",
            args = {boolean.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "pageUp",
            args = {boolean.class}
        )
    })
    public void testPageScroll() throws Throwable {
        final class PageUpRunnable implements Runnable {
            private boolean mResult;
            public void run() {
                mResult = mWebView.pageUp(false);
            }
            public boolean getResult() {
                return mResult;
            }
        }

        final class PageDownRunnable implements Runnable {
            private boolean mResult;
            public void run() {
                mResult = mWebView.pageDown(false);
            }
            public boolean getResult() {
                return mResult;
            }
        }

        runTestOnUiThread(new Runnable() {
            public void run() {
                DisplayMetrics metrics = mWebView.getContext().getResources().getDisplayMetrics();
                int dimension = 2 * Math.max(metrics.widthPixels, metrics.heightPixels);
                String p = "<p style=\"height:" + dimension + "px;\">" +
                        "Scroll by half the size of the page.</p>";
                mWebView.loadData("<html><body>" + p + p + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        runTestOnUiThread(new Runnable() {
            public void run() {
                assertTrue(mWebView.pageDown(false));
            }
        });

        PageDownRunnable pageDownRunnable = new PageDownRunnable();
        do {
            getInstrumentation().waitForIdleSync();
            runTestOnUiThread(pageDownRunnable);
        } while (pageDownRunnable.getResult());

        ScrollRunnable scrollRunnable = new ScrollRunnable();
        getInstrumentation().waitForIdleSync();
        runTestOnUiThread(scrollRunnable);
        int bottomScrollY = scrollRunnable.getScrollY();

        runTestOnUiThread(new Runnable() {
            public void run() {
                assertTrue(mWebView.pageUp(false));
            }
        });

        PageUpRunnable pageUpRunnable = new PageUpRunnable();
        do {
            getInstrumentation().waitForIdleSync();
            runTestOnUiThread(pageUpRunnable);
        } while (pageUpRunnable.getResult());

        getInstrumentation().waitForIdleSync();
        runTestOnUiThread(scrollRunnable);
        int topScrollY = scrollRunnable.getScrollY();

        // jump to the bottom
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertTrue(mWebView.pageDown(true));
            }
        });
        getInstrumentation().waitForIdleSync();
        runTestOnUiThread(scrollRunnable);
        assertEquals(bottomScrollY, scrollRunnable.getScrollY());

        // jump to the top
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertTrue(mWebView.pageUp(true));
            }
        });
        getInstrumentation().waitForIdleSync();
        runTestOnUiThread(scrollRunnable);
        assertEquals(topScrollY, scrollRunnable.getScrollY());
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "getContentHeight",
        args = {}
    )
    public void testGetContentHeight() throws Throwable {
        final class HeightRunnable implements Runnable {
            private int mHeight;
            private int mContentHeight;
            public void run() {
                mHeight = mWebView.getHeight();
                mContentHeight = mWebView.getContentHeight();
            }
            public int getHeight() {
                return mHeight;
            }
            public int getContentHeight() {
                return mContentHeight;
            }
        }

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadData("<html><body></body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        final int pageHeight = 600;
        // set the margin to 0
        final String p = "<p style=\"height:" + pageHeight
                + "px;margin:0px auto;\">Get the height of HTML content.</p>";
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals(mWebView.getHeight(), mWebView.getContentHeight() * mWebView.getScale(), 2f);
                mWebView.loadData("<html><body>" + p + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        HeightRunnable runnable = new HeightRunnable();
        runTestOnUiThread(runnable);
        assertTrue(runnable.getContentHeight() > pageHeight);
        int extraSpace = runnable.getContentHeight() - pageHeight;

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadData("<html><body>" + p + p + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();
        runTestOnUiThread(runnable);
        assertEquals(pageHeight + pageHeight + extraSpace, runnable.getContentHeight());
    }

    @TestTargetNew(
        level = TestLevel.SUFFICIENT,
        method = "clearCache",
        args = {boolean.class}
    )
    @UiThreadTest
    public void testClearCache() throws Exception {
        final File cacheFileBaseDir = CacheManager.getCacheFileBaseDir();
        mWebView.clearCache(true);
        assertEquals(0, cacheFileBaseDir.list().length);

        startWebServer(false);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        mWebView.loadUrl(url);
        waitForLoadComplete();
        new DelayedCheck(TEST_TIMEOUT) {
            @Override
            protected boolean check() {
                CacheResult result = CacheManager.getCacheFile(url, null);
                return result != null;
            }
        }.run();
        int cacheFileCount = cacheFileBaseDir.list().length;
        assertTrue(cacheFileCount > 0);

        mWebView.clearCache(false);
        // the cache files are still there
        // can not check other effects of the method
        assertEquals(cacheFileCount, cacheFileBaseDir.list().length);

        mWebView.clearCache(true);
        // check the files are deleted
        new DelayedCheck(TEST_TIMEOUT) {
            @Override
            protected boolean check() {
                return cacheFileBaseDir.list().length == 0;
            }
        }.run();
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.SUFFICIENT,
            method = "enablePlatformNotifications",
            args = {},
            notes = "Cannot simulate data state or proxy changes"
        ),
        @TestTargetNew(
            level = TestLevel.SUFFICIENT,
            method = "disablePlatformNotifications",
            args = {},
            notes = "Cannot simulate data state or proxy changes"
        )
    })
    @UiThreadTest
    public void testPlatformNotifications() {
        WebView.enablePlatformNotifications();
        WebView.disablePlatformNotifications();
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.SUFFICIENT,
            method = "getPluginList",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.SUFFICIENT,
            method = "refreshPlugins",
            args = {boolean.class}
        )
    })
    @UiThreadTest
    public void testAccessPluginList() {
        assertNotNull(WebView.getPluginList());

        // can not find a way to install plugins
        mWebView.refreshPlugins(false);
    }

    @TestTargetNew(
        level = TestLevel.SUFFICIENT,
        method = "destroy",
        args = {}
    )
    @UiThreadTest
    public void testDestroy() {
        // Create a new WebView, since we cannot call destroy() on a view in the hierarchy
        WebView localWebView = new WebView(getActivity());
        localWebView.destroy();
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "flingScroll",
        args = {int.class, int.class}
    )
    public void testFlingScroll() throws Throwable {
        runTestOnUiThread(new Runnable() {
            public void run() {
                DisplayMetrics metrics = mWebView.getContext().getResources().getDisplayMetrics();
                int dimension = 2 * Math.max(metrics.widthPixels, metrics.heightPixels);
                String p = "<p style=\"height:" + dimension + "px;" +
                        "width:" + dimension + "px\">Test fling scroll.</p>";
                mWebView.loadData("<html><body>" + p + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        ScrollRunnable runnable = new ScrollRunnable();
        runTestOnUiThread(runnable);
        int previousScrollX = runnable.getScrollX();
        int previousScrollY = runnable.getScrollY();

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.flingScroll(100, 100);
            }
        });

        int timeSlice = 500;
        Thread.sleep(timeSlice);
        runTestOnUiThread(runnable);
        assertTrue(runnable.getScrollX() > previousScrollX);
        assertTrue(runnable.getScrollY() > previousScrollY);

        previousScrollY = runnable.getScrollY();
        previousScrollX = runnable.getScrollX();
        Thread.sleep(timeSlice);
        runTestOnUiThread(runnable);
        assertTrue(runnable.getScrollX() >= previousScrollX);
        assertTrue(runnable.getScrollY() >= previousScrollY);

        previousScrollY = runnable.getScrollY();
        previousScrollX = runnable.getScrollX();
        Thread.sleep(timeSlice);
        runTestOnUiThread(runnable);
        assertTrue(runnable.getScrollX() >= previousScrollX);
        assertTrue(runnable.getScrollY() >= previousScrollY);
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "requestFocusNodeHref",
        args = {android.os.Message.class}
    )
    public void testRequestFocusNodeHref() throws Throwable {
        final String links = "<DL><p><DT><A HREF=\"" + TestHtmlConstants.HTML_URL1
                + "\">HTML_URL1</A><DT><A HREF=\"" + TestHtmlConstants.HTML_URL2
                + "\">HTML_URL2</A></DL><p>";
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadData("<html><body>" + links + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        final HrefCheckHandler handler = new HrefCheckHandler(mWebView.getHandler().getLooper());
        final Message hrefMsg = new Message();
        hrefMsg.setTarget(handler);

        // focus on first link
        handler.reset();
        getInstrumentation().sendKeyDownUpSync(KeyEvent.KEYCODE_DPAD_DOWN);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.requestFocusNodeHref(hrefMsg);
            }
        });
        new DelayedCheck() {
            @Override
            protected boolean check() {
                return handler.hasCalledHandleMessage();
            }
        }.run();
        assertEquals(TestHtmlConstants.HTML_URL1, handler.getResultUrl());

        // focus on second link
        handler.reset();
        final Message hrefMsg2 = new Message();
        hrefMsg2.setTarget(handler);
        getInstrumentation().sendKeyDownUpSync(KeyEvent.KEYCODE_DPAD_DOWN);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.requestFocusNodeHref(hrefMsg2);
            }
        });
        new DelayedCheck() {
            @Override
            protected boolean check() {
                return handler.hasCalledHandleMessage();
            }
        }.run();
        assertEquals(TestHtmlConstants.HTML_URL2, handler.getResultUrl());

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.requestFocusNodeHref(null);
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "requestImageRef",
        args = {android.os.Message.class}
    )
    public void testRequestImageRef() throws Exception, Throwable {
        final class GetLocationRunnable implements Runnable {
            private int[] mLocation;
            public void run() {
                mLocation = new int[2];
                mWebView.getLocationOnScreen(mLocation);
            }
            public int[] getLocation() {
                return mLocation;
            }
        }

        AssetManager assets = getActivity().getAssets();
        Bitmap bitmap = BitmapFactory.decodeStream(assets.open(TestHtmlConstants.LARGE_IMG_URL));
        int imgWidth = bitmap.getWidth();
        int imgHeight = bitmap.getHeight();

        startWebServer(false);
        final String imgUrl = mWebServer.getAssetUrl(TestHtmlConstants.LARGE_IMG_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadData("<html><title>Title</title><body><img src=\"" + imgUrl
                        + "\"/></body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        final HrefCheckHandler handler = new HrefCheckHandler(mWebView.getHandler().getLooper());
        final Message msg = new Message();
        msg.setTarget(handler);

        // touch the image
        handler.reset();
        GetLocationRunnable runnable = new GetLocationRunnable();
        runTestOnUiThread(runnable);
        int[] location = runnable.getLocation();

        long time = SystemClock.uptimeMillis();
        getInstrumentation().sendPointerSync(
                MotionEvent.obtain(time, time, MotionEvent.ACTION_DOWN,
                        location[0] + imgWidth / 2,
                        location[1] + imgHeight / 2, 0));
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.requestImageRef(msg);
            }
        });
        new DelayedCheck() {
            @Override
            protected boolean check() {
                return handler.hasCalledHandleMessage();
            }
        }.run();
        assertEquals(imgUrl, handler.mResultUrl);
    }

    @TestTargetNew(
        level = TestLevel.SUFFICIENT,
        method = "debugDump",
        args = {}
    )
    @UiThreadTest
    public void testDebugDump() {
        mWebView.debugDump();
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "getHitTestResult",
        args = {}
    )
    public void testGetHitTestResult() throws Throwable {
        class HitTestResultRunnable implements Runnable {
            private HitTestResult mHitTestResult;
            public void run() {
                mHitTestResult = mWebView.getHitTestResult();
            }
            public HitTestResult getHitTestResult() {
                return mHitTestResult;
            }
        }

        final String anchor = "<p><a href=\"" + TestHtmlConstants.EXT_WEB_URL1
                + "\">normal anchor</a></p>";
        final String blankAnchor = "<p><a href=\"\">blank anchor</a></p>";
        final String form = "<p><form><input type=\"text\" name=\"Test\"><br>"
                + "<input type=\"submit\" value=\"Submit\"></form></p>";
        String phoneNo = "3106984000";
        final String tel = "<p><a href=\"tel:" + phoneNo + "\">Phone</a></p>";
        String email = "test@gmail.com";
        final String mailto = "<p><a href=\"mailto:" + email + "\">Email</a></p>";
        String location = "shanghai";
        final String geo = "<p><a href=\"geo:0,0?q=" + location + "\">Location</a></p>";

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadDataWithBaseURL("fake://home", "<html><body>" + anchor + blankAnchor + form
                        + tel + mailto + geo + "</body></html>", "text/html", "UTF-8", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();
        HitTestResultRunnable runnable = new HitTestResultRunnable();

        // anchor
        moveFocusDown();
        runTestOnUiThread(runnable);
        assertEquals(HitTestResult.SRC_ANCHOR_TYPE, runnable.getHitTestResult().getType());
        assertEquals(TestHtmlConstants.EXT_WEB_URL1, runnable.getHitTestResult().getExtra());

        // blank anchor
        moveFocusDown();
        runTestOnUiThread(runnable);
        assertEquals(HitTestResult.SRC_ANCHOR_TYPE, runnable.getHitTestResult().getType());
        assertEquals("fake://home", runnable.getHitTestResult().getExtra());

        // text field
        moveFocusDown();
        runTestOnUiThread(runnable);
        assertEquals(HitTestResult.EDIT_TEXT_TYPE, runnable.getHitTestResult().getType());
        assertNull(runnable.getHitTestResult().getExtra());

        // submit button
        moveFocusDown();
        runTestOnUiThread(runnable);
        assertEquals(HitTestResult.UNKNOWN_TYPE, runnable.getHitTestResult().getType());
        assertNull(runnable.getHitTestResult().getExtra());

        // phone number
        moveFocusDown();
        runTestOnUiThread(runnable);
        assertEquals(HitTestResult.PHONE_TYPE, runnable.getHitTestResult().getType());
        assertEquals(phoneNo, runnable.getHitTestResult().getExtra());

        // email
        moveFocusDown();
        runTestOnUiThread(runnable);
        assertEquals(HitTestResult.EMAIL_TYPE, runnable.getHitTestResult().getType());
        assertEquals(email, runnable.getHitTestResult().getExtra());

        // geo address
        moveFocusDown();
        runTestOnUiThread(runnable);
        assertEquals(HitTestResult.GEO_TYPE, runnable.getHitTestResult().getType());
        assertEquals(location, runnable.getHitTestResult().getExtra());
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "setInitialScale",
        args = {int.class}
    )
    public void testSetInitialScale() throws Throwable {
        final String p = "<p style=\"height:1000px;width:1000px\">Test setInitialScale.</p>";
        final float defaultScale =
            getInstrumentation().getTargetContext().getResources().getDisplayMetrics().density;

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadData("<html><body>" + p + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals(defaultScale, mWebView.getScale(), .01f);

                mWebView.setInitialScale(0);
                // modify content to fool WebKit into re-loading
                mWebView.loadData("<html><body>" + p + "2" + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals(defaultScale, mWebView.getScale(), .01f);

                mWebView.setInitialScale(50);
                mWebView.loadData("<html><body>" + p + "3" + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals(0.5f, mWebView.getScale(), .02f);

                mWebView.setInitialScale(0);
                mWebView.loadData("<html><body>" + p + "4" + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals(defaultScale, mWebView.getScale(), .01f);
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.SUFFICIENT,
        notes = "No API to trigger receiving an icon. Favicon not loaded automatically.",
        method = "getFavicon",
        args = {}
    )
    @ToBeFixed(explanation = "Favicon is not loaded automatically.")
    @UiThreadTest
    public void testGetFavicon() throws Exception {
        startWebServer(false);
        String url = mWebServer.getAssetUrl(TestHtmlConstants.TEST_FAVICON_URL);
        assertLoadUrlSuccessfully(url);
        mWebView.getFavicon();
        // ToBeFixed: Favicon is not loaded automatically.
        // assertNotNull(mWebView.getFavicon());
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "clearHistory",
        args = {}
    )
    @UiThreadTest
    public void testClearHistory() throws Exception {
        startWebServer(false);
        String url1 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL1);
        String url2 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL2);
        String url3 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL3);

        assertLoadUrlSuccessfully(url1);
        delayedCheckWebBackForwardList(url1, 0, 1);

        assertLoadUrlSuccessfully(url2);
        delayedCheckWebBackForwardList(url2, 1, 2);

        assertLoadUrlSuccessfully(url3);
        delayedCheckWebBackForwardList(url3, 2, 3);

        mWebView.clearHistory();

        // only current URL is left after clearing
        delayedCheckWebBackForwardList(url3, 0, 1);
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "saveState",
            args = {Bundle.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "restoreState",
            args = {Bundle.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "copyBackForwardList",
            args = {}
        )
    })
    @ToBeFixed(explanation="Web history items do not get inflated after restore.")
    @UiThreadTest
    public void testSaveAndRestoreState() throws Throwable {
        // nothing to save
        assertNull(mWebView.saveState(new Bundle()));

        startWebServer(false);
        String url1 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL1);
        String url2 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL2);
        String url3 = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL3);

        // make a history list
        assertLoadUrlSuccessfully(url1);
        delayedCheckWebBackForwardList(url1, 0, 1);
        assertLoadUrlSuccessfully(url2);
        delayedCheckWebBackForwardList(url2, 1, 2);
        assertLoadUrlSuccessfully(url3);
        delayedCheckWebBackForwardList(url3, 2, 3);

        // save the list
        Bundle bundle = new Bundle();
        WebBackForwardList saveList = mWebView.saveState(bundle);
        assertNotNull(saveList);
        assertEquals(3, saveList.getSize());
        assertEquals(2, saveList.getCurrentIndex());
        assertEquals(url1, saveList.getItemAtIndex(0).getUrl());
        assertEquals(url2, saveList.getItemAtIndex(1).getUrl());
        assertEquals(url3, saveList.getItemAtIndex(2).getUrl());

        // change the content to a new "blank" web view without history
        final WebView newWebView = new WebView(getActivity());

        WebBackForwardList copyListBeforeRestore = newWebView.copyBackForwardList();
        assertNotNull(copyListBeforeRestore);
        assertEquals(0, copyListBeforeRestore.getSize());

        // restore the list
        final WebBackForwardList restoreList = newWebView.restoreState(bundle);
        assertNotNull(restoreList);
        assertEquals(3, restoreList.getSize());
        assertEquals(2, saveList.getCurrentIndex());
        /* ToBeFixed: The WebHistoryItems do not get inflated. Uncomment remaining tests when fixed.
        // wait for the list items to get inflated
        new DelayedCheck(TEST_TIMEOUT) {
            @Override
            protected boolean check() {
                return restoreList.getItemAtIndex(0).getUrl() != null &&
                       restoreList.getItemAtIndex(1).getUrl() != null &&
                       restoreList.getItemAtIndex(2).getUrl() != null;
            }
        }.run();
        assertEquals(url1, restoreList.getItemAtIndex(0).getUrl());
        assertEquals(url2, restoreList.getItemAtIndex(1).getUrl());
        assertEquals(url3, restoreList.getItemAtIndex(2).getUrl());

        WebBackForwardList copyListAfterRestore = newWebView.copyBackForwardList();
        assertNotNull(copyListAfterRestore);
        assertEquals(3, copyListAfterRestore.getSize());
        assertEquals(2, copyListAfterRestore.getCurrentIndex());
        assertEquals(url1, copyListAfterRestore.getItemAtIndex(0).getUrl());
        assertEquals(url2, copyListAfterRestore.getItemAtIndex(1).getUrl());
        assertEquals(url3, copyListAfterRestore.getItemAtIndex(2).getUrl());
        */
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "setWebViewClient",
        args = {WebViewClient.class}
    )
    public void testSetWebViewClient() throws Throwable {
        final class MockWebViewClient extends WebViewClient {
            private boolean mOnScaleChangedCalled = false;
            @Override
            public void onScaleChanged(WebView view, float oldScale, float newScale) {
                super.onScaleChanged(view, oldScale, newScale);
                mOnScaleChangedCalled = true;
            }
            public boolean onScaleChangedCalled() {
                return mOnScaleChangedCalled;
            }
        }

        final MockWebViewClient webViewClient = new MockWebViewClient();
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebViewClient(webViewClient);
            }
        });
        getInstrumentation().waitForIdleSync();
        assertFalse(webViewClient.onScaleChangedCalled());

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.zoomIn();
            }
        });
        getInstrumentation().waitForIdleSync();
        assertTrue(webViewClient.onScaleChangedCalled());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setCertificate",
            args = {SslCertificate.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getCertificate",
            args = {}
        )
    })
    @UiThreadTest
    public void testSetAndGetCertificate() {
        assertNull(mWebView.getCertificate());
        SslCertificate certificate = new SslCertificate("foo", "bar", new Date(42), new Date(43));
        mWebView.setCertificate(certificate);
        assertEquals(certificate, mWebView.getCertificate());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setCertificate",
            args = {SslCertificate.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getCertificate",
            args = {}
        )
    })
    public void testInsecureSiteClearsCertificate() throws Throwable {
        final SslCertificate certificate =
                new SslCertificate("foo", "bar", new Date(42), new Date(43));
        startWebServer(false);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebChromeClient(new LoadCompleteWebChromeClient());
                mWebView.setCertificate(certificate);
                mWebView.loadUrl(url);
            }
        });
        waitForUiThreadDone();

        runTestOnUiThread(new Runnable() {
            public void run() {
                new DelayedCheck(TEST_TIMEOUT) {
                    @Override
                    protected boolean check() {
                        return mWebView.getCertificate() == null;
                    }
                }.run();
            }
        });
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setCertificate",
            args = {SslCertificate.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getCertificate",
            args = {}
        )
    })
    public void testSecureSiteSetsCertificate() throws Throwable {
        final class MockWebViewClient extends WebViewClient {
            @Override
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                handler.proceed();
            }
        }

        startWebServer(true);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebViewClient(new MockWebViewClient());
                mWebView.setWebChromeClient(new LoadCompleteWebChromeClient());
                mWebView.setCertificate(null);
                mWebView.loadUrl(url);
            }
        });
        waitForUiThreadDone();

        runTestOnUiThread(new Runnable() {
            public void run() {
                new DelayedCheck(TEST_TIMEOUT) {
                    @Override
                    protected boolean check() {
                        return mWebView.getCertificate() != null;
                    }
                }.run();
                SslCertificate cert = mWebView.getCertificate();
                assertNotNull(cert);
                assertEquals("Android", cert.getIssuedTo().getUName());
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "clearSslPreferences",
        args = {}
    )
    @UiThreadTest
    public void testClearSslPreferences() {
        // FIXME: Implement this. See http://b/5378046.
        mWebView.clearSslPreferences();
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "WebViewClient.onReceivedSslError",
        args = {}
    )
    public void testOnReceivedSslError() throws Throwable {
        final class MockWebViewClient extends WebViewClient {
            private String mErrorUrl;
            private WebView mWebView;
            @Override
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                mWebView = view;
                mErrorUrl = error.getUrl();
                handler.proceed();
            }
            public String errorUrl() {
                return mErrorUrl;
            }
            public WebView webView() {
                return mWebView;
            }
        }

        startWebServer(true);
        final String errorUrl = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        final MockWebViewClient webViewClient = new MockWebViewClient();
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebViewClient(webViewClient);
                mWebView.setWebChromeClient(new LoadCompleteWebChromeClient());
                mWebView.clearSslPreferences();
                mWebView.loadUrl(errorUrl);
            }
        });
        waitForUiThreadDone();

        assertEquals(mWebView, webViewClient.webView());
        assertEquals(errorUrl, webViewClient.errorUrl());
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "WebViewClient.onReceivedSslError",
        args = {}
    )
    public void testOnReceivedSslErrorProceed() throws Throwable {
        final class MockWebViewClient extends WebViewClient {
            @Override
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                handler.proceed();
            }
        }

        startWebServer(true);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebViewClient(new MockWebViewClient());
                mWebView.setWebChromeClient(new LoadCompleteWebChromeClient());
                mWebView.loadUrl(url);
            }
        });
        waitForUiThreadDone();
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals(TestHtmlConstants.HELLO_WORLD_TITLE, mWebView.getTitle());
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "WebViewClient.onReceivedSslError",
        args = {}
    )
    public void testOnReceivedSslErrorCancel() throws Throwable {
        final class MockWebViewClient extends WebViewClient {
            @Override
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                handler.cancel();
            }
        }

        startWebServer(true);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebViewClient(new MockWebViewClient());
                mWebView.setWebChromeClient(new LoadCompleteWebChromeClient());
                mWebView.clearSslPreferences();
                mWebView.loadUrl(url);
            }
        });
        waitForUiThreadDone();
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertFalse(TestHtmlConstants.HELLO_WORLD_TITLE.equals(mWebView.getTitle()));
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "WebViewClient.onReceivedSslError",
        args = {}
    )
    public void testSslErrorProceedResponseReusedForSameHost() throws Throwable {
        // Load the first page. We expect a call to
        // WebViewClient.onReceivedSslError().
        final SslErrorWebViewClient webViewClient = new SslErrorWebViewClient();
        startWebServer(true);
        final String firstUrl = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL1);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebViewClient(webViewClient);
                mWebView.setWebChromeClient(new LoadCompleteWebChromeClient());
                mWebView.clearSslPreferences();
                mWebView.loadUrl(firstUrl);
            }
        });
        waitForUiThreadDone();
        assertTrue(webViewClient.wasOnReceivedSslErrorCalled());

        // Load the second page. We don't expect a call to
        // WebViewClient.onReceivedSslError(), but the page should load.
        webViewClient.resetWasOnReceivedSslErrorCalled();
        final String sameHostUrl = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL2);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadUrl(sameHostUrl);
            }
        });
        waitForUiThreadDone();
        assertFalse(webViewClient.wasOnReceivedSslErrorCalled());
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals("Second page", mWebView.getTitle());
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "WebViewClient.onReceivedSslError",
        args = {}
    )
    public void testSslErrorProceedResponseNotReusedForDifferentHost() throws Throwable {
        // Load the first page. We expect a call to
        // WebViewClient.onReceivedSslError().
        final SslErrorWebViewClient webViewClient = new SslErrorWebViewClient();
        startWebServer(true);
        final String firstUrl = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL1);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebViewClient(webViewClient);
                mWebView.setWebChromeClient(new LoadCompleteWebChromeClient());
                mWebView.clearSslPreferences();
                mWebView.loadUrl(firstUrl);
            }
        });
        waitForUiThreadDone();
        assertTrue(webViewClient.wasOnReceivedSslErrorCalled());

        // Load the second page. We expect another call to
        // WebViewClient.onReceivedSslError().
        webViewClient.resetWasOnReceivedSslErrorCalled();
        // The test server uses the host "localhost". "127.0.0.1" works as an
        // alias, but will be considered unique by the WebView.
        final String differentHostUrl = mWebServer.getAssetUrl(TestHtmlConstants.HTML_URL2).replace(
                "localhost", "127.0.0.1");
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadUrl(differentHostUrl);
            }
        });
        waitForUiThreadDone();
        assertTrue(webViewClient.wasOnReceivedSslErrorCalled());
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertEquals("Second page", mWebView.getTitle());
            }
        });
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "requestChildRectangleOnScreen",
        args = {View.class, Rect.class, boolean.class}
    )
    public void testRequestChildRectangleOnScreen() throws Throwable {
        runTestOnUiThread(new Runnable() {
            public void run() {
                DisplayMetrics metrics = mWebView.getContext().getResources().getDisplayMetrics();
                final int dimension = 2 * Math.max(metrics.widthPixels, metrics.heightPixels);
                String p = "<p style=\"height:" + dimension + "px;width:" + dimension + "px\">&nbsp;</p>";
                mWebView.loadData("<html><body>" + p + "</body></html>", "text/html", null);
                waitForLoadComplete();
            }
        });
        getInstrumentation().waitForIdleSync();

        runTestOnUiThread(new Runnable() {
            public void run() {
                int origX = mWebView.getScrollX();
                int origY = mWebView.getScrollY();

                DisplayMetrics metrics = mWebView.getContext().getResources().getDisplayMetrics();
                final int dimension = 2 * Math.max(metrics.widthPixels, metrics.heightPixels);
                int half = dimension / 2;
                Rect rect = new Rect(half, half, half + 1, half + 1);
                assertTrue(mWebView.requestChildRectangleOnScreen(mWebView, rect, true));
                assertTrue(mWebView.getScrollX() > origX);
                assertTrue(mWebView.getScrollY() > origY);
            }
        });
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setDownloadListener",
            args = {DownloadListener.class}
        ),
        @TestTargetNew(
            level = TestLevel.SUFFICIENT,
            method = "requestFocus",
            args = {int.class, Rect.class}
        )
    })
    @ToBeFixed(explanation="Mime type and content length passed to listener are incorrect.")
    public void testSetDownloadListener() throws Throwable {
        final class MyDownloadListener implements DownloadListener {
            public String url;
            public String mimeType;
            public long contentLength;
            public String contentDisposition;
            public boolean called;

            public void onDownloadStart(String url, String userAgent, String contentDisposition,
                    String mimetype, long contentLength) {
                this.called = true;
                this.url = url;
                this.mimeType = mimetype;
                this.contentLength = contentLength;
                this.contentDisposition = contentDisposition;
            }
        }

        final String mimeType = "application/octet-stream";
        final int length = 100;
        final MyDownloadListener listener = new MyDownloadListener();

        startWebServer(false);
        final String url = mWebServer.getBinaryUrl(mimeType, length);

        runTestOnUiThread(new Runnable() {
            public void run() {
                // By default, WebView sends an intent to ask the system to
                // handle loading a new URL. We set WebViewClient as
                // WebViewClient.shouldOverrideUrlLoading() returns false, so
                // the WebView will load the new URL.
                mWebView.setWebViewClient(new WebViewClient());
                mWebView.setDownloadListener(listener);
                mWebView.loadData("<html><body><a href=\"" + url + "\">link</a></body></html>",
                        "text/html", null);
                waitForLoadComplete();
            }
        });
        // Wait for layout to complete before setting focus.
        getInstrumentation().waitForIdleSync();
        runTestOnUiThread(new Runnable() {
            public void run() {
                assertTrue(mWebView.requestFocus(View.FOCUS_DOWN, null));
            }
        });
        getInstrumentation().waitForIdleSync();
        getInstrumentation().sendKeyDownUpSync(KeyEvent.KEYCODE_DPAD_CENTER);
        new DelayedCheck(TEST_TIMEOUT) {
            @Override
            protected boolean check() {
                return listener.called;
            }
        }.run();
        assertEquals(url, listener.url);
        assertTrue(listener.contentDisposition.contains("test.bin"));
        // ToBeFixed: uncomment the following tests after fixing the framework
        // assertEquals(mimeType, listener.mimeType);
        // assertEquals(length, listener.contentLength);
    }

    @TestTargetNew(
        level = TestLevel.PARTIAL,
        method = "setLayoutParams",
        args = {android.view.ViewGroup.LayoutParams.class}
    )
    @ToBeFixed(bug = "1695243", explanation = "the javadoc for setLayoutParams() is incomplete.")
    @UiThreadTest
    public void testSetLayoutParams() {
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(600, 800);
        mWebView.setLayoutParams(params);
        assertSame(params, mWebView.getLayoutParams());
    }

    @TestTargetNew(
        level = TestLevel.NOT_FEASIBLE,
        notes = "No documentation",
        method = "setMapTrackballToArrowKeys",
        args = {boolean.class}
    )
    @UiThreadTest
    public void testSetMapTrackballToArrowKeys() {
        mWebView.setMapTrackballToArrowKeys(true);
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "setNetworkAvailable",
        args = {boolean.class}
    )
    @UiThreadTest
    public void testSetNetworkAvailable() throws Exception {
        WebSettings settings = mWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        startWebServer(false);
        String url = mWebServer.getAssetUrl(TestHtmlConstants.NETWORK_STATE_URL);
        assertLoadUrlSuccessfully(url);
        assertEquals("ONLINE", mWebView.getTitle());

        mWebView.setNetworkAvailable(false);
        mWebView.reload();
        waitForLoadComplete();
        assertEquals("OFFLINE", mWebView.getTitle());

        mWebView.setNetworkAvailable(true);
        mWebView.reload();
        waitForLoadComplete();
        assertEquals("ONLINE", mWebView.getTitle());
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "setWebChromeClient",
        args = {WebChromeClient.class}
    )
    public void testSetWebChromeClient() throws Throwable {
        final class MockWebChromeClient extends WebChromeClient {
            private boolean mOnProgressChanged = false;
            @Override
            public void onProgressChanged(WebView view, int newProgress) {
                super.onProgressChanged(view, newProgress);
                mOnProgressChanged = true;
            }
            public boolean onProgressChangedCalled() {
                return mOnProgressChanged;
            }
        }

        final MockWebChromeClient webChromeClient = new MockWebChromeClient();

        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.setWebChromeClient(webChromeClient);
            }
        });
        getInstrumentation().waitForIdleSync();
        assertFalse(webChromeClient.onProgressChangedCalled());

        startWebServer(false);
        final String url = mWebServer.getAssetUrl(TestHtmlConstants.HELLO_WORLD_URL);
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadUrl(url);
            }
        });
        getInstrumentation().waitForIdleSync();

        new DelayedCheck(TEST_TIMEOUT) {
            @Override
            protected boolean check() {
                return webChromeClient.onProgressChangedCalled();
            }
        }.run();
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "pauseTimers",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "resumeTimers",
            args = {}
        )
    })
    public void testPauseResumeTimers() throws Throwable {
        class Monitor {
            private boolean mIsUpdated;
            public synchronized void update() {
                mIsUpdated  = true;
                notify();
            }
            public synchronized boolean waitForUpdate() {
                while (!mIsUpdated) {
                    try {
                        // This is slightly flaky, as we can't guarantee that
                        // this is a sufficient time limit, but there's no way
                        // around this.
                        wait(1000);
                        if (!mIsUpdated) {
                            return false;
                        }
                    } catch (InterruptedException e) {
                    }
                }
                mIsUpdated = false;
                return true;
            }
        };
        final Monitor monitor = new Monitor();
        final String updateMonitorHtml = "<html><head></head>" +
                "<body onload=\"monitor.update();\"></body></html>";

        // Test that JavaScript is executed even with timers paused.
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.getSettings().setJavaScriptEnabled(true);
                mWebView.addJavascriptInterface(monitor, "monitor");
                mWebView.pauseTimers();
                mWebView.loadData(updateMonitorHtml, "text/html", null);
            }
        });
        assertTrue(monitor.waitForUpdate());

        // Start a timer and test that it does not fire.
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.loadUrl("javascript:setTimeout(function(){monitor.update();},100)");
            }
        });
        assertFalse(monitor.waitForUpdate());

        // Resume timers and test that the timer fires.
        runTestOnUiThread(new Runnable() {
            public void run() {
                mWebView.resumeTimers();
            }
        });
        assertTrue(monitor.waitForUpdate());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "dispatchKeyEvent",
            args = {KeyEvent.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onAttachedToWindow",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onDetachedFromWindow",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onChildViewAdded",
            args = {View.class, View.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onChildViewRemoved",
            args = {View.class, View.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onDraw",
            args = {Canvas.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onFocusChanged",
            args = {boolean.class, int.class, Rect.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onGlobalFocusChanged",
            args = {View.class, View.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onKeyDown",
            args = {int.class, KeyEvent.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onKeyUp",
            args = {int.class, KeyEvent.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onMeasure",
            args = {int.class, int.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onScrollChanged",
            args = {int.class, int.class, int.class, int.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onSizeChanged",
            args = {int.class, int.class, int.class, int.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onTouchEvent",
            args = {MotionEvent.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onTrackballEvent",
            args = {MotionEvent.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_NECESSARY,
            method = "onWindowFocusChanged",
            args = {boolean.class}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_FEASIBLE,
            method = "computeScroll",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_FEASIBLE,
            method = "computeHorizontalScrollRange",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.NOT_FEASIBLE,
            method = "computeVerticalScrollRange",
            args = {}
        )
    })
    @UiThreadTest
    public void testInternals() {
        // Do not test these APIs. They are implementation details.
    }

    private static class HrefCheckHandler extends Handler {
        private boolean mHadRecieved;

        private String mResultUrl;

        public HrefCheckHandler(Looper looper) {
            super(looper);
        }

        public boolean hasCalledHandleMessage() {
            return mHadRecieved;
        }

        public String getResultUrl() {
            return mResultUrl;
        }

        public void reset(){
            mResultUrl = null;
            mHadRecieved = false;
        }

        @Override
        public void handleMessage(Message msg) {
            mResultUrl = msg.getData().getString("url");
            mHadRecieved = true;
        }
    }

    private void moveFocusDown() throws Throwable {
        // send down key and wait for idle
        sendKeys(KeyEvent.KEYCODE_DPAD_DOWN);
        // waiting for idle isn't always sufficient for the key to be fully processed
        Thread.sleep(500);
    }

    private final class ScrollRunnable implements Runnable {
        private int mScrollX;
        private int mScrollY;
        @Override
        public void run() {
            mScrollX = mWebView.getScrollX();
            mScrollY = mWebView.getScrollY();
        }
        public int getScrollX() {
            return mScrollX;
        }
        public int getScrollY() {
            return mScrollY;
        }
    }

    private void delayedCheckWebBackForwardList(final String currUrl, final int currIndex,
            final int size) {
        new DelayedCheck() {
            @Override
            protected boolean check() {
                WebBackForwardList list = mWebView.copyBackForwardList();
                return checkWebBackForwardList(list, currUrl, currIndex, size);
            }
        }.run();
    }

    private boolean checkWebBackForwardList(WebBackForwardList list, String currUrl,
            int currIndex, int size) {
        return (list != null)
                && (list.getSize() == size)
                && (list.getCurrentIndex() == currIndex)
                && list.getItemAtIndex(currIndex).getUrl().equals(currUrl);
    }

    private void assertGoBackOrForwardBySteps(boolean expected, int steps) {
        // skip if steps equals to 0
        if (steps == 0)
            return;

        int start = steps > 0 ? 1 : steps;
        int end = steps > 0 ? steps : -1;

        // check all the steps in the history
        for (int i = start; i <= end; i++) {
            assertEquals(expected, mWebView.canGoBackOrForward(i));

            // shortcut methods for one step
            if (i == 1) {
                assertEquals(expected, mWebView.canGoForward());
            } else if (i == -1) {
                assertEquals(expected, mWebView.canGoBack());
            }
        }
    }

    private void assertBitmapFillWithColor(Bitmap bitmap, int color) {
        for (int i = 0; i < bitmap.getWidth(); i ++)
            for (int j = 0; j < bitmap.getHeight(); j ++) {
                assertEquals(color, bitmap.getPixel(i, j));
            }
    }

    // Find b1 inside b2
    private boolean checkBitmapInsideAnother(Bitmap b1, Bitmap b2) {
        int w = b1.getWidth();
        int h = b1.getHeight();

        for (int i = 0; i < (b2.getWidth()-w+1); i++) {
            for (int j = 0; j < (b2.getHeight()-h+1); j++) {
                if (checkBitmapInsideAnother(b1, b2, i, j))
                    return true;
            }
        }
        return false;
    }

    private boolean comparePixel(int p1, int p2, int maxError) {
        int err;
        err = Math.abs(((p1&0xff000000)>>>24) - ((p2&0xff000000)>>>24));
        if (err > maxError)
            return false;

        err = Math.abs(((p1&0x00ff0000)>>>16) - ((p2&0x00ff0000)>>>16));
        if (err > maxError)
            return false;

        err = Math.abs(((p1&0x0000ff00)>>>8) - ((p2&0x0000ff00)>>>8));
        if (err > maxError)
            return false;

        err = Math.abs(((p1&0x000000ff)>>>0) - ((p2&0x000000ff)>>>0));
        if (err > maxError)
            return false;

        return true;
    }

    private boolean checkBitmapInsideAnother(Bitmap b1, Bitmap b2, int x, int y) {
        for (int i = 0; i < b1.getWidth(); i++)
            for (int j = 0; j < b1.getHeight(); j++) {
                if (!comparePixel(b1.getPixel(i, j), b2.getPixel(x + i, y + j), 10)) {
                    return false;
                }
            }
        return true;
    }

    private void assertLoadUrlSuccessfully(String url) {
        mWebView.loadUrl(url);
        waitForLoadComplete();
    }

    private void waitForLoadComplete() {
        new DelayedCheck(TEST_TIMEOUT) {
            @Override
            protected boolean check() {
                return mWebView.getProgress() == 100;
            }
        }.run();
        try {
            Thread.sleep(TIME_FOR_LAYOUT);
        } catch (InterruptedException e) {
            Log.w(LOGTAG, "waitForLoadComplete() interrupted while sleeping for layout delay.");
        }
    }

    private synchronized void notifyUiThreadDone() {
        mIsUiThreadDone = true;
        notify();
    }

    private synchronized void waitForUiThreadDone() throws InterruptedException {
        while (!mIsUiThreadDone) {
            try {
                wait(TEST_TIMEOUT);
            } catch (InterruptedException e) {
                continue;
            }
            if (!mIsUiThreadDone) {
                Assert.fail("Unexpected timeout");
            }
        }
    }

    final class LoadCompleteWebChromeClient extends WebChromeClient {
        @Override
        public void onProgressChanged(WebView webView, int progress) {
            super.onProgressChanged(webView, progress);
            if (progress == 100) {
                notifyUiThreadDone();
            }
        }
    }

    // Note that this class is not thread-safe.
    final class SslErrorWebViewClient extends WebViewClient {
        private boolean mWasOnReceivedSslErrorCalled;
        @Override
        public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
            mWasOnReceivedSslErrorCalled = true;
            handler.proceed();
        }
        public void resetWasOnReceivedSslErrorCalled() {
            mWasOnReceivedSslErrorCalled = false;
        }
        public boolean wasOnReceivedSslErrorCalled() {
            return mWasOnReceivedSslErrorCalled;
        }
    }
}
