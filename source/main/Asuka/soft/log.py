"""
内容:
	ログファイルを生成するクラス
情報:2023/06/23  K.Matsumoto
"""

import os
import time 
import csv

# logクラス
class Logs:
    def __init__(self):
        self.alllogs = []
        self.pointlogs = []
        # logフォルダーのパス
        self.log_folder_path = "./log"
        # logフォルダーが存在しない場合は作成する
        if not os.path.exists(self.log_folder_path):
            os.makedirs(self.log_folder_path)

    def start(self):
        # 経過時間を計測するために開始時刻を保存
        self.start = time.time()
    
    def addAll(self, time, ax, ay, aa, tx, ty, ta):
        self.alllogs.append({'time': time, 'ax': ax, 'ay': ay, 'aa': aa, 'tx': tx, 'ty': ty, 'ta': ta})
    
    def addPoint(self, time, ax, ay, aa, tx, ty, ta, order):
        self.pointlogs.append({'time': time, 'ax': ax, 'ay': ay, 'aa': aa, 'tx': tx, 'ty': ty, 'ta': ta, 'order': order})
    
    def showAll(self):
        for log in self.alllogs:
            print(log)

    def showPoint(self):
        for log in self.pointlogs:
            print(log)

    def makeCSV(self):
        # ファイル名を作成
        now = time.strftime('%Y%m%d_%H%M%S')
        filename = 'OpticalFlow_All-' + now + '.csv'
        # CSVファイルを保存
        file_path = os.path.join(self.log_folder_path, filename)
        with open(file_path, 'w') as f:
            writer = csv.DictWriter(f, ['time', 'ax', 'ay', 'aa', 'tx', 'ty', 'ta', 'order'])
            for log in self.alllogs:
                writer.writerow(log)

    def makeTXT(self):
        # ファイル名を作成
        now = time.strftime('%Y%m%d_%H%M%S')
        filename = 'OpticalFlow_Point-' + now + '.txt'
        # TXTファイルを保存
        file_path = os.path.join(self.log_folder_path, filename)
        with open(file_path, 'w') as f:
            for log in self.pointlogs:
                f.write(str(log) + '\n')