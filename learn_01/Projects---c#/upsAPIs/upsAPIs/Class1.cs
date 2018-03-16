using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;
using System.Net;
using System.IO;
using System.Runtime.Serialization.Json;
using System.Runtime.Serialization;


namespace upsAPIs
{
    public class upsWebAPIs
    {

        string str_ip="mes.ubtrobot.com";//10.10.1.21
        public upsWebAPIs() { 

        }

        /// <summary>  
        /// 指定Post地址使用Get 方式获取全部字符串  
        /// </summary>  
        /// <param name="url">请求后台地址</param>  
        /// <returns></returns>  
        public static string Post(string url, string dic)
        {
            string username = "admin";
            string password = "apple.1234";
            //注意这里的格式哦，为 "username:password"
            string usernamePassword = username + ":" + password;
            CredentialCache mycache = new CredentialCache();
            
            string result = "";
            HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
            req.Method = "POST";
            //req.ContentType = "application/x-www-form-urlencoded";
            req.ContentType = "application/json";

            mycache.Add(new Uri(url), "Basic", new NetworkCredential(username, password));
            req.Credentials = mycache;
            req.Headers.Add("Authorization", "Basic " + Convert.ToBase64String(new ASCIIEncoding().GetBytes(usernamePassword)));

            StringBuilder builder = new StringBuilder();
            int i = 0;
            foreach (var item in dic)
            {
                if (i > 0)
                    builder.Append("&");
                //builder.AppendFormat("{0}={1}", item.Key, item.Value);
                i++;
            }
            byte[] data = Encoding.UTF8.GetBytes(dic.ToString());
            req.ContentLength = data.Length;
            using (Stream reqStream = req.GetRequestStream())
            {
                reqStream.Write(data, 0, data.Length);
                reqStream.Close();
            }

            HttpWebResponse resp = (HttpWebResponse)req.GetResponse();
            Stream stream = resp.GetResponseStream();
            //获取响应内容  
            using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
            {
                result = reader.ReadToEnd();
            }
            return result;
        }
        /// <summary>  
        /// 指定Post地址使用Get 方式获取全部字符串  
        /// </summary>  
        /// <param name="url">请求后台地址</param>  
        /// <returns></returns>  
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
            req.ContentType = "application/x-www-form-urlencoded";

            mycache.Add(new Uri(url), "Basic", new NetworkCredential(username, password));
            req.Credentials = mycache;
            req.Headers.Add("Authorization", "Basic " + Convert.ToBase64String(new ASCIIEncoding().GetBytes(usernamePassword)));

            HttpWebResponse resp = (HttpWebResponse)req.GetResponse();
            Stream stream = resp.GetResponseStream();
            //获取响应内容  
            using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
            {
                result = reader.ReadToEnd();
            }
            return result;
        }
        /// <summary>
        /// 获取SN号对应的舵机ID号
        /// </summary>
        /// <param name="code"></param>
        /// <returns></returns>
        public string getIDByCode(string[] codes) {
            string url = "http://" + str_ip + "/api/production/getSortIDCode";
            string responsedata ="false";
            try
            {
                codelist clist = new codelist();
                code[] cs = new code[codes.Length];
                for (int i = 0; i < codes.Length; i++)
                {
                    code strcode = new code();
                    strcode.qcode = codes[i];
                    cs[i] = strcode;
                    
                }
                clist.codes = cs;
                string requestdata = getJsonByObject(clist);

                string result = Post(url, requestdata);
                ResponseData rd = new ResponseData();
                Object objrd = getObjectByJson(result, rd);
                ResponseData resdata = (ResponseData) objrd;
                string[] data = resdata.data;
  
                int counts = 0;
                if (data != null)
                    counts = data.Length;
                string strIDs = "";
                for (int i = 0; i < counts; i++)
                {
                    strIDs += data[i] + ";";
                }

                return strIDs;
                
            }
            catch(Exception e) {
                responsedata = e.ToString();
                return responsedata;
            }

            
        
        }
        /// <summary>
        /// 绑定舵机ID和SN号
        /// </summary>
        /// <param name="code"></param>
        /// <param name="id"></param>
        /// <returns></returns>
        public string bindIDToCode(string code,string id)
        {
            string url = "http://" + str_ip + "/api/production/product_sn_sortID/" + code + "/" + id;
            string result = "false";
            try
            {
                result = Get(url);
            }
            catch {
                result = "false";
            }
     

            return result;

        }
        /// <summary>
        /// 绑定PCB板号到SN
        /// </summary>
        /// <param name="code"></param>
        /// <param name="id"></param>
        /// <returns></returns>
        public string bindPCBToCode(string code, string pcb)
        {
            string url = "http://" + str_ip + "/api/production/product_sn_pcb/" + code + "/" + pcb;
            string result = "false";
            try
            {
                result = Get(url);
            }
            catch
            {
                result = "false";
            }


            return result;

        }
        /// <summary>
        /// 绑定weight到SN
        /// </summary>
        /// <param name="code"></param>
        /// <param name="id"></param>
        /// <returns></returns>
        public string bindWeightToCode(string code, string weight)
        {
            string url = "http://" + str_ip + "/api/production/product_sn_weight/" + code + "/" + weight;
            string result = "false";
            try
            {
                result = Get(url);
            }
            catch
            {
                result = "false";
            }


            return result;

        }
        /// <summary>
        /// 获取目标生产线当前产品的SN号
        /// </summary>
        /// <param name="line_id"></param>
        /// <returns></returns>
        public string getCode(string line_id)
        {
            string url = "http://" + str_ip + "/api/production/product_serial_auto_print/" + line_id;
            string result = "false";

            try
            {
                result = Get(url);
            }
            catch
            {
                result = "false";
            }

            return result;

        }
        /// <summary>
        /// 获取设备参数，提交SN作验证
        /// </summary>
        /// <param name="code"></param>
        /// <returns></returns>
        public string getParameterByCode(string code)
        {
            string equipment_id = "1";
            string url = "http://" + str_ip + "/api/equipments/getEquipmentParameters/" + equipment_id + "/" + code;
            string result = "false";
            try
            {
                result = Get(url);
            }
            catch
            {
                result = "false";
            }
     
            return result;

        }
        /// <summary>
        /// 提交当前SN产品的质量数据
        /// </summary>
        /// <param name="code"></param>
        /// <param name="qcode"></param>
        /// <param name="qvalue"></param>
        /// <param name="qresult"></param>
        /// <returns></returns>
        public string commitByCode(string[] code,string[,] qcode,string[,] qvalue,string[,] qresult) {
            Dictionary<String, QuanlityData[]> pList = new Dictionary<String, QuanlityData[]>();
   
            string result="false";
            try
            {
                int arrylength = code.Length;
                if (arrylength > 0)
                {
                    QuanlityData[] qdlist = new QuanlityData[arrylength];
                    for (int i = 0; i < arrylength; i++)
                    {
                        QuanlityData qd = new QuanlityData();
                        int length2 =qcode.Length/arrylength;
                        QParemeters[] qplist = new QParemeters[length2];
                        string strcode = code[i];
                        string strqcode = "", strqvalue = "", strqresult = "";
                        for (int j = 0; j < length2; j++)
                        {
                            strqcode = qcode[i,j];
                            strqvalue = qvalue[i,j];
                            strqresult = qresult[i,j];
                            QParemeters qp = new QParemeters();
                            qp.qcode = strqcode;
                            qp.qresult = strqresult;
                            qp.qvalue = strqvalue;

                            qplist[j] = qp;
                        }

                        qd.code = strcode;
                        qd.qparemeters = qplist;
                        qdlist[i] = qd;

                    }
                    pList.Add("quality_data", qdlist);

                    QuanlityDataList qdarry = new QuanlityDataList();
                    qdarry.quality_data = qdlist;

                    string requestdata = getJsonByObject(qdarry);

                    String url = "http://" + str_ip + "/api/production/commitQualityByCode";
                    result = Post(url, requestdata);
                    ResponseData rd = new ResponseData();
                    Object objrd = getObjectByJson(result, rd);
                    ResponseData resdata = (ResponseData)objrd;
                    string success = resdata.success;

                    return success;
                }
                else {
                    result = "false";
                }

            }
            catch (System.Exception em)
            {
                //Console.WriteLine("Error: {0}", em.Message);
                result = "false";
                result = em.ToString();
            }

            return result;
            
        }
        /// <summary>
        /// 提交工位异常
        /// </summary>
        /// <param name="code"></param>
        /// <param name="qcode"></param>
        /// <param name="qvalue"></param>
        /// <param name="qresult"></param>
        /// <returns></returns>
        public string updateStationExceptions(string stationCodes, string old_exceptionss, string new_exceptionss)
        {
            Dictionary<String, StationExceptionData[]> pList = new Dictionary<String, StationExceptionData[]>();
            string[] stationCode = stationCodes.Split(',');
            string[] old_exceptions = old_exceptionss.Split(',');
            string[] new_exceptions = new_exceptionss.Split(','); 
            string result = "false";
            try
            {
                int arrylength = stationCode.Length-1;
                if (arrylength > 0)
                {
                    StationExceptionData[] qdlist = new StationExceptionData[arrylength];
                    for (int i = 0; i < arrylength; i++)
                    {
                        StationExceptionData qd = new StationExceptionData();
                        string strcode = stationCode[i];
                        string old_exception = old_exceptions[i];
                        string new_exception = new_exceptions[i];
                 

                        qd.stationCode = strcode;
                        qd.old_exception = old_exception;
                        qd.new_exception = new_exception;
                        qdlist[i] = qd;

                    }
                    pList.Add("exceptions", qdlist);

                    StationExceptionDataList qdarry = new StationExceptionDataList();
                    qdarry.StationException_data = qdlist;

                    string requestdata = getJsonByObject(qdarry);
                    //return requestdata;

                    String url = "http://" + str_ip + "/api/andon/update_station_exception";
                    result = Post(url, requestdata);
                    ResponseData rd = new ResponseData();
                    Object objrd = getObjectByJson(result, rd);
                    ResponseData resdata = (ResponseData)objrd;
                    string success = resdata.success;

                    return requestdata;
                }
                else
                {
                    result = "false";
                }

            }
            catch (System.Exception em)
            {
                Console.WriteLine("Error: {0}", em.Message);
                result = em.Message;
            }

            return result;

        }

        /// <summary>
        /// 获取立库的当前配送任务
        /// </summary>
        /// <param name="line_id"></param>
        /// <returns></returns>
        public string getTask(string line_id)
        {
            string url = "http://" + str_ip + "/api/material/materialInstoreCommand/" + line_id;
            string result = "false";
            string result2 = "11";

            try
            {
                result = Get(url);
                result2 = result;
                ResponseData2 rd = new ResponseData2();
                Object objrd = getObjectByJson(result, rd);
                ResponseData2 resdata = (ResponseData2)objrd;
                bool success = resdata.success;
                if (success)
                {
                    result = resdata.data;
                }
                else 
                {
                    result = "false";
                }

            }
            catch
            {
                result = "false";
            }

            return result;

        }
        /// <summary>
        /// 更新立库的配送任务,如01012309
        /// </summary>
        /// <param name="line_id"></param>
        /// <returns></returns>
        public string updateTask(string cmd,string line_id)
        {
            string url = "http://" + str_ip + "/api/material/materialFinishCommand/"+cmd+"/" + line_id;
            string result = "false";

            try
            {
                result = Get(url);
                ResponseData2 rd = new ResponseData2();
                Object objrd = getObjectByJson(result, rd);
                ResponseData2 resdata = (ResponseData2)objrd;
                bool success = resdata.success;
                if (success)
                {
                    result = "true";
                }
                else
                {
                    result = "false";
                }

            }
            catch
            {
                result = "false";
            }

            return result;

        }

        private static string getJsonByObject(Object obj)
       {
            //实例化DataContractJsonSerializer对象，需要待序列化的对象类型
          DataContractJsonSerializer serializer = new DataContractJsonSerializer(obj.GetType());
            //实例化一个内存流，用于存放序列化后的数据
             MemoryStream stream = new MemoryStream();
            //使用WriteObject序列化对象
             serializer.WriteObject(stream, obj);
              //写入内存流中
             byte[] dataBytes = new byte[stream.Length];
             stream.Position = 0;
             stream.Read(dataBytes, 0, (int)stream.Length);
             //通过UTF8格式转换为字符串
             return Encoding.UTF8.GetString(dataBytes);
         }
 
        private static Object getObjectByJson(string jsonString, Object obj)
         {
             //实例化DataContractJsonSerializer对象，需要待序列化的对象类型
             DataContractJsonSerializer serializer = new DataContractJsonSerializer(obj.GetType());
             //把Json传入内存流中保存
             MemoryStream stream = new MemoryStream(Encoding.UTF8.GetBytes(jsonString));
             // 使用ReadObject方法反序列化成对象
             return serializer.ReadObject(stream);
         }
        
        
    }
}
