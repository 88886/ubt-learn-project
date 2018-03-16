package com.ubtechinc.cruzr.cruzrapirunner;

import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.Handler;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.TextUtils;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.graphics.drawable.Drawable;

import com.google.gson.Gson;

import com.ubtechinc.cruzr.cruzrapirunner.Resolver.ContentResolverControl;
import com.ubtechinc.cruzr.cruzrapirunner.Resolver.Location;
import com.ubtechinc.cruzr.cruzrapirunner.Service.SocketCMDService;
import com.ubtechinc.cruzr.cruzrapirunner.bean.CmdBean;
import com.ubtechinc.cruzr.cruzrapirunner.config.DataConfig;
import com.ubtechinc.cruzr.cruzrapirunner.event.Event;
import com.ubtechinc.cruzr.cruzrapirunner.event.ResultEvent;
import com.ubtechinc.cruzr.cruzrapirunner.event.StatusEvent;
import com.ubtechinc.cruzr.cruzrapirunner.rxbus.RxBus;
import com.ubtechinc.cruzr.cruzrapirunner.ui.ButtonAdapter;
import com.ubtechinc.cruzr.cruzrapirunner.ui.OnCmdLongPressListener;
import com.ubtechinc.cruzr.cruzrapirunner.ui.activity.BaseActivity;
import com.ubtechinc.cruzr.cruzrapirunner.utils.AppConstants;
import com.ubtechinc.cruzr.cruzrapirunner.utils.ConfigReader;
import com.ubtechinc.cruzr.cruzrapirunner.utils.PackageUtils;
import com.ubtechinc.cruzr.sdk.ros.RosConstant;
import com.ubtechinc.cruzr.sdk.ros.RosRobotApi;
import com.ubtechinc.cruzr.sdk.speech.SpeechRobotApi;
import com.ubtechinc.cruzr.serverlibutil.interfaces.RemoteCommonListener;
import com.ubtechinc.cruzr.serverlibutil.interfaces.RemoteDiagnosticDataListener;
import com.ubtechinc.cruzr.serverlibutil.interfaces.RemoteNavigationListener;
import com.ubtechinc.cruzr.serverlibutil.interfaces.SpeechTtsListener;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.TimeUnit;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


import rx.Observable;
import rx.Subscription;
import rx.android.schedulers.AndroidSchedulers;
import rx.functions.Action1;
import rx.functions.Func1;
import rx.subscriptions.CompositeSubscription;


public class MainActivity extends BaseActivity implements View.OnClickListener, OnCmdLongPressListener {
	private RecyclerView recyclerView;

	private TextView rosip;
	private TextView wifiip;
	private TextView console;
	private EditText wifissid;
	private EditText wifipassword;
	private ScrollView scrollViewconsole;
	private FrameLayout settingcontainer;
	private Button clear;
	private Button btnEnableNavigation;
    private Button btnEnableTransaction;
	private Button btnRelocation;
	private ApiControl control;
	private CompositeSubscription subscriptions = new CompositeSubscription();
	private CmdBean bean;
	private Map<Integer, Event> params = new HashMap<>();

	private ContentResolverControl contentResolverControl;
	private HashMap<Integer, Integer> cmdmaps = new HashMap<>();
    private Intent socketCMDIntent;
	ArrayList<Location> listlocation;
	private SocketCMDService socketService;
	private Messenger rMessenger,mMessenger;
	ServiceConnection serviceConnection;
	Intent socketServiceIntent;
	private CountDownTimer ctimer;
	private Timer ctimer2;
	private TimerTask mTimerTask;
    private String currentNavigationType="TRANSACTION";
	private int currentNavigationSessionID=0;
	private boolean isNavigation=false;



	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		findviews();

		control = new ApiControl(this);

		initBean();
		initResultEventObserver();
		initStatusEventObserver();
		initcontent();
		//initView();
		registfeedback();//注册反馈监听线程
		control.navigateReloacation();
        startSocketService("socketCMDIntent");

		socketServiceIntent = new Intent(this, SocketCMDService.class);
		serviceConnection = new ServiceConnection() {
			@Override
			public void onServiceConnected(ComponentName name, IBinder service){
				SocketCMDService.SocketCMDBinder binder = (SocketCMDService.SocketCMDBinder)service;
				socketService=binder.getService();// 获取到的Service
				rMessenger = new Messenger(binder.getBinder());//get the object of remote service
				mMessenger = new Messenger(mHandler);//initial the object of local service

				//addconsole("本机ip:"+binder.getHostIP());
				//wifiip.setText(binder.getHostIP());
				shakeHand();
			}
			@Override
			public void onServiceDisconnected(ComponentName name) {
				// 连接断开
				rMessenger=null;
			}
		};
		ctimer = new CountDownTimer(10000,1000) {
			@Override
			public void onTick(long millisUntilFinished) {
				addconsole((millisUntilFinished / 1000)+"后自动触发送料使能信号");
			}

			@Override
			public void onFinish() {
				sendMessage();
				btnEnableNavigation.setEnabled(false);
			}
		};
		ctimer2 = new Timer();


	}
	@Override
	protected void onStart() {
		super.onStart();
		bindService(socketServiceIntent, serviceConnection, BIND_AUTO_CREATE);
	}



	@Override
	protected void onStop() {
		super.onStop();
		unbindService(serviceConnection);
	}

	@Override
	protected void onResume() {
		super.onResume();

	}

	private void initcontent() {
		Observable.timer(1, TimeUnit.SECONDS).subscribe(new Action1<Long>() {
			@Override
			public void call(Long aLong) {
				contentResolverControl = new ContentResolverControl();
				listlocation = contentResolverControl.getUnSortWGPoints(getContentResolver(), PackageUtils.getMapName(RosRobotApi.get().getCurrentMap()));

			}
		}, new Action1<Throwable>() {
			@Override
			public void call(Throwable throwable) {
				Log.i("dan", "地图数据读取异常");
				addconsole("地图数据获取异常");
			}
		});

	}

	private void findviews() {
		recyclerView = (RecyclerView) findViewById(R.id.recycler);
		rosip = (TextView) findViewById(R.id.rosip);
		wifiip = (TextView) findViewById(R.id.wifiip);
		wifissid = (EditText) findViewById(R.id.wifissid);
		wifipassword = (EditText) findViewById(R.id.password);
		console = (TextView) findViewById(R.id.console);
		scrollViewconsole = (ScrollView) findViewById(R.id.consolescroll);
		clear = (Button) findViewById(R.id.clear);
		clear.setOnClickListener(this);
		btnEnableNavigation = (Button) findViewById(R.id.enableNavigation);
		btnEnableNavigation.setOnClickListener(this);
        btnEnableTransaction = (Button) findViewById(R.id.enableTransaction);
        btnEnableTransaction.setOnClickListener(this);
		btnRelocation = (Button) findViewById(R.id.relocation);
		btnRelocation.setOnClickListener(this);
	}

	private void initBean() {
		Gson gson = new Gson();
		String data = ConfigReader.read(this, "config.json");
		bean = gson.fromJson(data, CmdBean.class);
		//应为有些信息是要实时从系统里面读取的，所以下面方法就是获取新的数据来刷新bean对象
		bean.refreshProp();
	}

	private void initResultEventObserver() {
		subscriptions.add(RxBus.getDefault().toObservable(ResultEvent.class)
				//	.sample(100, TimeUnit.MILLISECONDS)
				.onBackpressureBuffer()
				.observeOn(AndroidSchedulers.mainThread())
				.subscribe(new Action1<ResultEvent>() {
					@Override
					public void call(ResultEvent resultEvent) {
						Log.i("dan", Thread.currentThread().getName() + "" + resultEvent.retcode);
						handleEvent(resultEvent);
					}
				}));
	}

	private void initStatusEventObserver() {
		subscriptions.add(RxBus.getDefault().toObservable(StatusEvent.class)
				.onBackpressureBuffer()
				.observeOn(AndroidSchedulers.mainThread())
				.filter(new Func1<StatusEvent, Boolean>() {
					@Override
					public Boolean call(StatusEvent statusEvent) {
						return !cmdmaps.containsKey(statusEvent.session_id);
					}
				})
				.subscribe(new Action1<StatusEvent>() {
					@Override
					public void call(StatusEvent statusEvent) {
						cmdmaps.put(statusEvent.session_id, statusEvent.cmd_id);
					}
				}));
	}

	private void initView() {
		List<CmdBean.Cmds> list = bean.getCmds();
		ButtonAdapter adapter = new ButtonAdapter(this, list, this, this);
		RecyclerView.LayoutManager manager = new GridLayoutManager(this, 5, LinearLayoutManager.VERTICAL, false);
		recyclerView.setLayoutManager(manager);
		recyclerView.setAdapter(adapter);
	}
    //通用监听
	RemoteCommonListener commonListener = new RemoteCommonListener() {
		@Override
		public void onResult(int sessionid, int status,String msg) {
			Log.i("dan", "sessionid=" + sessionid + "status=" + status);
			handleFeedBack(sessionid, status,msg);
		}
	};

	private void handleFeedBack(int sessionid, int status,String msg) {
		if(sessionid>0 && currentNavigationSessionID==sessionid) {
			ResultEvent event = new ResultEvent();
			if (RosConstant.Action.ACTION_FINISHED == status) {
				event.message = "导航执行完成";
				if (currentNavigationType == "NAVIGATION") {
					//btnEnableTransaction.setEnabled(true);
					if (mTimerTask != null)
						mTimerTask.cancel();
					ctimer2.purge();
					mTimerTask = new TimerTask() {
						@Override
						public void run() {
							SpeechRobotApi.get().speechStartTTS("请卸载物料！", new SpeechTtsListener() {
								@Override
								public void onAbort() {

								}

								@Override
								public void onEnd() {

								}
							});
						}
					};

					//mTime = Integer.parseInt(editer.getText().toString());
					ctimer2.schedule(mTimerTask, 2000, 2000);
					btnEnableTransaction.setEnabled(true);
				} else if (currentNavigationType == "TRANSACTION"||currentNavigationType=="BACKTOSTATION") {
					socketService.socketSendMessage(currentNavigationType + "|OK");
				}
				RxBus.getDefault().post(event);
				isNavigation=false;
			} else if (RosConstant.Action.ACTION_FAILED == status
					|| RosConstant.Action.ACTION_BE_IMPEDED == status
					|| RosConstant.Action.ACTION_CANCEL == status
					|| RosConstant.Action.ACTION_DEVICE_CONFLICT == status
					|| RosConstant.Action.ACTION_ABNORMAL_SUSPEND == status
					|| RosConstant.Action.ACTION_ACCESS_FORBIDDEN == status
					|| RosConstant.Action.ACTION_UNIMPLEMENTED == status
					|| RosConstant.Action.ACTION_EMERGENCY_STOPPED == status) {
				//control.navigateReloacation();
				event.message = "导航执行失败";
				socketService.socketSendMessage(currentNavigationType + "|NG");
				RxBus.getDefault().post(event);
				isNavigation=false;
			} else if (RosConstant.Action.ACTION_ON_GOING == status) {
				event.message = "请小心避让";
				RxBus.getDefault().post(event);
			}

		}else{
			ResultEvent event = new ResultEvent();
			if (
					 RosConstant.Action.ACTION_DEVICE_CONFLICT == status
					|| RosConstant.Action.ACTION_ACCESS_FORBIDDEN == status
					|| RosConstant.Action.ACTION_EMERGENCY_STOPPED == status) {
				//control.navigateReloacation();
				event.message = "非导航异常中断";
				event.retcode = status;
				//if(isNavigation){
					socketService.socketSendMessage(currentNavigationType + "|NG");
					isNavigation=false;
				//}
				RxBus.getDefault().post(event);
			}
		}
			/*ResultEvent event2 = new ResultEvent();
			event2.message = "sessionid:" + sessionid + "msg:" + msg + "底层返回状态码:";
			event2.retcode = status;
			RxBus.getDefault().post(event2);*/

	}
	private void handleFeedBack2(int sessionid, int status,String msg) {

		if (cmdmaps.containsKey(sessionid)) {
			switch (cmdmaps.get(sessionid)) {
				case DataConfig.CMD_ID_25:
					if (RosConstant.Action.ACTION_FINISHED == status) {
						control.setEnergyReleased(true);
						ResultEvent event = new ResultEvent();
						event.message = "释能执行完成";
						event.retcode = status;
						RxBus.getDefault().post(event);
					}
					break;
                case DataConfig.CMD_ID_17:
					ResultEvent event = new ResultEvent();
					if (RosConstant.Action.ACTION_FINISHED == status) {
						event.message = "导航执行完成";
						if(currentNavigationType=="NAVIGATION"){
							//btnEnableTransaction.setEnabled(true);
							if(mTimerTask!=null)
								mTimerTask.cancel();
							ctimer2.purge();
							mTimerTask = new TimerTask() {
								@Override
								public void run() {
									SpeechRobotApi.get().speechStartTTS("请卸载物料！", new SpeechTtsListener() {
										@Override
										public void onAbort() {

										}

										@Override
										public void onEnd() {

										}
									});
								}
							};


							//mTime = Integer.parseInt(editer.getText().toString());
							ctimer2.schedule(mTimerTask,2000,2000);
							btnEnableTransaction.setEnabled(true);
						}
						else if(currentNavigationType=="TRANSACTION")
							socketService.socketSendMessage(currentNavigationType+"|OK");
						RxBus.getDefault().post(event);
					}
					else if(RosConstant.Action.ACTION_FAILED == status||RosConstant.Action.ACTION_BE_IMPEDED== status||RosConstant.Action.ACTION_EMERGENCY_STOPPED== status){
						event.message = "导航执行失败";
						socketService.socketSendMessage(currentNavigationType+"|NG");
						RxBus.getDefault().post(event);
					}
					else if (RosConstant.Action.ACTION_ON_GOING == status) {
						event.message = "请小心避让";
						RxBus.getDefault().post(event);
					}
                    break;
				default:
					break;
			}
		}
		ResultEvent event = new ResultEvent();
		event.message = "sessionid:"+sessionid+"msg:"+msg+"底层返回状态码:";
		event.retcode = status;
		RxBus.getDefault().post(event);
	}

	private void registfeedback() {
		subscriptions.add(Observable.timer(2000, TimeUnit.MILLISECONDS).subscribe(new Action1<Long>() {
			@Override
			public void call(Long aLong) {
				RosRobotApi.get().registerCommonCallback(commonListener);
				RosRobotApi.get().registerDiagnosticDataCallback(new RemoteDiagnosticDataListener() {
					@Override
					public void onResult(int key, String info) {
						Log.i("dan", "key =" + key + "info" + info);
					}
				});
			}
		}));
	}

	private void handleEvent(ResultEvent event) {
		Object message = event.message;
		int retcode = event.retcode;
		switch (event.type) {
			case ResultEvent.ANDROID_WIFI_IP:
				String[] info = (String[]) message;
                String ip = info[0];
				String ssid = info[1];
				if (!TextUtils.isEmpty(ip)) {
					String origin = "当前WIFI(" + ssid + ")的IP:" + ip;
					SpannableString spa = new SpannableString(origin);
					int start = origin.indexOf("(") + 2;
					int end = origin.indexOf(")的IP:") - 1;
					spa.setSpan(new ForegroundColorSpan(Color.GREEN), start, end, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
					end += 6;
					spa.setSpan(new ForegroundColorSpan(Color.GREEN), end, spa.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
					wifiip.setText(spa);
				} else {
					wifiip.setText("当前WIFI(" + ssid + ")ip无法获取");
				}
				break;
			case ResultEvent.ROS_WIFI_IP:
				if (!TextUtils.isEmpty((String) message)) {
					String origin = "当前ROS系统ip:" + message;
					SpannableString spa = new SpannableString("当前ROS系统ip:" + message);
					int start = origin.indexOf(":") + 1;
					spa.setSpan(new ForegroundColorSpan(Color.GREEN), start, origin.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
					rosip.setText(spa);
				} else {
					rosip.setText("当前ROS系统ip无法获取");
				}
				break;
			case ResultEvent.OHTERS:
				String msg = (String) message;
				if (retcode != ResultEvent.DEFAULT_RET_CODE) {
					msg += retcode;
				}
				addconsole(msg);
				break;
			case ResultEvent.NAVIGATION:
				String msg2 = (String) message;
				if (retcode != ResultEvent.DEFAULT_RET_CODE) {
					msg2 += retcode;
				}
				SpeechRobotApi.get().speechStartTTS(msg2, new SpeechTtsListener() {
					@Override
					public void onAbort() {
						addconsole("语音播报失败");
					}

					@Override
					public void onEnd() {

					}
				});
				addconsole(msg2);
				break;
			default:
				break;
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

	private void clearConsole() {
		console.setText("");
	}

	@Override
	public void onClick(View view) {
		switch (view.getId()) {
			case DataConfig.CMD_ID_0:
				setWifi();
				break;
			case DataConfig.CMD_ID_1:
				control.openPS3joy();
				break;
			case DataConfig.CMD_ID_2:
				control.setRosIp();
				break;
			case DataConfig.CMD_ID_3:
				startActivity(AppConstants.MOVEACTIVITY);
				break;
			case DataConfig.CMD_ID_4:

				break;
			case DataConfig.CMD_ID_5:

				break;
			case DataConfig.CMD_ID_6:

				break;
			case DataConfig.CMD_ID_7:

				break;
			case DataConfig.CMD_ID_8:

				break;
			case DataConfig.CMD_ID_9:
				control.pauseMove();
				break;
			/*导航*/
			case DataConfig.CMD_ID_10:
				break;
			/*run*/
			case DataConfig.CMD_ID_11:
				control.run();
				break;
			case DataConfig.CMD_ID_12:
				control.TtsPlay("主人你好，我已准备就绪，很荣幸认识你");
				break;
			case DataConfig.CMD_ID_13:
				control.TtsStop();
				break;
			case DataConfig.CMD_ID_14:
				control.calldispatch();
				break;
			case DataConfig.CMD_ID_15:
				control.startAsr();
				break;
			case DataConfig.CMD_ID_16:
				control.setface();
				break;
			case DataConfig.CMD_ID_17:
				control.navgation(listlocation);
				break;
			case DataConfig.CMD_ID_18:
				control.cancelNavgation();
				break;
			case DataConfig.CMD_ID_19:
				control.ledflash();
				break;
			case DataConfig.CMD_ID_20:
				control.poweroff();
				break;
			case DataConfig.CMD_ID_21:
				control.getWifiIp();
				break;
			case DataConfig.CMD_ID_22:
/*				if (control.isEnergyReleased()) {
					control.zeroSetting();
				} else {
					ResultEvent event = new ResultEvent();
					event.message = "请必须先进行释能操作";
					RxBus.getDefault().post(event);
				}*/

				break;
			case DataConfig.CMD_ID_23:
				control.rightWaist();
				break;
			case DataConfig.CMD_ID_24:
				control.leftWaist();
				break;
			case DataConfig.CMD_ID_25:
				//control.servoEnergyRelease(DataConfig.CMD_ID_25);
				startActivity(AppConstants.RESETACTIVITY);
				break;
			case DataConfig.CMD_ID_26:
				startActivity(AppConstants.STRUCTUREATESTACTIVITY);
				break;
			case DataConfig.CMD_ID_27:
				control.switchToChinese();
				break;
			case DataConfig.CMD_ID_28:
				control.switchToEnglish();
				break;
			case DataConfig.CMD_ID_29:
				CmdBean.Cmds cmds = (CmdBean.Cmds) view.getTag();
				control.switchDigital((Button) view, cmds);
				break;
			case DataConfig.CMD_ID_30:
				control.dockOnStation();
				break;
			case R.id.clear:
				clearConsole();
				test();
				break;
			case R.id.enableNavigation:
				if(ctimer!=null)
					ctimer.cancel();
				sendMessage();
				btnEnableNavigation.setEnabled(false);
				break;
            case R.id.enableTransaction:
				if(mTimerTask!=null)
					mTimerTask.cancel();
				ctimer2.purge();
                socketService.socketSendMessage(currentNavigationType+"|OK");
                btnEnableTransaction.setEnabled(false);
                break;
			case R.id.relocation:
				control.navigateReloacation();
				break;
			default:
				break;

		}
	}

	private void setWifi() {
		String name = wifissid.getText().toString();
		String password = wifipassword.getText().toString();
		if (TextUtils.isEmpty(name)) {
			Toast.makeText(this, "请输入要设置的wifi名字", Toast.LENGTH_SHORT).show();
			return;
		}
		if (TextUtils.isEmpty(password)) {
			Toast.makeText(this, "请输入要设置的wifi密码", Toast.LENGTH_SHORT).show();
			return;
		}
		int code = RosRobotApi.get().setRosWifi(name, password);
		addconsole("wifi设置结果 = " + code);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (subscriptions != null && !subscriptions.isUnsubscribed()) {
			subscriptions.unsubscribe();
		}
		SpeechRobotApi.get().destory();
		RosRobotApi.get().destory();
		unbindService(serviceConnection);
        stopSocketService();

	}

	@Override
	public boolean onLongPressCmd(CmdBean.Cmds cmds) {
		Log.i("dan", "cmds" + cmds.getName());
		alertDialog.setMessage(cmds.getTip());
		alertDialog.show();
		alertDialog.getButton(AlertDialog.BUTTON_NEGATIVE).setVisibility(View.GONE);
		return true;
	}

	private void startActivity(String action) {
		Intent intent = new Intent();
		intent.setAction(action);
		startActivity(intent);
	}

	private  void startSocketService(String action){
        socketCMDIntent = new Intent(this,SocketCMDService.class);
        startService(socketCMDIntent);
    }

    private void stopSocketService(){
        stopService(socketCMDIntent);
    }

	private void test() {
		final List<String> list = ConfigReader.readlist(MainActivity.this, "chat.txt");
		Subscription sb = Observable.interval(1, TimeUnit.SECONDS)
				.map(new Func1<Long, String>() {
					@Override
					public String call(Long aLong) {
						return list.get(aLong.intValue() % list.size());
					}
				})
				// .delay(10, TimeUnit.SECONDS)
				.map(new Func1<String, String>() {
					@Override
					public String call(String s) {
						String regEx = "(~?/:).*$|(\\(|\\（)+(.*)(\\)|\\）)$|\\[[\\u4e00-\\u9fa5]*\\]";
						Pattern pattern = Pattern.compile(regEx);
						Matcher matcher = pattern.matcher(s);
						if (matcher.find()) {
							//	 s = matcher.group(0);
						}
						s = s.replaceAll(regEx, "");
						return s;
					}
				})
				.subscribe(new Action1<String>() {
					@Override
					public void call(String s) {
						Log.i("dan", "当前播报:" + s);
						//		SpeechRobotApi.get().speechStartTTS(s, 50);
					}
				});

	}

	private void sendMessage() {
		Message msg = Message.obtain(null, 2);//MessengerService.TEST=0
		msg.replyTo = mMessenger;
		try {
			rMessenger.send(msg);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	private void shakeHand() {
		Message msg = Message.obtain(null, 3);//MessengerService.TEST=0
		msg.replyTo = mMessenger;
		try {
			rMessenger.send(msg);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private Handler mHandler = new Handler(){
		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
				case 2://送料导航
					Log.e("AGVActivity", "Set btnEnableNavigation be enabled");
					isNavigation=true;
                    currentNavigationType="NAVIGATION";
					btnEnableNavigation.setEnabled(true);
					ctimer.start();
					addconsole("收到导航指令");
					break;
				case 3:
					addconsole("正在导航session:"+msg.obj.toString());
					currentNavigationSessionID = (int)msg.obj;
					isNavigation=true;
					break;
				case 4:
					addconsole(msg.obj.toString());
					//currentNavigationSessionID = (int)msg.obj;
					break;
                case 5://转移导航
                    Log.e("AGVActivity", "Set btnEnableNavigation be enabled");
					addconsole("收到transaction命令");
                    currentNavigationType="TRANSACTION";
					isNavigation=true;
                    break;
				case 6://转移导航
					Log.e("AGVActivity", "Set btnEnableNavigation be enabled");
					addconsole("收到backtostation命令");
					currentNavigationType="BACKTOSTATION";
					isNavigation=true;
					break;
				default:
					break;
			}
		}
	};


}
