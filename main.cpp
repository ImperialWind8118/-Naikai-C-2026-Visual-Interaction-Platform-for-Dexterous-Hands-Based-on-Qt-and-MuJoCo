// main.cpp - 程序入口
// 每个Qt程序都从这里开始，和C语言的main()一模一样
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // QApplication 管理整个图形界面程序的生命周期
    // 类比：就像C语言里初始化一个全局的"窗口管理器"
    QApplication app(argc, argv);

    // 创建我们自定义的主窗口（MainWindow是我们自己写的类）
    MainWindow window;
    window.setWindowTitle("机械臂关节控制演示");
    window.resize(900, 600);
    window.show();

    // app.exec() 进入事件循环，等待用户操作（点击、拖动等）
    // 类比：就像一个 while(1) 循环不断检查有没有新的用户输入
    return app.exec();
}
