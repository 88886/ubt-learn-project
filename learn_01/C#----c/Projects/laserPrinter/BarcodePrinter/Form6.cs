using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace LaserPrinter
{
    public partial class Form6 : Form
    {
        public Form6()
        {
            InitializeComponent();
        }
        private static byte[] result = new byte[1024]; 
        private void Form6_Load(object sender, EventArgs e)
        {
           
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //设定服务器IP地址  
            IPAddress ip = IPAddress.Parse("127.0.0.1");
            Socket clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                clientSocket.Connect(new IPEndPoint(ip, 2015)); //配置服务器IP与端口  
     
                textRecieve.Text += "连接服务器成功"+"\r\n";
            }
            catch
            {
          
                textRecieve.Text += "连接服务器失败，请按回车键退出！" + "\r\n";
                return;
            }
            //通过clientSocket接收数据  
            //int receiveLength = clientSocket.Receive(result);
 
            //textRecieve.Text += "接收服务器消息：{0}" + Encoding.ASCII.GetString(result, 0, receiveLength) + "\r\n";
            //通过 clientSocket 发送数据  
            for (int i = 0; i < 2; i++)
            {
                try
                {
                    Thread.Sleep(1000);    //等待1秒钟  
                    string sendMessage = "760";
                    sendMessage = textSendMessage.Text;
                    clientSocket.Send(Encoding.ASCII.GetBytes(sendMessage));

                    textRecieve.Text += "向服务器发送消息：{0}" + sendMessage + "\r\n";

                    int receiveLength2 = clientSocket.Receive(result);
                    textRecieve.Text += "接收服务器消息：{0}" + Encoding.ASCII.GetString(result, 0, receiveLength2) + "\r\n";
                }
                catch
                {
                    clientSocket.Shutdown(SocketShutdown.Both);
                    clientSocket.Close();
                    break;
                }
            }
        
            textRecieve.Text += "发送完毕，按回车键退出" + "\r\n";
   
        }
    }
}
