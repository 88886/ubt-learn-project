package com.ubtechinc.cruzr.cruzrapirunner.event;

/**
 * Created by Administrator on 2017/4/28.
 */

public class ResultEvent extends Event {

	public static final int ANDROID_WIFI_IP = 0;
	public static final int ROS_WIFI_IP = 1;
	public static final int OHTERS = 2;
	public static final int NAVIGATION = 3;
	public static final int DEFAULT_RET_CODE = -1;
	public int type = OHTERS;
	public int retcode = DEFAULT_RET_CODE;
	public Object message;
}
