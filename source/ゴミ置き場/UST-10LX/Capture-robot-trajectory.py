'''LRFを使ってロボットの軌跡を記録する'''
from hokuyolx import HokuyoLX
import matplotlib.pyplot as plt
import numpy as np
import math
import pprint
import time
import csv

DMAX = 10000
IMIN = 300.
IMAX = 2000.

# 閾値
THRESHOLD = 150
ROBOT_MINSIZE = 100
ROBOT_MAXSIZE = 400

# ROBOT_MINSIZE = 300
# ROBOT_MAXSIZE = 500

class Objects:
    def __init__(self):
        self.objects = []

    def update(self, Object):
        self.objects.append(Object)
    
    # ロボットの軌跡を習得する
    """
    def getRobots(self):
        robots = []
        for obj in self.objects:
            if obj.length > 1000:
                robots.append(obj)
        return robots
    """

# 物体のクラス
class Object:
    def __init__(self, timestamp):
        self.timestamp = timestamp
        # 物体の始点と終点
        self.start = {}
        self.end = {}
        # 物体の中心点
        self.center = {}
        # 物体の長さ
        self.length = -100
        self.robotFlag = False

    def setStartPoint(self, ang, len):
        self.start['x'], self.start['y'] = self.__getXY(ang, len)
    
    def setEndPoint(self, ang, len):
        self.end['x'], self.end['y'] = self.__getXY(ang, len)
        self.length = self.__getLength(self.start, self.end)
        self.center['x'], self.center['y'] = self.__getCenter(self.start, self.end)
        if self.length > ROBOT_MINSIZE and self.length < ROBOT_MAXSIZE:
            self.robotFlag = True

    def __getXY(self, ang, len):
        x = -len * math.sin(ang)
        y = len * math.cos(ang)
        return x, y

    def __getLength(self, start, end):
        len = math.sqrt((start['x'] - end['x']) ** 2 + (start['y'] - end['y']) ** 2)
        return len

    def __getCenter(self, start, end):
        x = (start['x'] + end['x']) / 2
        y = (start['y'] + end['y']) / 2
        return x, y

# 1回のLRFのデータ
class LRFData:
    # プロットのクラス
    class Plot:
        def __init__(self, ang, len):
            self.ang = ang
            self.len = len
        
        def getDegree(self):
            return math.degrees(self.ang)
        
        def getXY(self):
            x = -self.len * math.sin(self.ang)
            y = self.len * math.cos(self.ang)
            return x, y

    def __init__(self):
        self.plots = []
        self.timestamp = 0

    def setTimestamp(self, timestamp):
        self.timestamp = timestamp

    def update(self, ang, len):
        self.plots.append(self.Plot(ang, len))

# LRFのデータ群
class LRFDatas:
    def __init__(self):
        self.datas = []

    def update(self, LRFdata):
        self.datas.append(LRFdata)


def get_colors(intens):
    max_val = intens.max()
    return np.repeat(intens, 3).reshape((4,3))/max_val 

def update(laser, plot, text, lrfData, objects):
    timestamp, scan = laser.get_filtered_intens(dmax=DMAX, grouping=0)
    plot.set_offsets(scan[:, :2])
    plot.set_array(scan[:, 2])
    text.set_text('t: %d' % timestamp)
    plt.show()
    plt.pause(0.001)

    # スキャンしたデータを表示する
    # data[][0]: 角度
    # data[][1]: 距離
    
    lrfData.setTimestamp(timestamp)

    robotsCount = 0
    count = 0

    oldLen = 0
    newLen = 0
    objFlag = False

    for i in range(len(scan)):
        lrfData.update(scan[i][0], scan[i][1])
        newLen = lrfData.plots[i].len
        oldLen = 0 if i <= 0 else lrfData.plots[i-1].len
        
        if objFlag == False:
            # 物体を作成する
            objects.update(Object(timestamp))
            # 物体の始点を取得する
            objects.objects[-1].setStartPoint(scan[i][0], scan[i][1])
            objFlag = True
        elif objFlag == True and abs(newLen - oldLen) > THRESHOLD:
            # 物体の終点を取得する
            objects.objects[-1].setEndPoint(scan[i-1][0], scan[i-1][1])
            # 物体を作成する
            objects.update(Object(timestamp))
            # 物体の始点を取得する
            objects.objects[-1].setStartPoint(scan[i][0], scan[i][1])     
        elif objFlag == True and i == len(scan) - 1:
            # 物体の終点を取得する
            objects.objects[-1].setEndPoint(scan[i][0], scan[i][1])
        


def run(LRFDatas, Objects, LRFData, Object):
    plt.ion()
    laser = HokuyoLX(addr=('172.16.0.10', 10940))
    ax = plt.subplot(111, projection='polar')
    plot = ax.scatter([0, 1], [0, 1], s=5, c=[IMIN, IMAX], cmap=plt.cm.Greys_r, lw=0)
    text = plt.text(0, 1, '', transform=ax.transAxes)
    ax.set_rmax(DMAX)
    ax.grid(True)
    plt.show()
    lrfDatas = LRFDatas()
    objects = Objects()

    
    while plt.get_fignums():
        lrfDatas.update(LRFData())
        update(laser, plot, text, lrfDatas.datas[-1], objects)
    

    """    
    l = []
    for i in lrfDatas.datas:
        old = 0
        for j in i.plots:
            # print(int(j.getDegree()), j.len, abs(j.len - old))
            old = j.len
            x, y = j.getXY()
            l.append([x, y, j.len, j.getDegree()])
    """
    """
    lrfDatas.update(LRFData())
    update(laser, plot, text, lrfDatas.datas[-1], objects)
    """
    """
    print("物体risuto ")
    for i in objects.objects:
        print(i.center["x"], i.center["y"])
        print(i.length)
        print("\n\n")
    """
    # robotflagがTrueの物体の座標と長さをCSVに書き込む
    l = []
    for i in objects.objects:
        if i.robotFlag == True:
            l.append([i.timestamp, i.length, i.center["x"], i.center["y"], i.start["x"], i.start["y"], i.end["x"], i.end["y"]])
    # pprint.pprint(l)

    filename = time.strftime("%Y%m%d%H%M%S") + ".csv"
    with open(filename, 'w') as f:
        writer = csv.writer(f, lineterminator='\n')
        writer.writerows(l)

    """
    with open('test.csv', 'w') as f:
        writer = csv.writer(f, lineterminator='\n')
        writer.writerows(l)
    """

    laser.close()

if __name__ == '__main__':
    run(LRFDatas, Objects, LRFData, Object)