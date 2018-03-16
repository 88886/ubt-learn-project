package com.ubtechinc.cruzr.cruzrapirunner.ui.activity;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.FragmentActivity;
import android.support.v7.app.AppCompatActivity;


/**
 * Created by Administrator on 2017/5/27.
 */

public class BaseActivity extends FragmentActivity {
	public AlertDialog alertDialog;
	@Override
	protected void onCreate(@Nullable Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		initdialog();
	}
	private void initdialog() {
		AlertDialog.Builder builder = new AlertDialog.Builder(BaseActivity.this);
		builder.setMessage("");
		builder.setTitle("Tip");
		builder.setPositiveButton("了解", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();

			}
		});
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			}
		});
		alertDialog = builder.create();
	}
}
