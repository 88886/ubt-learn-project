package com.ubtechinc.cruzr.cruzrapirunner.ui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import rx.Observable;
import rx.functions.Action1;

/**
 * Created by Administrator on 2017/5/18.
 */

public class CameraView extends SurfaceView implements SurfaceHolder.Callback {

	SurfaceHolder holder;
	Camera myCamera;
	Camera.PreviewCallback previewCallback;
	IniListener listener;

	public CameraView(Context context, IniListener listener) {
		super(context);
		this.listener = listener;
		init();
	}

	public CameraView(Context context) {
		super(context);
		init();
	}

	public CameraView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public CameraView(Context context, AttributeSet attrs, int defStyleAttr) {
		super(context, attrs, defStyleAttr);
		init();
	}

	private void init() {
		holder = getHolder();//获得surfaceHolder引用
		holder.addCallback(this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);//设置类型
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		attchCamera();
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
							   int height) {
		if (myCamera != null) {
			Camera.Parameters params = myCamera.getParameters();
			params.setPictureFormat(PixelFormat.JPEG);
			params.setPreviewSize(640, 480);
			myCamera.setParameters(params);
			myCamera.startPreview();
		}
		if (listener != null) {
			listener.onSuccess(myCamera);
		}
	}

	private void attchCamera() {
		if (myCamera == null) {
			//	myCamera = Camera.open();//开启相机,不能放在构造函数中，不然不会显示画面.
			getFront();
			if (previewCallback != null) {
				myCamera.setPreviewCallback(previewCallback);
				myCamera.getParameters().getPictureSize();
			}
			try {
				myCamera.setPreviewDisplay(holder);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		}
	}

	public void openCamera() {
		if (myCamera == null) {
			attchCamera();
			Observable.timer(5, TimeUnit.SECONDS).subscribe(new Action1<Long>() {
				@Override
				public void call(Long aLong) {
					if (myCamera != null) {
						Camera.Parameters params = myCamera.getParameters();
						params.setPictureFormat(PixelFormat.JPEG);
						params.setPreviewSize(640, 480);
						myCamera.setParameters(params);
						myCamera.startPreview();
						Log.i("dan", "stratpreview");
					}
					if (listener != null) {
						listener.onSuccess(myCamera);
					}
				}
			});
		}
	}

	public void closeCamera() {
		releaseAll();
		ClearDraw();
	}

	private void ClearDraw() {
		Canvas canvas = holder.lockCanvas();
		if (canvas != null) {
			Paint p = new Paint();
			p.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
			canvas.drawPaint(p);
		}
		holder.unlockCanvasAndPost(canvas);
	}

	private void getFront() {
		int cameraCount = 0;
		Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
		cameraCount = Camera.getNumberOfCameras();//得到摄像头的个数

		for (int i = 0; i < cameraCount; i++) {
			Camera.getCameraInfo(i, cameraInfo);//得到每一个摄像头的信息
			//现在是后置，变更为前置
			if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {//代表摄像头的方位，CAMERA_FACING_FRONT前置      CAMERA_FACING_BACK后置
				myCamera = Camera.open(i);//打开当前选中的摄像头
				//	myCamera.setDisplayOrientation(90);
				break;
			}

		}
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		releaseAll();
	}

	private void releaseAll() {
		myCamera.stopPreview();//停止预览
		myCamera.release();//释放相机资源
		myCamera = null;
	}

	@Override
	public void onDraw(Canvas canvas) {
		super.onDraw(canvas);

	}

	public void tackPicture(Camera.PictureCallback cb) {
		myCamera.takePicture(null, null, cb);
	}

	public void setPreviewCallBack(Camera.PreviewCallback previewCallBack) {
		if (myCamera != null) {
			myCamera.setPreviewCallback(previewCallBack);
		} else {
			this.previewCallback = previewCallBack;
		}
	}

	public Camera.Size getPreviewSize() {
		return myCamera.getParameters().getPictureSize();
	}
}