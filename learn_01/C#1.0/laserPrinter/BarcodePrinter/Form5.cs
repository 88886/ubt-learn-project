using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using upsAPIs;

namespace LaserPrinter
{
    public partial class Form5 : Form
    {
        private static byte[] result = new byte[1024];
        private static int myProt = 2015;   //端口  
        static Socket serverSocket; 
        static string message = "";
        public Form5()
        {
            InitializeComponent();
        }

        private void Form5_Load(object sender, EventArgs e)
        {
            //服务器IP地址  
            IPAddress ip = IPAddress.Parse("127.0.0.1");
            serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            serverSocket.Bind(new IPEndPoint(ip, myProt));  //绑定IP地址：端口  
            serverSocket.Listen(10);    //设定最多10个排队连接请求  
            Console.WriteLine("启动监听{0}成功", serverSocket.LocalEndPoint.ToString());
            //通过Clientsoket发送数据  
            Thread myThread = new Thread(ListenClientConnect);
            myThread.Start();
            Console.ReadLine();
        }
        /// <summary>  
        /// 监听客户端连接  
        /// </summary>  
        private void ListenClientConnect()  
        {  
            while (true)  
            {  
                Socket clientSocket = serverSocket.Accept();  
                //clientSocket.Send(Encoding.ASCII.GetBytes("Server Say Hello"));  
                Thread receiveThread = new Thread(ReceiveMessage);  
                receiveThread.Start(clientSocket);  
            }  
        }  
  
        /// <summary>  
        /// 接收消息  
        /// </summary>  
        /// <param name="clientSocket"></param>  
        private void ReceiveMessage(object clientSocket)  
        {  
            Socket myClientSocket = (Socket)clientSocket;  
            while (true)  
            {  
                try  
                {  
                    //通过clientSocket接收数据  
                    byte[] buffer = new byte[1024 * 1024 * 3];
                    int receiveNumber = myClientSocket.Receive(buffer);
                    if (receiveNumber == 0)
                    {
                        break;
                    }
                    
                    string strMessage = Encoding.ASCII.GetString(buffer, 0, receiveNumber);

                    if (strMessage != null && !strMessage.Equals("")) 
                    {
                        if (strMessage == "706")
                        {
                            upsWebAPIs ups = new upsWebAPIs();
                            string code = ups.getCode("3");
                            //string code = "222112321";
                            code = code.Substring(2, 10) + "\r\n" + code.Substring(12, 10);
                            myClientSocket.Send(Encoding.ASCII.GetBytes(code));

                        }
                        else if (strMessage == "1111")
                        {
                            String currentTask = getTask();
                            myClientSocket.Send(Encoding.ASCII.GetBytes(currentTask));
                        }
                        else
                        {
                            bool updateResult = updateTask(strMessage);
                            if (updateResult)
                            {
                                
                            }
                            else //如果更新失败则再重复请求3次
                            {
                                for (int i = 0; i < 3; i++)
                                {
                                       Thread.Sleep(1000);
                                       if (updateTask(strMessage))
                                           break;
              
                                }

                            }
                        }
                    }
      
                    message =  "接收客户端{0}消息{1}" + myClientSocket.RemoteEndPoint.ToString() + ":" + strMessage;               
                    textRecieve.AppendText(message+"\r\n");
                }  
                catch(Exception ex)  
                {  
                    myClientSocket.Shutdown(SocketShutdown.Both);  
                    myClientSocket.Close();  
                    break;  
                }  
            }  
        }

        private void Form5_FormClosing(object sender, FormClosingEventArgs e)
        {
            serverSocket.Close();
        }
        private string getCode() 
        {
            
            return "";
        }

        private void textRecieve_TextChanged(object sender, EventArgs e)
        {

        }
        public String getTask()
        {
            String new_task = "";
            upsWebAPIs upsapi = new upsWebAPIs();
            new_task = upsapi.getTask("A");
            return new_task;
        }
        public bool updateTask(String msg)
        {
            bool result = false;
            if (msg != null && msg.Trim() != "")
            {
                upsWebAPIs upsapi = new upsWebAPIs();
                String str = upsapi.updateTask(msg, "A");
                if (str.Trim().Equals("true"))
                    result = true;
            }
            return result;
        }
    }  

    
}
