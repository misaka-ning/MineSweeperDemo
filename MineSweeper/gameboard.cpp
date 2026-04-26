#include "gameboard.h"
#include "ui_gameboard.h"
#include <random>
#include <QMessageBox>
#include <QTimer>

GameBoard::GameBoard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameBoard)
{
    ui->setupUi(this);

    GameBoardInit();
}

GameBoard::~GameBoard()
{
    delete ui;
}

void GameBoard::GameBoardupdate()
{
    GameBoardInit();
}

void GameBoard::GameBoardInit()
{
    setFixedSize(QSize(GameConfig::colCount() * GameConfig::cellPixel(), GameConfig::rowCount() * GameConfig::cellPixel()));

    // 1、初始化地雷数组
    // MineArrInit();
    m_firstclick = true;
    m_gameover = false;
    m_opencnt = 0;
    m_markcnt = 0;
    emit MarkTheChess(m_markcnt);
    // 第一次初始化棋子数组需要地雷数组，所以先创建一个空的地雷数组
    m_minearray = QVector<QVector<bool>>(GameConfig::colCount(), QVector<bool>(GameConfig::rowCount(), false));


    // 2、根据周围的地雷个数，初始化棋子数组
    CellArrInit();
}

void GameBoard::MineArrInit(int y, int x)
{
    // 重写地雷初始化函数，确保传入的坐标点以及其周围没有地雷
    m_minearray = QVector<QVector<bool>>(GameConfig::colCount(), QVector<bool>(GameConfig::rowCount(), false));

    // 判断是否为参数坐标的八个棋子之内
    auto isProtected = [&](int col, int row) -> bool
    {
        return (std::abs(col - y) <= 1) && (std::abs(row - x) <= 1);
    };

    QVector<QPoint> temp;
    temp.reserve(GameConfig::colCount() * GameConfig::rowCount());

    for(int col = 0; col < GameConfig::colCount(); ++col)
    {
        for(int row = 0; row < GameConfig::rowCount(); ++row)
        {
            if(isProtected(col, row)) continue;
            temp.push_back(QPoint(col, row));
        }
    }

    const int need = GameConfig::mineCount();
    if(need > temp.size()) return;

    std::random_device rd;    // 创建一个随机数生成器
    std::mt19937 g(rd());     // 创建一个梅森旋转算法的伪随机数生成器对象
    std::shuffle(temp.begin(), temp.end(), g);  // 洗牌算法打乱一维数组

    for(int i = 0; i < need; ++i)
    {
        const QPoint p = temp[i];
        m_minearray[p.x()][p.y()] = true;     // 注意这里当初的x()保存的就是col，所以没有问题
    }
}

void GameBoard::CellArrInit()
{
    for(int col = 0; col < m_cellarray.size(); ++col) // 由于此时游戏的行列已经与上一轮的棋盘大小不同，所以这里要用m_cellarray.size来获取实际要释放的范围
    {
        for(int row = 0; row < m_cellarray[col].size(); ++row)
        {
            // 如果还有棋子对象没有被释放，先释放之前创建的棋子对象
            if(m_cellarray[col][row])
            {
                m_cellarray[col][row]->deleteLater(); // 延迟删除，避免事件栈中的旧对象立即被销毁导致崩溃
                m_cellarray[col][row] = nullptr;
            }
        }
    }
    m_cellarray.clear();

    m_cellarray.resize(GameConfig::colCount(), QVector<MineCell*>(GameConfig::rowCount()));
    for(int col = 0; col < GameConfig::colCount(); ++col)
    {
        for(int row = 0; row < GameConfig::rowCount(); ++row)
        {
            if(m_minearray[col][row]) m_cellarray[col][row] = new MineCell(9, QPoint(col, row), this);
            else m_cellarray[col][row] = new MineCell(GetMineCnt(col, row), QPoint(col, row), this);
            m_cellarray[col][row]->move(col * GameConfig::cellPixel(), row * GameConfig::cellPixel());
            // connect(m_cellarray[col][row], &QPushButton::clicked, this, [=](){m_cellarray[col][row]->FlippingMine();});
            connect(m_cellarray[col][row], &MineCell::sigLeftReleaseInside, this, &GameBoard::onCellLeftReleaseInside);
            connect(m_cellarray[col][row], &MineCell::sigRightReleaseInside, this, &GameBoard::onCellRightReleaseInside);
            connect(m_cellarray[col][row], &MineCell::sigchordReleaseInside, this, &GameBoard::onCellCordReleaseInside);
            m_cellarray[col][row]->show();
        }
    }
}

int GameBoard::GetMineCnt(int y, int x)
{
    int cnt = 0;
    for(int i = 0; i < 8; ++i)
    {
        int row = x + GameConfig::dx().at(i);
        int col = y + GameConfig::dy().at(i);
        if(IsOut(col, row)) continue;
        if(m_minearray[col][row]) cnt++;
    }
    return cnt;
}

void GameBoard::ChainFlipping(int y, int x)
{
    m_cellarray[y][x]->FlippingMine();
    m_opencnt++;
    if(m_minearray[y][x] != 0) return;
    for(int i = 0; i < 8; ++i)
    {
        int row = x + GameConfig::dx().at(i);
        int col = y + GameConfig::dy().at(i);
        if(IsOut(col, row)) continue;
        ChainFlipping(col, row);
    }
}

// 判断行列坐标是否出界
bool GameBoard::IsOut(int col, int row) const
{
    if(row < 0 || col < 0 || row >= GameConfig::rowCount() || col >= GameConfig::colCount()) return true;
    return false;
}

void GameBoard::FloodFill(int y, int x)
{
    if(!m_cellarray[y][x]->CanFlip()) return;
    m_cellarray[y][x]->FlippingMine();
    m_opencnt++;

    if(m_cellarray[y][x]->GetNum() == 0)
    {
        for(int i = 0; i < 8; ++i)
        {
            int row = x + GameConfig::dx().at(i);
            int col = y + GameConfig::dy().at(i);
            if(IsOut(col, row)) continue;
            FloodFill(col, row);
        }
    }
    CheckWin();
}

// 统计周围被标记为地雷的数量
int GameBoard::CountAdjacentFlags(int y, int x) const
{
    int cnt = 0;
    for(int i = 0; i < 8; ++i)
    {
        int row = x + GameConfig::dx().at(i);
        int col = y + GameConfig::dy().at(i);
        if(IsOut(col, row)) continue;
        if(m_cellarray[col][row]->GetState() == FLACGED) ++cnt;
    }
    return cnt;
}

void GameBoard::RevealAroundIfChordSatisfied(int y, int x)
{
    MineCell *center = m_cellarray[y][x];
    if(center->GetState() != UNCOVERED) return;

    const int centerNum = center->GetNum();
    if(centerNum <= 0 || centerNum > 8) return;
    if(CountAdjacentFlags(y, x) != centerNum) return;

    for(int i = 0; i < 8; ++i)
    {
        int row = x + GameConfig::dx().at(i);
        int col = y + GameConfig::dy().at(i);
        if(IsOut(col, row)) continue;

        if(m_cellarray[col][row]->GetState() == FLACGED || m_cellarray[col][row]->GetState() == UNCOVERED) continue;

        if(m_cellarray[col][row]->IsMine())
        {
            EndGame(false);
            return;
        }

        if(m_cellarray[col][row]->GetNum() == 0)
        {
            FloodFill(col, row);
        }
        else
        {
            m_cellarray[col][row]->FlippingMine();
            m_opencnt++;
            CheckWin();
        }
    }
}

void GameBoard::EndGame(bool won)
{
    // 游戏结束处理函数
    // won->true  代表游戏胜利
    // won->false 代表游戏失败
    if(m_gameover) return;
    m_gameover = true;

    if(!won)
    {
        // 翻开所有地雷
        for(int col = 0; col < GameConfig::colCount(); ++col)
        {
            for(int row = 0; row < GameConfig::rowCount(); ++row)
            {
                if(!m_cellarray[col][row]->IsMine()) continue;

                // 只有未翻开状态才能翻开，被标记地雷或问号不能翻开
                if(m_cellarray[col][row]->GetState() != CellState::UNCOVERED) m_cellarray[col][row]->SetState(CellState::UNCOVERED);
                m_cellarray[col][row]->FlippingMine();
            }
        }
    }

    if(won) QMessageBox::information(this, "游戏胜利", "恭喜你，点击确定后重新开始");
    else QMessageBox::information(this, "游戏失败", "你踩到地雷了，点击确定后重新开始");

    QTimer::singleShot(0, this, [this](){
        GameBoardInit();
    });
}

void GameBoard::CheckWin()
{
    if(m_opencnt >= GameConfig::colCount() * GameConfig::rowCount() - GameConfig::mineCount())
        EndGame(true);
}

void GameBoard::onCellLeftReleaseInside(int y, int x)
{
    if(m_gameover) return;
    // 当游戏开始时，第一次点击应该保证第一次点击的位置以及其周围八个棋子没有地雷
    // 所以真正的棋盘初始化应该在这里开始
    if(m_firstclick)
    {
        MineArrInit(y, x);
        CellArrInit();
        m_firstclick = false;
        m_markcnt = 0;
        emit MarkTheChess(m_markcnt);
    }

    // 点击地雷->游戏结束
    // 点击空白->连锁翻开
    // 点击既不是空棋子也不是地雷->正常翻开

    MineCell *cell = m_cellarray[y][x];
    if(!cell->CanFlip()) return; // 点击已经翻开的棋子就不用管了
    if(cell->IsMine())
    {
        EndGame(false);
        return;
    }
    if(cell->GetNum() == 0)
    {
        FloodFill(y, x);
    }
    else if(cell->GetNum() > 0 && cell->GetNum() <= 8)
    {
        cell->FlippingMine();
        m_opencnt++;
        CheckWin();
    }
}

void GameBoard::onCellRightReleaseInside(int y, int x)
{
    if(m_gameover) return;
    if(m_cellarray[y][x]->GetState() == FLACGED) m_markcnt--;
    m_cellarray[y][x]->ToggleMark();
    if(m_cellarray[y][x]->GetState() == FLACGED) m_markcnt++;
    emit MarkTheChess(m_markcnt);
}

void GameBoard::onCellCordReleaseInside(int y, int x)
{
    if(m_gameover) return;
    RevealAroundIfChordSatisfied(y, x);
}
