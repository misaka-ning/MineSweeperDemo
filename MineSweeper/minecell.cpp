#include "minecell.h"

MineCell::MineCell(int num, QPoint point, QWidget *parent)
    : QPushButton{parent}, m_num(num), m_position(point), m_state(COVERED)
{
    MineInit();
}

void MineCell::MineInit()
{
    // 设置棋子大小
    setFixedSize(QSize(GameConfig::cellPixel(), GameConfig::cellPixel()));

    // 更新图片
    UpdateImage();
}

// 根据当前棋子状态更新对应图片
void MineCell::UpdateImage()
{
    switch (m_state)
    {
    case (COVERED):       // 未翻开,加载空图片
        m_image = QIcon();
        break;
    case (UNCOVERED):     // 已翻开，加载对应数字图片或地雷图片
        m_image = QIcon(GameConfig::iamgepath(m_num));
        break;
    case (FLACGED):       // 标记为红旗
        m_image = QIcon(":/res/redflag.png");
        break;
    case (QUESTIONED):    // 标记为问号
        m_image = QIcon(":/res/quemark.png");
        break;
    }
    // 设置图片以及更改图片大小
    setIcon(m_image);
    setIconSize(QSize(25, 25));

    // 更改样式效果（qss）
    UpdateVisualStyle();
}

//
void MineCell::UpdateVisualStyle()
{
    QString cellStyle; // 样式文本

    if(m_state == UNCOVERED) // 已翻开
    {
        if(m_num == 9) // 翻开并且是地雷
            cellStyle = "QPushButton { border: 1px solid #888888; background-color: #FFD0D0;}";
        else           // 翻开但不是地雷
            cellStyle = "QPushButton { border: 1px solid #888888; background-color: #D0D0D0;}";
    }
    else // 未翻开、红旗
    {
        cellStyle = "QPushButton { border: 2px outset #808080; background-color: #BFBFBF;}"
                    "QPushButton:hover { background-color: #B0B0B0; }"
                    "QPushButton:pressed { border: 1px inset #707070; background-color: #A0A0A0;}";
    }

    setStyleSheet(cellStyle); // 应用样式文本
}

// 翻开棋子
void MineCell::FlippingMine()
{
    m_state = UNCOVERED;
    UpdateImage();
}

void MineCell::ToggleMark()
{
    if(m_state == UNCOVERED) return; // 如果是已翻开，就不能右键切换标记

    if(m_state == COVERED) m_state = FLACGED;
    else if(m_state == FLACGED) m_state = QUESTIONED;
    else if(m_state == QUESTIONED) m_state = COVERED;

    UpdateImage();
}

int MineCell::GetNum()
{
    return m_num;
}

CellState MineCell::GetState()
{
    return m_state;
}

bool MineCell::IsMine() const
{
    return m_num == 9;
}

bool MineCell::CanFlip() const
{
    return m_state == COVERED;
}

void MineCell::SetState(CellState state)
{
    m_state = state;
}

void MineCell::mousePressEvent(QMouseEvent *event)
{
    // 只处理左右键，其他事件交还给父类
    if(event->button() != Qt::LeftButton && event->button() != Qt::RightButton)
    {
        QPushButton::mousePressEvent(event);
        return;
    }

    // 只认可在棋子内按下
    const bool inside = rect().contains(event->position().toPoint());
    if(!inside)
    {
        event->ignore();
        return;
    }

    // 根据棋子是否翻开，选择允许的交互类型
    // 未翻开(或已标记)->允许左键单击与右键单击，实现翻开、标记功能
    // 已翻开->允许和弦功能
    const bool uncovered = (m_state == UNCOVERED);

    if(!m_gesture.active) // 如果开未开始一轮跟踪手势，就复位并开始
    {
        m_gesture.reset();
        m_gesture.active = true;
    }

    if(!uncovered) // 未翻开或已标记
    {
        // 未翻开不允许两键同时进入，没有和弦功能
        if((event->button() == Qt::LeftButton && m_gesture.rightStarted) || (event->button() == Qt::RightButton && m_gesture.leftStarted))
        {
            m_gesture.invalid = true;
            m_gesture.chordvalid = false; // 顺手把和弦禁用
            event->accept();
            return;
        }

        // 记录按下了哪个按键
        if(event->button() == Qt::LeftButton)
        {
            m_gesture.leftStarted = true;
            m_gesture.leftDown = true;

            setDown(true);
        }
        else  // event->button() == Qt::RightButton
        {
            m_gesture.rightStarted = true;
            m_gesture.rightDown = true;
            // 右键不需要setDown
        }
        event->accept();
        return;
    }
    else // 已翻开，判断和弦操作
    {
        if(event->button() == Qt::LeftButton)
        {
            // 和弦第一步：按下左键
            m_gesture.leftStarted = true;
            m_gesture.leftDown = true;
            setDown(true);
            event->accept();
            return;
        }
        else // event->button() == Qt::RightButton
        {
            if(m_gesture.leftStarted && m_gesture.leftDown)
            {
                // 和弦第二步，按下左键同时按下右键
                m_gesture.rightStarted = true;
                m_gesture.rightDown = true;

                m_gesture.chordEligible = true;  // 具备和弦资格
            }
            else // 如果右键按下但左键没有按下，即单击右键，已翻开棋子不处理
            {
                m_gesture.invalid = true;
                m_gesture.chordvalid = false;
            }
        }
    }
    event->accept();
}

void MineCell::mouseMoveEvent(QMouseEvent *event)
{
    // 只有正在跟踪手势时才关心move事件
    if(!m_gesture.active)
    {
        QPushButton::mouseMoveEvent(event);
        return;
    }

    // 当按住按钮时鼠标移出棋子之外，标记为出界
    const bool inside = rect().contains(event->position().toPoint());
    if(!inside)
    {
        m_gesture.invalid = true;
        m_gesture.chordvalid = false;

        setDown(false); // 出界之后视觉效果恢复为正常
    }
    else
    {
        if(m_gesture.leftDown) setDown(true); // 若在按钮内却左键按下，保持按下的视觉效果
    }
    event->accept();
}

void MineCell::mouseReleaseEvent(QMouseEvent *event)
{
    // 只有左右键松开时才关心，其他事件交还给父类
    if(event->button() != Qt::LeftButton && event->button() != Qt::RightButton)
    {
        QPushButton::mouseReleaseEvent(event);
        return;
    }

    // 只有正在跟踪手势时才关心Release事件
    if(!m_gesture.active)
    {
        event->ignore();
        return;
    }

    // 松开时是否在棋子内也是触发条件的一部分

    const bool inside = rect().contains(event->position().toPoint());
    if(!inside)
    {
        m_gesture.invalid = true;
        m_gesture.chordvalid = false;
    }

    // 更新down状态
    if(event->button() == Qt::LeftButton)
    {
        m_gesture.leftDown = false;
        setDown(false);
    }
    else // Qt::RightButton
    {
        m_gesture.rightDown = false;
    }

    // 根据手势状态发射信号
    if(m_gesture.botUp())
    {
        const bool uncovered = (m_state == UNCOVERED);

        // 只有invalid为false时才发射信号
        if(!m_gesture.invalid)
        {
            if(!uncovered)
            {
                if(m_gesture.onlyLeftGesture()) emit sigLeftReleaseInside(m_position.x(), m_position.y());         // 左键单击翻开信号
                else if(m_gesture.onlyRightGesture()) emit sigRightReleaseInside(m_position.x(), m_position.y());  // 右键切换标记信号
            }
            else
            {
                if(m_gesture.chordEligible && m_gesture.chordvalid) emit sigchordReleaseInside(m_position.x(), m_position.y()); // 和弦请求信号
            }
        }
        m_gesture.reset();
    }
    event->accept();
}







