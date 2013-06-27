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
package com.android.cts.verifier.camera.analyzer;

import com.android.cts.verifier.R;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.IOException;

public class CameraAnalyzerActivity extends Activity {

    Bitmap mInputImage;
    TextView mResultText;
    SurfaceView mCameraView;
    ImageView mResultView;
    Camera mCamera;
    boolean mProcessingPicture = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ca_main);

        findViewById(R.id.runbutton).setOnClickListener(mRunListener);

        mCameraView = (SurfaceView)findViewById(R.id.cameraview);
        mResultView = (ImageView)findViewById(R.id.resultview);
        mResultText = (TextView)findViewById(R.id.resulttext);
        mCameraView.getHolder().addCallback(mSurfaceChangeListener);
    }

    @Override
    public void onResume() {
        super.onResume();
        mCamera = Camera.open(0);
        Camera.Parameters params = mCamera.getParameters();
        params.setPictureFormat(ImageFormat.JPEG);
        mCamera.setParameters(params);
    }

    @Override
    public void onPause() {
        super.onPause();
        mCamera.release();
    }

    private View.OnClickListener mRunListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (!mProcessingPicture) {
                mCamera.takePicture(null, null, null, mJpegListener);
                mProcessingPicture = true;
            }
        }
    };

    private Camera.PictureCallback mJpegListener = new Camera.PictureCallback() {
        public void onPictureTaken(byte[] data, Camera mCamera) {
            mCamera.startPreview();
            mInputImage = BitmapFactory.decodeByteArray(data, 0, data.length);
            ColorChecker checker = new ColorChecker(mInputImage);
            mResultView.setImageBitmap(checker.getDebugOutput());
            if (checker.isValid()) {
                String patchValueTxt = new String();
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 6; x++) {
                        patchValueTxt +=
                                String.format("[ %.3f, %.3f, %.3f] ",
                                              checker.getPatchValue(x,y,0),
                                              checker.getPatchValue(x,y,1),
                                              checker.getPatchValue(x,y,2));
                    }
                    patchValueTxt += "\n";
                }
                mResultText.setText(patchValueTxt);
            } else {
                mResultText.setText("Can't find color checker!");
            }
            mProcessingPicture = false;
        }
    };

    private SurfaceHolder.Callback mSurfaceChangeListener =
            new SurfaceHolder.Callback() {
        @Override
        public void surfaceChanged(SurfaceHolder holder,
                                   int format,
                                   int width,
                                   int height) {

        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            try {
                mCamera.setPreviewDisplay(mCameraView.getHolder());
            } catch (IOException e) {
                throw new RuntimeException("Unable to connect camera to display: " + e);
            }
            mCamera.startPreview();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {

        }

    };
}
