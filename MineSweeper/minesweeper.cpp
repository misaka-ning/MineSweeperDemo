#include "minesweeper.h"
#include "./ui_minesweeper.h"

#include <QPoint>
#include <QSize>

MineSweeper::MineSweeper(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MineSweeper)
{
    ui->setupUi(this);

    MineSweeperInit();
}

MineSweeper::~MineSweeper()
{
    qDebug() << "fine";
    delete ui;
}

void MineSweeper::MineSweeperInit()
{
    SetWindowSize();

    m_gameboard = new GameBoard(this);     // 构造棋盘对象
    m_gameboard->move(QPoint(0, GameConfig::titlePixel()));       // 移动到左上角
    m_gameboard->show();                   // 显示棋盘

    m_gametitle = new GameTitle(this);
    m_gametitle->move(0, 0);
    m_gametitle->show();

    connect(m_gametitle, &GameTitle::difficultyChangRequested, this, [=](GameConfig::Difficulty diff){
        GameConfig::setDifficulty(diff);
        setFixedSize(QSize(GameConfig::colCount() * GameConfig::cellPixel(), GameConfig::rowCount() * GameConfig::cellPixel() + GameConfig::titlePixel()));
        m_gametitle->setFixedSize(QSize(GameConfig::colCount() * GameConfig::cellPixel(), GameConfig::titlePixel()));
        m_gameboard->GameBoardupdate();
    });

    connect(m_gameboard, &GameBoard::MarkTheChess, this, [=](int n){
        m_gametitle->updateRemaininglabel(GameConfig::mineCount() - n);
    });
}

// 根据游戏设置初始化窗口大小
void MineSweeper::SetWindowSize()
{
    setFixedSize(QSize(GameConfig::colCount() * GameConfig::cellPixel(), GameConfig::rowCount() * GameConfig::cellPixel() + GameConfig::titlePixel()));
}
