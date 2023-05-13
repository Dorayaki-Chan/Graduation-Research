using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace changeExcel
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged_1(object sender, EventArgs e)
        {

        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            // -q:ここはどんな時に実行されるの？
            // -a:画面が描画されるときに実行される。
        }

        // D&Dの許可
        private void panel1_DragEnter(object sender, DragEventArgs e) {

        }

        // D&Dの実行
        private void panel1_DragDrop(object sender, DragEventArgs e)
        {
            // ドロップされたファイルのパスを取得する
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            // 取得したファイルPathをconsoleに表示する
            foreach (string file in files)
            {
                // ファイルの拡張子がtxtの場合
                if (Path.GetExtension(file) == ".txt")
                {
                    // ファイルの内容を読み込む
                    string text = File.ReadAllText(file);
                    // ファイルの内容を表示する
                    //Console.WriteLine(text);
                    // ファイルの内容を表示する
                    Console.WriteLine(file);
                }
            }
        }
        private void panel1_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "Text files (*.txt)|*.txt";
            openFileDialog1.FilterIndex = 2;
            openFileDialog1.RestoreDirectory = true;
            openFileDialog1.Multiselect = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                // 出力先を取得する
                using (FolderBrowserDialog dialog = new FolderBrowserDialog())
                {
                    // ダイアログのタイトルを設定
                    dialog.Description = "出力先フォルダーを選択してください";

                    // ルートフォルダーを指定（デフォルトはDesktop）
                    dialog.RootFolder = Environment.SpecialFolder.Desktop;

                    // ダイアログを表示して、ユーザーが選択したフォルダーのパスを取得
                    DialogResult result = dialog.ShowDialog();
                    if (result == DialogResult.OK)
                    {

                        string folderPath = dialog.SelectedPath;
                        int return_counter = 0;
                        foreach (string filePath in openFileDialog1.FileNames)
                        {
                            // ファイルパスを使った処理を行う
                            return_counter += new Moulding().changeExcel(filePath, folderPath);
                        }
                        if (return_counter == openFileDialog1.FileNames.Length)
                        {
                            string message = $"{openFileDialog1.FileNames.Length}件のログを\n\n{folderPath}\n\nにぶち込みました。";
                            string title = "正常";
                            MessageBox.Show(message, title);
                        }
                        else {
                            string title = "エラー";
                            string message = $"失敗しました。データが違うか、このアプリがおかしいです。ごめんっぴ🥺";
                            MessageBox.Show(message, title);
                        }
                    }
                }
            }
        }
    }
}
