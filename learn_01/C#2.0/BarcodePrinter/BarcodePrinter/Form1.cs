using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using MySql.Data;
using MySql.Data.MySqlClient;


namespace BarcodePrinter
{
    public partial class Form1 : Form
    {
        bool flag = true;
        databaseSQL db2;
        LabelManager2.Application labelapp=null;
        Thread task_thread;
        public Form1()
        {
            InitializeComponent();

        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        private void button_start_Click(object sender, EventArgs e)
        {
            flag = true;
            this.db2 = new databaseSQL();
            this.db2.SSHConnectMySql();
            this.labelapp = new LabelManager2.Application(); //创建lppa.exe进程 
            textBox_log.Text = "创建lppa.exe进程\r\n";
            if (!textBox_path.Text.Trim().Equals(""))
            {
                //timer1.Enabled = true;
                task_thread = new Thread(get_task);
                task_thread.IsBackground = true;
                task_thread.Start();

                button_start.Enabled = false;
                button_selfile.Enabled = false;
                button_stop.Enabled = true;
            }
            else
            {
                MessageBox.Show("请指定一个打印CS模板文件！");
                this.textBox_path.Focus();
                return;
            }
        }
        private void get_task() 
        {
            
            while (true)
            {
                if (flag)
                {
                    flag = false;
                    String selstr = "select distinct tag_index from list_barcodeprint where print_status=0 and processpoint = '" + comboBox1.Text + "'";

                    DataTable reader = this.db2.Select(selstr);
                    if (reader != null)
                    {
                        if (reader.Rows.Count > 0)
                        {
                            Printbarcode(this.labelapp);

                        }
                    }
                    flag = true;
                    
                }
                Thread.Sleep(1000);
            }
        }
        private void timer1_Tick(object sender, EventArgs e)
        {
            Thread.Sleep(100);
            if (flag)
            {
                flag = false;
                
                String selstr = "select distinct tag_index from list_barcodeprint where print_status=0 and processpoint = '" + comboBox1.Text + "'";

                DataTable reader = this.db2.Select(selstr);
                if (reader != null)
                {
                    if (reader.Rows.Count > 0)
                    {
                        Printbarcode(this.labelapp);
                        
                    }
                    flag = true;

                }
                //db2.CloseDB();//退出
            }
            
        }
        public void Printbarcode(LabelManager2.Application labelapp)
        {

            try
            {

                //db.SSHConnectMySql();
                String selstr = "select distinct tag_index from list_barcodeprint where print_status=0 and processpoint = '" + comboBox1.Text + "'";

                DataTable reader = this.db2.Select(selstr);
                String itemstr = "";

                if (reader != null)
                {
                    
                    for (int i = 0; i < reader.Rows.Count; i++)
                    {

                        string strPath = textBox_path.Text.Trim();
                        this.labelapp.Documents.Open(strPath, false);
                        LabelManager2.Document labeldoc = labelapp.ActiveDocument;
                        int vcount = labeldoc.Variables.FormVariables.Count;

                        textBox_log.Text += "新建打印文档对象\r\n";
                        String selsql1 = "select barcode,varible_index from list_barcodeprint where print_status=0 and tag_index=" + reader.Rows[i][0] + "  and processpoint = '" + comboBox1.Text + "' order by varible_index";
                        DataTable reader2 = this.db2.Select(selsql1);
                        if (reader2 != null && (reader2.Rows.Count == vcount))
                        {
                            for (int j = 0; j < reader2.Rows.Count; j++)
                            {
                                itemstr = "var_" + reader2.Rows[j][1].ToString();
                                labeldoc.Variables.FormVariables.Item(itemstr).Value = reader2.Rows[j][0].ToString();
                            }
   
                            textBox_log.Text += "文档开始打印:标签" + (i + 1).ToString()+"\r\n";
                            labeldoc.PrintDocument(); //打印一次
                            textBox_log.Text += "文档结束打印:标签" + (i + 1).ToString() + "\r\n";
                            labeldoc.FormFeed(); //结束打印

                            String updatesql1 = "update list_barcodeprint set print_status=1 where print_status=0 and tag_index=" + reader.Rows[i][0] + "  and processpoint = '" + comboBox1.Text + "'";
                            this.db2.Update(updatesql1);
                        }
                        textBox_log.Text += "文档关闭\r\n";
                        labeldoc.Close(true);

                        
                    }
                   
                }


            }
            catch (Exception exceptions)
            {
                MessageBox.Show("标签打印出错！请联系设备技术员处理。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            finally
            {
                labelapp.Documents.CloseAll();
            }
        }
        private void button_stop_Click(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            flag = false;
            button_start.Enabled = true;
            button_selfile.Enabled = true;
            button_stop.Enabled = false;
            if(this.db2!=null)
            this.db2.CloseDB();
            if(this.labelapp!=null)
            this.labelapp.Quit();
            textBox_log.Text += "结束lppa.exe进程\r\n";
        }

        private void button_selfile_Click(object sender, EventArgs e)
        {
            OpenFileDialog openfile = new OpenFileDialog();
            openfile.InitialDirectory = Application.StartupPath;
            openfile.Filter = "All files(*.*)|*.*";
            openfile.RestoreDirectory = true;
            if (openfile.ShowDialog() == DialogResult.OK)
            {
                textBox_path.Text = openfile.FileName.ToString();
            }
        }

        private void Form1_FormClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
        {
            if(db2!=null)
            db2.CloseDB();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            textBox_path.Text = Application.StartupPath+Properties.Settings.Default["templatename"].ToString();
            comboBox1.Text = Properties.Settings.Default["processpoint"].ToString();
        }


        
    }
}
