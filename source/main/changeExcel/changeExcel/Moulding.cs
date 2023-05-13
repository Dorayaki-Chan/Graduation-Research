using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

using Excel = Microsoft.Office.Interop.Excel;


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
                else if (line.StartsWith("Walk")) {
                    // 最後の要素を取得
                    // -q:空の配列の場合どうなるの？
                    // -a:空の配列の場合はエラーになる
                    // リストの長さが1以上なら
                    int[]　last = {0, 0, 0};
                    if (walkOutput.Count > 0)
                    {
                        last = walkOutput.Last();
                    }

                    string[] parts = line.Split(':');
                    Console.WriteLine(line);
                    Console.WriteLine(parts.Length);
                    for (int i = 0; i < parts.Length; i++)
                    {
                        Console.WriteLine($"Part{i}: {parts[i]}");
                    }
                    int n = int.Parse(parts[1].Replace(" X", ""));
                    int x = last[1] + (int.Parse(parts[2].Replace(" ,Y", ""))*10);
                    int y = last[2] + int.Parse(parts[3])*10;

                    // output配列に追加する
                    int[] walk = { n, x, y };
                    walkOutput.Add(walk);
                }
            }
            
            // エクセルに書き込む
            new writeExcel(lrfOutput, walkOutput, folderPath, filePath);

            return 0;
        }
        private class writeExcel
        {
            public writeExcel(List<int[]> lrfOutput, List<int[]> walkOutput, string outputFolder, string inputFilePath)
            {
                Excel.Application excelApp = new Excel.Application();

                // Excelアプリケーションを表示する
                excelApp.Visible = false;

                // 新しいブックを作成する
                Excel.Workbook workbook = excelApp.Workbooks.Add();

                // シートを取得する
                Excel.Worksheet lrfSheet = workbook.Sheets.Add();
                Excel.Worksheet walkSheet = workbook.Sheets.Add();

                // シート名を変更する
                lrfSheet.Name = "LRF";
                walkSheet.Name = "Walk";

                for (int i = 0; i < lrfOutput.Count; i++) {
                    // セルに値を書き込む
                    lrfSheet.Cells[i+1, 1] = lrfOutput[i][0];
                    lrfSheet.Cells[i+1, 2] = lrfOutput[i][1];
                    lrfSheet.Cells[i+1, 3] = lrfOutput[i][2];
                }
                for (int i = 0; i < walkOutput.Count; i++)
                {
                    // セルに値を書き込む
                    walkSheet.Cells[i+1, 1] = walkOutput[i][0];
                    walkSheet.Cells[i+1, 2] = walkOutput[i][1];
                    walkSheet.Cells[i+1, 3] = walkOutput[i][2];
                }

                // 元のファイル名を取得する
                string fileName = Path.GetFileName(inputFilePath).Replace(".txt", "");

                // ブックを保存する
                workbook.SaveAs(outputFolder + "\\" + fileName + ".xlsx");

                // ブックを閉じる
                workbook.Close();

                // Excelアプリケーションを終了する
                excelApp.Quit();
            }
        }
    }
}
