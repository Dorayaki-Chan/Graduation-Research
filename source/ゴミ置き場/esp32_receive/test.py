string1= "S0001000033E"
string= "S-008300062ES-008600054ES-008000063ES-005000047ES-005700124ES-005000128ES-002200101ES-000700102ES0000900102ES-000100102E"
string2 = "S-001100033ES-006500023ES-009000050ES-012400057ES-009300045ES-009800033ES-0102-0001ES-013600002ES-009600038E"

'''
print(len(string))
print(string[0:12])
print(string[12:24])
print(string[24:36])
print(string[36:48])
print(string[48:60])
print(string[60:72])
print(string[72:84])
print(string[84:96])
print(string[96:108])
print(string[108:120])
'''

for i in range(0, len(string)-12, 12):
    data = string[i:i+12]
    print(data)
    pass

'''
if len(string) == 12 and string[0] == 'S' and string[11] == 'E':
    print(int(string[1:6]))
    print(string[6:11])
    pass
else:
    print("ダメ")
'''