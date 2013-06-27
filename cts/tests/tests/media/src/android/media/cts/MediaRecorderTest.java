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
package android.media.cts;

import dalvik.annotation.TestLevel;
import dalvik.annotation.TestTargetClass;
import dalvik.annotation.TestTargetNew;
import dalvik.annotation.TestTargets;

import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.media.MediaRecorder;
import android.media.MediaRecorder.OnErrorListener;
import android.media.MediaRecorder.OnInfoListener;
import android.media.MediaMetadataRetriever;
import android.os.Environment;
import android.test.ActivityInstrumentationTestCase2;
import android.test.UiThreadTest;
import android.view.Surface;

import android.util.Log;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileOutputStream;

@TestTargetClass(MediaRecorder.class)
public class MediaRecorderTest extends ActivityInstrumentationTestCase2<MediaStubActivity> {
    private final String TAG = "MediaRecorderTest";
    private final String OUTPUT_PATH;
    private final String OUTPUT_PATH2;
    private static final float TOLERANCE = 0.0002f;
    private static final int RECORD_TIME = 3000;
    private static final int VIDEO_WIDTH = 176;
    private static final int VIDEO_HEIGHT = 144;
    private static final long MAX_FILE_SIZE = 5000;
    private static final int MAX_DURATION_MSEC = 200;
    private static final float LATITUDE = 0.0000f;
    private static final float LONGITUDE  = -180.0f;
    private boolean mOnInfoCalled;
    private boolean mOnErrorCalled;
    private File mOutFile;
    private File mOutFile2;
    private Camera mCamera;

    /*
     * InstrumentationTestRunner.onStart() calls Looper.prepare(), which creates a looper
     * for the current thread. However, since we don't actually call loop() in the test,
     * any messages queued with that looper will never be consumed. We instantiate the recorder
     * in the constructor, before setUp(), so that its constructor does not see the
     * nonfunctional Looper.
     */
    private MediaRecorder mMediaRecorder = new MediaRecorder();

    public MediaRecorderTest() {
        super("com.android.cts.stub", MediaStubActivity.class);
        OUTPUT_PATH = new File(Environment.getExternalStorageDirectory(),
                "record.out").getAbsolutePath();
        OUTPUT_PATH2 = new File(Environment.getExternalStorageDirectory(),
                "record2.out").getAbsolutePath();
    }

    @Override
    protected void setUp() throws Exception {
        mOutFile = new File(OUTPUT_PATH);
        mOutFile2 = new File(OUTPUT_PATH2);
        mMediaRecorder.reset();
        mMediaRecorder.setOutputFile(OUTPUT_PATH);
        mMediaRecorder.setOnInfoListener(new OnInfoListener() {
            public void onInfo(MediaRecorder mr, int what, int extra) {
                mOnInfoCalled = true;
            }
        });
        mMediaRecorder.setOnErrorListener(new OnErrorListener() {
            public void onError(MediaRecorder mr, int what, int extra) {
                mOnErrorCalled = true;
            }
        });
        super.setUp();
    }

    @Override
    protected void tearDown() throws Exception {
        mMediaRecorder.release();
        if (mOutFile != null && mOutFile.exists()) {
            mOutFile.delete();
        }
        if (mOutFile2 != null && mOutFile2.exists()) {
            mOutFile2.delete();
        }
        if (mCamera != null)  {
            mCamera.release();
            mCamera = null;
        }
        super.tearDown();
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "MediaRecorder",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "release",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOutputFormat",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setVideoEncoder",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setVideoSize",
            args = {int.class, int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setVideoSource",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOutputFile",
            args = {String.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "reset",
            args = {}
        )
    })
    public void testRecorderCamera() throws Exception {
        if (!hasCamera()) {
            return;
        }
        mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.DEFAULT);
        mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.DEFAULT);
        mMediaRecorder.setVideoSize(VIDEO_WIDTH, VIDEO_HEIGHT);
        mMediaRecorder.setPreviewDisplay(getActivity().getSurfaceHolder().getSurface());
        mMediaRecorder.prepare();
        mMediaRecorder.start();
        Thread.sleep(1000);
        mMediaRecorder.stop();
        checkOutputExist();
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setVideoFrameRate",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setLocation",
            args = {float.class,float.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setCamera",
            args = {Camera.class}
        )
    })
    @UiThreadTest
    public void testSetCamera() throws Exception {
        int nCamera = Camera.getNumberOfCameras();
        for (int cameraId = 0; cameraId < nCamera; cameraId++) {
            mCamera = Camera.open(cameraId);

            // FIXME:
            // We should add some test case to use Camera.Parameters.getPreviewFpsRange()
            // to get the supported video frame rate range.
            Camera.Parameters params = mCamera.getParameters();
            int frameRate = params.getPreviewFrameRate();

            mCamera.unlock();
            mMediaRecorder.setCamera(mCamera);
            mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
            mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.DEFAULT);
            mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.DEFAULT);
            mMediaRecorder.setVideoFrameRate(frameRate);
            mMediaRecorder.setVideoSize(VIDEO_WIDTH, VIDEO_HEIGHT);
            mMediaRecorder.setPreviewDisplay(getActivity().getSurfaceHolder().getSurface());
            mMediaRecorder.setOutputFile(OUTPUT_PATH);
            mMediaRecorder.setLocation(LATITUDE, LONGITUDE);

            mMediaRecorder.prepare();
            mMediaRecorder.start();
            Thread.sleep(1000);
            mMediaRecorder.stop();
            assertTrue(mOutFile.exists());

            mCamera.release();
            assertTrue(checkLocationInFile(OUTPUT_PATH));
        }
    }

    private boolean checkLocationInFile(String fileName) {
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        retriever.setDataSource(fileName);
        String location = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_LOCATION);
        if (location == null) {
            Log.v(TAG, "No location information found in file " + fileName);
            return false;
        }

        // parsing String location and recover the location inforamtion in floats
        // Make sure the tolerance is very small - due to rounding errors?.
        Log.v(TAG, "location: " + location);

        // Get the position of the -/+ sign in location String, which indicates
        // the beginning of the longtitude.
        int index = location.lastIndexOf('-');
        if (index == -1) {
            index = location.lastIndexOf('+');
        }
        assertTrue("+ or - is not found", index != -1);
        assertTrue("+ or - is only found at the beginning", index != 0);
        float latitude = Float.parseFloat(location.substring(0, index - 1));
        float longitude = Float.parseFloat(location.substring(index));
        assertTrue("Incorrect latitude: " + latitude, Math.abs(latitude - LATITUDE) <= TOLERANCE);
        assertTrue("Incorrect longitude: " + longitude, Math.abs(longitude - LONGITUDE) <= TOLERANCE);
        return true;
    }

    private void checkOutputExist() {
        assertTrue(mOutFile.exists());
        assertTrue(mOutFile.length() > 0);
        assertTrue(mOutFile.delete());
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "MediaRecorder",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "prepare",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOutputFile",
            args = {FileDescriptor.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOutputFormat",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setPreviewDisplay",
            args = {Surface.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setVideoEncoder",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setVideoSize",
            args = {int.class, int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setVideoSource",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "start",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "stop",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setMaxFileSize",
            args = {long.class}
        )
    })
    public void testRecorderVideo() throws Exception {
        if (!hasCamera()) {
            return;
        }
        mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.DEFAULT);
        mMediaRecorder.setOutputFile(OUTPUT_PATH2);
        mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.DEFAULT);
        mMediaRecorder.setPreviewDisplay(getActivity().getSurfaceHolder().getSurface());
        mMediaRecorder.setVideoSize(VIDEO_WIDTH, VIDEO_HEIGHT);
        FileOutputStream fos = new FileOutputStream(OUTPUT_PATH2);
        FileDescriptor fd = fos.getFD();
        mMediaRecorder.setOutputFile(fd);
        long maxFileSize = MAX_FILE_SIZE * 10;
        recordMedia(maxFileSize, mOutFile2);
        assertFalse(checkLocationInFile(OUTPUT_PATH2));
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "MediaRecorder",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "getMaxAmplitude",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "prepare",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setAudioEncoder",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setAudioSource",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOutputFile",
            args = {String.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOutputFormat",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "start",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "stop",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setMaxFileSize",
            args = {long.class}
        )
    })
    public void testRecorderAudio() throws Exception {
        if (!hasMicrophone()) {
            return;
        }
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        assertEquals(0, mMediaRecorder.getMaxAmplitude());
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
        mMediaRecorder.setOutputFile(OUTPUT_PATH);
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
        recordMedia(MAX_FILE_SIZE, mOutFile);
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "MediaRecorder",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "prepare",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setAudioEncoder",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setAudioSource",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOutputFormat",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "start",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setMaxDuration",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOnInfoListener",
            args = {OnInfoListener.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOnErrorListener",
            args = {OnErrorListener.class}
        )
    })
    public void testOnInfoListener() throws Exception {
        if (!hasMicrophone()) {
            return;
        }
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
        mMediaRecorder.setMaxDuration(MAX_DURATION_MSEC);
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
        mMediaRecorder.prepare();
        mMediaRecorder.start();
        Thread.sleep(RECORD_TIME);
        assertTrue(mOnInfoCalled);
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "setMaxDuration",
        args = {int.class}
    )
    public void testSetMaxDuration() throws Exception {
        if (!hasMicrophone()) {
            return;
        }
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
        mMediaRecorder.setMaxDuration(0);
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
        mMediaRecorder.prepare();
        mMediaRecorder.start();
        Thread.sleep(RECORD_TIME * 30);
        mMediaRecorder.stop();
        checkOutputExist();
    }

    @TestTargetNew(
        level = TestLevel.COMPLETE,
        method = "setMaxFileSize",
        args = {int.class}
    )
    public void testSetMaxFileSize() throws Exception {
        if (!hasMicrophone()) {
            return;
        }
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
        mMediaRecorder.setMaxFileSize(0);
        mMediaRecorder.prepare();
        mMediaRecorder.start();
        Thread.sleep(RECORD_TIME * 30);
        mMediaRecorder.stop();
        checkOutputExist();
    }

    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "MediaRecorder",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "prepare",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setAudioEncoder",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setAudioSource",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOutputFormat",
            args = {int.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "start",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "stop",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setOnErrorListener",
            args = {OnErrorListener.class}
        ),
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            method = "setMaxFileSize",
            args = {long.class}
        )
    })
    public void testOnErrorListener() throws Exception {
        if (!hasMicrophone()) {
            return;
        }
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.DEFAULT);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);

        recordMedia(MAX_FILE_SIZE, mOutFile);
        // TODO: how can we trigger a recording error?
        assertFalse(mOnErrorCalled);
    }

    private void recordMedia(long maxFileSize, File outFile) throws Exception {
        mMediaRecorder.setMaxFileSize(maxFileSize);
        mMediaRecorder.prepare();
        mMediaRecorder.start();
        Thread.sleep(RECORD_TIME);
        mMediaRecorder.stop();
        assertTrue(outFile.exists());
        // The max file size is always guaranteed.
        // We just make sure that the margin is not too big
        assertTrue(outFile.length() < 1.1 * maxFileSize);
        assertTrue(outFile.length() > 0);
    }

    private boolean hasCamera() {
        return getActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
    }

    private boolean hasMicrophone() {
        return getActivity().getPackageManager().hasSystemFeature(
                PackageManager.FEATURE_MICROPHONE);
    }
}
