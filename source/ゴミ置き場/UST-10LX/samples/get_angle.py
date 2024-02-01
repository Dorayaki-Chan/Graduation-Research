"""レーダーの角度を取得するプログラム"""

from hokuyolx import HokuyoLX
import csv
import math

laser = HokuyoLX(addr=('172.16.0.10', 10940))
arry1 = laser.get_angles()
for i in range(0, len(arry1)):
    arry1[i] = math.degrees(arry1[i])

print(arry1)