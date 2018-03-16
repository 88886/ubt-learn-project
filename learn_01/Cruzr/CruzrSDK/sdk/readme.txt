SDK 更新日志：

*******************************************************************
2017.04.07

1、优化听写回调
2、新增ros ledSetworkByTimes 接口
3、新增clearMap接口

*******************************************************************
2017.04.19

1、优化SDK initializ初始化成功回调
2、新增ros getVersionInfo 接口
3、新增ros getDiagnosticStatus 接口
4、新增ros zeroSetting 接口
5、优化ROS客户端创建连接
6、更新热词上传接口HashMap参数

*******************************************************************
2017.05.02

1、SDK TTS增加onAbort中断回调

*******************************************************************
2017.05.10

1、SDK 增加启动ps2move接口
2、SDK 增加舵机释能接口
3、SDK RosConstant参数定义与ROS同步
4、SDK destory接口私有化
5、SDK 解决监听弱引用内存泄漏问题

*******************************************************************
2017.05.16

1、增加语音识别音量大小回调
2、增加ROS led RGB设置接口
3、增加ROS导航重定向接口
4、开放destory接口释放引用


*******************************************************************
2017.05.26

1、增加增加模块检测的接口
2、增加ROS常量Action 值
3、增加获取表情ID接口


*******************************************************************
2017.06.06

1、增加状态机定位状态
2、同步ROS常量定义



*******************************************************************
2017.06.28

1、增加ROS电子皮肤的接口
public float[] getHumiture()
public int runWithEskinEnable(String[] name, int eskinMask)
public int registerEskinStatusCallback(RemoteEskinStatusListener listener)
2.增加Speech获取是否处于TTS 状态中的接口
public int isTtsSpeaking()


*******************************************************************

*******************************************************************
2017.07.03

1.增加导航预设速度相关方法
public int setNavigateSpeed(float speed);
public float getNavigateSpeed();
public int navigateToByPresetedSpeed(String packageName,float x, float y, float theta);


*******************************************************************
2017.07.14

1.语音设置语速、发音人私有化
public int speechSetTtsSpeed(int speed)
public float speechSetVoiceName(String strVoiceName)



*******************************************************************
2017.07.17

1.语音设置语速、发音人私有化
public int setProperty(String key, String value)
public String getProperty(String key, String defalutValue)



*******************************************************************
2017.08.04
零点设置手指角度调试
int setFingerAdjust(String packageName,String value);
底层固件升级
int firmwareUpgrade(String packageName);
轨道模式导航到绝对坐标为(x,y)的地方去
int navigateTrackTo(String app_packg_name,float x, float y, float theta, float maxSpeed, int retryCount, int interval);



*******************************************************************