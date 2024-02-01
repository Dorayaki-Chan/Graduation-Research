import time
import pprint

# 1回のLRFのデータ
class LRFData:
    # プロットのクラス
    class Plot:
        def __init__(self, ang, len):
            self.ang = ang
            self.len = len

    def __init__(self):
        self.timestamp = time.time()
        self.plots = []

    def update(self, ang, len):
        self.plots.append(self.Plot(ang, len))

# LRFのデータ群
class LRFDatas:
    def __init__(self):
        self.datas = []

    def update(self, LRFdata):
        self.datas.append(LRFdata)

LRFdatas = LRFDatas()
# 1回のレーザーのデータ
for i in range(0, 2):
    LRFdata = LRFData()
    for j in range(0, 10):
        LRFdata.update(j*10, j*100)
    LRFdatas.update(LRFdata)
    time.sleep(1)

pprint.pprint(vars(LRFdatas))