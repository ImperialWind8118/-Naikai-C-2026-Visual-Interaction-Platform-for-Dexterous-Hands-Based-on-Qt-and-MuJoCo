// mainwindow.cpp - 主窗口的"实现"
// 这个文件是整个项目的核心，每一行都有详细注释

#include "mainwindow.h"

// Qt的布局管理器（自动排列控件位置）
#include <QVBoxLayout>    // 垂直布局：控件从上到下排列
#include <QHBoxLayout>    // 水平布局：控件从左到右排列
#include <QGroupBox>      // 分组框：给一组控件加个边框和标题
#include <QPainter>       // 画笔：用于自定义绘图
#include <QFont>          // 字体设置

#include <cmath>          // 数学函数（sin, cos等）

// ==================== 构造函数 ====================
// 类比C语言：
//   struct Window* create_window() {
//       struct Window* w = malloc(sizeof(struct Window));
//       init_ui(w);
//       return w;
//   }
// C++把这个过程封装成了"构造函数"，创建对象时自动调用。
// ================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)  // 先调用父类的构造函数（类似先初始化基础部分）
    , m_tickCount(0)       // 初始化计数器为0
{
    setupUI();          // 搭建界面

    // 启动定时器：每100毫秒触发一次（模拟10Hz的传感器数据）
    m_timer = new QTimer(this);  // this表示"这个定时器属于当前窗口"
    m_timer->start(100);         // 开始计时，单位毫秒

    setupConnections(); // 连接信号与槽
}

// ==================== 搭建界面 ====================
// 这是你需要重点理解的函数！
// Qt的界面搭建逻辑：
//   1. 创建控件（按钮、滑块、标签……）
//   2. 创建布局（垂直排列、水平排列、网格……）
//   3. 把控件放进布局
//   4. 把布局设置给窗口
// ================================================
void MainWindow::setupUI()
{
    // ====== 第1步：创建一个"中心部件" ======
    // QMainWindow要求必须有一个centralWidget作为内容区域
    // 类比：这是一张白纸，所有控件都画在这张纸上
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // ====== 第2步：创建整体水平布局 ======
    // 左边放控制面板，右边留给绘图区域
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(16, 16, 16, 16);  // 四周留白
    mainLayout->setSpacing(16);                        // 控件间距

    // ====== 第3步：左侧 - 控制面板 ======
    QVBoxLayout *controlLayout = new QVBoxLayout();

    // --- 标题 ---
    QLabel *titleLabel = new QLabel("关节控制面板");
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    controlLayout->addWidget(titleLabel);

    // --- 关节1滑块组 ---
    // 用一个辅助lambda来避免重复代码
    // 【C语言对比】lambda就是一个"匿名函数"，可以在需要的地方直接定义
    //   C语言里你需要单独写一个函数然后传函数指针
    //   C++的lambda更方便：auto f = [](int x){ return x*2; };
    auto createJointGroup = [](const QString &name, QSlider *&slider, QLabel *&label) -> QGroupBox*
    {
        QGroupBox *group = new QGroupBox(name);
        QVBoxLayout *layout = new QVBoxLayout(group);

        // 创建水平布局：滑块 + 数值标签
        QHBoxLayout *row = new QHBoxLayout();

        slider = new QSlider(Qt::Horizontal);  // 水平滑块
        slider->setRange(-180, 180);           // 范围：-180°到+180°
        slider->setValue(0);                    // 初始值：0°
        slider->setTickPosition(QSlider::TicksBelow);  // 在滑块下方显示刻度

        label = new QLabel("0°");
        label->setMinimumWidth(50);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        row->addWidget(slider);
        row->addWidget(label);
        layout->addLayout(row);

        return group;
    };

    // 用上面的辅助函数创建三个关节控制组
    QGroupBox *group1 = createJointGroup("关节1 - 底座旋转", m_sliderJoint1, m_labelJoint1);
    QGroupBox *group2 = createJointGroup("关节2 - 肩部俯仰", m_sliderJoint2, m_labelJoint2);
    QGroupBox *group3 = createJointGroup("关节3 - 肘部弯曲", m_sliderJoint3, m_labelJoint3);

    controlLayout->addWidget(group1);
    controlLayout->addWidget(group2);
    controlLayout->addWidget(group3);

    // --- 重置按钮 ---
    m_btnReset = new QPushButton("重置所有关节");
    m_btnReset->setMinimumHeight(36);
    controlLayout->addWidget(m_btnReset);

    // --- 状态栏 ---
    m_labelStatus = new QLabel("就绪 - 拖动滑块控制关节角度");
    m_labelStatus->setStyleSheet("color: #666; padding: 8px 0;");
    controlLayout->addWidget(m_labelStatus);

    // 弹簧：把剩余空间顶到底部（让控件紧凑排列在上方）
    controlLayout->addStretch();

    // ====== 第4步：组装到主布局 ======
    // 左侧控制面板占1份宽度，右侧绘图区占2份
    QWidget *controlPanel = new QWidget();
    controlPanel->setLayout(controlLayout);
    controlPanel->setMaximumWidth(320);

    mainLayout->addWidget(controlPanel, 1);   // stretch factor = 1
    mainLayout->addStretch(2);                 // 右侧空间（绘图区）占2份

    // 设置窗口背景色
    setStyleSheet("QMainWindow { background-color: #fafafa; }");
}

// ==================== 连接信号与槽 ====================
// 这里是Qt的精髓！
// connect(发送者, 信号, 接收者, 槽函数)
//
// 类比C语言的回调：
//   register_callback(slider, ON_VALUE_CHANGE, my_handler);
// Qt写法：
//   connect(slider, &QSlider::valueChanged, this, &MainWindow::onJointSliderChanged);
//
// 翻译成人话：
//   "当slider的值改变时，调用MainWindow的onJointSliderChanged函数"
// ====================================================
void MainWindow::setupConnections()
{
    // 三个滑块都连接到同一个槽函数
    connect(m_sliderJoint1, &QSlider::valueChanged,
            this, &MainWindow::onJointSliderChanged);
    connect(m_sliderJoint2, &QSlider::valueChanged,
            this, &MainWindow::onJointSliderChanged);
    connect(m_sliderJoint3, &QSlider::valueChanged,
            this, &MainWindow::onJointSliderChanged);

    // 重置按钮点击 → 调用onResetClicked
    connect(m_btnReset, &QPushButton::clicked,
            this, &MainWindow::onResetClicked);

    // 定时器超时 → 调用onTimerTick
    connect(m_timer, &QTimer::timeout,
            this, &MainWindow::onTimerTick);
}

// ==================== 槽函数实现 ====================

void MainWindow::onJointSliderChanged(int value)
{
    // 更新三个标签的显示文字
    // QString::number() 把数字转成字符串，类似C语言的sprintf
    m_labelJoint1->setText(QString::number(m_sliderJoint1->value()) + "°");
    m_labelJoint2->setText(QString::number(m_sliderJoint2->value()) + "°");
    m_labelJoint3->setText(QString::number(m_sliderJoint3->value()) + "°");

    // 更新状态栏
    m_labelStatus->setText(
        QString("关节角度: [%1°, %2°, %3°]")
            .arg(m_sliderJoint1->value())
            .arg(m_sliderJoint2->value())
            .arg(m_sliderJoint3->value())
    );

    // 触发重绘（告诉Qt："界面变了，请重新画一下"）
    update();
}

void MainWindow::onResetClicked()
{
    // 把三个滑块都归零
    m_sliderJoint1->setValue(0);
    m_sliderJoint2->setValue(0);
    m_sliderJoint3->setValue(0);
    m_torqueHistory.clear();
    m_tickCount = 0;
    m_labelStatus->setText("已重置所有关节");
    update();
}

void MainWindow::onTimerTick()
{
    // 模拟力矩数据（用正弦波+关节角度生成假数据）
    // 你将来接入MuJoCo后，这里替换成真实的物理引擎数据
    double j1 = m_sliderJoint1->value();
    double j2 = m_sliderJoint2->value();
    double torque = 5.0 * sin(m_tickCount * 0.1) + j1 * 0.02 + j2 * 0.01;

    m_torqueHistory.append(torque);
    if (m_torqueHistory.size() > 200) {
        m_torqueHistory.removeFirst();  // 只保留最近200个数据点
    }
    m_tickCount++;

    update();  // 触发重绘
}

// ==================== 自定义绘图 ====================
// paintEvent是Qt的绘图回调函数
// 每次调用update()或窗口大小改变时，Qt自动调用此函数
//
// 类比C语言：
//   就像在一块画布上用画笔(QPainter)画线、画圆、写字
//   void draw(Canvas* canvas) {
//       draw_line(canvas, x1, y1, x2, y2);
//       draw_text(canvas, x, y, "hello");
//   }
// ================================================
void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);  // 先让父类画默认内容

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 抗锯齿，让线条更平滑

    // ====== 绘图区域 ======
    // 右半部分用来画东西
    int drawAreaX = 340;
    int drawAreaY = 20;
    int drawAreaW = width() - drawAreaX - 20;
    int drawAreaH = height() - 40;

    if (drawAreaW < 100 || drawAreaH < 100) return;

    // --- 上半部分：画一个简易2D机械臂 ---
    int armCenterX = drawAreaX + drawAreaW / 2;
    int armCenterY = drawAreaY + drawAreaH / 3;

    // 绘制标题
    painter.setPen(QColor("#333"));
    QFont headerFont;
    headerFont.setPointSize(13);
    headerFont.setBold(true);
    painter.setFont(headerFont);
    painter.drawText(drawAreaX, drawAreaY + 16, "机械臂姿态预览");

    // 获取三个关节的角度（转换为弧度）
    double a1 = m_sliderJoint1->value() * M_PI / 180.0;
    double a2 = m_sliderJoint2->value() * M_PI / 180.0;
    double a3 = m_sliderJoint3->value() * M_PI / 180.0;

    // 连杆长度
    double L1 = 80, L2 = 60, L3 = 40;

    // 正向运动学：逐段计算每个关节末端的位置
    // 这就是你将来在MuJoCo里会用到的核心算法！
    double x0 = armCenterX, y0 = armCenterY;

    double x1 = x0 + L1 * cos(a1 - M_PI / 2);
    double y1 = y0 + L1 * sin(a1 - M_PI / 2);

    double x2 = x1 + L2 * cos(a1 + a2 - M_PI / 2);
    double y2 = y1 + L2 * sin(a1 + a2 - M_PI / 2);

    double x3 = x2 + L3 * cos(a1 + a2 + a3 - M_PI / 2);
    double y3 = y2 + L3 * sin(a1 + a2 + a3 - M_PI / 2);

    // 画底座（灰色圆）
    painter.setBrush(QColor("#ddd"));
    painter.setPen(QPen(QColor("#999"), 2));
    painter.drawEllipse(QPointF(x0, y0), 12, 12);

    // 画连杆1（蓝色粗线）
    painter.setPen(QPen(QColor("#2979ff"), 6, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(x0, y0), QPointF(x1, y1));

    // 画关节1（蓝色小圆）
    painter.setBrush(QColor("#2979ff"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(x1, y1), 6, 6);

    // 画连杆2（绿色）
    painter.setPen(QPen(QColor("#00c853"), 5, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));

    painter.setBrush(QColor("#00c853"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(x2, y2), 5, 5);

    // 画连杆3（橙色）
    painter.setPen(QPen(QColor("#ff6d00"), 4, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(x2, y2), QPointF(x3, y3));

    // 末端执行器（红色小圆）
    painter.setBrush(QColor("#d50000"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(x3, y3), 5, 5);

    // --- 下半部分：力矩数据折线图 ---
    int chartX = drawAreaX;
    int chartY = drawAreaY + drawAreaH * 2 / 3 + 10;
    int chartW = drawAreaW;
    int chartH = drawAreaH / 3 - 30;

    // 图表标题
    painter.setPen(QColor("#333"));
    headerFont.setPointSize(12);
    painter.setFont(headerFont);
    painter.drawText(chartX, chartY - 4, "实时力矩数据 (模拟)");

    // 绘制图表背景和坐标轴
    painter.setPen(QPen(QColor("#ccc"), 1));
    painter.setBrush(QColor("#fff"));
    painter.drawRect(chartX, chartY, chartW, chartH);

    // 画零线
    int zeroY = chartY + chartH / 2;
    painter.setPen(QPen(QColor("#eee"), 1, Qt::DashLine));
    painter.drawLine(chartX, zeroY, chartX + chartW, zeroY);

    // 画数据曲线
    if (m_torqueHistory.size() > 1) {
        painter.setPen(QPen(QColor("#2979ff"), 2));

        double maxVal = 10.0;  // 力矩范围 [-10, 10]
        for (int i = 1; i < m_torqueHistory.size(); i++) {
            double px1 = chartX + (double)(i - 1) / 200.0 * chartW;
            double py1 = zeroY - (m_torqueHistory[i - 1] / maxVal) * (chartH / 2);
            double px2 = chartX + (double)i / 200.0 * chartW;
            double py2 = zeroY - (m_torqueHistory[i] / maxVal) * (chartH / 2);
            painter.drawLine(QPointF(px1, py1), QPointF(px2, py2));
        }
    }

    // Y轴标签
    QFont smallFont;
    smallFont.setPointSize(9);
    painter.setFont(smallFont);
    painter.setPen(QColor("#999"));
    painter.drawText(chartX - 2, chartY + 12, "+10");
    painter.drawText(chartX + 4, zeroY + 4, "0");
    painter.drawText(chartX - 2, chartY + chartH - 2, "-10");
}
