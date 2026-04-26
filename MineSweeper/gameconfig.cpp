#include "gameconfig.h"

// 初始化静态成员变量
GameConfig::Difficulty GameConfig::s_currentDifficulty = USER_DEFAULTMODE;
int GameConfig::s_cellPixel = USER_CELLPIXEL;
int GameConfig::s_rowCount  = 16;
int GameConfig::s_colCount = 16;
int GameConfig::s_mineCount = 40;
int GameConfig::s_titlePixel = USER_TITLEPIXEL;

// 8方向偏移量 固定值：上、下、左、右、左上、右上、左下、右下
const QVector<int> GameConfig::s_dx = { -1, 1, 0, 0, -1, -1, 1, 1 };
const QVector<int> GameConfig::s_dy = { 0, 0, -1, 1, -1, 1, -1, 1 };

// 初始化图片路径数组
const QVector<QString> GameConfig::s_imagepath = { "",
    ":/res/num1.png", ":/res/num2.png", ":/res/num3.png", ":/res/num4.png",
    ":/res/num5.png", ":/res/num6.png", ":/res/num7.png", ":/res/num8.png", ":/res/mine.png"};

// 通过周围地雷数量，返回对应图片路径字符串
const QString GameConfig::iamgepath(int n)
{
    if(n >= 0 && n < s_imagepath.size()) return s_imagepath[n];
    else return "";
}

// 获取8邻域行偏移数组
const QVector<int>& GameConfig::dx() { return s_dx; }
// 获取8邻域列偏移数组
const QVector<int>& GameConfig::dy() { return s_dy; }

void GameConfig::setDifficulty(Difficulty diff)
{
    s_currentDifficulty = diff;
    // 根据选择的游戏难度，切换对应的行、列、地雷数
    switch (diff)
    {
        case EASY:
        {
            s_rowCount = 9;
            s_colCount = 9;
            s_mineCount = 10;
        }
        break;
        case MEDIUM:
        {
            s_rowCount = 16;
            s_colCount = 16;
            s_mineCount = 40;
        }
        break;
        case HARD:
        {
            s_rowCount = 16;
            s_colCount = 30;
            s_mineCount = 99;
        }
        break;
    }
}
