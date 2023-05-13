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
                foreach (string filePath in openFileDialog1.FileNames)
                {
                    // ファイルパスを使った処理を行う
                    int result = new Moulding().changeExcel(filePath);
                }
            }
        }
    }
}
