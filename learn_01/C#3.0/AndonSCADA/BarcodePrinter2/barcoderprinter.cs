using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MySql.Data;
using MySql.Data.MySqlClient;
using System.Data;
using System.Threading;
using System.Configuration;
using System.Windows.Forms;

namespace BarcodePrinter2
{
    class BarcodePrinter
    {
        bool flag = true;
        databaseSQL db2;
        LabelManager2.Application labelapp = null;
        static string templatepath = "";
        static string processpoint = "";

        ~BarcodePrinter() 
        {
            stopprint();

        }
        public void stopprint() 
        {
            if (this.db2 != null)
                this.db2.CloseDB();
            if (this.labelapp != null)
                this.labelapp.Quit();
        }
        public void startprint()
        {
            Configuration config = System.Configuration.ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
            //根据Key读取<add>元素的Value
            templatepath = config.AppSettings.Settings["TemplatePATH"].Value;
            processpoint = config.AppSettings.Settings["processpoint"].Value;

            this.db2 = new databaseSQL();
            this.db2.SSHConnectMySql();
            this.labelapp = new LabelManager2.Application(); //创建lppa.exe进程 

            System.Timers.Timer t = new System.Timers.Timer(100);
            t.Elapsed += new System.Timers.ElapsedEventHandler(theout);
            t.AutoReset = true;   //设置是执行一次（false）还是一直执行(true)；   
            t.Enabled = true;     //是否执行System.Timers.Timer.Elapsed事件；   
        }
        public void theout(object source, System.Timers.ElapsedEventArgs e)
        {

            Thread.Sleep(100);
            if (flag)
            {
                flag = false;
                String selstr = "select distinct tag_index from list_barcodeprint where print_status=0 and processpoint = '" + processpoint + "'";

                DataTable reader = this.db2.Select(selstr);
                if (reader != null)
                {
                    if (reader.Rows.Count > 0)
                    {
                        Printbarcode(this.labelapp);

                    }
                    flag = true;

                }

            } 
        }
        public void Printbarcode(LabelManager2.Application labelapp)
        {

            try
            {

                //db.SSHConnectMySql();
                String selstr = "select distinct tag_index from list_barcodeprint where print_status=0 and processpoint = '" + processpoint + "'";

                DataTable reader = this.db2.Select(selstr);
                String itemstr = "";

                if (reader != null)
                {

                    for (int i = 0; i < reader.Rows.Count; i++)
                    {

                        string strPath = templatepath;
                        this.labelapp.Documents.Open(strPath, false);
                        LabelManager2.Document labeldoc = labelapp.ActiveDocument;
                        int vcount = labeldoc.Variables.FormVariables.Count;

                        String selsql1 = "select barcode,varible_index from list_barcodeprint where print_status=0 and tag_index=" + reader.Rows[i][0] + "  and processpoint = '" + processpoint + "' order by varible_index";
                        DataTable reader2 = this.db2.Select(selsql1);
                        if (reader2 != null && (reader2.Rows.Count == vcount))
                        {
                            for (int j = 0; j < reader2.Rows.Count; j++)
                            {
                                itemstr = "var_" + reader2.Rows[j][1].ToString();
                                labeldoc.Variables.FormVariables.Item(itemstr).Value = reader2.Rows[j][0].ToString();
                            }

                            labeldoc.PrintDocument(); //打印一次
                            labeldoc.FormFeed(); //结束打印

                            String updatesql1 = "update list_barcodeprint set print_status=1 where print_status=0 and tag_index=" + reader.Rows[i][0] + "  and processpoint = '" + processpoint + "'";
                            this.db2.Update(updatesql1);
                        }
                        //textBox_log.Text += "文档关闭\r\n";
                        labeldoc.Close(true);


                    }

                }


            }
            catch (Exception exceptions)
            {
                MessageBox.Show("打印标签出错，请联系设备管理员处理！");
                return;
            }
            finally
            {
                labelapp.Documents.CloseAll();
            }
        }
    
    }
}
