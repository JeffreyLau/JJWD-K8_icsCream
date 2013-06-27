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

import java.net.URL;
import android.util.Log;
import android.app.Activity;
import android.widget.Button;
import android.view.View;
import android.widget.TextView;
import android.view.View.OnClickListener;
import android.os.IBinder;
import android.content.Intent;
import android.content.Context;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.Bundle;

import android.provider.Downloads;
import android.content.ContentValues;

public class SystemUpdateDownloadPrompt extends Activity {
    final static String LOG_TAG = "TelechipsSystemUpdater";

    private Button mDownloadButton;

    private URL mFileURL;
    private String mVersion;
    private String mDetails;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.download_prompt);

        bindService(new Intent(this, SystemUpdateService.class),
                    mConnection, Context.BIND_AUTO_CREATE);

        mDownloadButton = (Button) findViewById(R.id.download_button);
    }

    private OnClickListener mDownloadButtonListener = new OnClickListener() {
        public void onClick(View v) {
            URL url = mFileURL;
            ContentValues values = new ContentValues();
            values.put(Downloads.Impl.COLUMN_URI, url.toString());
            values.put(Downloads.Impl.COLUMN_MIME_TYPE, "application/x-compressed");
            values.put(Downloads.Impl.COLUMN_DESTINATION, Downloads.Impl.DESTINATION_CACHE_PARTITION);
            values.put(Downloads.Impl.COLUMN_FILE_NAME_HINT, mService.getInstallFile());
            values.put(Downloads.Impl.COLUMN_TITLE, getString(R.string.downloading_notification_title));
            values.put(Downloads.Impl.COLUMN_VISIBILITY, Downloads.Impl.VISIBILITY_VISIBLE);
            values.put(Downloads.Impl.COLUMN_NOTIFICATION_PACKAGE, getPackageName());
            values.put(Downloads.Impl.COLUMN_NOTIFICATION_CLASS, SystemUpdateService.Receiver.class.getName());
            getContentResolver().insert(Downloads.Impl.CONTENT_URI, values);

            finish();
        }
    };

    @Override
    public void onDestroy() {
        super.onDestroy();
        unbindService(mConnection);
    }

    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            mService = ((SystemUpdateService.LocalBinder) service).getService();

            mFileURL = mService.getUpdateFileURL();
            mVersion = mService.getUpdateVersion();
            mDetails = mService.getUpdateDetails();

            TextView details = (TextView) findViewById(R.id.update_details);
            details.setText(mDetails);

            mDownloadButton.setOnClickListener(mDownloadButtonListener);
        }
            
        public void onServiceDisconnected(ComponentName className) {
            mService = null;
        }
    };

    private SystemUpdateService mService;
}
