package com.ubtechinc.cruzr.cruzrapirunner.ui;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.RecyclerView.ViewHolder;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import com.ubtechinc.cruzr.cruzrapirunner.R;
import com.ubtechinc.cruzr.cruzrapirunner.bean.CmdBean;

import java.util.List;

/**
 * Created by Administrator on 2017/4/26.
 */

public class ButtonAdapter extends RecyclerView.Adapter<ButtonAdapter.MyViewHolder> implements View.OnLongClickListener {
	private Context context;
	private List<CmdBean.Cmds> cmds;
	private View.OnClickListener listener;
	private OnCmdLongPressListener onLongClickListener;
	private final static int CMDS_TAG = 0;

	public ButtonAdapter(Context context, List<CmdBean.Cmds> lists, View.OnClickListener listener, OnCmdLongPressListener longClickListener) {
		this.context = context;
		this.cmds = lists;
		this.listener = listener;
		this.onLongClickListener = longClickListener;
	}

	@Override
	public MyViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
		MyViewHolder holder = new MyViewHolder(LayoutInflater.from(
				context).inflate(R.layout.item_button, parent,
				false));
		holder.btn.setOnClickListener(listener);
		holder.btn.setOnLongClickListener(this);
		return holder;
	}

	@Override
	public void onBindViewHolder(MyViewHolder holder, int position) {
		holder.btn.setText(cmds.get(position).getName());
		holder.btn.setId(cmds.get(position).getCid());
		holder.btn.setTag(cmds.get(position));
	}

	@Override
	public int getItemCount() {
		return cmds.size();
	}

	@Override
	public boolean onLongClick(View v) {
		CmdBean.Cmds cmds = (CmdBean.Cmds) (v.getTag());
		return onLongClickListener.onLongPressCmd(cmds);
	}

	class MyViewHolder extends ViewHolder {

		Button btn;
		public MyViewHolder(View view) {
			super(view);
			btn = (Button) view.findViewById(R.id.item_button);
		}
	}
}
