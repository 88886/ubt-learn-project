using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MySql.Data;
using MySql.Data.MySqlClient;
using Renci.SshNet;
using System.Data;


namespace BarcodePrinter
{
    class databaseSQL
    {
        MySqlConnection conn=null;
        String strcon = "";
        SshClient client=null;

        public void SSHConnectMySql()
        {
            //string SSHHost = "10.10.1.21";
            string SSHHost = "210.75.21.106";// SSH地址
            int SSHPort = 8888;              // SSH端口
            string SSHUser = "root";           // SSH用户名
            string SSHPassword = "Xrllwzz..&%22)";           // SSH密码

            string sqlIPA = "127.0.0.1";// 映射地址  实际上也可以写其它的   Linux上的MySql的my.cnf bind-address 可以设成0.0.0.0 或者不设置
            string sqlHost = "127.0.0.1"; // mysql安装的机器IP 也可以是内网IP 比如：192.168.1.20
            uint sqlport = 3306;        // 数据库端口及映射端口
            string sqlConn = "Database='ums';Data Source=" + sqlIPA + ";Port=" + sqlport + ";User Id='root';Password='Ubt.83474428';CharSet='utf8'";
            //string sqlSELECT = "select * from user";

            PasswordConnectionInfo connectionInfo = new PasswordConnectionInfo(SSHHost, SSHPort, SSHUser, SSHPassword);
            connectionInfo.Timeout = TimeSpan.FromSeconds(30);
            this.client = new SshClient(connectionInfo);
           
                try
                {
                    client.Connect();
                    if (!client.IsConnected)
                    {
                        MessageBox.Show("SSH connect failed");
                    }

                    var portFwdL = new ForwardedPortLocal(sqlIPA, sqlport, sqlHost, sqlport); //映射到本地端口sqlHost,
                    client.AddForwardedPort(portFwdL);
                    portFwdL.Start();
                    if (!client.IsConnected)
                    {
                        MessageBox.Show("port forwarding failed");
                    }

                    this.conn = new MySqlConnection(sqlConn);
                        
                    
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            
        }
        public void ConnetDB()
        {
            try 
            {
                
                this.conn = new  MySql.Data.MySqlClient.MySqlConnection("Database='ums';Data Source='127.0.0.1';User Id='root';Password='Ubt.83474428';charset='utf8'");
                this.conn.Open();
            }
            catch(Exception e){
                MessageBox.Show("数据库连接出错！请联系设备技术员处理。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
        }
        public void Closeconn()
        {
            try
            {
                if(this.conn!=null)
                    this.conn.Close();
                
            }
            catch (Exception e)
            {
                MessageBox.Show("数据库连接出错！请联系设备技术员处理。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        public void CloseDB()
        {
            try
            {
                
                if (this.client != null & this.client.IsConnected)
                    this.client.Disconnect();
            }
            catch (Exception e)
            {
                MessageBox.Show("数据库连接出错！请联系设备技术员处理。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        public DataTable Select(string selectSql)
        {
            //this.SSHConnectMySql();
            
            this.conn.Open();
            MySqlCommand cmd = new MySqlCommand(selectSql, this.conn);
            MySqlDataAdapter sda = new MySqlDataAdapter(cmd);
            DataTable dt = new DataTable();
            sda.Fill(dt);
            //MySqlDataReader reader = cmd.ExecuteReader();

            this.Closeconn();
            return dt;
        }

        public void Insert(string insertSql)
        {
            this.conn.Open();
            MySqlCommand cmd = new MySqlCommand(insertSql, this.conn);
            cmd.ExecuteNonQuery();
            cmd.Dispose();
            this.Closeconn();
        }

        public void Update( string updateSql)
        {
            this.conn.Open();
            MySqlCommand cmd = new MySqlCommand(updateSql, this.conn);
            cmd.ExecuteNonQuery();
            cmd.Dispose();
            this.Closeconn();
        }

        public void Delete(string deleteSql)
        {
            this.conn.Open();
            MySqlCommand cmd = new MySqlCommand(deleteSql, this.conn);
            cmd.ExecuteNonQuery();
            cmd.Dispose();
            this.Closeconn();
        }  
    }
}
