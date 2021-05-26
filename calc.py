#! /usr/local/bin/env python3
#! -*- coding: utf-8 -*-
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import pprint
import os


# サンプリングした値をテキストファイルに書き込む？
isSave = False
# グラフを描画する？
isPlot = True


# 諸変数の定義
outputFile = 'timedomain.txt'
Fs = 8000
Fc = 1000
lim = 16


# x(t) の定義
x = lambda t: np.cos(2*np.pi*Fc*t) # 演習課題１、２
#x = lambda t: 1 # 演習問題３


# 窓関数 w(n) の定義
lim_w = [0, 7]
def wn(x, n, p): return float(x*p) if(n >= lim_w[0]) and (n <= lim_w[1]) else 0.


# 各種数値の計算
xn = [round(x(n/Fs), 2) for n in range(lim+1)]
text = ''
data = {'n': [], 'x(n)': [], 'w(n)': [], 'xw(n)': [], 'k': [], 'ω': [], 'xw(ejω)実部': [], 'xw(ejω)虚部': [], '振幅': []}
# n, x(n), w(n), xw(n), k, ω
for n in range(-1*lim, lim+1):
    text += f'{xn[n]}\n'
    data['n'] += [n]
    data['x(n)'] += [xn[n]]
    data['w(n)'] += [wn(1, n, 1)]
    data['xw(n)'] += [wn(xn[n], n, 1)]
    data['k'] += [n]
    data['ω'] += [round(2*np.pi*float(n)/lim, 2)]
# 実部, 虚部, 振幅
for n in range(-1*lim, lim+1):
    real, imagin = 0., 0.
    for m in range(lim):
        real += data['xw(n)'][m+lim]*np.cos(-1*data['ω'][n+lim]*float(m))
        imagin += data['xw(n)'][m+lim]*np.sin(-1*data['ω'][n+lim]*float(m))
    data['xw(ejω)実部'] += [round(real,  2)]
    data['xw(ejω)虚部'] += [round(imagin, 2)]
    data['振幅'] += [round(np.sqrt(data['xw(ejω)実部'][n+lim]**2 + data['xw(ejω)虚部'][n+lim]**2), 2)]


# 計算結果をデータフレームに格納
df = pd.DataFrame(data)


# テキストファイルに書き込む関数
def save() -> None:
    os.chdir(os.getcwd())
    with open(outputFile, mode='w', encoding='utf8') as _outputFile: _outputFile.write(text)


if(__name__ == '__main__'):

    # データフレームの出力
    pprint.pprint(df[['k', 'ω', 'xw(ejω)実部', 'xw(ejω)実部', '振幅']])
    #pprint.pprint(df)


    # グラフの描画
    if isPlot:
        plt.grid('on')
        plt.title('振幅')
        plt.xlabel('正規化角周波数 [Hz]')
        plt.ylabel('振幅')
        plt.plot(df['ω'], df['振幅'])
        plt.scatter(df['ω'], df['振幅'])
        plt.show()


    # テキストファイルへの書き込み
    if isSave: save()
