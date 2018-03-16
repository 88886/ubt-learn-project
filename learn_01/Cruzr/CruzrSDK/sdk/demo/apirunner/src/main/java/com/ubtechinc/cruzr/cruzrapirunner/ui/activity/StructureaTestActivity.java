package com.ubtechinc.cruzr.cruzrapirunner.ui.activity;

import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.rk.RockVideoPlayer.Service.IRockVideoPlayer;
import android.rk.RockVideoPlayer.Service.ServiceCmd;
import android.support.annotation.Nullable;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import com.google.gson.Gson;
import com.ubtechinc.cruzr.cruzrapirunner.R;
import com.ubtechinc.cruzr.cruzrapirunner.bean.CmdBean;
import com.ubtechinc.cruzr.cruzrapirunner.config.SctructData_Config;
import com.ubtechinc.cruzr.cruzrapirunner.event.TestEvent;
import com.ubtechinc.cruzr.cruzrapirunner.rxbus.RxBus;
import com.ubtechinc.cruzr.cruzrapirunner.ui.ButtonAdapter;
import com.ubtechinc.cruzr.cruzrapirunner.ui.CameraView;
import com.ubtechinc.cruzr.cruzrapirunner.ui.OnCmdLongPressListener;
import com.ubtechinc.cruzr.cruzrapirunner.utils.ConfigReader;
import com.ubtechinc.cruzr.cruzrapirunner.utils.SpeechUtils;
import com.ubtechinc.cruzr.sdk.ros.RosRobotApi;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.TimeUnit;

import rx.Observable;
import rx.Subscription;
import rx.android.schedulers.AndroidSchedulers;
import rx.functions.Action0;
import rx.functions.Action1;
import rx.functions.Func1;
import rx.subscriptions.CompositeSubscription;

/**
 * Created by Administrator on 2017/5/26.
 */

public class StructureaTestActivity extends BaseActivity implements View.OnClickListener, OnCmdLongPressListener {
	private Subscription pressure1, pressure2;
	private Subscription cameravideo;
	private Subscription dances;
	private CmdBean bean;
	private RecyclerView recyclerView;
	CameraView cameraView;
	private Button clear;
	private Button stop;
	private LinearLayout linearlayout;
	private LinearLayout container;
	private TextView console;
	private ScrollView scrollViewconsole;
	private CheckBox fringer,yao,tou,shouwan,zhou1,zhou2,jian1,jian2;
	public static final float PI_ = 3.141592653589793f;
	//舞蹈动作集
	private String[] dance1 = {"doitbetter", "doitbetter01", "danceend"};
	private String[] dance2 = {"yankee1", "yankee1", "danceend"};
	private String[] dance3 = {"arabia1", "arabia2", "arabia1", "danceend"};
	private String[] dance4 = {"spanish1", "spanish2", "danceend"};
	private String[] dance5 = {"poppin1", "poppin2", "poppin3"};
	private CompositeSubscription subscriptions = new CompositeSubscription();
	ArrayList<String[]> dancelist = new ArrayList<>();
	//安卓自带视频播放器
	public final static String video_pgk = "android.rk.RockVideoPlayer";
	IRockVideoPlayer mRockVideoPlayer;
	//当前是否在执行测试
	private boolean isRunningTest = false;
	private final int fringer_time = 20;
	private final int yao_time = 20;
	private final int tou_time = 20;
	private final int shouwan_time = 20;
	private final int zhou1_time = 20;
	private final int zhou2_time = 20;
	private final int jian1_time = 20;
	private final int jian2_time = 20;
	private int total_time = 0;

	@Override
	protected void onCreate(@Nullable Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.structurea_test_activity_layout);
		findviews();
		initBean();
		initView();
		initTestEventObserver();
		bindservice();

	}

	private void initBean() {
		Gson gson = new Gson();
		String data = ConfigReader.read(this, "structurea_config.json");
		bean = gson.fromJson(data, CmdBean.class);
	}

	private void findviews() {
		recyclerView = (RecyclerView) findViewById(R.id.struct_recycler);
		console = (TextView) findViewById(R.id.struct_console);
		scrollViewconsole = (ScrollView) findViewById(R.id.struct_consolescroll);
		clear = (Button) findViewById(R.id.struct_clear);
		clear.setOnClickListener(this);
		stop = (Button) findViewById(R.id.stop_all);
		stop.setOnClickListener(this);
		linearlayout = (LinearLayout) findViewById(R.id.linearlayout);
		container = (LinearLayout) findViewById(R.id.container);

		fringer = (CheckBox) findViewById(R.id.fringer);
		yao = (CheckBox) findViewById(R.id.yao);
		tou = (CheckBox) findViewById(R.id.tou);
		shouwan = (CheckBox) findViewById(R.id.shouwan);
		zhou1 = (CheckBox) findViewById(R.id.zhou1);
		zhou2 = (CheckBox) findViewById(R.id.zhou2);
		jian1 = (CheckBox) findViewById(R.id.jian1);
		jian2 = (CheckBox) findViewById(R.id.jian2);

		//	cameraView = (CameraView) findViewById(R.id.cameraview);
	}

	private void initView() {
		List<CmdBean.Cmds> list = bean.getCmds();
		ButtonAdapter adapter = new ButtonAdapter(this, list, this, this);
		RecyclerView.LayoutManager manager = new GridLayoutManager(this, 5, LinearLayoutManager.VERTICAL, false);
		recyclerView.setLayoutManager(manager);
		recyclerView.setAdapter(adapter);
		dancelist.add(dance1);
		dancelist.add(dance2);
		dancelist.add(dance3);
		dancelist.add(dance4);
		dancelist.add(dance5);
	}

	private void initTestEventObserver() {
		subscriptions.add(RxBus.getDefault().toObservable(TestEvent.class)
				.onBackpressureBuffer()
				.observeOn(AndroidSchedulers.mainThread())
				.subscribe(new Action1<TestEvent>() {
					@Override
					public void call(TestEvent event) {
						addconsole(event.msg);
					}
				}));
	}

	private void bindservice() {
		Intent intent = new Intent("android.rk.RockVideoPlayer.Service.PlayService");
		intent.setPackage("android.rk.RockVideoPlayer");
		bindService(intent, conn, BIND_AUTO_CREATE);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		stopAndReleaseAll();
		RosRobotApi.get().registerDiagnosticDataCallback(null);
	/*	RefWatcher refWatcher = App.getRefWatcher(this);
		refWatcher.watch(this);*/
	}

	private void stopAndReleaseAll() {
		if (cameravideo != null && !cameravideo.isUnsubscribed()) {
			cameravideo.unsubscribe();
		}
		if (dances != null && !dances.isUnsubscribed()) {
			dances.unsubscribe();
		}
		if (pressure1 != null && !pressure1.isUnsubscribed()) {
			pressure1.unsubscribe();
		}
		if (pressure2 != null && !pressure2.isUnsubscribed()) {
			pressure2.unsubscribe();
		}
		if (subscriptions != null && !subscriptions.isUnsubscribed()) {
			subscriptions.unsubscribe();
		}
		isRunningTest = false;
		RosRobotApi.get().stopMove();
		Observable.timer(0, TimeUnit.SECONDS)
				.map(new Func1<Long, Object>() {
					@Override
					public Object call(Long aLong) {
						if (!isRunningTest) {
							RosRobotApi.get().stopRun();
						}
						return null;
					}
				})
				.delay(2, TimeUnit.SECONDS)
				.map(new Func1<Object, Object>() {
					@Override
					public Object call(Object o) {
						if (!isRunningTest) {
							RosRobotApi.get().run("rest");
						}
						return null;
					}
				}).subscribe(new Action1<Object>() {
			@Override
			public void call(Object queue) {

			}
		});

		addconsole("停止测试成功！！！");
	}

	@Override
	protected void onStart() {
		super.onStart();
	}

	@Override
	protected void onStop() {
		super.onStop();
	}

	@Override
	protected void onPause() {
		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
	}

	private void addconsole(String text) {
		console.append(text);
		console.append("\n");
		scrollViewconsole.fullScroll(ScrollView.FOCUS_DOWN);
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
			case SctructData_Config.CMD_ID_0:
				//压力测试1
				if (!checkState()) {
					isRunningTest = true;
					pressureTest1();
				}
				break;
			case SctructData_Config.CMD_ID_1:
				//老化测试
				if (!checkState()) {
					isRunningTest = true;
					ageingTest();
				}
				break;
			case SctructData_Config.CMD_ID_2:
				//压力测试2
				if (!checkState()) {
					isRunningTest = true;
					pressureTest2();
				}
				break;
			case R.id.stop_all:
				stopAndReleaseAll();
				break;
			case R.id.struct_clear:
				clearConsole();
				break;
			default:
				break;
		}

	}

	/**
	 * 返回当前是否正在执行测试
	 *
	 * @return
	 */
	private boolean checkState() {
		if (isRunningTest) {
			addconsole("如要执行新的测试，请先停止当前测试！！！");
		}
		return isRunningTest;
	}

	@Override
	public boolean onLongPressCmd(CmdBean.Cmds cmds) {
		alertDialog.setMessage(cmds.getTip());
		alertDialog.show();
		alertDialog.getButton(AlertDialog.BUTTON_NEGATIVE).setVisibility(View.GONE);
		return true;
	}

	private void pressureTest1() {
		addconsole("正在执行压力测试！！！");
		pressuremMove1();
		cameraAndVideo();
	}

	private void pressureTest2() {

		ArrayList<String> actions = new ArrayList<>();
		int time = 0;

		if (fringer.isChecked()) {
			actions.add("fringer");
			time += fringer_time;
		}
		if (yao.isChecked()) {
			actions.add("yao");
			time += yao_time;
		}
		if (tou.isChecked()) {
			actions.add("tou");
			time += tou_time;
		}
		if (shouwan.isChecked()) {
			actions.add("shouwan");
			time += shouwan_time;
		}
		if (zhou1.isChecked()) {
			actions.add("zhou1");
			time += zhou1_time;
		}
		if (zhou2.isChecked()) {
			actions.add("zhou2");
			time += zhou2_time;
		}
		if (jian1.isChecked()) {
			actions.add("jian1");
			time += jian1_time;
		}
		if (jian2.isChecked()) {
			actions.add("jian2");
			time += jian2_time;
		}
		if (actions.isEmpty()) {
			addconsole("请至少请选择一个动作！");
			return;
		}
		total_time = time;
		Log.i("dan", "box ="  + fringer.isChecked()+ "|" + yao.isChecked()+ "|" + tou.isChecked()+ "|" + shouwan.isChecked()+ "|" + zhou1.isChecked()+ "|" + zhou2.isChecked()+ "|" + jian1.isChecked()+ "|" + jian2.isChecked());
		String[] action = new String[]{};
		action =actions.toArray(action);
		Log.i("dan", "action" + Arrays.toString(action));
		Log.i("dan", "time = " + total_time);
		addconsole("正在执行压力测试！！！");
		pressuremMove2(action);
		cameraAndVideo();
	}

	private void pressuremMove1() {
		if (pressure1 != null && !pressure1.isUnsubscribed()) {
			pressure1.unsubscribe();
		}
		pressure1 = Observable.timer(5, TimeUnit.SECONDS)
				.map(new Func1<Long, Long>() {
					@Override
					public Long call(Long aLong) {
						Log.i("dan", "移动三米");
						RosRobotApi.get().moveTo(3.0f, 0.0f, 0.0f, 2.0f);
						TestEvent event = new TestEvent("前进3米");
						RxBus.getDefault().post(event);
						return null;
					}
				})
				.delay(5, TimeUnit.SECONDS)
				.map(new Func1<Long, Long>() {
					@Override
					public Long call(Long aLong) {
						Log.i("dan", "左转90度");
						RosRobotApi.get().moveTo(0, 0, SpeechUtils.getRadian(90), PI_ / 3);
						TestEvent event = new TestEvent("执行左转90度");
						RxBus.getDefault().post(event);
						return null;
					}
				})
				.repeat(Integer.MAX_VALUE)
				.doOnSubscribe(new Action0() {
					@Override
					public void call() {
						Log.i("dan", "开始执行");
						TestEvent event = new TestEvent("开始执行");
						RxBus.getDefault().post(event);
					}
				})
				.subscribe(new Action1<Long>() {
					@Override
					public void call(Long aLong) {
					}
				});
	}

	private void pressuremMove2(final String[] action) {
		if (pressure2 != null && !pressure2.isUnsubscribed()) {
			pressure2.unsubscribe();
		}
		pressure2 = Observable.timer(2, TimeUnit.SECONDS)
				.map(new Func1<Long, Long>() {
					@Override
					public Long call(Long aLong) {
						Log.i("dan", "极限动作"+Arrays.toString(action));
						RosRobotApi.get().run(action);
						TestEvent event = new TestEvent("执行极限动作1");
						RxBus.getDefault().post(event);
						return null;
					}
				})
				.delay(total_time, TimeUnit.SECONDS)
				.repeat(Integer.MAX_VALUE)
				.doOnSubscribe(new Action0() {
					@Override
					public void call() {
						Log.i("dan", "开始执行");
						TestEvent event = new TestEvent("开始执行");
						RxBus.getDefault().post(event);
					}
				})
				.subscribe(new Action1<Long>() {
					@Override
					public void call(Long aLong) {
					}
				});
	}

	private void ageingTest() {
		addconsole("正在执行老化测试！！！");
		/*无限执行动作*/
		cameraAndVideo();
		dances();
	}

	private void cameraAndVideo() {
		if (cameravideo != null && !cameravideo.isUnsubscribed()) {
			cameravideo.unsubscribe();
		}
		cameravideo = Observable.interval(0, 30
				, TimeUnit.MINUTES)
				.observeOn(AndroidSchedulers.mainThread())
				.map(new Func1<Long, Long>() {
					@Override
					public Long call(Long aLong) {
						if (aLong % 2 == 0) {
							//打开摄像头
							addCameraView();
							stopVideo();
							Log.i("dan", "关闭视频");
							TestEvent event = new TestEvent("打开摄像头");
							RxBus.getDefault().post(event);
						} else {
							//打开视频
							removeCameraView();
							startVideo();
							TestEvent event = new TestEvent("关闭摄像头");
							RxBus.getDefault().post(event);
						}
						return null;
					}
				})
				.doOnUnsubscribe(new Action0() {
					@Override
					public void call() {
						stopVideo();
						removeCameraView();
					}
				})
				.subscribe(new Action1<Long>() {
					@Override
					public void call(Long aLong) {
						Log.i("dan", "执行完毕");
					}
				});
	}

	private void dances() {
		if (dances != null && !dances.isUnsubscribed()) {
			dances.unsubscribe();
		}
		dances = Observable.interval(2, 30, TimeUnit.SECONDS)
				.map(new Func1<Long, Long>() {
					@Override
					public Long call(Long aLong) {
						String[] action = dancelist.get((int) (aLong % dancelist.size()));
						Log.i("dan", "执行" + Arrays.toString(action));
						RosRobotApi.get().run(action);
						if (aLong % 2 == 0) {
							RosRobotApi.get().moveTo(0, 0, SpeechUtils.getRadian(360), PI_ / 3);
						}
						return aLong;
					}
				})
				.subscribe(new Action1<Long>() {
					@Override
					public void call(Long aLong) {
						//		Log.i("dan", "执行完毕" + aLong);
					}
				});
	}

	private void addCameraView() {
		if (cameraView == null) {
			cameraView = new CameraView(this);
			LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(1200, ViewGroup.LayoutParams.MATCH_PARENT);
			container.addView(cameraView, params);
		}
	}

	private void removeCameraView() {
		container.removeView(cameraView);
		cameraView = null;
	}

	private void startVideo() {
		String videoPath = "/sdcard/Movies/CruzrIntroduce.mp4";
		Intent it = new Intent(Intent.ACTION_VIEW);
		it.setPackage(video_pgk);
		it.setDataAndType(Uri.parse("file://" + videoPath), "video/MP4");
		it.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(it);
	}

	private void stopVideo() {
		Log.i("dan", "ret=" + mRockVideoPlayer);
		if (mRockVideoPlayer != null) {

			try {
				int currentState = mRockVideoPlayer.control(ServiceCmd.CMD_STATE, "");
				Log.e("dan", "currentState==" + currentState);
				if (currentState == 0 || currentState == 1) {//当前处于暂停/播放状态
					int res = mRockVideoPlayer.control(ServiceCmd.CMD_EXIT, "");
				} else {
					//当前播放器没有在前台，则不执行暂停、继续播放、停止的指令
				}
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
	}

	private void clearConsole() {
		console.setText("");
	}

	private ServiceConnection conn = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			mRockVideoPlayer = IRockVideoPlayer.Stub.asInterface(service);
		}

		@Override
		public void onServiceDisconnected(ComponentName name) {
			mRockVideoPlayer = null;
		}
	};
}
