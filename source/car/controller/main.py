import tkinter as tk
import tkinter.ttk as ttk
from tkinter import font
import serial

ser = serial.Serial('/dev/ttyACM0', 9600)

f_speed = 254
b_speed = 0

def move_forward():
    print("前進")
    # ser.write(bytes('f', 'utf-8'))
    ser.write(bytes([255, f_speed,  f_speed, 255]))

def move_backward():
    print("後進")
    # ser.write(bytes('b', 'utf-8'))
    ser.write(bytes([255, b_speed,  b_speed, 255]))

def turn_right():
    print("右回転")
    # ser.write(bytes('r', 'utf-8'))
    ser.write(bytes([255, b_speed, f_speed, 255]))

def turn_left():
    print("左回転")
    # ser.write(bytes('l', 'utf-8'))
    ser.write(bytes([255, f_speed,  b_speed, 255]))

def move_stop():
    print("停止")
    # ser.write(bytes('s', 'utf-8'))
    ser.write(bytes([255, 127,  127, 255]))

window = tk.Tk()
window.title("リモコン")

# ウィンドウの背景色を青に設定
window.configure(bg="blue")

# ボタンの幅と高さを指定
button_width = 20
button_height = 10

button_font = font.Font(family="Helvetica", size=20, weight="bold")

# ボタンを作成し、レイアウトを設定する
forward_button = tk.Button(window, text="前進", command=move_forward, width=button_width, height=button_height, repeatdelay=1, repeatinterval=1, font=button_font)
forward_button.grid(row=0, column=1)

backward_button = tk.Button(window, text="後進", command=move_backward, width=button_width, height=button_height, repeatdelay=1, repeatinterval=1, font = button_font)
backward_button.grid(row=2, column=1)

right_button = tk.Button(window, text="面舵", command=turn_right, width=button_width, height=button_height, repeatdelay=1, repeatinterval=1, font=button_font)
right_button.grid(row=1, column=2)

left_button = tk.Button(window, text="取舵", command=turn_left, width=button_width, height=button_height, repeatdelay=1, repeatinterval=1, font=button_font)
left_button.grid(row=1, column=0)

stop_button = tk.Button(window, text="停止", command=move_stop, width=button_width, height=button_height, repeatdelay=1, repeatinterval=1, font=button_font)
stop_button.grid(row=1, column=1)

window.mainloop()
