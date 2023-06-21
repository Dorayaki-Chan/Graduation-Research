import math

def howManyTimesDoIHaveToTurn(x, y):
        dif_x = x - 500
        dif_y = y - 500
        dif_rotation = math.degrees(math.atan2(dif_y, dif_x)) - 90
        
        if dif_rotation > 180:
            dif_rotation -= 360
        elif dif_rotation < -180:
            dif_rotation += 360
        return dif_rotation

def howManyMove(x, y):
        dif_x = x - 500
        dif_y = y - 500
        return math.sqrt(dif_x ** 2 + dif_y ** 2)

OPTICAL_KEISUU = 0.188679245
OPTICAL_HANKEI = 60
ENSHU = 2 * OPTICAL_HANKEI * math.pi
def xToAngle(x):
    angle = (abs(x) * OPTICAL_KEISUU * 360) / (2 * OPTICAL_HANKEI * math.pi)
    return  angle

# print(howManyTimesDoIHaveToTurn(-500, -500))
# print(howManyMove(-500, -500))