/**
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.accessibilityservice.cts;

import static android.view.accessibility.AccessibilityNodeInfo.ACTION_CLEAR_FOCUS;
import static android.view.accessibility.AccessibilityNodeInfo.ACTION_CLEAR_SELECTION;
import static android.view.accessibility.AccessibilityNodeInfo.ACTION_FOCUS;
import static android.view.accessibility.AccessibilityNodeInfo.ACTION_SELECT;

import android.accessibilityservice.AccessibilityService;
import android.content.Context;
import android.graphics.Rect;
import android.os.RemoteException;
import android.test.ActivityInstrumentationTestCase2;
import android.test.suitebuilder.annotation.LargeTest;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityNodeInfo;

import com.android.cts.accessibilityservice.R;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

/**
 * Activity for testing the accessibility APIs for querying of
 * the screen content. These APIs allow exploring the screen and
 * requesting an action to be performed on a given view from an
 * AccessiiblityService.
 * <p>
 * Note: The accessibility CTS tests are composed of two APKs, one with delegating
 * accessibility service and another with the instrumented activity and test cases.
 * The motivation for two APKs design is that CTS tests cannot access the secure
 * settings which is required for enabling accessibility services, hence there is
 * no way to manipulate accessibility settings programmaticaly. Further, manually
 * enabling an accessibility service in the tests APK will not work either because
 * the instrumentation restarts the process under test which would break the binding
 * between the accessibility service and the system.
 * <p>
 * Therefore, manual installation of the
 * <strong>CtsAccessibilityServiceTestMockService.apk</strong>
 * whose source is located at <strong>cts/tests/accessibility</strong> is required.
 * Once the former package has been installed the service must be enabled
 * (Settings -> Accessibility -> Delegating Accessibility Service), and then the CTS tests
 * in this package can be successfully run.
 * </p>
 */
public class AccessibilityWindowQueryActivityTest
        extends ActivityInstrumentationTestCase2<AccessibilityWindowQueryActivity> {

    private static AccessibilityQueryBridge sQueryBridge;

    private interface AccessibilityEventFilter {
        public boolean accept(AccessibilityEvent event);
    }

    public AccessibilityWindowQueryActivityTest() {
        super(AccessibilityWindowQueryActivity.class);
    }

    @Override
    public void setUp() {
        // start the activity and wait for a handle to its window root.
        startActivityAndWaitForFirstEvent();
    }

    @LargeTest
    public void testFindByText() throws Exception {
        // find a view by text
        List<AccessibilityNodeInfo> buttons =
            getQueryBridge().findAccessibilityNodeInfosByText("butto");
        assertEquals(9, buttons.size());
    }

    @LargeTest
    public void testFindByContentDescription() throws Exception {
        // find a view by text
        AccessibilityNodeInfo button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.contentDescription));
        assertNotNull(button);
    }

    @LargeTest
    public void testTraverseWindow() throws Exception {
        // make list of expected nodes
        List<String> classNameAndTextList = new ArrayList<String>();
        classNameAndTextList.add("com.android.internal.policy.impl.PhoneWindow$DecorView");
        classNameAndTextList.add("android.widget.LinearLayout");
        classNameAndTextList.add("android.widget.FrameLayout");
        classNameAndTextList.add("android.widget.LinearLayout");
        classNameAndTextList.add("android.widget.LinearLayout");
        classNameAndTextList.add("android.widget.LinearLayout");
        classNameAndTextList.add("android.widget.LinearLayout");
        classNameAndTextList.add("android.widget.ButtonButton1");
        classNameAndTextList.add("android.widget.ButtonButton2");
        classNameAndTextList.add("android.widget.ButtonButton3");
        classNameAndTextList.add("android.widget.ButtonButton4");
        classNameAndTextList.add("android.widget.ButtonButton5");
        classNameAndTextList.add("android.widget.ButtonButton6");
        classNameAndTextList.add("android.widget.ButtonButton7");
        classNameAndTextList.add("android.widget.ButtonButton8");
        classNameAndTextList.add("android.widget.ButtonButton9");

        Queue<AccessibilityNodeInfo> fringe = new LinkedList<AccessibilityNodeInfo>();
        fringe.add(getQueryBridge().getAwaitedAccessibilityEventSource());

        // do a BFS traversal and check nodes
        while (!fringe.isEmpty()) {
            AccessibilityNodeInfo current = fringe.poll();

            CharSequence text = current.getText();
            String receivedClassNameAndText = current.getClassName().toString()
                + ((text != null) ? text.toString() : "");
            String expectedClassNameAndText = classNameAndTextList.remove(0);

            assertEquals("Did not get the expected node info",
                    expectedClassNameAndText, receivedClassNameAndText);

            final int childCount = current.getChildCount();
            for (int i = 0; i < childCount; i++) {
                AccessibilityNodeInfo child = getQueryBridge().getChild(current, i);
                fringe.add(child);
            }
        }
    }

    @LargeTest
    public void testPerformActionFocus() throws Exception {
        // find a view and make sure it is not focused
        AccessibilityNodeInfo button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));
        assertFalse(button.isFocused());

        // focus the view
        assertTrue(getQueryBridge().performAction(button, ACTION_FOCUS));

        // find the view again and make sure it is focused
        button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));
        assertTrue(button.isFocused());
    }

    @LargeTest
    public void testPerformActionClearFocus() throws Exception {
        // find a view and make sure it is not focused
        AccessibilityNodeInfo button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));
        assertFalse(button.isFocused());

        // focus the view
        assertTrue(getQueryBridge().performAction(button, ACTION_FOCUS));

        // find the view again and make sure it is focused
        button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));
        assertTrue(button.isFocused());

        // unfocus the view
        assertTrue(getQueryBridge().performAction(button, ACTION_CLEAR_FOCUS));

        // find the view again and make sure it is not focused
        button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(getString(
                R.string.button5));
        assertFalse(button.isFocused());
    }

    @LargeTest
    public void testPerformActionSelect() throws Exception {
        // find a view and make sure it is not selected
        AccessibilityNodeInfo button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));
        assertFalse(button.isSelected());

        // select the view
        assertTrue(sQueryBridge.performAction(button, ACTION_SELECT));

        // find the view again and make sure it is selected
        button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));
        assertTrue(button.isSelected());
    }

    @LargeTest
    public void testPerformActionClearSelection() throws Exception {
        // find a view and make sure it is not selected
        AccessibilityNodeInfo button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));
        assertFalse(button.isSelected());

        // select the view
        assertTrue(sQueryBridge.performAction(button, ACTION_SELECT));

        // find the view again and make sure it is selected
        button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));

        assertTrue(button.isSelected());

        // unselect the view
        assertTrue(getQueryBridge().performAction(button, ACTION_CLEAR_SELECTION));

        // find the view again and make sure it is not selected
        button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
                getString(R.string.button5));
        assertFalse(button.isSelected());
    }

    @LargeTest
    public void testGetEventSource() throws Exception {
        // find a view and make sure it is not focused
        final AccessibilityNodeInfo button =
            getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(getString(R.string.button5));
        assertFalse(button.isSelected());

        // focus and wait for the event
        getQueryBridge().perfromActionAndWaitForEvent(
                new Runnable() {
            @Override
            public void run() {
                assertTrue(getQueryBridge().performAction(button, ACTION_FOCUS));
            }
        },
                new AccessibilityEventFilter() {
            @Override
            public boolean accept(AccessibilityEvent event) {
                return (event.getEventType() == AccessibilityEvent.TYPE_VIEW_FOCUSED);
            }
        });

        // check that last event source
        AccessibilityNodeInfo source = getQueryBridge().getAwaitedAccessibilityEventSource();
        assertNotNull(source);

        // bounds
        Rect buttonBounds = new Rect();
        button.getBoundsInParent(buttonBounds);
        Rect sourceBounds = new Rect();
        source.getBoundsInParent(sourceBounds);

        assertEquals(buttonBounds.left, sourceBounds.left);
        assertEquals(buttonBounds.right, sourceBounds.right);
        assertEquals(buttonBounds.top, sourceBounds.top);
        assertEquals(buttonBounds.bottom, sourceBounds.bottom);

        // char sequence attributes
        assertEquals(button.getPackageName(), source.getPackageName());
        assertEquals(button.getClassName(), source.getClassName());
        assertEquals(button.getText(), source.getText());
        assertSame(button.getContentDescription(), source.getContentDescription());

        // boolean attributes
        assertSame(button.isFocusable(), source.isFocusable());
        assertSame(button.isClickable(), source.isClickable());
        assertSame(button.isEnabled(), source.isEnabled());
        assertNotSame(button.isFocused(), source.isFocused());
        assertSame(button.isLongClickable(), source.isLongClickable());
        assertSame(button.isPassword(), source.isPassword());
        assertSame(button.isSelected(), source.isSelected());
        assertSame(button.isCheckable(), source.isCheckable());
        assertSame(button.isChecked(), source.isChecked());
    }

    @LargeTest
    public void testObjectContract() throws Exception {
        // find a view and make sure it is not focused
        AccessibilityNodeInfo button = getQueryBridge().findAccessibilityNodeInfoByTextFromRoot(
               getString(R.string.button5));
        AccessibilityNodeInfo parent = getQueryBridge().getParent(button);
        final int childCount = parent.getChildCount();
        for (int i = 0; i < childCount; i++) {
            AccessibilityNodeInfo child = getQueryBridge().getChild(parent, i);
            assertNotNull(child);
            if (child.equals(button)) {
                assertEquals("Equal objects must have same hasCode.", button.hashCode(),
                        child.hashCode());
                return;
            }
        }
        fail("Parent's children do not have the info whose parent is the parent.");
    }

    @Override
    protected void scrubClass(Class<?> testCaseClass) {
        /* intentionally do not scrub */
    }

    private AccessibilityQueryBridge getQueryBridge() {
        if (sQueryBridge == null) {
            sQueryBridge = new AccessibilityQueryBridge(getInstrumentation().getContext());
        }
        return sQueryBridge;
    }

    /**
     * Starts the activity under tests and waits for the first accessibility
     * event from that activity.
     */
    private void startActivityAndWaitForFirstEvent() {
        getQueryBridge().perfromActionAndWaitForEvent(
                new Runnable() {
            @Override
            public void run() {
                getActivity();
            }
        },
                new AccessibilityEventFilter() {
            @Override
            public boolean accept(AccessibilityEvent event) {
                final int eventType = event.getEventType();
                CharSequence packageName = event.getPackageName();
                Context targetContext = getInstrumentation().getTargetContext();
                return (eventType == AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED
                        && targetContext.getPackageName().equals(packageName));
            }
        });
    }

    /**
     * @return The string for a given <code>resId</code>.
     */
    private String getString(int resId) {
        return getInstrumentation().getContext().getString(resId);
    }

    /**
     * This class serves as a bridge for querying the screen content.
     * The bride is connected of a delegating accessibility service.
     */
    static class AccessibilityQueryBridge extends AccessibilityService {

        /**
         * Helper for connecting to the delegating accessibility service.
         */
        private final AccessibilityDelegateHelper mAccessibilityDelegateHelper;

        /**
         * The last received accessibility event.
         */
        private AccessibilityEvent mAwaitedAccessbiliyEvent;

        /**
         * Barrier for synchronizing waiting client and this bridge.
         */
        private CyclicBarrier mBarrier = new CyclicBarrier(2);

        /**
         * Filter for the currently waited event.
         */
        private AccessibilityEventFilter mWaitedFilter;

        private AccessibilityQueryBridge(Context context) {
            mAccessibilityDelegateHelper = new AccessibilityDelegateHelper(this);
            mAccessibilityDelegateHelper.bindToDelegatingAccessibilityService(context);
        }

        @Override
        public void onAccessibilityEvent(AccessibilityEvent event) {
            if (mWaitedFilter != null && mWaitedFilter.accept(event)) {
                mAwaitedAccessbiliyEvent = AccessibilityEvent.obtain(event);
                awaitOnBarrier();
            }
        }

        @Override
        public void onInterrupt() {
            /* do nothing */
        }

        /**
         * Finds the first accessibility info that contains text. The search starts
         * from the given <code>root</code>
         *
         * @param text The searched text.
         * @return The node with this text or null.
         */
        public AccessibilityNodeInfo findAccessibilityNodeInfoByTextFromRoot(String text) {
            List<AccessibilityNodeInfo> nodes = findAccessibilityNodeInfosByText(text);
            if (nodes != null && !nodes.isEmpty()) {
                return nodes.get(0);
            }
            return null;
        }

        /**
         * @return The event that was waited for.
         */
        public AccessibilityNodeInfo getAwaitedAccessibilityEventSource() {
            if (mAwaitedAccessbiliyEvent != null) {
                return getSource(mAwaitedAccessbiliyEvent);
            }
            return null;
        }

        public List<AccessibilityNodeInfo> findAccessibilityNodeInfosByText(String text) {
            AccessibilityNodeInfo root = getAwaitedAccessibilityEventSource();
            if (root != null) {
                // Sending a node info across processes recycles
                // it so use a clone to avoid losing state
                AccessibilityNodeInfo rootClone = AccessibilityNodeInfo.obtain(root);
                try {
                     return mAccessibilityDelegateHelper.getQueryConnection()
                            .findAccessibilityNodeInfosByText(rootClone, text);
                } catch (RemoteException re) {
                    /* ignore */
                }
            }
            return Collections.emptyList();
        }

        public AccessibilityNodeInfo getParent(AccessibilityNodeInfo child) {
            try {
                // Sending a node info across processes recycles
                // it so use a clone to avoid losing state
                AccessibilityNodeInfo childClone = AccessibilityNodeInfo.obtain(child);
                return mAccessibilityDelegateHelper.getQueryConnection().getParent(childClone);
            } catch (RemoteException re) {
                /* ignore */
            }
            return null;
        }

        public AccessibilityNodeInfo getChild(AccessibilityNodeInfo parent, int index) {
            try {
                // Sending a node info across processes recycles
                // it so use a clone to avoid losing state
                AccessibilityNodeInfo parentClone = AccessibilityNodeInfo.obtain(parent);
                return mAccessibilityDelegateHelper.getQueryConnection().getChild(parentClone,
                        index);
            } catch (RemoteException re) {
                /* ignore */
            }
            return null;
        }

        public boolean performAction(AccessibilityNodeInfo target, int action) {
            try {
                // Sending a node info across processes recycles
                // it so use a clone to avoid losing state
                AccessibilityNodeInfo targetClone = AccessibilityNodeInfo.obtain(target);
                return mAccessibilityDelegateHelper.getQueryConnection().performAccessibilityAction(
                        targetClone, action);
            } catch (RemoteException re) {
                /* ignore */
            }
            return false;
        }

        private AccessibilityNodeInfo getSource(AccessibilityEvent event) {
            try {
                return mAccessibilityDelegateHelper.getQueryConnection().getSource(event);
            } catch (RemoteException re) {
                /* ignore */
            }
            return null;
        }

        /**
         * Performs an action and waits for the resulting event.
         *
         * @param action The action to perform.
         * @param filter Filter for recognizing the waited event.
         */
        public void perfromActionAndWaitForEvent(Runnable action,
                AccessibilityEventFilter filter) {
            reset();
            mWaitedFilter = filter;
            action.run();
            awaitOnBarrier();
        }

        /**
         * Rests the internal state.
         */
        private void reset() {
            if (mAwaitedAccessbiliyEvent != null) {
                mAwaitedAccessbiliyEvent.recycle();
                mAwaitedAccessbiliyEvent = null;
            }
            mBarrier.reset();
            mWaitedFilter = null;
        }

        /**
         * Calls await of the barrier taking care of the exceptions.
         */
        private void awaitOnBarrier() {
            try {
                mBarrier.await(AccessibilityDelegateHelper.TIMEOUT_ASYNC_PROCESSING,
                        TimeUnit.MILLISECONDS);
            } catch (InterruptedException ie) {
                /* ignore */
            } catch (BrokenBarrierException bbe) {
                /* ignore */
            } catch (TimeoutException te) {
                /* ignore */
            }
        }
    }
}
