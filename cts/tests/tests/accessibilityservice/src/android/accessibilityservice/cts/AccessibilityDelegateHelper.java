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

package android.accessibilityservice.cts;

import static junit.framework.Assert.fail;

import android.accessibilityservice.AccessibilityService;
import android.accessibilityservice.AccessibilityServiceInfo;
import android.accessibilityservice.IAccessibilityServiceDelegate;
import android.accessibilityservice.IAccessibilityServiceDelegateConnection;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ServiceInfo;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.SystemClock;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityManager;

import java.util.List;

/**
 * This class is a helper for implementing delegation of accessibility events.
 * This is required because changing accessibility settings can be performed
 * either via writing to the secure settings, which a CTS test cannot do, or
 * by manual interaction from the settings application. However, manually
 * enabling the testing accessibility service does not work because the test
 * runner restarts the package before running the tests, thus breaking the
 * bond between the system and the manually enabled testing service. So,
 * we have a delegating service that is manually enabled which services as a
 * proxy to deliver accessibility events to the testing service.
 */
class AccessibilityDelegateHelper implements ServiceConnection {

   /**
    * Timeout required for pending Binder calls or event processing to
    * complete.
    */
    public static final long TIMEOUT_ASYNC_PROCESSING = 500;

    /**
     * The package of the accessibility service mock interface.
     */
    private static final String DELEGATING_SERVICE_PACKAGE =
        "android.accessibilityservice.delegate";

    /**
     * The package of the delegating accessibility service interface.
     */
    private static final String DELEGATING_SERVICE_CLASS_NAME =
        "android.accessibilityservice.delegate.DelegatingAccessibilityService";

    /**
     * The package of the delegating accessibility service connection interface.
     */
    private static final String DELEGATING_SERVICE_CONNECTION_CLASS_NAME =
        "android.accessibilityservice.delegate."
            + "DelegatingAccessibilityService$DelegatingConnectionService";

    /**
     * The client accessibility service to which to delegate.
     */
    private final AccessibilityService mAccessibilityService;

    /**
     * Lock for synchronization.
     */
    private final Object mLock = new Object();

    /**
     * Whether this delegate is initialized.
     */
    private boolean mInitialized;

    /**
     * Query connection to the delegating accessibility service.
     */
    private IAccessibilityServiceDelegateConnection mQueryConnection;

    /**
     * Creates a new instance.
     *
     * @param service The service to which to delegate.
     */
    public AccessibilityDelegateHelper(AccessibilityService service) {
        mAccessibilityService = service;
    }

    /**
     * Ensures the required setup for the test performed and that it is bound to the
     * DelegatingAccessibilityService which runs in another process. The setup is
     * enabling accessibility and installing and enabling the delegating accessibility
     * service this test binds to.
     * </p>
     * Note: Please look at the class description for information why such an
     *       approach is taken.
     */
    public void bindToDelegatingAccessibilityService(Context context) {
        // check if accessibility is enabled
        AccessibilityManager accessibilityManager = (AccessibilityManager) context
                .getSystemService(Service.ACCESSIBILITY_SERVICE);

        if (!accessibilityManager.isEnabled()) {
            throw new IllegalStateException("Delegating service not enabled. "
                    + "(Settings -> Accessibility -> Delegating Accessibility Service)");
        }

        // check if the delegating service is running
        List<AccessibilityServiceInfo> enabledServices =
            accessibilityManager.getEnabledAccessibilityServiceList(
                    AccessibilityServiceInfo.FEEDBACK_ALL_MASK);
        boolean delegatingServiceRunning = false;
        for (AccessibilityServiceInfo enabledService : enabledServices) {
            ServiceInfo serviceInfo = enabledService.getResolveInfo().serviceInfo;
            if (DELEGATING_SERVICE_PACKAGE.equals(serviceInfo.packageName)
                    && DELEGATING_SERVICE_CLASS_NAME.equals(serviceInfo.name)) {
                delegatingServiceRunning = true;
                break;
            }
        }

        if (!delegatingServiceRunning) {
            // delegating service not running, so check if it is installed at all
            try {
                PackageManager packageManager = context.getPackageManager();
                packageManager.getServiceInfo(new ComponentName(DELEGATING_SERVICE_PACKAGE,
                        DELEGATING_SERVICE_CLASS_NAME), 0);
            } catch (NameNotFoundException nnfe) {
                throw new IllegalStateException("CtsDelegatingAccessibilityService.apk" +
                        " not installed.");
            }

            throw new IllegalStateException("Delegating Accessibility Service not running."
                     + "(Settings -> Accessibility -> Delegating Accessibility Service)");
        }

        Intent intent = new Intent().setClassName(DELEGATING_SERVICE_PACKAGE,
                DELEGATING_SERVICE_CONNECTION_CLASS_NAME);
        context.bindService(intent, this, Context.BIND_AUTO_CREATE);

        final long beginTime = SystemClock.uptimeMillis();
        synchronized (mLock) {
            while (true) {
                if (mInitialized) {
                    return;
                }
                final long elapsedTime = (SystemClock.uptimeMillis() - beginTime);
                final long remainingTime = TIMEOUT_ASYNC_PROCESSING - elapsedTime;
                if (remainingTime <= 0) {
                    if (!mInitialized) {
                        throw new IllegalStateException("Cound not connect to the delegating"
                                + " accessibility service");
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
     * {@inheritDoc ServiceConnection#onServiceConnected(ComponentName,IBinder)}
     */
    public void onServiceConnected(ComponentName name, IBinder service) {
        mQueryConnection = IAccessibilityServiceDelegateConnection.Stub.asInterface(service);
        try {
            mQueryConnection.setAccessibilityServiceDelegate(
                    new IAccessibilityServiceDelegate.Stub() {
                @Override
                public void onAccessibilityEvent(AccessibilityEvent event) {
                    mAccessibilityService.onAccessibilityEvent(event);
                }
                @Override
                public void onInterrupt() {
                    mAccessibilityService.onInterrupt();
                }
            });
            mInitialized = true;
            synchronized (mLock) {
                mLock.notifyAll();
            }
        } catch (RemoteException re) {
            fail("Could not set delegate to the delegating service.");
        }
    }

    /**
     * {@inheritDoc ServiceConnection#onServiceDisconnected(ComponentName)}
     */
    public void onServiceDisconnected(ComponentName name) {
        mInitialized = false;
        /* do nothing */
    }

    /**
     * Gets the query connection to the delegating accessibility service.
     *
     * @return The connection.
     */
    public IAccessibilityServiceDelegateConnection getQueryConnection() {
        return mQueryConnection;
    }
}
