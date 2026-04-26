#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QVector>

#include "minecell.h"
#include "gameconfig.h"

namespace Ui {
class GameBoard;
}

class GameBoard : public QWidget
{
    Q_OBJECT

signals:
    void MarkTheChess(int n);

public:
    explicit GameBoard(QWidget *parent = nullptr);
    ~GameBoard();
    void GameBoardupdate();

private:
    Ui::GameBoard *ui;
    QVector<QVector<bool>> m_minearray;
    QVector<QVector<MineCell*>> m_cellarray;
    bool m_firstclick;                 // 第一次点击
    bool m_gameover;                   // 游戏是否结束
    int m_opencnt;                     // 统计翻开的棋子数
    int m_markcnt;                     // 标记的棋子数

    void GameBoardInit();              // 游戏棋盘初始化
    void MineArrInit(int y, int x);    // 地雷数组初始化
    void CellArrInit();                // 棋盘数组初始化
    int GetMineCnt(int y, int x);      // 获取周围8格地雷数量
    void ChainFlipping(int y, int x);  // 连锁翻开空白区域函数
    inline bool IsOut(int col, int row) const;

    void FloodFill(int y, int x);                   // 批量翻开空白棋子
    int CountAdjacentFlags(int y, int x) const;     // 统计周围旗帜数
    void RevealAroundIfChordSatisfied(int y, int x); // 和弦翻开
    void EndGame(bool won);                         // 结束游戏
    void CheckWin();                                // 检查是否获胜

    void onCellLeftReleaseInside(int y, int x);     // 处理左键单击
    void onCellRightReleaseInside(int y, int x);    // 处理右键单击
    void onCellCordReleaseInside(int y, int x);     // 处理和弦操作
};

#endif // GAMEBOARD_H
