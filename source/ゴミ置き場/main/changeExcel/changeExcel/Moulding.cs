using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Drawing;
using System.Windows.Forms.DataVisualization.Charting;
using System.Drawing.Imaging;

using Excel = Microsoft.Office.Interop.Excel;


namespace changeExcel
{
    internal class Moulding
    {
        //textをエクセルに変換する関数
        public int changeExcel (string filePath, String folderPath)
        {
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
            drawChart(lrfOutput, "LRF", folderPath, filePath);
            drawChart(walkOutput, "Walk", folderPath, filePath);
            return 1;
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
        private void drawChart(List<int[]> data, string type, string folderPath, string filePath) {

            // サンプルデータの作成
            int[] xData = new int[data.Count];
            int[] yData = new int[data.Count];

            for (int i = 0; i < data.Count; i++)
            {
                xData[i] = data[i][2];
                yData[i] = data[i][1];
            }

            // Chartオブジェクトの作成と設定
            Chart chart = new Chart();
            chart.ChartAreas.Add(new ChartArea("scatterArea"));
            chart.Series.Add(new Series("scatterSeries"));
            chart.Series["scatterSeries"].ChartType = SeriesChartType.Point;

            // データの追加
            for (int i = 0; i < xData.Length; i++)
            {
                chart.Series["scatterSeries"].Points.AddXY(xData[i], yData[i]);
            }
            // 軸の設定
            chart.ChartAreas["scatterArea"].AxisX.Crossing = 0;
            chart.ChartAreas["scatterArea"].AxisX.IsMarginVisible = true;
            chart.ChartAreas["scatterArea"].AxisY.Crossing = 0;
            chart.ChartAreas["scatterArea"].AxisY.IsMarginVisible = true;

            // 目盛軸の線の設定
            chart.ChartAreas["scatterArea"].AxisX.MajorGrid.LineColor = Color.LightGray;
            chart.ChartAreas["scatterArea"].AxisX.MajorGrid.LineWidth = 1;
            chart.ChartAreas["scatterArea"].AxisY.MajorGrid.LineColor = Color.LightGray;
            chart.ChartAreas["scatterArea"].AxisY.MajorGrid.LineWidth = 1;

            double minX = xData.Min();
            double maxX = xData.Max();
            double minY = yData.Min();
            double maxY = yData.Max();
            double interval = 2000;
            double minTickX = Math.Floor(minX / interval) * interval;
            double maxTickX = Math.Ceiling(maxX / interval) * interval;
            double minTickY = Math.Floor(minY / interval) * interval;
            double maxTickY = Math.Ceiling(maxY / interval) * interval;

            // 方眼紙にするために大きい方に表示最大値を合わせる
            double max = maxTickX >= maxTickY ? maxTickX : maxTickY;
            double min = minTickX <= minTickY ? minTickX : minTickY;
            // 目盛軸の設定
            chart.ChartAreas["scatterArea"].AxisX.Minimum = min;
            chart.ChartAreas["scatterArea"].AxisX.Maximum = max;
            chart.ChartAreas["scatterArea"].AxisX.Interval = interval;
            chart.ChartAreas["scatterArea"].AxisY.Minimum = min;
            chart.ChartAreas["scatterArea"].AxisY.Maximum = max;
            chart.ChartAreas["scatterArea"].AxisY.Interval = interval;

            // 解像度の設定
            chart.Width = 2000;
            chart.Height = 2000;

            // 元のファイル名を取得する
            string fileName = Path.GetFileName(filePath).Replace(".txt", "");

            // グラフの保存
            string imagePath = $"{folderPath}\\{type}_{fileName}";
            chart.ChartAreas["scatterArea"].BackColor = Color.Transparent;
            chart.SaveImage($"{imagePath}.jpeg", ChartImageFormat.Jpeg);

            // Chartコントロールの描画をBitmapに変換
            Bitmap bitmap = new Bitmap(chart.Width, chart.Height);
            chart.DrawToBitmap(bitmap, new Rectangle(0, 0, chart.Width, chart.Height));

            // Bitmapの背景を透明にする
            bitmap.MakeTransparent(Color.White);

            // BitmapをPNGファイルとして保存
            bitmap.Save($"{imagePath}.png", ImageFormat.Png);
        }
    }
}
