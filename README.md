# Qt 入门 - 机械臂关节控制演示

## 如何在Qt Creator中运行

1. 打开 Qt Creator
2. 点击 **文件 → 打开文件或项目**
3. 选择 `RobotArmDemo.pro` 文件
4. 左下角选择你的编译器（Kit），点击 **配置项目**
5. 按 `Ctrl+R`（或点击左下角绿色三角形）运行

## 文件说明

```
RobotArmDemo.pro  ← 项目配置文件（告诉Qt要编译哪些文件）
main.cpp           ← 程序入口（和C语言的main()一样）
mainwindow.h       ← 主窗口的声明（类似C的.h头文件）
mainwindow.cpp     ← 主窗口的实现（所有逻辑在这里）
```

## 你将学到的Qt核心概念

### 1. 控件（Widget）
Qt里一切可见的东西都是Widget：按钮、标签、滑块、甚至整个窗口。
就像C语言的struct，每个Widget有自己的属性和行为。

### 2. 布局（Layout）
布局管理器自动排列控件，不需要你手动算坐标：
- `QVBoxLayout`：从上到下排列
- `QHBoxLayout`：从左到右排列
- 可以嵌套使用

### 3. 信号与槽（Signal & Slot）
这是Qt最重要的概念：
```cpp
connect(滑块, &QSlider::valueChanged, 窗口, &MainWindow::处理函数);
//      ^^^^                           ^^^^
//   发送者+信号                     接收者+槽
```
当"发送者"发出"信号"时，"接收者"的"槽函数"会被自动调用。

### 4. 自定义绘图（QPainter）
重写 `paintEvent()` 函数，用 QPainter 画任何东西：
```cpp
painter.drawLine(起点, 终点);    // 画线
painter.drawEllipse(中心, rx, ry); // 画椭圆/圆
painter.drawText(x, y, "文字");    // 写字
```

## 下一步：从这个Demo走向MuJoCo

这个Demo的架构已经为你的最终项目做好了铺垫：

| Demo中的部分 | 最终项目中替换为 |
|---|---|
| 2D机械臂绘图 | MuJoCo的OpenGL 3D渲染窗口 |
| sin()模拟力矩 | MuJoCo的 `mj_step()` 真实物理数据 |
| 三个滑块 | 灵巧手每个关节对应一个滑块 |
| QTimer定时更新 | MuJoCo仿真循环的帧刷新 |

你不需要推翻重来，只需要逐步替换每个模块。
