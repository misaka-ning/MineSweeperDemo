#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <QMainWindow>

#include "gameboard.h"
#include "gametitle.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MineSweeper;
}
QT_END_NAMESPACE

class MineSweeper : public QMainWindow
{
    Q_OBJECT

public:
    MineSweeper(QWidget *parent = nullptr);
    ~MineSweeper();

private:
    Ui::MineSweeper *ui;
    GameBoard *m_gameboard;  // 游戏棋盘
    GameTitle *m_gametitle;  // 标题栏

    void MineSweeperInit(); // 游戏初始化
    void SetWindowSize();   // 设置窗口大小
};
#endif // MINESWEEPER_H
