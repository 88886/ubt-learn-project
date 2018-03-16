using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace upsAPIs
{
    public class QuanlityData
    {
        public string code;
        public QParemeters[] qparemeters;

    }
    public class StationExceptionData
    {
        public string stationCode;
        public string old_exception;
        public string new_exception;

    }
    public class QParemeters 
    {
        public string qcode;
        public string qvalue;
        public string qresult;
    }
    public class StationExceptionDataList 
    {
        public StationExceptionData[] StationException_data;
    }
    public class QuanlityDataList
    {
        public QuanlityData[] quality_data;
    }
    public class code 
    {
        public string qcode;
    }
    public class codelist 
    {
        public code[] codes;
    }
    public class ResponseData 
    {
        public string success;
        public string[] data;
    }
}
