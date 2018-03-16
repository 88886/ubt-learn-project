using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.IO;
using upsAPIs;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string[] codes = { "020147UBT16090900157", "020147UBT16090900158", "020147UBT16090900159" };
            upsWebAPIs ups = new upsWebAPIs();

            object result = ups.getIDByCode(codes);


        }
        public static string Get(string url)
        {
            string username = "admin";
            string password = "apple.1234";
            //注意这里的格式哦，为 "username:password"
            string usernamePassword = username + ":" + password;
            CredentialCache mycache = new CredentialCache();

            string result = "";
            HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
            req.Method = "GET";
            //req.ContentType = "application/json";

            mycache.Add(new Uri(url), "Basic", new NetworkCredential(username, password));
            req.Credentials = mycache;
            req.Headers.Add("Authorization", "Basic " + Convert.ToBase64String(new ASCIIEncoding().GetBytes(usernamePassword)));

            WebResponse resp = req.GetResponse();
            Stream stream = resp.GetResponseStream();
            //获取响应内容  
            using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
            {
                result = reader.ReadToEnd();
            }
            return result;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            string username = "admin";
            string password = "aplle.1234";
            string usernamePassword = username + ":" + password;
            CredentialCache mycache = new CredentialCache();

            string url = " http://123.207.84.235:8080/api/planning/pending_batches/1";

            HttpWebRequest webrequest = (HttpWebRequest)System.Net.HttpWebRequest.Create(url);
            webrequest.Method = "GET";
            webrequest.Timeout = 20000;

            mycache.Add(new Uri(url), "Basic", new NetworkCredential(username, password));
            webrequest.Credentials = mycache;
            webrequest.Headers.Add("Authorization", "Basic " + Convert.ToBase64String(new ASCIIEncoding().GetBytes(usernamePassword)));

            try
            {
                WebResponse wr = webrequest.GetResponse();
                Stream receiveStream = wr.GetResponseStream();
                StreamReader reader = new StreamReader(receiveStream, Encoding.UTF8);
                String content = reader.ReadToEnd();//返回值
            }
            catch (Exception exp)
            {
                exp.ToString();
            }

        }

        private void button2_Click(object sender, EventArgs e)
        {
            upsWebAPIs ups = new upsWebAPIs();
            string[] code = { "020147UBT16090900158" };
            string[,] qcode = { { "servo-I-R" } }; 
            string[,] value = {{ "123" }};
            string[,] result = {{ "123" }};
            string result22=ups.commitByCode(code, qcode, value, result);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            try
            {
                
                string starttime2 = DateTime.Now.ToString();
                textBox2.Text = starttime2;
                upsWebAPIs up = new upsWebAPIs();
                string[] codes = { "020147UBT16090900225", "020147UBT16090900226", "020147UBT16090900227", "020147UBT16090900228" };
                string result = up.getIDByCode(codes);
                textBox1.Text = result;
                string endtime = DateTime.Now.ToString();
                textBox3.Text = endtime;
            }
            catch(Exception es) 
            {
                string erro = es.ToString();
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            upsWebAPIs up = new upsWebAPIs();
            string[] codes = { "020147UBT16090900225", "020147UBT16090900226", "020147UBT16090900227", "020147UBT16090900228" };
            string qcode = { "020147UBT16090900225", "020147UBT16090900226", "020147UBT16090900227", "020147UBT16090900228" };
        }
    }
}
