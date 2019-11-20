#!/usr/bin/env python
# -*- coding:utf-8 -*-
import time,tkinter,pyautogui,pyperclip
root = tkinter.Tk()#创建窗体
root.title('QQ刷屏程序')
root.geometry('300x300')#设置窗体大小
#创建一个label，提示如何开始程序，停止程序
label1=tkinter.Label(root,text='提示：点下面的[开始刷屏]按钮开始程序')
label1.place(x=25,y=10)
label2=tkinter.Label(root,text='需要停止程序请将鼠标移动至屏幕左上角')
label2.place(x=25,y=30)
#创建label,提示输入框输的输入
label3=tkinter.Label(root,text='设置刷屏内容：')
label3.place(x=25,y=60)
label4=tkinter.Label(root,text='设置刷屏次数：')
label4.place(x=25,y=90)
label5=tkinter.Label(root,text='设置发送方式：')
label5.place(x=25,y=120)
#输入框，获取输入刷屏内容以及次数,
entin=tkinter.Entry(root)
entin.insert(0,"请输入内容...")
entin.place(x=110,y=64,width=135, height=20)
entin2=tkinter.Entry(root)
entin2.insert(1,"5")
entin2.place(x=110,y=94,width=135, height=20)
#选择框，选择发送方式
var = tkinter.IntVar()
chosen=tkinter.Radiobutton(root,text='使用enter发送',value=0,variable=var)
chosen1=tkinter.Radiobutton(root,text='使用shift+enter发送',value=1,variable=var)
chosen.place(x=110,y=120)
chosen1.place(x=110,y=140)
def startE():#按钮主要事件
    if (entin2.get()).isdigit():
        cnt=int(entin2.get())#刷屏次数
    else:
        cnt=1#检查是否为数字，输入不是数字则次数为1
    time.sleep(3)
    while cnt>0:
        txt = entin.get()  # 获取刷屏内容
        pyperclip.copy(txt)  # 对内容复制
        pyautogui.hotkey('ctrl','v')#粘贴
        if var.get():
            pyautogui.hotkey('shift', 'enter')  # shift+enter发送
        else:
            pyautogui.press('enter')#回车键发送
        cnt-=1
    time.sleep(0.5)
def hel():
    pyautogui.alert('按下开始刷屏，鼠标点击QQ输入框,等待三秒即会开始刷屏。\n程序操作为：复制编辑框文字到剪切板再按下ctrl+v，再按下enter')
#加载一个按钮1，按下开始刷屏
button1=tkinter.Button(root,text='开始刷屏',command=startE)
button1.place(x=200,y=200)
#加载一个帮助按钮
button2=tkinter.Button(root,text='帮助',command=hel)
button2.place(x=25,y=200)
root.mainloop()#循环窗体
