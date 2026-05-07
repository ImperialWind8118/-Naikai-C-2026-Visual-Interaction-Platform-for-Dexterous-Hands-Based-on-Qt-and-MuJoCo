// mainwindow.h - 主窗口的"声明"（类似C语言的.h头文件）
// 
// 【C语言对比】
// C语言里你会在.h文件里写函数声明：  void doSomething(int x);
// C++的类也一样：在.h里声明有哪些成员变量和函数，在.cpp里写具体实现

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>   // Qt提供的主窗口基类
#include <QSlider>       // 滑块控件
#include <QLabel>        // 文本标签
#include <QPushButton>   // 按钮
#include <QTimer>        // 定时器（后面用来做实时数据更新）
#include <QVector>       // Qt版的动态数组（类似C语言的可变长数组）

// ==================== 核心概念：类（class）====================
// 如果你只学过C语言，可以把class理解为一个"超级struct"：
//   - struct只能放数据（变量）
//   - class既能放数据，也能放函数（叫"方法"）
//   - class还有访问控制：public（外部可用）、private（内部专用）
//
// MainWindow 继承自 QMainWindow：
//   相当于说"MainWindow是一种特殊的QMainWindow"，
//   自动拥有QMainWindow的所有功能（菜单栏、状态栏等），
//   我们只需要添加自己的东西。
// ============================================================

class MainWindow : public QMainWindow
{
    Q_OBJECT  // Qt的宏，启用"信号与槽"机制（后面会解释）

public:
    // 构造函数：创建MainWindow对象时自动调用
    // 类比C语言：就像 init_window() 初始化函数
    MainWindow(QWidget *parent = nullptr);

// ==================== 核心概念：信号与槽（Signal & Slot）====================
// 这是Qt最重要的机制，理解了它你就理解了Qt的80%
//
// 类比：想象一个"广播电台"系统：
//   - 信号(signal) = 电台发出广播（"滑块被拖动了！新值是45！"）
//   - 槽(slot)     = 收音机收到广播后执行动作（"好的，更新标签显示45"）
//   - connect()    = 把某个收音机调到某个电台的频率
//
// 在C语言里，你可能用回调函数(callback)实现类似功能。
// Qt的信号槽更强大：一个信号可以连接多个槽，而且类型安全。
// ====================================================================

private slots:
    // "槽函数"：当某个信号触发时，这些函数会被自动调用
    void onJointSliderChanged(int value);  // 滑块值改变时调用
    void onResetClicked();                 // 重置按钮点击时调用
    void onTimerTick();                    // 定时器每隔一段时间调用

private:
    // ====== 界面控件（都是指针，用new在堆上创建）======
    // 类比C语言：QSlider* slider = (QSlider*)malloc(sizeof(QSlider));
    // C++更简洁：QSlider* slider = new QSlider();
    
    // 三个关节的滑块和对应的数值标签
    QSlider *m_sliderJoint1;   // m_ 前缀是命名惯例，表示"member成员变量"
    QSlider *m_sliderJoint2;
    QSlider *m_sliderJoint3;
    QLabel  *m_labelJoint1;
    QLabel  *m_labelJoint2;
    QLabel  *m_labelJoint3;

    // 状态显示
    QLabel  *m_labelStatus;    // 显示当前状态文字

    // 按钮
    QPushButton *m_btnReset;   // 重置按钮

    // 定时器：每隔固定时间触发一次（模拟实时数据）
    QTimer *m_timer;

    // ====== 数据存储 ======
    // 存储历史力矩数据，后面用于绘图
    QVector<double> m_torqueHistory;
    int m_tickCount;

    // ====== 辅助函数 ======
    void setupUI();       // 搭建界面布局
    void setupConnections(); // 建立信号与槽的连接
    
    // 自定义绘图：重写父类的paintEvent
    // 每次窗口需要重绘时，Qt会自动调用这个函数
    void paintEvent(QPaintEvent *event) override;
};

#endif // MAINWINDOW_H
