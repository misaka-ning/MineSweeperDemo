# MineSweeperDemo

这是我在 Bilibili 发布的教程  
**【Qt6.8 从零到一实现经典扫雷游戏的全过程】**  
https://www.bilibili.com/video/BV1FcXjBGEMX?vd_source=5c1b12b66171a9862c79dcafb489f9b8  
对应的完整项目代码仓库。

## 项目简介

本项目基于 **Qt Widgets + C++17** 实现经典扫雷，包含以下核心功能：

- 三档难度切换：简单（9x9, 10雷）、中等（16x16, 40雷）、高级（16x30, 99雷）
- 首击安全：第一次点击及其周围 8 格不会生成地雷
- 空白区域自动连锁翻开（Flood Fill）
- 右键循环标记：旗帜 -> 问号 -> 取消标记
- 和弦操作：已翻开数字格满足旗帜数量后可一键翻开周围格子
- 胜负判定与自动重开

## 代码结构说明

主要代码位于 `MineSweeper/` 目录：

- `main.cpp`  
  程序入口，创建 `QApplication` 并启动主窗口。

- `minesweeper.h/.cpp/.ui`  
  主窗口层，负责组合标题栏和棋盘，并处理难度切换时的窗口尺寸与棋盘刷新。

- `gameconfig.h/.cpp`  
  全局配置与参数管理：难度、行列数、地雷数、像素尺寸、8 邻域偏移与数字贴图路径。

- `gameboard.h/.cpp/.ui`  
  核心游戏逻辑：地雷生成、棋盘初始化、左/右键事件处理、连锁翻开、和弦翻开、胜负判定。

- `minecell.h/.cpp`  
  单格子组件（继承 `QPushButton`），封装格子状态、显示样式与鼠标手势识别逻辑。

- `gametitle.h/.cpp/.ui`  
  顶部信息栏，显示难度与剩余雷数，并提供难度菜单切换。

- `resources.qrc`  
  Qt 资源文件，管理数字、地雷、红旗、问号等图片资源。

- `CMakeLists.txt`  
  工程构建脚本，支持 Qt6（优先）与 Qt5（回退）。

## 本地构建运行

在项目根目录执行（示例）：

```bash
cmake -S MineSweeper -B build
cmake --build build --config Release
```

构建完成后运行生成的 `MineSweeper` 可执行文件即可。
