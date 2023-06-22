string1= "S0001000033E"
string= "S1234567891E"
if len(string) == 12 and string[0] == 'S' and string[11] == 'E':
    print(int(string[1:6]))
    print(string[6:11])
    pass
else:
    print("ダメ")