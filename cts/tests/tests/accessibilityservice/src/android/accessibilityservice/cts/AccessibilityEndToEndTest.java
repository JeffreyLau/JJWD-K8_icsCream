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

package android.accessibilityservice.cts;

import android.accessibilityservice.AccessibilityService;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.SystemClock;
import android.test.ActivityInstrumentationTestCase2;
import android.test.suitebuilder.annotation.LargeTest;
import android.view.accessibility.AccessibilityEvent;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;

import com.android.cts.accessibilityservice.R;

import junit.framework.TestCase;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

/**
 * This class performs end-to-end testing of the accessibility feature by
 * creating an {@link Activity} and poking around so {@link AccessibilityEvent}s
 * are generated and their correct dispatch verified.
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
public class AccessibilityEndToEndTest extends
        ActivityInstrumentationTestCase2<AccessibilityEndToEndTestActivity> {

    /**
     * Timeout required for pending Binder calls or event processing to
     * complete.
     */
    private static final long TIMEOUT_ASYNC_PROCESSING = 500;

    /**
     * Creates a new instance for testing {@link AccessibilityEndToEndTestActivity}.
     *
     * @throws Exception If any error occurs.
     */
    public AccessibilityEndToEndTest() throws Exception {
        super(AccessibilityEndToEndTestActivity.class);
    }

    @LargeTest
    public void testTypeViewSelectedAccessibilityEvent() throws Throwable {
        Activity activity = getActivity();

        // Wait for accessibility events to settle i.e. for all events generated
        // while bringing the activity up to be delivered so they do not interfere.
        SystemClock.sleep(TIMEOUT_ASYNC_PROCESSING);

        // create and populate the expected event
        AccessibilityEvent selectedEvent = AccessibilityEvent.obtain();
        selectedEvent.setEventType(AccessibilityEvent.TYPE_VIEW_SELECTED);
        selectedEvent.setClassName(ListView.class.getName());
        selectedEvent.setPackageName(getActivity().getPackageName());
        selectedEvent.getText().add(activity.getString(R.string.second_list_item));
        selectedEvent.setItemCount(2);
        selectedEvent.setCurrentItemIndex(1);
        selectedEvent.setEnabled(true);
        selectedEvent.setScrollable(false);
        selectedEvent.setFromIndex(0);
        selectedEvent.setToIndex(1);

        // set expectations
        MockAccessibilityService service = MockAccessibilityService.getInstance(activity);
        service.expectEvent(selectedEvent);
        service.replay();

        // trigger the event
        final ListView listView = (ListView) activity.findViewById(R.id.listview);
        getActivity().runOnUiThread(new Runnable() {
            public void run() {
                listView.setSelection(1);
            }
        });

        // verify if all expected methods have been called
        service.verify();
    }

    @LargeTest
    public void testTypeViewClickedAccessibilityEvent() throws Throwable {
        Activity activity = getActivity();

        // Wait for accessibility events to settle i.e. for all events generated
        // while bringing the activity up to be delivered so they do not interfere.
        SystemClock.sleep(TIMEOUT_ASYNC_PROCESSING);

        // create and populate the expected event
        AccessibilityEvent clickedEvent = AccessibilityEvent.obtain();
        clickedEvent.setEventType(AccessibilityEvent.TYPE_VIEW_CLICKED);
        clickedEvent.setClassName(Button.class.getName());
        clickedEvent.setPackageName(getActivity().getPackageName());
        clickedEvent.getText().add(activity.getString(R.string.button_title));
        clickedEvent.setEnabled(true);

        // set expectations
        MockAccessibilityService service = MockAccessibilityService.getInstance(activity);
        service.expectEvent(clickedEvent);
        service.replay();

        // trigger the event
        final Button button = (Button) activity.findViewById(R.id.button);
        activity.runOnUiThread(new Runnable() {
            public void run() {
                button.performClick();
            }
        });

        // verify if all expected methods have been called
        service.verify();
    }

    @LargeTest
    public void testTypeViewLongClickedAccessibilityEvent() throws Throwable {
        Activity activity = getActivity();

        // Wait for accessibility events to settle i.e. for all events generated
        // while bringing the activity up to be delivered so they do not interfere.
        SystemClock.sleep(TIMEOUT_ASYNC_PROCESSING);

        // create and populate the expected event
        AccessibilityEvent longClickedEvent = AccessibilityEvent.obtain();
        longClickedEvent.setEventType(AccessibilityEvent.TYPE_VIEW_LONG_CLICKED);
        longClickedEvent.setClassName(Button.class.getName());
        longClickedEvent.setPackageName(getActivity().getPackageName());
        longClickedEvent.getText().add(activity.getString(R.string.button_title));
        longClickedEvent.setEnabled(true);

        // set expectations
        MockAccessibilityService service = MockAccessibilityService.getInstance(activity);
        service.expectEvent(longClickedEvent);
        service.replay();

        // trigger the event
        final Button button = (Button) activity.findViewById(R.id.button);
        activity.runOnUiThread(new Runnable() {
            public void run() {
                button.performLongClick();
            }
        });

        // verify if all expected methods have been called
        service.verify();
    }

    @LargeTest
    public void testTypeViewFocusedAccessibilityEvent() throws Throwable {
        Activity activity = getActivity();

        // Wait for accessibility events to settle i.e. for all events generated
        // while bringing the activity up to be delivered so they do not interfere.
        SystemClock.sleep(TIMEOUT_ASYNC_PROCESSING);

        // create and populate the expected event
        AccessibilityEvent focusedEvent = AccessibilityEvent.obtain();
        focusedEvent.setEventType(AccessibilityEvent.TYPE_VIEW_FOCUSED);
        focusedEvent.setClassName(Button.class.getName());
        focusedEvent.setPackageName(getActivity().getPackageName());
        focusedEvent.getText().add(activity.getString(R.string.button_title));
        focusedEvent.setItemCount(3);
        focusedEvent.setCurrentItemIndex(2);
        focusedEvent.setEnabled(true);

        // set expectations
        MockAccessibilityService service = MockAccessibilityService.getInstance(activity);
        service.expectEvent(focusedEvent);
        service.replay();

        // trigger the event
        final Button button = (Button) activity.findViewById(R.id.button);
        activity.runOnUiThread(new Runnable() {
            public void run() {
                button.requestFocus();
            }
        });

        // verify if all expected methods have been called
        service.verify();
    }

    @LargeTest
    public void testTypeViewTextChangedAccessibilityEvent() throws Throwable {
        final Activity activity = getActivity();

        // focus the edit text
        final EditText editText = (EditText) activity.findViewById(R.id.edittext);
        activity.runOnUiThread(new Runnable() {
            public void run() {
                editText.requestFocus();
            }
        });

        // wait for the generated focus event to be dispatched
        SystemClock.sleep(TIMEOUT_ASYNC_PROCESSING);


        final String beforeText = activity.getString(R.string.text_input_blah);
        final String newText = activity.getString(R.string.text_input_blah_blah);
        final String afterText = beforeText.substring(0, 3) + newText;

        // create and populate the expected event
        AccessibilityEvent textChangedEvent = AccessibilityEvent.obtain();
        textChangedEvent.setEventType(AccessibilityEvent.TYPE_VIEW_TEXT_CHANGED);
        textChangedEvent.setClassName(EditText.class.getName());
        textChangedEvent.setPackageName(getActivity().getPackageName());
        textChangedEvent.getText().add(afterText);
        textChangedEvent.setBeforeText(beforeText);
        textChangedEvent.setFromIndex(3);
        textChangedEvent.setAddedCount(9);
        textChangedEvent.setRemovedCount(1);
        textChangedEvent.setEnabled(true);

        // set expectations
        MockAccessibilityService service = MockAccessibilityService.getInstance(activity);
        service.expectEvent(textChangedEvent);
        service.replay();

        // trigger the event
        activity.runOnUiThread(new Runnable() {
            public void run() {
                editText.getEditableText().replace(3, 4, newText);
            }
        });

        // verify if all expected methods have been called
        service.verify();
    }

    @LargeTest
    public void testTypeWindowStateChangedAccessibilityEvent() throws Throwable {
        Activity activity = getActivity();

        // Wait for accessibility events to settle i.e. for all events generated
        // while bringing the activity up to be delivered so they do not interfere.
        SystemClock.sleep(TIMEOUT_ASYNC_PROCESSING);

        String title = activity.getString(R.string.alert_title);
        String message = activity.getString(R.string.alert_message);

        // create and populate the expected event
        AccessibilityEvent windowStateChangedEvent = AccessibilityEvent.obtain();
        windowStateChangedEvent.setEventType(AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED);
        windowStateChangedEvent.setClassName(AlertDialog.class.getName());
        windowStateChangedEvent.setPackageName(getActivity().getPackageName());
        windowStateChangedEvent.getText().add(title);
        windowStateChangedEvent.getText().add(message);
        windowStateChangedEvent.setEnabled(true);

        // set expectations
        MockAccessibilityService service = MockAccessibilityService.getInstance(activity);
        service.expectEvent(windowStateChangedEvent);
        service.replay();

        // trigger the event
        final EditText editText = (EditText) activity.findViewById(R.id.edittext);
        activity.runOnUiThread(new Runnable() {
            public void run() {
                AlertDialog dialog = (new AlertDialog.Builder(getActivity())
                        .setTitle(R.string.alert_title).setMessage(R.string.alert_message))
                        .create();
                dialog.show();
            }
        });

        // verify if all expected methods have been called
        service.verify();
    }

    @LargeTest
    public void testTypeNotificationStateChangedAccessibilityEvent() throws Throwable {
        Activity activity = getActivity();

        // Wait for accessibility events to settle i.e. for all events generated
        // while bringing the activity up to be delivered so they do not interfere.
        SystemClock.sleep(TIMEOUT_ASYNC_PROCESSING);

        String message = activity.getString(R.string.notification_message);

        // create the notification to send
        int notificationId = 1;
        Notification notification = new Notification();
        notification.icon = android.R.drawable.stat_notify_call_mute;
        notification.contentIntent = PendingIntent.getActivity(getActivity(), 0, new Intent(),
                PendingIntent.FLAG_CANCEL_CURRENT);
        notification.tickerText = message;
        notification.setLatestEventInfo(getActivity(), "", "", notification.contentIntent);

        // create and populate the expected event
        AccessibilityEvent notificationChangedEvent = AccessibilityEvent.obtain();
        notificationChangedEvent.setEventType(AccessibilityEvent.TYPE_NOTIFICATION_STATE_CHANGED);
        notificationChangedEvent.setClassName(Notification.class.getName());
        notificationChangedEvent.setPackageName(getActivity().getPackageName());
        notificationChangedEvent.getText().add(message);
        notificationChangedEvent.setParcelableData(notification);

        // set expectations
        MockAccessibilityService service = MockAccessibilityService.getInstance(activity);
        service.expectEvent(notificationChangedEvent);
        service.replay();

        // trigger the event
        NotificationManager notificationManager = (NotificationManager) activity
                .getSystemService(Service.NOTIFICATION_SERVICE);
        notificationManager.notify(notificationId, notification);

        // verify if all expected methods have been called
        service.verify();

        // remove the notification
        notificationManager.cancel(notificationId);
    }

    static class MockAccessibilityService extends AccessibilityService {

        /**
         * Helper for connecting to the delegating accessibility service.
         */
        private final AccessibilityDelegateHelper mAccessibilityDelegateHelper;

        /**
         * The singleton instance.
         */
        private static MockAccessibilityService sInstance;

        /**
         * The events this service expects to receive.
         */
        private final Queue<AccessibilityEvent> mExpectedEvents =
            new LinkedList<AccessibilityEvent>();

        /**
         * Reusable temporary builder.
         */
        private final StringBuilder mTempBuilder = new StringBuilder();

        /**
         * Interruption call this service expects to receive.
         */
        private boolean mExpectedInterrupt;

        /**
         * Flag if the mock is currently replaying.
         */
        private boolean mReplaying;

        /**
         * Lock for synchronization.
         */
        private final Object mLock = new Object();

        /**
         * Gets the {@link MockAccessibilityService} singleton.
         *
         * @param context A context handle.
         * @return The mock service.
         */
        public static MockAccessibilityService getInstance(Context context) {
            if (sInstance == null) {
                // since we do bind once and do not unbind from the delegating
                // service and JUnit3 does not support @BeforeTest and @AfterTest,
                // we will leak a service connection after the test but this
                // does not affect the test results and the test is twice as fast
                sInstance = new MockAccessibilityService(context);
            }
            return sInstance;
        }

        /**
         * Creates a new instance.
         */
        private MockAccessibilityService(Context context) {
            mAccessibilityDelegateHelper = new AccessibilityDelegateHelper(this);
            mAccessibilityDelegateHelper.bindToDelegatingAccessibilityService(
                    context);
        }

        /**
         * Starts replaying the mock.
         */
        public void replay() {
            mReplaying = true;
        }

        /**
         * Verifies the mock service.
         *
         * @throws IllegalStateException If the verification has failed.
         */
        public void verify() throws IllegalStateException {
            StringBuilder problems = mTempBuilder;
            final long startTime = SystemClock.uptimeMillis();
            synchronized (mLock) {
                while (true) {
                    if (!mReplaying) {
                        throw new IllegalStateException("Did you forget to call replay()?");
                    }
                    if (!mExpectedInterrupt && mExpectedEvents.isEmpty()) {
                        reset();
                        return; // success
                    }
                    problems.setLength(0);
                    if (mExpectedInterrupt) {
                        problems.append("Expected call to #interrupt() not received.");
                    }
                    if (!mExpectedEvents.isEmpty()) {
                        problems.append("Expected a call to onAccessibilityEvent() for events \""
                                + mExpectedEvents + "\" not received.");
                    }
                    final long elapsedTime = SystemClock.uptimeMillis() - startTime;
                    final long remainingTime = TIMEOUT_ASYNC_PROCESSING - elapsedTime;
                    if (remainingTime <= 0) {
                        reset();
                        if (problems.length() > 0) {
                            throw new IllegalStateException(problems.toString());
                        }
                        return;
                    }
                    try {
                        mLock.wait(remainingTime);
                    } catch (InterruptedException ie) {
                        /* ignore */
                    }
                }
            }
        }

        /**
         * Resets this instance so it can be reused.
         */
        private void reset() {
            synchronized (mLock) {
                mExpectedEvents.clear();
                mExpectedInterrupt = false;
                mReplaying = false;
                mLock.notifyAll();
            }
        }

        /**
         * Sets an expected call to
         * {@link #onAccessibilityEvent(AccessibilityEvent)} with given event as
         * argument.
         *
         * @param expectedEvent The expected event argument.
         */
        private void expectEvent(AccessibilityEvent expectedEvent) {
            mExpectedEvents.add(expectedEvent);
        }

        /**
         * Sets an expected call of {@link #onInterrupt()}.
         */
        public void expectInterrupt() {
            mExpectedInterrupt = true;
        }

        @Override
        public void onAccessibilityEvent(AccessibilityEvent receivedEvent) {
            synchronized (mLock) {
                if (!mReplaying) {
                    return;
                }
                if (mExpectedEvents.isEmpty()) {
                    throw new IllegalStateException("Unexpected event: " + receivedEvent);
                }
                AccessibilityEvent expectedEvent = mExpectedEvents.poll();
                assertEqualsAccessiblityEvent(expectedEvent, receivedEvent);
                mLock.notifyAll();
            }
        }

        @Override
        public void onInterrupt() {
            synchronized (mLock) {
                if (!mReplaying) {
                    return;
                }
                if (!mExpectedInterrupt) {
                    throw new IllegalStateException("Unexpected call to onInterrupt()");
                }
                mExpectedInterrupt = false;
                mLock.notifyAll();
            }
        }

        /**
         * Compares all properties of the <code>expectedEvent</code> and the
         * <code>receviedEvent</code> to verify that the received event is the
         * one that is expected.
         */
        private void assertEqualsAccessiblityEvent(AccessibilityEvent expectedEvent,
                AccessibilityEvent receivedEvent) {
            TestCase.assertEquals("addedCount has incorrect value", expectedEvent.getAddedCount(),
                    receivedEvent.getAddedCount());
            TestCase.assertEquals("beforeText has incorrect value", expectedEvent.getBeforeText(),
                    receivedEvent.getBeforeText());
            TestCase.assertEquals("checked has incorrect value", expectedEvent.isChecked(),
                    receivedEvent.isChecked());
            TestCase.assertEquals("className has incorrect value", expectedEvent.getClassName(),
                    receivedEvent.getClassName());
            TestCase.assertEquals("contentDescription has incorrect value", expectedEvent
                    .getContentDescription(), receivedEvent.getContentDescription());
            TestCase.assertEquals("currentItemIndex has incorrect value", expectedEvent
                    .getCurrentItemIndex(), receivedEvent.getCurrentItemIndex());
            TestCase.assertEquals("enabled has incorrect value", expectedEvent.isEnabled(),
                    receivedEvent.isEnabled());
            TestCase.assertEquals("eventType has incorrect value", expectedEvent.getEventType(),
                    receivedEvent.getEventType());
            TestCase.assertEquals("fromIndex has incorrect value", expectedEvent.getFromIndex(),
                    receivedEvent.getFromIndex());
            TestCase.assertEquals("fullScreen has incorrect value", expectedEvent.isFullScreen(),
                    receivedEvent.isFullScreen());
            TestCase.assertEquals("itemCount has incorrect value", expectedEvent.getItemCount(),
                    receivedEvent.getItemCount());
            assertEqualsNotificationAsParcelableData(expectedEvent, receivedEvent);
            TestCase.assertEquals("password has incorrect value", expectedEvent.isPassword(),
                    receivedEvent.isPassword());
            TestCase.assertEquals("removedCount has incorrect value", expectedEvent
                    .getRemovedCount(), receivedEvent.getRemovedCount());
            TestCase.assertEquals("scrollable has incorrect value", expectedEvent.isScrollable(),
                    receivedEvent.isScrollable());
            TestCase.assertEquals("toIndex has incorrect value", expectedEvent.getToIndex(),
                    receivedEvent.getToIndex());
            TestCase.assertEquals("recordCount has incorrect value", expectedEvent.getRecordCount(),
                    receivedEvent.getRecordCount());
            TestCase.assertEquals("scrollX has incorrect value", expectedEvent.getScrollX(),
                    receivedEvent.getScrollX());
            TestCase.assertEquals("scrollY has incorrect value", expectedEvent.getScrollY(),
                    receivedEvent.getScrollY());

            assertEqualsText(expectedEvent, receivedEvent);
        }

        /**
         * Compares the {@link android.os.Parcelable} data of the
         * <code>expectedEvent</code> and <code>receivedEvent</code> to verify
         * that the received event is the one that is expected.
         */
        private void assertEqualsNotificationAsParcelableData(AccessibilityEvent expectedEvent,
                AccessibilityEvent receivedEvent) {
            String message = "parcelableData has incorrect value";
            Notification expectedNotification = (Notification) expectedEvent.getParcelableData();
            Notification receivedNotification = (Notification) receivedEvent.getParcelableData();

            if (expectedNotification == null) {
                if (receivedNotification == null) {
                    return;
                }
            }

            TestCase.assertNotNull(message, receivedNotification);

            // we do a very simple sanity check
            TestCase.assertEquals(message, expectedNotification.tickerText.toString(),
                    receivedNotification.tickerText.toString());
        }

        /**
         * Compares the text of the <code>expectedEvent</code> and
         * <code>receivedEvent</code> by comparing the string representation of
         * the corresponding {@link CharSequence}s.
         */
        private void assertEqualsText(AccessibilityEvent expectedEvent,
                AccessibilityEvent receivedEvent) {
            String message = "text has incorrect value";
            List<CharSequence> expectedText = expectedEvent.getText();
            List<CharSequence> receivedText = receivedEvent.getText();

            TestCase.assertEquals(message, expectedText.size(), receivedText.size());

            Iterator<CharSequence> expectedTextIterator = expectedText.iterator();
            Iterator<CharSequence> receivedTextIterator = receivedText.iterator();

            for (int i = 0; i < expectedText.size(); i++) {
                // compare the string representation
                TestCase.assertEquals(message, expectedTextIterator.next().toString(),
                        receivedTextIterator.next().toString());
            }
        }
    }
}
