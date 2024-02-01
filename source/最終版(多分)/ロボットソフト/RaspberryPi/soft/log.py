#!/usr/bin/env python

"""
内容:
	ログファイルを生成するクラス
情報:2023/06/23  K.Matsumoto
"""
import os
import time 
import csv

class Logs:
    """ログファイルを生成するクラス"""
    def __init__(self):
        self.startTime = 0
        self.alllogs = []
        self.pointlogs = []
        # logフォルダーのパス
        self.log_folder_path = "./log"
        # logフォルダーが存在しない場合は作成する
        if not os.path.exists(self.log_folder_path):
            os.makedirs(self.log_folder_path)

    def start(self):
        """経過時間の計測を開始する."""
        self.startTime = time.time()
    
    def addAll(self, ax, ay, aa, tx, ty, ta):
        """全ての座標を追加する.

        引数:
            ax (float): 絶対x座標
            ay (float): 絶対y座標
            aa (float): 絶対の傾き
            tx (float): 相対x座標
            ty (float): 相対y座標
            ta (float): 総体の傾き
        """
        nowTime = self.startTime if self.startTime != 0 else self.__get_elapsed_time()
        self.alllogs.append({'time': nowTime, 'ax': ax, 'ay': ay, 'aa': aa, 'tx': tx, 'ty': ty, 'ta': ta})
    
    def addPoint(self, ax, ay, aa, tx, ty, ta, order):
        """重要ポイントの座標を追加する.

        引数:
            ax (float): 絶対x座標
            ay (float): 絶対y座標
            aa (float): 絶対の傾き
            tx (float): 相対x座標
            ty (float): 相対y座標
            ta (float): 総体の傾き
            order (String): 説明
        """
        nowTime = self.startTime if self.startTime != 0 else self.__get_elapsed_time()
        self.pointlogs.append({'time': nowTime, 'ax': ax, 'ay': ay, 'aa': aa, 'tx': tx, 'ty': ty, 'ta': ta, 'order': order})
    
    def showAll(self):
        """全てのログを表示する."""
        for log in self.alllogs:
            print(log)

    def showPoint(self):
        """重要ポイントのログを表示する."""
        for log in self.pointlogs:
            print(log)

    def makeCSV(self, name):
        """全ログファイルを生成する."""
        # ファイル名を作成
        now = time.strftime('%Y%m%d_%H%M%S')
        filename = name + '_All-' + now + '.csv'
        # CSVファイルを保存
        file_path = os.path.join(self.log_folder_path, filename)
        with open(file_path, 'w') as f:
            writer = csv.DictWriter(f, ['time', 'ax', 'ay', 'aa', 'tx', 'ty', 'ta', 'order'])
            for log in self.alllogs:
                writer.writerow(log)

    def makeTXT(self, name):
        """重要ポイントのログファイルを生成する."""
        # ファイル名を作成
        now = time.strftime('%Y%m%d_%H%M%S')
        filename = name + '_Point-' + now + '.txt'
        # TXTファイルを保存
        file_path = os.path.join(self.log_folder_path, filename)
        with open(file_path, 'w') as f:
            for log in self.pointlogs:
                f.write(str(log) + '\n')

    def __get_elapsed_time(self):
        """時間を秒で返す"""
        return round(time.time() - self.startTime, 4)