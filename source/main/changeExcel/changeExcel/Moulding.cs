using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Microsoft.Office.Interop.Excel;


namespace changeExcel
{
    internal class Moulding
    {
        //textをエクセルに変換する関数
        public int changeExcel (string filePath, String folderPath)
        {
            Console.WriteLine(folderPath);
            Console.WriteLine(filePath);
            string text = File.ReadAllText(filePath);

            List<int[]> lrfOutput = new List<int[]>();
            List<int[]> walkOutput = new List<int[]>();

            // 改行で区切って回す
            foreach (string line in text.Split('\n'))
            {
                // LRF
                if (line.StartsWith("LRF"))
                {
                    string[] del = {": "};
                    string[] parts = line.Split(del, StringSplitOptions.None);
                    int n = int.Parse(parts[1].Replace(" X", ""));
                    int x = int.Parse(parts[2].Replace(", Y ", ""));
                    int y = int.Parse(parts[3]);
                    
                    // nが0の時はoutput配列を初期化する
                    if (n == 0)
                    {
                        lrfOutput = new List<int[]>();
                        walkOutput = new List<int[]>();
                    }

                    // output配列に追加する
                    int[] lrf = { n, x, y };
                    lrfOutput.Add(lrf);
                }
                // walk
                else if (line.StartsWith("walk")) {
                    // 最後の要素を取得
                    // -q:空の配列の場合どうなるの？
                    // -a:空の配列の場合はエラーになる
                    // リストの長さが1以上なら
                    int[]　last = {0, 0, 0};
                    if (walkOutput.Count > 0)
                    {
                        last = walkOutput.Last();
                    }

                    string[] del = {":"};
                    string[] parts = line.Split(del, StringSplitOptions.None);
                    int n = int.Parse(parts[1].Replace(" X", ""));
                    int x = last[1] + int.Parse(parts[2].Replace(" ,Y ", ""))*10;
                    int y = last[2] + int.Parse(parts[3])*10;

                    // output配列に追加する
                    int[] walk = { n, x, y };
                    walkOutput.Add(walk);
                }
            }
            
            // エクセルに書き込む
            

            return 0;
        }
        private class writeExcel
        {
            public writeExcel()
            {
                using Excel = Microsoft.Office.Interop.Excel;
            }
        }
    }
}
