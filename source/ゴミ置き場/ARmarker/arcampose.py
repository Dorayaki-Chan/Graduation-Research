#!/usr/bin/env python
# -*- coding: utf-8 -*
import numpy as np
import cv2
from cv2 import aruco
import math
import serial

def main():
    ser = serial.Serial('/dev/ttyACM0',9600)
    F = 0.45 #[m] 
    elevation_angle=0
    cap = cv2.VideoCapture(0)
    # マーカーサイズ
    marker_length = 0.07 # [m]
    # マーカーの辞書選択
    dictionary = aruco.getPredefinedDictionary(aruco.DICT_ARUCO_ORIGINAL)

    camera_matrix = np.load("./models/mtx3.npy")
    distortion_coeff = np.load("./models/dist3.npy")

    while True:
        ret, img = cap.read()
        corners, ids, rejectedImgPoints = aruco.detectMarkers(img, dictionary)
        # 可視化
        aruco.drawDetectedMarkers(img, corners, ids, (0,255,255))

        if len(corners) > 0:
            # マーカーごとに処理
            for i, corner in enumerate(corners):
                # rvec -> rotation vector, tvec -> translation vector
                rvec, tvec, _ = aruco.estimatePoseSingleMarkers(corner, marker_length, camera_matrix, distortion_coeff)

                # < rodoriguesからeuluerへの変換 >
                
                # 不要なaxisを除去
                tvec = np.squeeze(tvec)
                rvec = np.squeeze(rvec)
                
                # 回転ベクトルからrodoriguesへ変換
                rvec_matrix = cv2.Rodrigues(rvec)
                rvec_matrix = rvec_matrix[0] # rodoriguesから抜き出し
                # 並進ベクトルの転置
                transpose_tvec = tvec[np.newaxis, :].T
                # 合成
                proj_matrix = np.hstack((rvec_matrix, transpose_tvec))
                # オイラー角への変換
                euler_angle = cv2.decomposeProjectionMatrix(proj_matrix)[6] # [deg]

                # 可視化
                draw_pole_length = marker_length/2 # 現実での長さ[m]
                cv2.drawFrameAxes(img, camera_matrix, distortion_coeff, rvec, tvec, draw_pole_length,3)

        cv2.imshow('drawDetectedMarkers', img)
        k=cv2.waitKey(10)
        if k == ord('q'):
            break
        elif (k == ord('a')or ser.in_waiting) and len(corners) > 0:
            
            val_arduino = ser.readline()
            print(tvec)
            print(rvec)
            """
            print("tx : " + str(tvec[0]))
            print("ty : " + str(tvec[1]))
            print("tz : " + str(tvec[2]))
            print("rx : " + str(rvec[0]))
            print("ry : " + str(rvec[1]))
            print("rz : " + str(rvec[2]))
            print("roll : " + str(euler_angle[0]))
            print("pitch: " + str(euler_angle[1]))
            print("yaw  : " + str(euler_angle[2]))
            """
            c = -np.arctan(tvec[1]/tvec[2])
            l1 = (tvec[1]**2+tvec[2]**2)**0.5*np.cos(np.radians(elevation_angle)+c)
            l=(l1**2+tvec[0]**2)**0.5
            angle=np.degrees(np.arctan(tvec[0]/l1))
            rvec_mat, Jacob = cv2.Rodrigues(rvec) 
            pose_angle = float(np.degrees(np.arcsin(rvec_mat[2][0])))
            l2=int(l*100)
            print(l2,angle,pose_angle)
            if abs(angle)<3:
                angle = 0
            if abs(pose_angle)<3:
                pose_angle = 0
            a = math.degrees(math.atan2(F*math.sin(math.radians(-pose_angle))+l*math.sin(math.radians(-angle)),F*math.cos(math.radians(-pose_angle))-l*math.cos(math.radians(-angle))))
            #a=int(a)
            a=int(l2)
            c=0
            print(a)
            if a<0: 
                c=1
                a=abs(a)
            b = int(-pose_angle)
            #b=int(-angle)
            d = 0
            if b<0:
                #b = abs(int(pose_angle))
                b=abs(int(-angle))
                d = 1
            print(a,b,c,d)

            ser.write(bytes(str(a)+'a','utf-8'))
            #time.sleep(0.012)
            ser.write(bytes(str(b)+'a','utf-8'))
            #time.sleep(0.012)
            ser.write(bytes(str(c)+'a','utf-8'))
            #time.sleep(0.012)
            ser.write(bytes(str(d)+'a','utf-8'))
            
            
            val_arduino1 = ser.readline()
            #time.sleep(0.011)
            val_arduino2 = ser.readline()
            #time.sleep(0.011)
            val_arduino3 = ser.readline()
            #time.sleep(0.011)
            val_arduino4 = ser.readline()
            
            val_decoded1 = int(repr(val_arduino1.decode())[1:-5])
            val_decoded2 = int(repr(val_arduino2.decode())[1:-5])
            val_decoded3 = int(repr(val_arduino3.decode())[1:-5])
            val_decoded4 = int(repr(val_arduino4.decode())[1:-5])

            print(val_decoded1 ,val_decoded2,val_decoded3,val_decoded4)

    cv2.destroyAllWindows()
    ser.close()

if __name__ == '__main__':
    main()
