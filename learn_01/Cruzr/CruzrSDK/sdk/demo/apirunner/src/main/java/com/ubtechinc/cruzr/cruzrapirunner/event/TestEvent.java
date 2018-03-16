package com.ubtechinc.cruzr.cruzrapirunner.event;

/**
 * Created by Administrator on 2017/5/31.
 */

public class TestEvent extends Event {
	public String msg = "";
	public TestEvent(String msg) {
		this.msg = msg;
	}
}
