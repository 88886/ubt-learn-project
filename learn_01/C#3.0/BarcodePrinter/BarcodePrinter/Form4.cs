using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Threading; 

namespace BarcodePrinter
{
    public partial class Form4 : Form
    {
        public Form4()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            TcpClient tcpClient = new TcpClient();
            //tcpClient.Connect(IPAddress.Parse("170.0.0.78"), 2014);  
            int port1 = 2014;
            if(textport1.Text.Trim()!="")
                port1 = Convert.ToInt32(textport1.Text.Trim());
            tcpClient.Connect(IPAddress.Parse("127.0.0.1"), port1);

            NetworkStream ntwStream = tcpClient.GetStream();
            if (ntwStream.CanWrite)
            {
                Byte[] bytSend = Encoding.UTF8.GetBytes(txtSendMssg.Text);
                ntwStream.Write(bytSend, 0, bytSend.Length);
            }
            else
            {
                MessageBox.Show("无法写入数据流");

                ntwStream.Close();
                tcpClient.Close();

                return;
            }

            ntwStream.Close();
            tcpClient.Close();  
        }

        private void Form4_Load(object sender, EventArgs e)
        {
            //初始化控件  
            txtSendMssg.Text = "测试数据";

            //打开Listener开始监听  
            Thread thrListener = new Thread(new ThreadStart(Listen));
            thrListener.Start();

            //打开Listener开始监听  
            Thread thrListener2 = new Thread(new ThreadStart(Listen2));
            thrListener2.Start();

            //打开Listener开始监听  
            Thread thrListener3 = new Thread(new ThreadStart(Listen3));
            thrListener3.Start();
        }

        private void Form4_FormClosing(object sender, FormClosingEventArgs e)
        {
            //强制关闭程序（强行终止Listener）  
            Environment.Exit(0);  
        }
        //监听数据  
        private void Listen()
        {
            Socket listener = new Socket(AddressFamily.InterNetwork,
                SocketType.Stream, ProtocolType.Tcp);
            int port1 = 2015;
            if (resvrport1.Text.Trim() != "")
                port1 = Convert.ToInt32(resvrport1.Text.Trim());
            listener.Bind(new IPEndPoint(IPAddress.Any, port1));

            //不断监听端口  
            while (true)
            {
                listener.Listen(0);
                Socket socket = listener.Accept();
                NetworkStream ntwStream = new NetworkStream(socket);
                StreamReader strmReader = new StreamReader(ntwStream);
                Invoke(new PrintRecvMssgDelegate(PrintRecvMssg),
                    new object[] { strmReader.ReadToEnd() });
                socket.Close();
            }

            //程序的listener一直不关闭  
            //listener.Close();  
        }

        //线程内向文本框txtRecvMssg中添加字符串及委托  
        private delegate void PrintRecvMssgDelegate(string s);
        private void PrintRecvMssg(string info)
        {
            txtRecvMssg.Text += string.Format("[{0}]:{1}\r\n",
                DateTime.Now.ToLongTimeString(), info);
        }

        private void Listen2()
        {
            Socket listener2 = new Socket(AddressFamily.InterNetwork,
                SocketType.Stream, ProtocolType.Tcp);
            int port2 = 2015;
            if (resvrport2.Text.Trim() != "")
                port2 = Convert.ToInt32(resvrport2.Text.Trim());
            listener2.Bind(new IPEndPoint(IPAddress.Any, port2));

            //不断监听端口  
            while (true)
            {
                listener2.Listen(0);
                Socket socket = listener2.Accept();
                NetworkStream ntwStream = new NetworkStream(socket);
                StreamReader strmReader = new StreamReader(ntwStream);
                string result = strmReader.ReadToEnd();
                Invoke(new PrintRecvMssgDelegate(PrintRecvMssg2),
                    new object[] { result });
                
                if (result == "706")
                    socket.Send(Encoding.ASCII.GetBytes("welcome to server"));
                socket.Close();
            }

            //程序的listener一直不关闭  
            //listener.Close();  
        }

        private void PrintRecvMssg2(string info)
        {
            txtRecvMssg2.Text += string.Format("[{0}]:{1}\r\n",
                DateTime.Now.ToLongTimeString(), info);
        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            TcpClient tcpClient = new TcpClient();
            //tcpClient.Connect(IPAddress.Parse("170.0.0.78"), 2014);  
            int port2 = 2015;
            if (textport2.Text.Trim() != "")
                port2 = Convert.ToInt32(textport2.Text.Trim());
            tcpClient.Connect(IPAddress.Parse("127.0.0.1"), port2);

            NetworkStream ntwStream = tcpClient.GetStream();
            if (ntwStream.CanWrite)
            {
                Byte[] bytSend = Encoding.UTF8.GetBytes(txtSendMssg.Text);
                ntwStream.Write(bytSend, 0, bytSend.Length);
            }
            else
            {
                MessageBox.Show("无法写入数据流");

                ntwStream.Close();
                tcpClient.Close();

                return;
            }

            ntwStream.Close();
            tcpClient.Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {

        }

        public static void SendMessage()
        {
            Socket socket = serverSocket.Accept();
            Console.WriteLine("Connected a client:{0}", socket.RemoteEndPoint);
            socket.Send(Encoding.ASCII.GetBytes("welcome to server"));
            //Thread thread = new Thread(ReceiveMessage);
            // thread.Start();
        }
        public static void ReceiveMessage(object obj)
        {
            Socket socket = (Socket)obj;
            byte[] data = new byte[1024];
            int len = socket.Receive(data);
            string dataString = Encoding.ASCII.GetString(data, 0, len);
            Console.WriteLine("Receive Data:{0} from {1}", dataString, socket.RemoteEndPoint);
            //Thread thread = new Thread(SendMessage);
            //thread.Start(socket);
        }
        static Socket serverSocket;
        public void Listen3()
        {
            //定义接收数据长度变量
            int recv;
            //定义接收数据的缓存
            byte[] data = new byte[1024];
            //定义侦听端口
            IPEndPoint ipEnd = new IPEndPoint(IPAddress.Any, 5566);
            //定义套接字类型
            serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            //连接
            serverSocket.Bind(ipEnd);
            //开始侦听
            serverSocket.Listen(10);
            //控制台输出侦听状态
            Console.Write("Waiting for a client");
            //Socket client;
            while (true)
            {
                //一旦接受连接，创建一个客户端
                Socket client = serverSocket.Accept();
                //获取客户端的IP和端口
                IPEndPoint ipEndClient = (IPEndPoint)client.RemoteEndPoint;
                //输出客户端的IP和端口
                Console.WriteLine("Connect with {0} at port {1}", ipEndClient.Address, ipEndClient.Port);
                //定义待发送字符
                string welcome = "Welcome to my server";
                //数据类型转换
                data = Encoding.ASCII.GetBytes(welcome);
                while (true)
                {

                    try
                    {
                        //发送
                        client.Send(data, data.Length, SocketFlags.None);
                        //接收数据可以用线程也可以不用
                        //ReceiveMessage(client);
                        Thread thread = new Thread(ReceiveMessage);
                        thread.Start(client);
                        ////对data清零
                        //data = new byte[1024];
                        ////获取收到的数据的长度
                        //recv = client.Receive(data);
                        ////如果收到的数据长度为0，则退出
                        //if (recv == 0)
                        //    break;
                        ////输出接收到的数据
                        //Console.WriteLine(Encoding.ASCII.GetString(data, 0, recv));
                        //将接收到的数据再发送出去
                        // client.Send(data, recv, SocketFlags.None);
                    }
                    catch (Exception)
                    {
                        client.Close();
                        serverSocket.Close();
                    }
                }
                client.Close();
            }
            //Console.WriteLine("Disconnect form{0}", ipEndClient.Address);

            serverSocket.Close();

        }


    }
}
