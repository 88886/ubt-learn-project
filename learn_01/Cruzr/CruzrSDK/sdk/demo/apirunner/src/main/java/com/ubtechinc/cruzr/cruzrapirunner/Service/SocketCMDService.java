package com.ubtechinc.cruzr.cruzrapirunner.Service;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.IntentFilter;
import android.os.IBinder;
import android.content.Intent;
import android.os.Binder;
import android.os.Message;
import android.util.Log;
import android.os.Messenger;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import com.ubtechinc.cruzr.cruzrapirunner.ApiControl;
import com.ubtechinc.cruzr.cruzrapirunner.Resolver.ContentResolverControl;
import com.ubtechinc.cruzr.cruzrapirunner.Resolver.Location;
import com.ubtechinc.cruzr.cruzrapirunner.Socket.*;
import com.ubtechinc.cruzr.cruzrapirunner.utils.PackageUtils;
import com.ubtechinc.cruzr.sdk.ros.RosRobotApi;

import rx.Observable;
import rx.functions.Action1;

/**
 * Created by ubt on 2017/10/19.
 * 此服务用于后台处理机器人接收到的Socket指令，并将机器人的调度反馈信号发送给调度系统
 */

public class SocketCMDService extends Service {
    private final MyHandler myHandler = new MyHandler(this);
    private MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
    @SuppressLint("StaticFieldLeak")
    public static Context context;
    private static TcpServer tcpServer = null;
    ExecutorService exec = Executors.newCachedThreadPool();
    private ApiControl control;
    ArrayList<Location> listlocation;
    private ContentResolverControl contentResolverControl;
    private String socketSendMSG="";
    private boolean navigationEnable=false;
    private Messenger mMessenger = new Messenger(myHandler);
    private Messenger cMessenger;
    private String currentNavigationTask="";

    //服务创建
    @Override
    public void onCreate() {

        super.onCreate();
        context = this;

        control = new ApiControl(this);
        initcontent();//初始化读取地图中的位置列表
        bindReceiver();

    }

    // 服务启动
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        bindReceiver();
        tcpServer = new TcpServer(1234);//默认开放端口
        exec.execute(tcpServer);
        return super.onStartCommand(intent, flags, startId);
    }

    //服务销毁
    @Override
    public void onDestroy() {
        tcpServer.closeSelf();
        stopSelf(); //自杀服务
        super.onDestroy();
    }

    //绑定服务
    @Override
    public IBinder onBind(Intent intent) {
        return new SocketCMDBinder();
    }
    // IBinder是远程对象的基本接口，是为高性能而设计的轻量级远程调用机制的核心部分。但它不仅用于远程
    // 调用，也用于进程内调用。这个接口定义了与远程对象交互的协议。
    public class SocketCMDBinder extends Binder {
        /** * 获取Service的方法 * @return 返回PlayerService */
        public SocketCMDService getService(){
            return SocketCMDService.this;
        }
        public IBinder getBinder(){
            return mMessenger.getBinder();
        }
        public String getHostIP(){
            return getHostIP();
        }
    }

    /**
     * 获取ip地址
     * @return
     */
    public String getHostIP() {

        String hostIp = null;
        try {
            Enumeration nis = NetworkInterface.getNetworkInterfaces();
            InetAddress ia = null;
            while (nis.hasMoreElements()) {
                NetworkInterface ni = (NetworkInterface) nis.nextElement();
                Enumeration<InetAddress> ias = ni.getInetAddresses();
                while (ias.hasMoreElements()) {
                    ia = ias.nextElement();
                    if (ia instanceof Inet6Address) {
                        continue;// skip ipv6
                    }
                    String ip = ia.getHostAddress();
                    if (!"127.0.0.1".equals(ip)) {
                        hostIp = ia.getHostAddress();
                        break;
                    }
                }
            }
        } catch (SocketException e) {
            Log.i("FuncTcpServer", "SocketException");
            e.printStackTrace();
        }
        return hostIp;

    }

    private class MyHandler extends android.os.Handler{
        private final WeakReference<SocketCMDService> mService;
        MyHandler(SocketCMDService service){
            mService = new WeakReference<SocketCMDService>(service);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 1:
                    SocketCMDService service = mService.get();
                    if (service!= null){
                        //socket指令处理
                        socketRequestProcess(msg);
                    }
                    break;
                case 2:
                    Log.e("mainActivity", "Get Message from MainActivity.");
                    cMessenger = msg.replyTo;
                    navigationEnable=true;
                    execNavigation();//执行当前导航任务
                    break;
                case 3:
                    Log.e("mainActivity", "Get Message from MainActivity.");
                    cMessenger = msg.replyTo;
                    break;
                default:
                    break;
            }


        }
    }

    private class MyBroadcastReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String mAction = intent.getAction();
            switch (mAction){
                case "tcpServerReceiver":
                    String msg = intent.getStringExtra("tcpServerReceiver");
                    Message message = Message.obtain();
                    message.what = 1;
                    message.obj = msg;
                    myHandler.sendMessage(message);
                    break;

            }
        }
    }

    private void bindReceiver(){
        IntentFilter intentFilter = new IntentFilter("tcpServerReceiver");
        registerReceiver(myBroadcastReceiver,intentFilter);
    }

    private void initcontent() {
        Observable.timer(1, TimeUnit.SECONDS).subscribe(new Action1<Long>() {
            @Override
            public void call(Long aLong) {
                contentResolverControl = new ContentResolverControl();
                String currentMap = RosRobotApi.get().getCurrentMap();
                listlocation = contentResolverControl.getUnSortWGPoints(getContentResolver(), PackageUtils.getMapName(RosRobotApi.get().getCurrentMap()));

            }
        }, new Action1<Throwable>() {
            @Override
            public void call(Throwable throwable) {
                Log.i("dan", "地图数据读取异常");
               // addconsole("地图数据获取异常");
            }
        });

    }
    public void socketRequestProcess(Message msg){
        String socketCmd = msg.obj.toString();
        Log.i("AGV_Service", "收到指令: " +socketCmd);
        String[] arryMsg = socketCmd.split("\\|");//sock指令格式为"指令代号|参数1，参数2"
        Log.i("AGV_Service", "分解指令: " +arryMsg[0]);
        if(arryMsg!=null && arryMsg.length>0){
            switch(arryMsg[0]){
                case "NAVIGATION"://运送导航，等待10s
                    socketSendMessage("NAVIGATION|RSUCCESS");
                    Message inimsg = Message.obtain(null, 2);//MessengerService.TEST=0
                    try {
                        cMessenger.send(inimsg);
                        currentNavigationTask = arryMsg[1];//新的导航目的地会覆盖旧的目的地
                        Log.i("AGV_Service", "收到运送导航指令: " +currentNavigationTask);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;
                case "TRANSACTION"://转移导航，立即执行
                    socketSendMessage("TRANSACTION|RSUCCESS");
                    try {
                        Thread.sleep(100);
                    }
                    catch (Exception e){
                        e.printStackTrace();
                    }

                    Message inimsg2 = Message.obtain(null, 5);//MessengerService.TEST=0
                    try {
                        cMessenger.send(inimsg2);
                        currentNavigationTask = arryMsg[1];//新的导航目的地会覆盖旧的目的地
                        Log.i("AGV_Service", "收到转移导航指令: " +currentNavigationTask);
                        execNavigation();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;
                case "BACKTOSTATION"://转移导航，立即执行
                    socketSendMessage("BACKTOSTATION|RSUCCESS");
                    try {
                        Thread.sleep(100);
                    }
                    catch (Exception e){
                        e.printStackTrace();
                    }
                    Message inimsg3 = Message.obtain(null, 6);//MessengerService.TEST=0
                    try {
                        cMessenger.send(inimsg3);
                        currentNavigationTask = arryMsg[1];//新的导航目的地会覆盖旧的目的地
                        Log.i("AGV_Service", "收到转移导航指令: " +currentNavigationTask);
                        execNavigation();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;
                case "GETBATTERY"://获取电量
                    socketSendMessage("GETBATTERY|RSUCCESS");
                    try {
                        Thread.sleep(10);
                    }
                    catch (Exception e){
                        e.printStackTrace();
                    }
                    int battery = control.getBattery();
                    socketSendMessage("GETBATTERY|"+battery);
                    break;
                case "GETLOCATION"://获取位置
                    socketSendMessage("GETLOCATION|RSUCCESS");
                    try {
                        Thread.sleep(10);
                    }
                    catch (Exception e){
                        e.printStackTrace();
                    }
                    String position = control.getPosition();
                    socketSendMessage("GETLOCATION|"+position);
                    break;
                case "GETSTATIONLOCATION"://获取位置
                    socketSendMessage("GETSTATIONLOCATION|RSUCCESS");
                    try {
                        Thread.sleep(10);
                    }
                    catch (Exception e){
                        e.printStackTrace();
                    }
                    String position2 = getStationLocation(arryMsg[1]);
                    if(position2.trim().equals(""))
                        socketSendMessage("GETSTATIONLOCATION|NG");
                    else
                        socketSendMessage("GETSTATIONLOCATION|"+position2);
                    break;
                case "GETAGVSTATUS"://获取Action工作状态
                    break;
            }
        }

    }
    public void socketSendMessage(String msg){
        socketSendMSG = msg;
        exec.execute(new Runnable() {
            @Override
            public void run() {
                tcpServer.SST.get(0).send(socketSendMSG);
            }
        });
    }
    public void execNavigation(){
            int sessionid=0;
            if(currentNavigationTask!=null && !currentNavigationTask.trim().equals("") && listlocation != null && listlocation.size()>0){
                for (int i=0;i<listlocation.size();i++){
                    if(listlocation.get(i).getName().equals(currentNavigationTask)){
                        //control.cancelNavigation();
                        socketSendMessage("MOVING");
                        sessionid=control.navigation(listlocation.get(i));

                        Message msg = Message.obtain();
                        msg.what=3;
                        msg.obj=sessionid;
                        try{
                            cMessenger.send(msg);
                        }catch (Exception e){
                            e.printStackTrace();
                        }
                        break;
                    }
                    else if(i==listlocation.size()-1){
                        Message msg = Message.obtain();
                        msg.what=4;
                        msg.obj="导航目的地不存在";
                        try{
                            cMessenger.send(msg);
                        }catch (Exception e){
                            e.printStackTrace();
                        }

                    }
                }
            }

    }
    public String getStationLocation(String stationName){
        String position="";
        if(stationName!=null && !stationName.trim().equals("") && listlocation != null && listlocation.size()>0){
            for (int i=0;i<listlocation.size();i++){
                if(listlocation.get(i).getName().equals(stationName)){
                    float x = listlocation.get(i).getX();
                    float y = listlocation.get(i).getY();
                    position = x+","+y;
                    break;
                }
            }
        }
        return position;

    }

}
