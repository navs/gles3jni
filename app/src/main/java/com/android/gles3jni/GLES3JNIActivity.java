/*
 * Copyright 2013 The Android Open Source Project
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

package com.android.gles3jni;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;

import java.io.File;

public class GLES3JNIActivity extends Activity implements ActivityCompat.OnRequestPermissionsResultCallback {

    GLES3JNIView mView;
    View mMainView;

    static final int READ_STORAGE_PERMISSION_REQUEST_CODE = 10001;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

//        mView = new GLES3JNIView(getApplication());
//        setContentView(mView);

        LayoutInflater vi = (LayoutInflater)getApplicationContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mMainView = vi.inflate(R.layout.main, null);
        setContentView(mMainView);

        if (checkPErmissionForReadExternalStorage(this)) {
            mView = new GLES3JNIView(getApplication());
            setContentView(mView);

        } else {
            try {
                requestPermissionForReadExternalStorage();
            } catch (Exception e) {
                // @TODO Failed ...
            }
        }
    }

    @Override protected void onPause() {
        super.onPause();
        if (mView != null) {
            mView.onPause();
        }
    }

    @Override protected void onResume() {
        super.onResume();
        if (mView != null) {
            mView.onResume();
        }
    }


    public boolean checkPErmissionForReadExternalStorage(Activity context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            int result = context.checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE);
            Log.v("GLES3JNI", "permissionCheck=" + result + " == ? " + context.getPackageManager().PERMISSION_GRANTED);
            return result == context.getPackageManager().PERMISSION_GRANTED;
        }
        return false;
    }

    public void requestPermissionForReadExternalStorage() throws Exception {
        try {
            Log.v("GLES3JNI", "Request....");
            ActivityCompat.requestPermissions(this, new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE
            }, READ_STORAGE_PERMISSION_REQUEST_CODE);
        } catch (Exception e) {
            e.printStackTrace();
            throw e;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case READ_STORAGE_PERMISSION_REQUEST_CODE:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Log.v("GLES3JNI", "GRANTED: READ_EXTERNAL_STORAGE permission.");

                    mView = new GLES3JNIView(getApplication());
                    setContentView(mView);

                } else {
                    Log.e("GLES3JNI", "DENIED: READ_EXTERNAL_STORAGE permission.");
                }
                return;
        }

    }

//    @Override
//    public void onRequestPermissionResult(int requestCode, String[] permissions, int[] grantResults) {
//        switch (requestCode) {
//            case READ_STORAGE_PERMISSION_REQUEST_CODE:
//                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
//                    Log.v("GLES3JNI", "GRANTED: READ_EXTERNAL_STORAGE permission.");
//                } else {
//                    Log.e("GLES3JNI", "DENIED: READ_EXTERNAL_STORAGE permission.");
//                }
//                return;
//        }
//    }
}
