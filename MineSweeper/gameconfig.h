#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QString>
#include <QVector>

// 默认参数设置
#define USER_CELLPIXEL      50                     // 单个棋子所占的像素大小
#define USER_TITLEPIXEL     80                     // 标题栏所占像素高度
#define USER_DEFAULTMODE    MEDIUM                 // 默认难度

class GameConfig
{
public:
    // 扫雷难度枚举
    enum Difficulty{
        EASY,         // 简单 （9x9）  （10地雷）
        MEDIUM,       // 中级 （16x16）（40地雷）（默认）
        HARD          // 高级 （16x30）（99地雷）
    };
    static const QString iamgepath(int n);        // 获取对应图片路径
    static void setDifficulty(Difficulty diff);   // 设置游戏难度
    static Difficulty GetDifficulty(){return s_currentDifficulty;};
    static int rowCount(){return s_rowCount;};    // 获取当前行数
    static int colCount(){return s_colCount;};    // 获取当前列数
    static int mineCount(){return s_mineCount;};  // 获取地雷个数
    static int cellPixel(){return s_cellPixel;};  // 获取棋子像素大小
    static int titlePixel(){return s_titlePixel;};// 获取标题栏像素高度
    // 获取8方向偏移量X轴
    static const QVector<int>& dx();
    // 获取8方向偏移量Y轴
    static const QVector<int>& dy();

private:
    static const QVector<QString> s_imagepath;    // 图片路径数组
    static Difficulty s_currentDifficulty;        // 当前选中难度
    static int s_rowCount;                        // 当前行数
    static int s_colCount;                        // 当前列数
    static int s_mineCount;                       // 当前地雷数
    static int s_cellPixel;                       // 当前棋子像素大小
    static int s_titlePixel;                      // 标题栏所占像素高度

    // 8个方向的偏移量 (扫雷核心，上下左右+四个斜角)
    static const QVector<int> s_dx;
    static const QVector<int> s_dy;
};

#endif // GAMECONFIG_H
