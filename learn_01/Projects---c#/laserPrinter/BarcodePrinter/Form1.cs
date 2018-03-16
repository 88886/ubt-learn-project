﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using upsAPIs;
using System.IO;
using System.Threading;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace LaserPrinter
{
    public partial class ElectronicDataColector : Form
    {
        private SerialPort ComDevice = new SerialPort();

        public ElectronicDataColector()
        {
            InitializeComponent();
        }

        string filename;
        FileStream fs;
        StreamWriter wr;
        Thread weightThread;
        Thread sqlThread;
        upsWebAPIs ups = new upsWebAPIs();
        string sn_code;
        bool isOkBind = false;
        bool isOkCheck = true;
        Regex re = new Regex(@"[\\+](.+?  [k]?g{1})");
        XElement element;

        //URL在配置文件
        string checkUrl;
        string bindUrl;

        public void init()
        {

            //filename = System.Windows.Forms.Application.StartupPath;
            //string newFilename = filename + "\\SN_WEIGHT.txt";
            //if (!File.Exists(newFilename))
            //{
              //  wr = File.CreateText(newFilename);
            //}
            // else
            //{
              //  wr = File.AppendText(newFilename);
            //}

            
           
            cbbComList.Items.AddRange(SerialPort.GetPortNames());
            if (cbbComList.Items.Count > 0)
            {
                cbbComList.SelectedIndex = 0;
            }
            cbbBaudRate.SelectedIndex = 0;
            cbbDataBits.SelectedIndex = 0;
            cbbParity.SelectedIndex = 0;
            cbbStopBits.SelectedIndex = 0;
            pictureBox1.BackColor = Color.Red;

            ComDevice.DataReceived += new SerialDataReceivedEventHandler(Com_DataReceived);//绑定事件


            
           
        }

        public void startServer()
        {
            try {
                weightThread = new Thread(getWeightThread);
                weightThread.IsBackground = true;
                weightThread.Start();

                sqlThread = new Thread(getSql);
                sqlThread.IsBackground = true;
                sqlThread.Start();

                filename = System.Windows.Forms.Application.StartupPath;
                string newFilename = filename + "\\config.xml";
                XDocument document = XDocument.Load(newFilename);
                XElement root = document.Root;

                element = root.Element("checkUrl");
                checkUrl = element.Value.Trim();

                element = root.Element("bindUrl");
                bindUrl = element.Value.Trim();
                

            }
            catch (Exception e)
            {
                txtShowData.AppendText("请查看网络连接，或者联系后台管理人员\r\n");
            }
          

        }

        //判断是否需要绑定
        private void getSql()
        {
            while (true)
            {
                
                    if (isOkCheck == true)
                    {
                        Thread.Sleep(500);

                        try
                        {
                            string result = upsWebAPIs.Get(checkUrl);
                            ResponseData2 checkWeight = new ResponseData2();

                            Object obj = upsWebAPIs.getObjectByJson(result, checkWeight);
                           
                            checkWeight = (ResponseData2)obj;

                            if (checkWeight != null)
                            {
                                if (checkWeight.success == true)
                                {
                                    sn_code = checkWeight.data;
                                    isOkCheck = false;
                                    isOkBind = true;
                                }
                            }
                             
                        }
                        catch (Exception e)
                        {
                            getMessage(e.Message+":MES系统获取信息失败，请联系后台管理人员！\r\n");
                        }                        
                        
                    }
               
                
            }
        }

        //绑定重量
        private void getWeightThread()
        {
            while (true)
            {
                Thread.Sleep(1500);
                try
                {

                    if (isOkCheck == false && isOkBind == true)
                    {

                        if (txtWeight.Text.Trim() == "")
                        {
                            getMessage("获取不到重量请查看电子秤！\r\n");

                        }
                        else if (txtWeight.Text.Trim() == "0.0g")
                        {
                            getMessage("警告重量为0.0g，请放置重物！\r\n");
                        }
                        else
                        {

                            string st = txtWeight.Text.Replace("g", "");
                            float f1 = Convert.ToSingle(st);

                            if (f1 < 3)
                            {
                                getMessage("重量为0或者不准\r\n");
                            }
                            else
                            {
                                string result = upsWebAPIs.Get(bindUrl + "/" + sn_code + "/" + txtWeight.Text);
                                

                                ResponseMotorMes bindMes = new ResponseMotorMes();
                                ResponseMotor2 data = new ResponseMotor2();

                                Object obj = upsWebAPIs.getObjectByJson(result, bindMes);
                                bindMes = (ResponseMotorMes)obj;

                                if (bindMes != null)
                                {
                                    if(bindMes.success == "true")
                                    {
                                        data = (ResponseMotor2)bindMes.data;
                                        getMessage("SN:"+sn_code+" 绑定重量成功 重量为:" + txtWeight.Text);

                                        sn_code = null;
                                        bindMes = null;

                                        isOkCheck = true;
                                        isOkBind = false;
                                   
                                    }
          
                                }

                            }
                        }
                    }

                }
                catch (Exception e)
                {
                    getMessage(e.Message+"绑定重量失败，请联系后台管理人员!\r\n");
                }
            }   
            
        }

        private void getMessage(string mes)
        {
            this.BeginInvoke(new MethodInvoker(delegate
            {
                txtShowData.AppendText(mes);
            }));
        }
        
        private void button4_Click(object sender, EventArgs e)
        {
            byte[] sendData = null;
            
          

            
        }

        private void ElectronicDataColector_Load(object sender, EventArgs e)
        {
           
            init();
        }

        private void button_clearR_Click(object sender, EventArgs e)
        {
            txtShowData.Clear();
        }

        private void button_clearS_Click(object sender, EventArgs e)
        {
             
        }

        /// <summary>
        /// 接收数据
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Com_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try 
            {
                byte[] ReDatas = new byte[ComDevice.BytesToRead];
                
                if (ReDatas.Length > Convert.ToInt32(txtLength.Text.Trim()))
                {
                    ComDevice.Read(ReDatas, 0, ReDatas.Length);//读取数据
                    this.AddData(ReDatas);//输出数据
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            
        }

        /// <summary>
        /// 添加数据
        /// </summary>
        /// <param name="data">字节数组</param>
        public void AddData(byte[] data)
        {
            if (rbtnHex.Checked)
            {
                StringBuilder sb = new StringBuilder();
                for (int i = 0; i < data.Length; i++)
                {
                    sb.AppendFormat("{0:x2}" + " ", data[i]);
                }
                AddContent(sb.ToString().ToUpper());
            }
            else if (rbtnASCII.Checked)
            {
                AddContent(new ASCIIEncoding().GetString(data));
            }
            else if (rbtnUTF8.Checked)
            {
                AddContent(new UTF8Encoding().GetString(data));
            }
            else if (rbtnUnicode.Checked)
            {
                AddContent(new UnicodeEncoding().GetString(data));
            }

            /*lblRevCount.Invoke(new MethodInvoker(delegate
            {
                lblRevCount.Text = (int.Parse(lblRevCount.Text) + data.Length).ToString();
            }));*/
        }


        /// <summary>
        /// 输入到显示区域
        /// </summary>
        /// <param name="content"></param>
        private void AddContent(string content)
        {
            this.BeginInvoke(new MethodInvoker(delegate
            {
                //txtWeight.Text=content;
                //txtShowData.AppendText(content+"--\r\n");
                
                txtWeight.Text = getWeight(content);
                
            }));
        }

        private void btnOpen_Click(object sender, EventArgs e)
        {
            if (cbbComList.Items.Count <= 0)
            {
                MessageBox.Show("没有发现串口,请检查线路！");
                return;
            }

            if (ComDevice.IsOpen == false)
            {
                ComDevice.PortName = cbbComList.SelectedItem.ToString();
                ComDevice.BaudRate = Convert.ToInt32(cbbBaudRate.SelectedItem.ToString());
                ComDevice.Parity = (Parity)Convert.ToInt32(cbbParity.SelectedIndex.ToString());
                ComDevice.DataBits = Convert.ToInt32(cbbDataBits.SelectedItem.ToString());
                ComDevice.StopBits = (StopBits)Convert.ToInt32(cbbStopBits.SelectedItem.ToString());
                try
                {
                    ComDevice.Open();
                     
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                btnOpen.Text = "关闭串口";
                pictureBox1.BackColor = Color.Green;
            }
            else
            {
                try
                {
                    ComDevice.Close();
                     
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                btnOpen.Text = "打开串口";
                pictureBox1.BackColor = Color.Red;
            }

            cbbComList.Enabled = !ComDevice.IsOpen;
            cbbBaudRate.Enabled = !ComDevice.IsOpen;
            cbbParity.Enabled = !ComDevice.IsOpen;
            cbbDataBits.Enabled = !ComDevice.IsOpen;
            cbbStopBits.Enabled = !ComDevice.IsOpen;
        }
        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public bool SendData(byte[] data)
        {
            if (ComDevice.IsOpen)
            {
                try
                {
                    ComDevice.Write(data, 0, data.Length);//发送数据
                    return true;
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                MessageBox.Show("串口未打开", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return false;
        }

        /// <summary>
        /// 字符串转换16进制字节数组
        /// </summary>
        /// <param name="hexString"></param>
        /// <returns></returns>
        private byte[] strToHexByte(string hexString)
        {
            hexString = hexString.Replace(" ", "");
            if ((hexString.Length % 2) != 0)
                hexString += " ";
            byte[] returnBytes = new byte[hexString.Length / 2];
            for (int i = 0; i < returnBytes.Length; i++)
                returnBytes[i] = Convert.ToByte(hexString.Substring(i * 2, 2).Replace(" ", ""), 16);
            return returnBytes;
        }

        private void btnReceive_Click(object sender, EventArgs e)
        {
            //string weight=getWeight();
            //lblWeight.Text = weight;
        }

        private String getWeight(string weight) 
        {



            String str = re.Match(weight).Groups[1].Value;
            str = str.Replace("  ", "");
            return str;

                
            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //String weight = getWeight();
            //txtWeight.Text = weight;
            //String sncode = txt_code.Text.Trim();


            if (button1.Text == "启动")
            {
                startServer();
                button1.Text = "关闭";
                isOkBind = false;
                isOkCheck = true;
            }
            else
            {
                weightThread.Abort();
                sqlThread.Abort();
                button1.Text = "启动";
            }


            //wr.WriteLine(weight);
            //wr.Flush();
            //updateToMes("1","20");

        }
        
        private void updateToMes(String code,String weight)
        {
            if (!code.Trim().Equals("") && !weight.Trim().Equals(""))
            {
               
               

                //upsWebAPIs UPS = new upsWebAPIs();
                //String result = UPS.bindWeightToCode(code, weight);

                //if (result == "false")
                  //  MessageBox.Show("提交mes失败！");
            }
            else
            {
                MessageBox.Show("SN号或重量数据不能为空！");
            }
        }

        private void txt_code_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyValue ==13) 
            {
                //String weight = getWeight();
                //txtWeight.Text = weight;
                 
            }
        }

        private void txtWeight_TextChanged(object sender, EventArgs e)
        {

        }

        private void label8_Click(object sender, EventArgs e)
        {

        }

        private void txtShowData_TextChanged(object sender, EventArgs e)
        {

        }

        private void rbtnHex_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void rbtnUTF8_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void groupBox4_Enter(object sender, EventArgs e)
        {

        }

        private void rbtnASCII_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void txtLength_TextChanged(object sender, EventArgs e)
        {

        }

        private void button_clearR_Click_1(object sender, EventArgs e)
        {
            txtShowData.Clear();
        }

        private void label9_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }
    }
}
