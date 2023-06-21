# 目標座標との角度を計算するプログラム
import math

now_x = 0
now_y = 0
now_rotation = -90

def howRotation(x, y):
    dif_x = x - now_x
    dif_y = y - now_y
    
    return math.degrees(math.atan2(dif_y, dif_x))

def howDiffRotation(rotation):
    dif_rotation = rotation - now_rotation
    if dif_rotation > 180:
        dif_rotation -= 360
    elif dif_rotation < -180:
        dif_rotation += 360
    return dif_rotation

# print(howRotation(-1, -1))
# print(howDiffRotation(howRotation(-1, -1)))

# print(-now_rotation)
print(round(math.cos(math.radians(60)), 5))