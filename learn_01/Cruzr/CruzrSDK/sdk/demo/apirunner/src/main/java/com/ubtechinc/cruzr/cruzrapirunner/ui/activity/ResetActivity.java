package com.ubtechinc.cruzr.cruzrapirunner.ui.activity;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.TextUtils;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;

import com.ubtechinc.cruzr.cruzrapirunner.R;
import com.ubtechinc.cruzr.cruzrapirunner.config.DataConfig;
import com.ubtechinc.cruzr.cruzrapirunner.event.ResetEvent;
import com.ubtechinc.cruzr.cruzrapirunner.event.ResultEvent;
import com.ubtechinc.cruzr.cruzrapirunner.rxbus.RxBus;
import com.ubtechinc.cruzr.sdk.ros.RosConstant;
import com.ubtechinc.cruzr.sdk.ros.RosRobotApi;
import com.ubtechinc.cruzr.serverlibutil.interfaces.RemoteCommonListener;
import com.ubtechinc.cruzr.serverlibutil.interfaces.RemoteDiagnosticDataListener;

import java.util.concurrent.TimeUnit;

import rx.Observable;
import rx.Subscription;
import rx.android.schedulers.AndroidSchedulers;
import rx.functions.Action1;
import rx.subscriptions.CompositeSubscription;

/**
 * Created by Administrator on 2017/8/3.
 */

public class ResetActivity extends BaseActivity implements View.OnClickListener {
	private Button leftfingerup, leftfingerdown, rightfingerup, rightfingerdown, zerosetting, releaseenerg;
	private ScrollView scrollViewconsole;
	private TextView console;
	private CompositeSubscription subscriptions = new CompositeSubscription();
	private boolean isrelease = false;

	@Override
	protected void onCreate(@Nullable Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.resetlayout);
		findView();
	}

	private void findView() {
		leftfingerup = (Button) this.findViewById(R.id.leftfingerup);
		leftfingerup.setOnClickListener(this);
		leftfingerdown = (Button) this.findViewById(R.id.leftfingerdown);
		leftfingerdown.setOnClickListener(this);
		rightfingerup = (Button) this.findViewById(R.id.rightfingerup);
		rightfingerup.setOnClickListener(this);
		rightfingerdown = (Button) this.findViewById(R.id.rightfingerdown);
		rightfingerdown.setOnClickListener(this);
		zerosetting = (Button) this.findViewById(R.id.zerosetting);
		zerosetting.setOnClickListener(this);
		releaseenerg = (Button) this.findViewById(R.id.release);
		releaseenerg.setOnClickListener(this);

		console = (TextView) this.findViewById(R.id.console);
		scrollViewconsole =(ScrollView) this.findViewById(R.id.scrollViewconsole);
	}

	private void initResultEventObserver() {
		subscriptions.add(RxBus.getDefault().toObservable(ResetEvent.class)
				//	.sample(100, TimeUnit.MILLISECONDS)
				.onBackpressureBuffer()
				.observeOn(AndroidSchedulers.mainThread())
				.subscribe(new Action1<ResetEvent>() {
					@Override
					public void call(ResetEvent resetEvent) {
						Log.i("dan", Thread.currentThread().getName() + "" + resetEvent.retcode);
						handleEvent(resetEvent);
					}
				}));
	}

	private void handleEvent(ResetEvent event) {
		Object message = event.message;
		int retcode = event.retcode;
		String msg = (String) message;
		if (retcode != ResultEvent.DEFAULT_RET_CODE) {
			msg += retcode;
			addconsole(msg);
		}
	}

	private void addconsole(String text) {
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
	protected void onResume() {
		super.onResume();
		registfeedback();
		initResultEventObserver();
	}

	RemoteCommonListener commonListener = new RemoteCommonListener() {
		@Override
		public void onResult(int sessionid, int status,String msg) {
			Log.i("dan", "sessionid=" + sessionid + "status=" + status);
			handleFeedBack(sessionid, status);
		}
	};

	private void handleFeedBack(int sessionid, int status) {
		ResetEvent event = new ResetEvent();
		event.message = "底层返回状态码:";
		event.retcode = status;
		RxBus.getDefault().post(event);
	}

	private void registfeedback() {
		subscriptions.add(Observable.timer(2000, TimeUnit.MILLISECONDS).subscribe(new Action1<Long>() {
			@Override
			public void call(Long aLong) {
				RosRobotApi.get().registerCommonCallback(commonListener);
			}
		}));
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
			case R.id.leftfingerup:
				if (!isrelease) {
					return;
				}
				RosRobotApi.get().setFingerAdjust(RosConstant.FingerAdjust.LEFTINCREASE);
				break;
			case R.id.leftfingerdown:
				if (!isrelease) {
					return;
				}
				RosRobotApi.get().setFingerAdjust(RosConstant.FingerAdjust.LEFTDECREASE);
				break;
			case R.id.rightfingerdown:
				if (!isrelease) {
					return;
				}
				RosRobotApi.get().setFingerAdjust(RosConstant.FingerAdjust.RIGHTDECREASE);
				break;
			case R.id.rightfingerup:
				if (!isrelease) {
					return;
				}
				RosRobotApi.get().setFingerAdjust(RosConstant.FingerAdjust.RIGHTINCREASE);
				break;
			case R.id.zerosetting:
				if (!isrelease) {
					return;
				}
				RosRobotApi.get().zeroSetting();
				break;
			case R.id.release:
				RosRobotApi.get().servoEnergyRelease();
				isrelease = true;
				break;
		}
	}

	@Override
	protected void onPause() {
		super.onPause();
		Log.i("dan", "反注册");
		if (subscriptions != null && !subscriptions.isUnsubscribed()) {
			subscriptions.unsubscribe();
		}
	}
}
