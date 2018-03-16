using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MySql.Data;
using MySql.Data.MySqlClient;
using System.Data;
using System.Threading;
using System.Configuration;

namespace BarcodePrinter2
{
    class Program
    {

        static void Main(string[] args)
        {
            BarcodePrinter bp= new BarcodePrinter();
            bp.startprint();
        }
    }
}
