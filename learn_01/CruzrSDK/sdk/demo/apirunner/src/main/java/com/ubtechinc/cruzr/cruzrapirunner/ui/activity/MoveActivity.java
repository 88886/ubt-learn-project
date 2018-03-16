package com.ubtechinc.cruzr.cruzrapirunner.ui.activity;

import android.graphics.Color;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.TextUtils;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

import com.ubtechinc.cruzr.cruzrapirunner.R;
import com.ubtechinc.cruzr.cruzrapirunner.event.MoveEvent;
import com.ubtechinc.cruzr.cruzrapirunner.event.ResultEvent;
import com.ubtechinc.cruzr.cruzrapirunner.rxbus.RxBus;
import com.ubtechinc.cruzr.sdk.ros.RosRobotApi;
import com.ubtechinc.cruzr.serverlibutil.interfaces.RemoteCommonListener;

import java.util.concurrent.TimeUnit;

import rx.Observable;
import rx.android.schedulers.AndroidSchedulers;
import rx.functions.Action1;
import rx.subscriptions.CompositeSubscription;

/**
 * Created by Administrator on 2017/7/22.
 */

public class MoveActivity extends BaseActivity implements View.OnClickListener {
	private Button movetoward, moveback, moveleft, moveright, rotationleft, rotationright, moveexcute, rotateexcute, stop;
	private TextView console;
	private EditText inputx, inputy, inputrotationspeed;
	private ScrollView scrollViewconsole;
	private CompositeSubscription subscriptions;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.movelayout);

		findView();
		initObserver();
	}

	private void findView() {
		movetoward = (Button) this.findViewById(R.id.movetoward);
		movetoward.setOnClickListener(this);
		moveback = (Button) this.findViewById(R.id.moveback);
		moveback.setOnClickListener(this);
		moveleft = (Button) this.findViewById(R.id.moveleft);
		moveleft.setOnClickListener(this);
		moveright = (Button) this.findViewById(R.id.moveright);
		moveright.setOnClickListener(this);
		moveexcute = (Button) this.findViewById(R.id.excute_move);
		moveexcute.setOnClickListener(this);
		rotationleft = (Button) this.findViewById(R.id.rotationleft);
		rotationleft.setOnClickListener(this);
		rotationright = (Button) this.findViewById(R.id.rotationright);
		rotationright.setOnClickListener(this);
		rotateexcute = (Button) this.findViewById(R.id.excute_rotate);
		rotateexcute.setOnClickListener(this);
		stop = (Button) this.findViewById(R.id.stopmove);
		stop.setOnClickListener(this);


		console = (TextView) this.findViewById(R.id.console);

		inputx = (EditText) this.findViewById(R.id.inputx);
		inputy = (EditText) this.findViewById(R.id.inputy);
		inputrotationspeed = (EditText) this.findViewById(R.id.inputrotationspeed);
		scrollViewconsole = (ScrollView) this.findViewById(R.id.scrollViewconsole);
	}

	private void initObserver() {
		subscriptions = new CompositeSubscription();
		subscriptions.add(RxBus.getDefault().toObservable(MoveEvent.class)
				.observeOn(AndroidSchedulers.mainThread())
				.subscribe(new Action1<MoveEvent>() {
					@Override
					public void call(MoveEvent moveEvent) {
						handleEvent(moveEvent);
					}
				}));

	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
			case R.id.movetoward:
				moveToward();
				break;
			case R.id.moveback:
				moveBackward();
				break;
			case R.id.moveleft:
				leftMove();
				break;
			case R.id.moveright:
				rightMove();
				break;
			case R.id.stopmove:
				pauseMove();
				break;
			case R.id.excute_move:
				excuteMove();
				break;
			case R.id.excute_rotate:
				excuteRotate();
				break;
			case R.id.rotationleft:
				leftAround();
				break;
			case R.id.rotationright:
				rightAround();
				break;
			default:
				break;
		}

	}

	private void moveToward() {
		int code = RosRobotApi.get().moveToward(0.2f, 0, 0);
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "前进返回 session_id =";
		RxBus.getDefault().post(event);
	}

	private void moveBackward() {
		int code = RosRobotApi.get().moveToward(-0.2f, 0, 0);
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "前进返回 session_id =";
		RxBus.getDefault().post(event);
	}

	public void leftAround() {
		int code = RosRobotApi.get().moveToward(0, 0, 0.2f, new RemoteCommonListener() {

			@Override
			public void onResult(int sectionId, int status,String msg) {
				Log.i("paul", "左转状态:" + status);
				MoveEvent event = new MoveEvent();
				event.retcode = status;
				event.message = "左转状态=";
				RxBus.getDefault().post(event);
			}
		});
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "左转返回 session_id =";
		RxBus.getDefault().post(event);
	}

	public void rightAround() {
		int code = RosRobotApi.get().moveToward(0, 0, -0.2f, new RemoteCommonListener() {

			@Override
			public void onResult(int sectionId, int status,String msg) {
				Log.i("paul", "状态:" + status);
				MoveEvent event = new MoveEvent();
				event.retcode = status;
				event.message = "状态=";
				RxBus.getDefault().post(event);
			}
		});
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "右转返回 session_id =";
		RxBus.getDefault().post(event);
	}

	public void leftMove() {
		int code = RosRobotApi.get().moveToward(0, 0.2f, 0, new RemoteCommonListener() {

			@Override
			public void onResult(int sectionId, int status,String msg) {
				Log.i("paul", "状态:" + status);
				MoveEvent event = new MoveEvent();
				event.retcode = status;
				event.message = "状态=";
				RxBus.getDefault().post(event);
			}
		});
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "左移返回 session_id =";
		RxBus.getDefault().post(event);
	}

	public void rightMove() {
		int code = RosRobotApi.get().moveToward(0, -0.2f, 0, new RemoteCommonListener() {
			@Override
			public void onResult(int sectionId, int status,String msg) {
				Log.i("paul", "返回:" + status);
				MoveEvent event = new MoveEvent();
				event.retcode = status;
				event.message = "状态=";
				RxBus.getDefault().post(event);
			}
		});
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "右移返回 session_id =";
		RxBus.getDefault().post(event);
	}

	public void pauseMove() {
		int code = RosRobotApi.get().stopMove();
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "停止返回 session_id =";
		RxBus.getDefault().post(event);

	}

	public void excuteMove() {
		if (TextUtils.isEmpty(inputx.getText().toString()) || TextUtils.isEmpty(inputy.getText().toString())) {
			return;
		}
		float x = Float.parseFloat(inputx.getText().toString());
		float y = Float.parseFloat(inputy.getText().toString());
		int code = RosRobotApi.get().moveToward(x, y, 0.0f, new RemoteCommonListener() {
			@Override
			public void onResult(int sectionId, int status,String msg) {
				Log.i("paul", "移动状态:" + status);
				MoveEvent event = new MoveEvent();
				event.retcode = status;
				event.message = "状态=";
				RxBus.getDefault().post(event);
			}
		});
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "执行移动:参数:(" + x + "|" + y + ")返回 session_id =";
		RxBus.getDefault().post(event);
	}

	public void excuteRotate() {
		if (TextUtils.isEmpty(inputrotationspeed.getText())) {
			return;
		}
		float d = Float.parseFloat(inputrotationspeed.getText().toString());

		int code = RosRobotApi.get().moveToward(0, 0, d, new RemoteCommonListener() {
			@Override
			public void onResult(int sectionId, int status,String msg) {
				Log.i("paul", "状态:" + status);
				MoveEvent event = new MoveEvent();
				event.retcode = status;
				event.message = "状态=";
				RxBus.getDefault().post(event);
			}
		});
		MoveEvent event = new MoveEvent();
		event.retcode = code;
		event.message = "执行转身:参数:(" + d + ")返回 session_id =";
		RxBus.getDefault().post(event);

	}

	private void handleEvent(MoveEvent event) {
		String msg = event.message + event.retcode;
		addconsole(msg);
	}

	private void addconsole(String text) {
		Log.i("dan", "text=" + text);

		if (TextUtils.isEmpty(text)) {
			return;
		}
		console.append(text);
		console.append("\n");
		Observable.timer(200, TimeUnit.MILLISECONDS)
				.subscribe(new Action1<Long>() {
					@Override
					public void call(Long aLong) {
						Log.i("dan", "当前线程：" + Thread.currentThread().getName());
						scrollViewconsole.fullScroll(ScrollView.FOCUS_DOWN);
					}
				});
	}

	@Override
	protected void onPause() {
		super.onPause();
		Log.i("dan", "onPause");
	}

	@Override
	protected void onStop() {
		super.onStop();
		Log.i("dan", "onStop");
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		Log.i("dan", "onDestroy");
		if (subscriptions != null && !subscriptions.isUnsubscribed()) {
			subscriptions.unsubscribe();
		}
	}
}
