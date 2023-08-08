"""レーダーの角度を取得するプログラム"""

from hokuyolx import HokuyoLX
import csv

laser = HokuyoLX(addr=('172.16.0.10', 10940))
arry1 = laser.get_angles()
for i in range(0, len(arry1)):
    arry1[i] = arry1[i] * 180 / 3.1415926

print(arry1)