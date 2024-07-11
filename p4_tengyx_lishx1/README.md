You may need to execute `chmod +x ./dfu-util` to make the `./dfu-util -l | grep 28e9:0189` instruction work.

If you encounter `LIBUSB_ERROR_ACCESS`, please do: 
```
sudo chmod -R 777 /dev/bus/usb/
```

The board is $160\times 80$ pixels. 

每个横行可以放19个字符，但是最后一个字母后面还有大概一个字母的地方，说明一个字符横着占 $160/20=8$ 像素；一共可以放5行，说明一个字符竖着占 $80/5=16$ 像素。可以自行计算Title等的位置。


Here we are using the `0.9` version of `dfu-util`.

---

```
.section .text
.globl your_function_name
.type your_function_name, @function
your_function_name:
	# The above lines declares a function that can be accessed from C code.
	# When you're implementing your own function, you make a new file in
	# this folder, copy the above 4 lines, and replace all
	# "your_function_name" to your function's name.

	# Then you can add code after the label on line 4.

	# The rest of things work the same as in Venus, except that the directive
	# ".asciiz" is removed. Use ".string" instead.

	# Also, please make sure that each assembly function has a filename with 
	# extension name ".S", not ".s". Otherwise it will not be recognized
	# by the riscv-nuclei-elf-as and will throw an error.

	# You're free to remove this function after you've read the above.
```

When you call a RISC-V function inside a `.c` file, remember to add a function declaration before.

---
## Operational Guide

开机之前，先把底下的USB供电插上，再把上面的USB供电插上，再开机。顺序其实无所谓，最重要的是保证两根USB都插上了，不然下面按钮上来的信号都不可预测，总是会读出来高电平，这样就把你操作了...

除了开始界面和主菜单栏（进入Level Selection或者进入Scoreboard），都可以用`SW1`回退到上一级（游戏里也可以这样），除了完成游戏后，点`SW1`是会进到Scoreboard.

因为 `JOY_UP` 有冲突，所以所有的上下排列的选择，只能使用 `JOY_DOWN` 向下滚，当然滚到最底下就滚回到最上面了。

所有进入下一级菜单的选项，都可以用 `BOOT0` 和 `JOY_CTR`（摇杆往下按下去）来选中并打开。

Scoreboard的三个界面 (1-3 box) 是可以通过左右摇杆切换的。

游戏中，使用 `JOY_CTR` 代替 `JOY_UP`，做向上推的操作。

一重启，Scoreboard的记录就都没了，所以实际上是这次开机后的成绩记录。

---

# Project 4: Longan Nano minigame

This repo contains the framework code for your Project 4. 

In this project, you will be implementing a Pong game using the Longan Nano development board we've given to you. We've provided you a framework here.

You can also choose to implement your own minigame.

We hope this project will help you experience programming RISC-V on a real machine rather than in a simulator.

For full details of this project, please see course website

## PLEASE READ

The projects are part of your design project worth 2 credit points. As such they run in parallel to the actual course. So be aware that the due date for project and homework might be very close to each other! Start early and do not procrastinate.

## Download Framework

Download the framework from gitlab and Check if you already have the following files after pulling.

```
.
├── LICENSE
├── Makefile
├── README.md
├── dfu-util
├── include
│   ├── README
│   ├── fatfs
│   │   ├── diskio.h
│   │   ├── ff.h
│   │   ├── ffconf.h
│   │   └── tf_card.h
│   ├── gd32v_pjt_include.h
│   ├── gd32vf103_libopt.h
│   ├── lcd
│   │   ├── bmp.h
│   │   ├── lcd.h
│   │   └── oledfont.h
│   ├── systick.h
│   └── utils.h
├── platformio.ini
└── src
    ├── assembly
    │   └── example.S
    ├── fatfs
    │   ├── 00history.txt
    │   ├── 00readme.txt
    │   ├── ff.c
    │   ├── ffsystem.c
    │   ├── ffunicode.c
    │   └── tf_card.c
    ├── lcd
    │   └── lcd.c
    ├── main.c
    ├── systick.c
    └── utils.c
```

---
## Command line instructions
You can also upload existing files from your computer using the instructions below.

### Git global setup
```
git config --global user.name "lishx1"
git config --global user.email "lishx1@shanghaitech.edu.cn"
```

### Create a new repository
```
git clone https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s_projects/p4_tengyx_lishx1.git
cd p4_tengyx_lishx1
git switch -c main
touch README.md
git add README.md
git commit -m "add README"
git push -u origin main
```

### Push an existing folder
```
cd existing_folder
git init --initial-branch=main
git remote add origin https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s_projects/p4_tengyx_lishx1.git
git add .
git commit -m "Initial commit"
git push -u origin main
```

### Push an existing Git repository
```
cd existing_repo
git remote rename origin old-origin
git remote add origin https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s_projects/p4_tengyx_lishx1.git
git push -u origin --all
git push -u origin --tags
```