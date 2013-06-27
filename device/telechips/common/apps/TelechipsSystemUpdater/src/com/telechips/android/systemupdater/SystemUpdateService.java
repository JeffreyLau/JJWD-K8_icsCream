/*
 * Copyright (C) 2010 Telechips, Inc.
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

package com.telechips.android.systemupdater;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.net.MalformedURLException;
import android.util.Log;
import android.os.Binder;
import android.os.IBinder;
import android.os.Bundle;
import android.os.Process;
import android.os.Looper;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.app.Service;
import android.app.Notification;
import android.app.NotificationManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Activity;
import android.provider.Downloads;
import android.preference.PreferenceManager;
import android.os.RecoverySystem;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.entity.StringEntity;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.util.EntityUtils;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class SystemUpdateService extends Service {
    final static String LOG_TAG = "SystemUpdateService";
    final static boolean DEBUG = false;

    private static final long CHECK_INTERVAL = 7 * AlarmManager.INTERVAL_DAY;

    final static String PROPERTY_FOTA_URL = "ro.fota.url";
    final static String PREF_SCHEDULED_CHECK = "scheduled_check";

    public final static String KEY_CHECK = "check";

    public final static int RESULT_NO_UPDATE = 0;
    public final static int RESULT_UPDATE_AVAILABLE = 1;
    public final static int RESULT_BAD_URL_ERROR = -1;
    public final static int RESULT_CHECK_ERROR = -2;

    private AlarmManager mAlarmManager;
    private PendingIntent mScheduledCheckIntent;

    private URL mUpdateFileURL;
    private String mUpdateVersion;
    private String mUpdateDetails;
    private String mInstallFile;

    public static class Receiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (DEBUG) Log.v(LOG_TAG, intent.toString());
            if (Downloads.Impl.ACTION_DOWNLOAD_COMPLETED.equals(intent.getAction())) {
                Intent i = new Intent(context, SystemUpdateInstallDialog.class);
                i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(i);
            } else if (Intent.ACTION_BOOT_COMPLETED.equals(intent.getAction())) {
                context.startService(new Intent(context, SystemUpdateService.class));
            } else {
                Bundle args = new Bundle();
                args.putInt(SystemUpdateService.KEY_CHECK, 1);
                context.startService(new Intent(context, SystemUpdateService.class).putExtras(args));
            }
        }
    }

    public class LocalBinder extends Binder {
        SystemUpdateService getService() {
            return SystemUpdateService.this;
        }
    }

    @Override
    public void onCreate() {
        Log.i(LOG_TAG, "Service started");

        super.onCreate();

        mAlarmManager = (AlarmManager) getSystemService(ALARM_SERVICE);

        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        if (prefs.getBoolean(PREF_SCHEDULED_CHECK, false)) {
            setScheduledCheck(true);
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Bundle args = intent.getExtras();
        if (args != null) {
            if (args.getInt(KEY_CHECK) != 0) {
                checkUpdate(mCheckUpdateCallback);
            }
        }
        return START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    private final IBinder mBinder = new LocalBinder();

    private class XMLHandler extends DefaultHandler {
        String currentValue;
        String currentElement;

        private URL mFileURL;
        private String mVersion;
        private String mDetails = "";

        @Override
        public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
            currentElement = localName;
        }

        @Override
        public void endElement(String uri, String localName, String qName) throws SAXException {
            currentElement = "";
        }

        @Override
        public void characters(char[] ch, int start, int length) throws SAXException {
            if (currentElement.equalsIgnoreCase("file")) {
                try {
                    mFileURL = new URL(new String(ch, start, length));
                    if (DEBUG) Log.v(LOG_TAG, "file = " + mFileURL.toString());
                } catch (MalformedURLException e) {
                    e.printStackTrace();
                }
            } else if (currentElement.equalsIgnoreCase("version")) {
                mVersion = new String(ch, start, length);
                if (DEBUG) Log.v(LOG_TAG, "version = " + mVersion);
            } else if (currentElement.equalsIgnoreCase("details")) {
                mDetails += new String(ch, start, length);
                if (DEBUG) Log.v(LOG_TAG, "details = " + mDetails);
            }
        }

        public String getVersion() {
            return mVersion;
        }

        public URL getFileURL() {
            return mFileURL;
        }

        public String getDetails() {
            return mDetails;
        }
    }

    public String getUpdateVersion() {
        return mUpdateVersion;
    }

    public URL getUpdateFileURL() {
        return mUpdateFileURL;
    }

    public String getInstallFile() {
        return mInstallFile;
    }

    public String getUpdateDetails() {
        return mUpdateDetails;
    }

    public void setScheduledCheck(boolean on) {
        if (on) {
            PendingIntent i = PendingIntent.getBroadcast(this, 0, new Intent(this, Receiver.class), 0);
            long delay = 1000 * 30;     // 30 seconds
            mAlarmManager.setInexactRepeating(AlarmManager.RTC,
                System.currentTimeMillis() + delay,
                CHECK_INTERVAL, i);
            mScheduledCheckIntent = i;
            if (DEBUG) Log.v(LOG_TAG, "scheduled check on");
        } else {
            mAlarmManager.cancel(mScheduledCheckIntent);
            if (DEBUG) Log.v(LOG_TAG, "scheduled check off");
        }
    }

    void checkUpdate(ISystemUpdateCallback callback) {
        String url = SystemProperties.get(PROPERTY_FOTA_URL, "");
        if (url.equals("")) {
            callback.onResult(RESULT_BAD_URL_ERROR);
            return;
        }

        DefaultHttpClient client = new DefaultHttpClient();
        HttpPost request = new HttpPost(url);
        XMLHandler handler = new XMLHandler();

        try {
            ByteArrayEntity entity =
                new ByteArrayEntity(SystemProperties.get("ro.build.description").getBytes());
            entity.setContentType("text/plain");
            request.setEntity(entity);

            HttpResponse response = client.execute(request);

            SAXParser sp = SAXParserFactory.newInstance().newSAXParser();
            XMLReader xr = sp.getXMLReader();
            xr.setContentHandler(handler);
            xr.parse(new InputSource(response.getEntity().getContent()));
        } catch (Exception e) {
            e.printStackTrace();
            callback.onResult(RESULT_CHECK_ERROR);
            return;
        }

        if (handler.getFileURL() == null || handler.getFileURL().equals("")) {
            callback.onResult(RESULT_NO_UPDATE);
        } else {
            mUpdateFileURL = handler.getFileURL();
            mUpdateVersion = handler.getVersion();
            mUpdateDetails = handler.getDetails();
            mInstallFile = "/cache" + mUpdateFileURL.getFile();
            callback.onResult(RESULT_UPDATE_AVAILABLE);
        }
    }

    public void rebootAndUpdate(File file) {
        new InstallAndRebootThread(this, file).start();
    }

    private class InstallAndRebootThread extends Thread {
        private final Context mContext;
        private final File mFile;

        InstallAndRebootThread(Context context, File file) {
            mContext = context;
            mFile = file;
        }

        public void run() {
            try {
                RecoverySystem.installPackage(mContext, mFile);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
     } 

    ISystemUpdateCallback mCheckUpdateCallback = new ISystemUpdateCallback() {
        public void onResult(int resultCode) {
            if (resultCode == 1) {
                NotificationManager mgr =
                    (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
                Notification notification =
                    new Notification(R.drawable.notification_system_update_available,
                                     getString(R.string.update_available_notification_title), 0);
                Intent i = new Intent(SystemUpdateService.this, SystemUpdateDownloadPrompt.class);
                i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                PendingIntent pendingIntent =
                    PendingIntent.getActivity(SystemUpdateService.this, 0, i, 0);
                notification.setLatestEventInfo(SystemUpdateService.this,
                    getString(R.string.update_available_notification_title),
                    getString(R.string.update_available_notification_message), pendingIntent);
                mgr.notify(1, notification);
                }
            }
        };
}
