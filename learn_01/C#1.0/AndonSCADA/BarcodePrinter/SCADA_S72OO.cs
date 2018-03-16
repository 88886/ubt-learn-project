using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MySql.Data;
using MySql.Data.MySqlClient;


namespace AndonSCADA
{
    public partial class SCADA_S72OO : Form
    {
        PC_Access OPC_S7;
        databaseSQL db2;
        public SCADA_S72OO()
        {
            InitializeComponent();
        }

        private void SCADA_S72OO_Load(object sender, EventArgs e)
        {
            OPC_S7 = new PC_Access();
            
            
        }

        private void button_start_Click(object sender, EventArgs e)
        {
            OPC_S7.connectServer();
            //OPC_S7.writeSyn();
            this.db2 = new databaseSQL();
            this.db2.SSHConnectMySql();
        }

        private void button_stop_Click(object sender, EventArgs e)
        {
            OPC_S7.disconnectServer();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (OPC_S7 != null)
            {
                int stationsCount = Convert.ToInt32(comboBox1.Text);
                OPC_S7.ArryAdress = new String[stationsCount];
                OPC_S7.ItemDefs = new OPC.Data.OPCItemDef[stationsCount];
                OPC_S7.rItm = new OPC.Data.OPCItemResult[stationsCount];
                for (int i = 0; i < stationsCount; i++)
                {
                    OPC_S7.ArryAdress[i] = "VB" + i.ToString();
                }
                OPC_S7.addItemToGrp();
            }
        }

        private void button_onceread_Click(object sender, EventArgs e)
        {
            if (OPC_S7 != null) 
            {
                int length = OPC_S7.ArryAdress.Length;
                int[] itemhandel = new int[length];
                for (int i = 0; i < length; i++)
                {
                    itemhandel[i] = OPC_S7.rItm[i].HandleServer;
                }
                String[] resultstr = new String[length];
                OPC_S7.readSyn(itemhandel,length,out resultstr);

                if (resultstr != null)
                { 
                    foreach(String item in resultstr)
                    {
                        textBox1.Text = item + textBox1.Text;
                    }
                }
            }
        }
    }
}
