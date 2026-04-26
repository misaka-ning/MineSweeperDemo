#ifndef GAMETITLE_H
#define GAMETITLE_H

#include <QWidget>
#include "gameconfig.h"

namespace Ui {
class GameTitle;
}

class GameTitle : public QWidget
{
    Q_OBJECT

signals:
    void difficultyChangRequested(GameConfig::Difficulty diff);

public:
    explicit GameTitle(QWidget *parent = nullptr);
    ~GameTitle();
    void updateDifficultylabel(GameConfig::Difficulty diff);
    void updateRemaininglabel(int num);

private:
    Ui::GameTitle *ui;

    void GameTitleInit();
};

#endif // GAMETITLE_H
