using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using OPC.Common;
using OPC.Data.Interface;
using OPC.Data;
using System.Runtime.InteropServices;

namespace AndonSCADA
{
    class PC_Access
    {
        public String ServerProgID = "OPC.SimaticNet";//OPC服务器ID
        public String ItemConfig = "2:192.168.2.1:0202:0201";//监控PLC IP地址
        public String[] ArryAdress;//监控PLC地址数组
        public String dataType = "BYTE";//数据类型
        public String ReadWriteType = "R";//R只读，W只写，RW读写

        private OpcServer theSrv;
        private OpcGroup TheGrp;
        public OPCItemDef[] ItemDefs;
        public int[] HandlesSrv;
        public OPCItemResult[] rItm;

        public PC_Access() 
        { 

        }
        public bool connectServer() 
        {
            try 
            {
                theSrv = new OpcServer();
                theSrv.Connect(ServerProgID);

                TheGrp = theSrv.AddGroup("S7_200_01",false,900);
                TheGrp.SetEnable(true);
                TheGrp.Active = true;
                return true;
            }
            catch(Exception e)
            {
                System.Windows.Forms.MessageBox.Show("连接服务器出错:"+e.ToString());
                return false;
            }
            
        }
        public void disconnectServer()
        {
            try 
            {
                if (theSrv != null)
                {
                    theSrv.Disconnect();
                }

            }
            catch(Exception e)
            {
                System.Windows.Forms.MessageBox.Show("断开服务器连接出错："+e.ToString());
            }
        }
        public void addItemToGrp()
        {
            try 
            {
                if (TheGrp != null)
                {
                    //String[] Items = new String[30];
                    for(int i=0;i<ArryAdress.Length;i++)
                    {
                        if (ArryAdress[i] != "")
                        {
                            String stritem = ItemConfig + "," + ArryAdress[i] + "," + dataType + "," + ReadWriteType;
                            ItemDefs[i] = new OPCItemDef(stritem,true,i+1,System.Runtime.InteropServices.VarEnum.VT_EMPTY);
                        }
                       
                    }
                    TheGrp.AddItems(ItemDefs,out rItm);
                }
            }
            catch(Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.ToString());
            }
        }
        public int[] writeSyn(int[] arryHandleServer,Object[] arryVal) 
        {
            int[] erro=new int[1];
            try
            {
                TheGrp.SyncWrite(arryHandleServer, arryVal,out erro);
                
            }
            catch(Exception e){
                System.Windows.Forms.MessageBox.Show(e.ToString());
            }
            if (erro == null)
                erro[0] = 0;
            return erro;

        }
        public void readSyn(int[] arryHandleServer,int length, out String[] arryResult)
        {
            OPCItemState[] arrystate = new OPCItemState[length];
            arryResult = new String[length];
            try 
            {
                if (arryHandleServer != null) 
                {
                    TheGrp.SyncRead(OPCDATASOURCE.OPC_DS_DEVICE, arryHandleServer, out arrystate);
                }
                if (arrystate != null)
                {
                    for (int i = 0; i < arrystate.Length; i++)
                    {
                        if (arrystate[i].Quality == 192)
                            arryResult[i] = arrystate[i].DataValue.ToString();
                        else
                            arryResult[i] = arrystate[i].Quality.ToString();
                    }
                }
                
            }
            catch(Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.ToString());
            }
        }
        public int[] writeAsyn(int[] arryHandleServer, Object[] arryVal)
        {
            int[] erro = new int[1];
            int CancelID;
            try
            {
                TheGrp.AsyncWrite(arryHandleServer, arryVal, 55667788, out CancelID, out erro);

            }
            catch (Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.ToString());
            }
            if (erro == null)
                erro[0] = 0;
            return erro;

        }
        public void readAsyn(int[] arryHandleServer, out String[] arryResult)
        {
            int[] ae;
            arryResult = new String[30];
            int CancelID;
            try
            {
                if (arryHandleServer != null)
                {
                    TheGrp.AsyncRead(arryHandleServer, 5567788,out CancelID, out ae);
                }

            }
            catch (Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.ToString());
            }
        }

    }
}
