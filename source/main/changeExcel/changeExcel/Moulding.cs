using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace changeExcel
{
    internal class Moulding
    {
        //textをエクセルに変換する関数
        public int changeExcel (string filePath)
        {
            Console.WriteLine(filePath);
            string text = File.ReadAllText(filePath);
            return 0;
        }
    }
}
