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
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.net.URL;
import java.net.MalformedURLException;
import android.util.Log;
import android.app.Activity;
import android.widget.Button;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;
import android.view.View.OnClickListener;
import android.os.IBinder;
import android.content.Intent;
import android.content.Context;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.app.ProgressDialog;
import android.os.Message;
import android.os.Handler;
import android.os.SystemProperties;

public class SystemUpdateInstallDialog extends Activity {
    final static String LOG_TAG = "TelechipsSystemUpdater";

    private Button mNowButton;
    private Button mLaterButton;

    private String mFile;
    private String mVersion;
    private String mDetails;
    private SystemUpdateInstallDialog instance;
    private ProgressDialog mProgress;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.install_dialog);

        bindService(new Intent(this, SystemUpdateService.class),
                    mConnection, Context.BIND_AUTO_CREATE);

        instance = this;
        mNowButton = (Button) findViewById(R.id.now);
        mLaterButton = (Button) findViewById(R.id.later);
        mLaterButton.setOnClickListener(mLaterButtonListener);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unbindService(mConnection);
    }

    private OnClickListener mNowButtonListener = new OnClickListener() {
        public void onClick(View v) {
            if (mFile != null) {
                Log.i(LOG_TAG, "Install system update: " + mFile);
                if (false) { //if (SystemProperties.getBoolean("tcc.solution.mbox", false)) {
                    mProgress = ProgressDialog.show(instance, "",
                            "Copy to cache directory", true);
                    Message msg = Message.obtain();
                    msg.obj = mFile;
                    handler.sendMessage(msg);
                } else {
                    mService.rebootAndUpdate(new File(mFile));
                    finish();
                }
            }
        }
    };

    private OnClickListener mLaterButtonListener = new OnClickListener() {
        public void onClick(View v) {
            setResult(RESULT_CANCELED);
            finish();
        }
    };

    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            mService = ((SystemUpdateService.LocalBinder) service).getService();
            String arg = SystemUpdateInstallDialog.this.getIntent().getStringExtra("file");
            if (arg != null) {
                mFile = arg;
            } else {
                mFile = mService.getInstallFile();
            }
            mNowButton.setOnClickListener(mNowButtonListener);
        }
            
        public void onServiceDisconnected(ComponentName className) {
            mService = null;
        }
    };

    private SystemUpdateService mService;

    Handler handler = new Handler(new Handler.Callback() {
        public boolean handleMessage(Message msg) {
            final File inFile = new File((String)msg.obj);
            final File outFile = new File("/cache/update.zip");

            if (inFile.exists() == false) {
                mProgress.dismiss();
                Toast.makeText(instance, "Can't find update file!", Toast.LENGTH_SHORT).show();
                return true;
            }

            boolean result = copyFile(inFile, outFile);
            mProgress.dismiss();
            if (result) {
                mService.rebootAndUpdate(outFile);
            }
            instance.finish();
            return true;
        }
    });

    private boolean copyFile(File inFile, File outFile) {
        try {
            FileInputStream in = new FileInputStream(inFile);
            FileOutputStream out = new FileOutputStream(outFile);

            if (in == null || out == null) {
                return false;
            }

            final int length = 256;
            byte[] buf = new byte[length];
            int index;
            final int offset = 0;
            while ((index = in.read(buf, offset, length)) != -1) {
                out.write(buf, offset, index);
            }
            out.close();
            in.close();
            return true;
        } catch(IOException e) {
            e.printStackTrace();
            return false;
        }
    }
}
