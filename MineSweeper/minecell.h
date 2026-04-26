#ifndef MINECELL_H
#define MINECELL_H

#include <QPushButton>
#include <QIcon>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QMouseEvent>

#include "gameconfig.h"

// 棋子状态枚举
enum CellState{
    COVERED,    // 未翻开
    UNCOVERED,  // 已翻开
    FLACGED,    // 标记为地雷
    QUESTIONED  // 标记为问号
};

class MineCell : public QPushButton
{
    Q_OBJECT
public:
    explicit MineCell(int num, QPoint point, QWidget *parent = nullptr);
    void FlippingMine();       // 翻开棋子
    void ToggleMark();         // 切换右键标记
    int GetNum();
    CellState GetState();
    bool IsMine() const;       // 判断此棋子是否为地雷
    bool CanFlip() const;      // 判断是否能翻开
    void SetState(CellState state);           // 设置状态


signals:
    // 左键按下并在按钮内松开发射信号
    void sigLeftReleaseInside(int y, int x);
    // 右键按下并在按钮内松开发射信号
    void sigRightReleaseInside(int y, int x);
    // 先左键按下，再按下右键（左键保持），最后在按钮内完成释放发射信号（触发扫雷的和弦操作）
    void sigchordReleaseInside(int y, int x);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int m_num;                 // 棋子数字（周围有几颗地雷）
    QIcon m_image;             // 棋子图片
    CellState m_state;         // 棋子状态
    QPoint m_position;         // 棋子位置（几行几列）

    void MineInit();           // 棋子初始化
    void UpdateImage();        // 更新图片
    void UpdateVisualStyle();  // 更新qss样式

    struct GestureState{
        bool active = false;      // 是否在跟踪一段手势
        bool invalid = false;     // 一旦离开棋子按钮区域，置为true

        bool leftStarted = false;  // 左键是否在本次跟踪中按下过
        bool rightStarted = false; // 右键是否在本次跟踪中按下过

        bool leftDown = false;     // 左键当前是否按下
        bool rightDown = false;    // 右键当前是否按下

        bool chordEligible = false; // 是否满足“先左键按下，再按下右键”
        bool chordvalid = true;     // 和弦过程中是否仍然有效

        void reset() {*this = GestureState{};}  // 重置结构体，开始新的跟踪

        bool botUp() const {return !leftDown && !rightDown;}
        bool onlyLeftGesture() const {return leftStarted && !rightStarted;}
        bool onlyRightGesture() const {return rightStarted && !leftStarted;}
    };

    GestureState m_gesture;     // 创建一个鼠标手势跟踪变量
};

#endif // MINECELL_H
