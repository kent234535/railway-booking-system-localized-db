QT += core widgets sql

CONFIG += c++17
CONFIG += console

TARGET = railway
TEMPLATE = app

SOURCES += kent.cpp

# 设置输出目录
DESTDIR = ./

# 编译器设置
QMAKE_CXX = D:/newdesktop/desktopfilebodies/Qt/Tools/mingw1310_64/bin/g++.exe
QMAKE_CC = D:/newdesktop/desktopfilebodies/Qt/Tools/mingw1310_64/bin/gcc.exe

# Windows特定设置
win32 {
    CONFIG += windows
}
