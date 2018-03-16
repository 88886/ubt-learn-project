using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using upsAPIs;
using System.Windows.Forms;

namespace LaserPrinter
{
    public partial class AutoStorageConnector : Form
    {
        public AutoStorageConnector()
        {
            InitializeComponent();
        }

        Socket socketSend;
        Boolean transEnable = true;
        String currentTask = "00000000";
        Boolean connectEnable = false;
        Thread c_thread;
        Thread task_thread;

        private void AutoStorageConnector_Load(object sender, EventArgs e)
        {
            Control.CheckForIllegalCrossThreadCalls = false;

            txt_ip.Text = Properties.Settings.Default.serverIP;
            txt_port.Text = Properties.Settings.Default.port;
            comboBox1.Text = Properties.Settings.Default.storageNO;

            connectServer();
        }
        /// <summary>
        /// 连接服务端
        /// </summary>
        public void connectServer() {
            try
            {
                if (txt_ip.Text.ToString().Trim().Equals("") || txt_port.Text.ToString().Trim().Equals(""))
                {
                    MessageBox.Show("服务端IP和端口号设置不能为空！");
                }
                else 
                {
                    //创建客户端Socket，获得远程ip和端口号
                    socketSend = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                    IPAddress ip = IPAddress.Parse(txt_ip.Text.ToString());
                    IPEndPoint point = new IPEndPoint(ip, Convert.ToInt32(txt_port.Text.ToString()));

                    socketSend.Connect(point);
                    connectEnable = true;
                    transEnable = true;
                    ShowMsg("连接成功!");
     
                    //开启新的线程，不停的接收服务器发来的消息
                    c_thread = new Thread(Received);
                    c_thread.IsBackground = true;
                    c_thread.Start();

                    task_thread = new Thread(tryGetTask);
                    task_thread.IsBackground = true;
                    task_thread.Start();

                    btn_disconnect.Enabled = true;
                    btn_connect.Enabled = false;
                    //timer1.Enabled = true;
                }
                
            }
            catch (Exception erroMsg)
            {
                ShowMsg("连接失败：" + erroMsg.Message.ToString());
                btn_disconnect.Enabled = false;
                btn_connect.Enabled = true;
               
            }
        }
        public void ShowMsg(string str)
         {
             textBox1.AppendText(str + "\r\n");
         }
        public void Send(String sendMSG)
        {
            try
             {
                 if (sendMSG != null && !sendMSG.Trim().Equals("")) 
                 {
                     byte[] buffer = new byte[1024 * 1024 * 3];

                     //buffer = Encoding.UTF8.GetBytes(sendMSG);
                     String cmd = sendMSG.Substring(0,4);
                     String strx = sendMSG.Substring(4,2);
                     String stry = sendMSG.Substring(6,2);
                     int x = Convert.ToInt16(strx);
                     int y = Convert.ToInt16(stry);
                     String str_x = x.ToString("X2");
                     String str_y = y.ToString("X2");
                     String strcmd = cmd + str_x + str_y;
                     buffer = strToToHexByte(strcmd);
                     socketSend.Send(buffer);
                     ShowMsg("发送：" + sendMSG);
                 }
                 
             }
             catch(Exception erro) 
             {
                ShowMsg("发送失败：" + sendMSG+"  原因："+erro.Message.ToString());
             }
        }
         /// <summary>
         /// 接收服务端返回的消息
         /// </summary>
         public void Received()
         {
             while (connectEnable)
             {
                 Thread.Sleep(100);
                 try
                 {
                     byte[] buffer = new byte[1024 * 1024 * 3];
                     //实际接收到的有效字节数
                     int len = socketSend.Receive(buffer);
                     if (len == 0)
                     {
                         break;
                     }
                     //string str = Encoding.UTF8.GetString(buffer, 0, len);
                     string str = byteToHexStr(buffer,0,len);
                     if (str != null && (str.Length==10) && str.Substring(str.Length - 2, 2).Equals("01")) 
                     {
                         String cmd = str.Substring(0,4);
                         String strx = str.Substring(4, 2);
                         String stry = str.Substring(6, 2);
                         int x = Int32.Parse(strx, System.Globalization.NumberStyles.HexNumber);
                         int y = Int32.Parse(stry, System.Globalization.NumberStyles.HexNumber);
                         String x2 = x.ToString();
                         String y2 = y.ToString();
                         if (x < 10)
                             x2 = "0"+x2;
                         if (y < 10)
                             y2 = "0" + y2;


                         String strmsg = cmd + x2 + y2;
                         //ShowMsg("反馈mes:" + strmsg);

                         Thread.Sleep(100);
                         bool updateResult = updateTask(strmsg);
                         if (updateResult)
                         {
                             ShowMsg("更新MES配送任务成功！");
                             transEnable = true;
                         }
                         else //如果更新失败则再重复请求3次
                         {
                             transEnable = false;
                             ShowMsg("更新MES配送任务失败！请检查网络连接是否正常。");
                             /*for (int i = 0; i < 3; i++) 
                             {
                                 if (!transEnable)
                                 {
                                     Thread.Sleep(1000);
                                     if (updateTask(strmsg))
                                     {
                                         transEnable = true;
                                         ShowMsg("更新MES配送任务成功！");
                                     }
                                     else
                                         ShowMsg("更新MES配送任务失败！请检查网络连接是否正常。");
                                 }
                                 
                             }*/
                             /*while (!transEnable)
                             {
                                 Thread.Sleep(5000);
                                 if (updateTask(strmsg))
                                 {
                                     transEnable = true;
                                     ShowMsg("更新MES配送任务成功！");
                                 }
                                 else
                                     ShowMsg("更新MES配送任务失败！请检查网络连接是否正常。");
                             }*/
                             
                                 
                         }
                     }
                     ShowMsg(socketSend.RemoteEndPoint + ":" + str);
                 }
                 catch(Exception erro) 
                 {
                     ShowMsg( "接收消息出错:" + erro.Message.ToString());
                 }
             }
         }

         private void tryGetTask() {
             bool flag = true;
             while(true)
             {
                 if (transEnable && flag)
                 {
                     flag = false;
                     String strTask = getTask();
                     if (strTask != null && !strTask.Equals("false"))
                     {
                         currentTask = strTask;

                         Send(strTask);
                         transEnable = false;
                     }
                     flag = true;

                 }
                 Thread.Sleep(1000);
              }
             
         }

         private void btn_connect_Click(object sender, EventArgs e)
         {
             connectServer();
             
             
         }

         private void btn_disconnect_Click(object sender, EventArgs e)
         {
             if (socketSend != null) 
             {
                 socketSend.Disconnect(true);
                 connectEnable = false;
                 transEnable = false;
                 ShowMsg("连接断开!");
                 btn_connect.Enabled = true;
                 btn_disconnect.Enabled = false;
         

                 c_thread.Join();
                 
             }
         }
         public String getTask() 
         {
             String new_task = "";
             upsWebAPIs upsapi = new upsWebAPIs();
             new_task = upsapi.getTask(comboBox1.Text.ToString());
             return new_task;
         }
         public bool updateTask(String msg)
         {
             bool result = false;
             if (msg != null && msg.Trim() != "")
             {
                 upsWebAPIs upsapi = new upsWebAPIs();
                 String str = upsapi.updateTask(msg, comboBox1.Text.ToString());
                 if (str.Trim().Equals("true"))
                     result = true;
             }
             return result;
         }

         private void checkBox1_CheckedChanged(object sender, EventArgs e)
         {
             if (checkBox1.Checked)
             {
                 txt_ip.Enabled = false;
                 txt_port.Enabled = false;
                 comboBox1.Enabled = false;
             }
             else 
             {
                 txt_ip.Enabled = true;
                 txt_port.Enabled = true;
                 comboBox1.Enabled = true;
             }
         }

         private void button1_Click(object sender, EventArgs e)
         {
             textBox1.Text = "";
             //Send("01012243");
             //String cmd = getTask();
             //bool cmd = updateTask("01020101");
             //String task = getTask();
             //textBox1.Text = task;

         }

         private void label4_Click(object sender, EventArgs e)
         {

         }

         private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
         {
             Properties.Settings.Default.storageNO = comboBox1.Text.ToString();
             Properties.Settings.Default.Save();
         }

         private void txt_ip_TextChanged(object sender, EventArgs e)
         {
             Properties.Settings.Default.serverIP = txt_ip.Text.ToString();
             Properties.Settings.Default.Save();
         }

         private void txt_port_TextChanged(object sender, EventArgs e)
         {
             Properties.Settings.Default.port = txt_port.Text.ToString();
             Properties.Settings.Default.Save();
         }
         /// <summary> 
         /// 字符串转16进制字节数组 
         /// </summary> 
         /// <param name="hexString"></param> 
         /// <returns></returns> 
         private static byte[] strToToHexByte(string hexString)
         {
             hexString = hexString.Replace(" ", "");
             if ((hexString.Length % 2) != 0)
                 hexString += " ";
             byte[] returnBytes = new byte[hexString.Length / 2];
             for (int i = 0; i < returnBytes.Length; i++)
                 returnBytes[i] = Convert.ToByte(hexString.Substring(i * 2, 2), 16);
             return returnBytes;
         }

         /// <summary> 
         /// 字节数组转16进制字符串 
         /// </summary> 
         /// <param name="bytes"></param> 
         /// <returns></returns> 
         public static string byteToHexStr(byte[] bytes,int index,int len)
         {
             string returnStr = "";
             if (bytes != null)
             {
                 for (int i = index; i < len; i++)
                 {
                     returnStr += bytes[i].ToString("X2");
                 }
             }
             return returnStr;
         }
        
    }
}
