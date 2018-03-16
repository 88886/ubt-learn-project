package com.ubtechinc.cruzr.cruzrapirunner.ui;

import android.app.Service;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.os.IBinder;
import android.util.Log;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.TextView;


import com.ubtechinc.cruzr.sdk.recognition.RawImage;
import com.ubtechinc.cruzr.sdk.recognition.arcsoft.ArcConstant;
import com.ubtechinc.cruzr.sdk.recognition.arcsoft.ArcDetectionResult;
import com.ubtechinc.cruzr.sdk.recognition.arcsoft.ArcSoftEngine;


import java.nio.ByteBuffer;


/**
 * Created by Administrator on 2017/5/18.
 */

public class CameraService extends Service implements Camera.PictureCallback, Camera.PreviewCallback, IniListener {
	private static final String TAG = CameraService.class.getSimpleName();

	private Camera mCamera;

	private boolean isRunning; // 是否已在监控拍照

	private String commandId; // 指令ID
	WindowManager wManager;
	WindowManager.LayoutParams mParams;
	CameraView cameraView;
	boolean flag = true;
	RawImage mHandleImage;
	private int mCameraWidth = 400;
	private int mCameraHeight = 400;
	private ByteBuffer mBufferForGLRenderer;
	private int capacity = 0;
	private boolean detectedable = false;
	private ArcSoftEngine mRecognitionEngine = ArcSoftEngine.getInstance();
	private boolean findpepole = false;
	private TextView textView;

	@Override
	public void onPictureTaken(byte[] data, Camera camera) {
		Log.d(TAG, "onPictureTaken...");

	}

	@Override
	public void onCreate() {
		mRecognitionEngine.init();
		wManager = (WindowManager) getApplicationContext().getSystemService(
				this.WINDOW_SERVICE);
		mParams = new WindowManager.LayoutParams();
		mParams.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;// 系统提示window
		mParams.format = PixelFormat.TRANSLUCENT;// 支持透明
		//mParams.format = PixelFormat.RGBA_8888;
		mParams.flags |= WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;// 焦点
		mParams.gravity = Gravity.TOP|Gravity.RIGHT;
		mParams.width = 80;//窗口的宽和高
		mParams.height = 80;
		mParams.x = 0;//窗口位置的偏移量
		mParams.y = 0;
		cameraView = new CameraView(this, this);
		cameraView.setPreviewCallBack(this);

		textView = new TextView(this);
		textView.setTextColor(Color.YELLOW);
		mHandleImage = new RawImage();

		super.onCreate();
	}

	@Override
	public int onStartCommand(final Intent intent, int flags, int startId) {
		if (flag) {
			flag = false;
			wManager.addView(cameraView, mParams);//添加窗口
			wManager.addView(textView, mParams);
		}
		return START_NOT_STICKY;
	}


	@Override
	public void onDestroy() {
		super.onDestroy();
		Log.d(TAG, "onDestroy...");
		isRunning = false;
		releaseCamera();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}


	private void releaseCamera() {
		if (mCamera != null) {
			Log.d(TAG, "releaseCamera...");
			mCamera.stopPreview();
			mCamera.release();
			mCamera = null;
		}
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		/*if (detectedable) {
			mBufferForGLRenderer.put(data);
			mBufferForGLRenderer.position(0);
			mHandleImage.setData(mBufferForGLRenderer);
			mHandleImage.setFormat(ArcConstant.format.ASVL_PAF_NV21);
			mHandleImage.setWidth(mCameraWidth);
			mHandleImage.setHeight(mCameraHeight);
			ArcDetectionResult result = (ArcDetectionResult) mRecognitionEngine.detect(mHandleImage);

			int count = result.getCount();
			if (count>0){
				if (!findpepole){
					Log.i("dan","有人");
					notifyEvent(WakeUpEvent.WAKE_UP_TYPE_FACE_IN);
					findpepole = true;
				}
			}else {
				if (findpepole){
					//send message
					Log.i("dan","没人");
					notifyEvent(WakeUpEvent.WAKE_UP_TYPE_FACE_OUT);
					findpepole = false;
				}
			}


			//Log.i("dan", "count =" + count);
			//	RxBus.getDefault().post(count);
		}*/
	}
/*	public void notifyEvent(int type) {
		WakeUpEvent event = new WakeUpEvent();
		event.type = type;
		NotificationCenter.defaultCenter().publish(event);
	}*/
	private void allocateByteBuffer(int capacity) {

		if (mBufferForGLRenderer != null) {
			return;
		}
		mBufferForGLRenderer = ByteBuffer.allocateDirect(capacity);
		mBufferForGLRenderer.order(java.nio.ByteOrder.LITTLE_ENDIAN);
		mBufferForGLRenderer.position(0);
	}

	@Override
	public void onSuccess(Camera camera) {
		Camera.Size size = camera.getParameters().getPreviewSize();
		mCameraWidth = size.width;
		mCameraHeight = size.height;
		capacity = mCameraWidth * mCameraHeight * 3 / 2;
		allocateByteBuffer(capacity);
		detectedable = true;
	}
}

interface IniListener {
	public void onSuccess(Camera camera);
}