using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using upsAPIs;

namespace LaserPrinter
{
    public partial class ElectronicDataColector : Form
    {
        private SerialPort ComDevice = new SerialPort();

        public ElectronicDataColector()
        {
            InitializeComponent();
        }

        public void init()
        {
            btnSend.Enabled = false;
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
        private void button4_Click(object sender, EventArgs e)
        {
            byte[] sendData = null;

            if (rbtnSendASCII.Checked)
            {
                sendData = strToHexByte(txtSendData.Text.Trim());
            }
            else if (rbtnSendASCII.Checked)
            {
                sendData = Encoding.ASCII.GetBytes(txtSendData.Text.Trim());
            }
            else if (rbtnSendUTF8.Checked)
            {
                sendData = Encoding.UTF8.GetBytes(txtSendData.Text.Trim());
            }
            else if (rbtnUnicode.Checked)
            {
                sendData = Encoding.Unicode.GetBytes(txtSendData.Text.Trim());
            }
            else
            {
                sendData = Encoding.ASCII.GetBytes(txtSendData.Text.Trim());
            }

            
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
            txtSendData.Clear();
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
                txtShowData.Text=content;
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
                    btnSend.Enabled = true;
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
                    btnSend.Enabled = false;
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
            string weight=getWeight();
            lblWeight.Text = weight;
        }

        private String getWeight() 
        {
            String currentContent = txtShowData.Text.Trim();
            if (!currentContent.Equals(""))
            {
                String[] arry = currentContent.Split('W');
                if (arry.Length > 0)
                {
                    String a = arry[1];
                    String[] b = a.Split('+');
                    if (b.Length > 0)
                    {
                        String c = b[1].Trim();
                        if(c.Length>0)
                            return c.Substring(0,c.Length-1);
                    }
                }
            }
            return "0";
        }

        private void button1_Click(object sender, EventArgs e)
        {
            String weight = getWeight();
            txtWeight.Text = weight;
            String sncode = txt_code.Text.Trim();
            updateToMes(sncode, weight);

        }
        private void updateToMes(String code,String weight)
        {
            if (!code.Trim().Equals("") && !weight.Trim().Equals(""))
            {
                upsWebAPIs UPS = new upsWebAPIs();
                String result = UPS.bindWeightToCode(code, weight);

                if (result == "false")
                    MessageBox.Show("提交mes失败！");
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
                String weight = getWeight();
                txtWeight.Text = weight;
                String sncode = txt_code.Text.Trim();
                updateToMes(sncode, weight);
            }
        }
    }
}
