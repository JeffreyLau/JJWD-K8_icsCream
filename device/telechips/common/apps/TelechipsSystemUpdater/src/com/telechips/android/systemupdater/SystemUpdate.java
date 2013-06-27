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

import android.util.Log;
import android.app.Activity;
import android.app.AlertDialog;
import android.provider.Downloads;
import android.content.Intent;
import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.DialogInterface;
import android.content.ContentValues;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;
import android.os.IBinder;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceGroup;
import android.preference.PreferenceActivity;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.os.Bundle;
import android.os.storage.StorageManager;
import android.os.storage.StorageVolume;
import android.os.Environment;

import java.util.ArrayList;

public class SystemUpdate extends PreferenceActivity
    implements Preference.OnPreferenceChangeListener {
    static final String LOG_TAG = "TelechipsSystemUpdater";

    private static final String PROPERTY_FOTA_ENABLE = "ro.fota.enable";

    public static final String SCHEDULED_CHECK_KEY = "scheduled_check";
    public static final String INSTALL_SYSTEM_UPDATE_KEY = "install_system_update";

    private Button mCheckNowButton;
    private ListPreference mUpdateFrom;
    private boolean mFOTAEnabled;
    private StorageManager mStorageManager;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ListPreference mStorageSelect;
        CharSequence[] mEntries;
        CharSequence[] mValues;
        CharSequence[] values;
        ArrayList<CharSequence> revisedEntries = new ArrayList<CharSequence>();
        ArrayList<CharSequence> revisedValues = new ArrayList<CharSequence>();

        mFOTAEnabled = SystemProperties.getBoolean(PROPERTY_FOTA_ENABLE, false);

        addPreferencesFromResource(R.xml.system_update_settings);

        findPreference(SCHEDULED_CHECK_KEY).setOnPreferenceChangeListener(this);

        mStorageManager = (StorageManager) getSystemService(Context.STORAGE_SERVICE);
        StorageVolume[] storageVolumes = mStorageManager.getVolumeList();
        mStorageSelect = (ListPreference) findPreference(INSTALL_SYSTEM_UPDATE_KEY);
        mStorageSelect.setOnPreferenceChangeListener(this);

        if(false) {//SystemProperties.getBoolean("tcc.solution.mbox", false))) {
            int length = storageVolumes.length;
            for (int i = 0; i < length; i++) {
                StorageVolume storageVolume = storageVolumes[i];
                String state = mStorageManager.getVolumeState(storageVolume.getPath());
                if (Environment.MEDIA_MOUNTED.equals(state)) {
                    revisedEntries.add(storageVolume.getDescription());
                    revisedValues.add(storageVolume.getPath());
                }
            }
            mStorageSelect.setEntries(revisedEntries.toArray(new CharSequence[revisedEntries.size()]));
            mStorageSelect.setEntryValues(revisedValues.toArray(new CharSequence[revisedValues.size()]));
        }


        /* Add FOTA related items if it is enabled */
        if (mFOTAEnabled) {
            setContentView(R.layout.system_update_activity);

            mCheckNowButton = (Button) findViewById(R.id.check_now_button);
        } else {
            getPreferenceScreen().removePreference(findPreference(SCHEDULED_CHECK_KEY));
        }
        bindService(new Intent(this, SystemUpdateService.class),
                    mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unbindService(mConnection);
    }

    public boolean onPreferenceChange(Preference preference, Object newValue) {
        if (preference.getKey().equals(SCHEDULED_CHECK_KEY)) {
            if (((Boolean) newValue).equals(true)) {
                mService.setScheduledCheck(true);
            } else {
                mService.setScheduledCheck(false);
            }
        } else if (preference.getKey().equals(INSTALL_SYSTEM_UPDATE_KEY)) {
            String updateFile;

            if (((String) newValue).equals("0"))
                updateFile = "/nand/update.zip";
            else if (((String) newValue).equals("1"))
                updateFile = "/sdcard/update.zip";
            else if (((String) newValue).equals("2"))
                updateFile = "/usb/update.zip";
            else if (((String) newValue).equals("3"))
                updateFile = "/sata/update.zip";
            else
                updateFile = (String)newValue + "/update.zip";

            startActivity(new Intent(this, SystemUpdateInstallDialog.class)
                                   .putExtra("file", updateFile));
        }
        return true;
    }

    ISystemUpdateCallback mCheckUpdateCallback = new ISystemUpdateCallback() {
        public void onResult(int resultCode) {
            Log.v(LOG_TAG, "onResult: resultCode = " + resultCode);
            switch (resultCode) {
            case SystemUpdateService.RESULT_NO_UPDATE:
                /* No upate available */
                startActivity(new Intent(SystemUpdate.this, SystemUpdateNoUpdate.class));
                break;

            case SystemUpdateService.RESULT_UPDATE_AVAILABLE:
                /* There are system update and can be downloaded */
                Intent intent = new Intent(SystemUpdate.this, SystemUpdateDownloadPrompt.class);
                startActivity(intent);
                break;

            case SystemUpdateService.RESULT_BAD_URL_ERROR:
                /* URL is not specified */
                Toast.makeText(SystemUpdate.this,
                    R.string.url_is_not_specified,
                    Toast.LENGTH_SHORT).show();
                break;

            case SystemUpdateService.RESULT_CHECK_ERROR:
                /* Error while checking update */
                Toast.makeText(SystemUpdate.this,
                    R.string.cannot_check_update,
                    Toast.LENGTH_SHORT).show();
                break;
            }
        }
    };

    private OnClickListener mCheckNowListener = new OnClickListener() {
       public void onClick(View v) {
           mService.checkUpdate(mCheckUpdateCallback);
        }
    };

    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            mService = ((SystemUpdateService.LocalBinder) service).getService();
            if (mFOTAEnabled)
                mCheckNowButton.setOnClickListener(mCheckNowListener);
        }
            
        public void onServiceDisconnected(ComponentName className) {
            mService = null;
        }
    };

    private SystemUpdateService mService;
}
