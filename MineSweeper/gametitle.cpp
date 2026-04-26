#include "gametitle.h"
#include "ui_gametitle.h"

#include <QStyle>
#include <QMenu>
#include <QPoint>

GameTitle::GameTitle(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameTitle)
{
    ui->setupUi(this);

    GameTitleInit();
}

GameTitle::~GameTitle()
{
    delete ui;
}

void GameTitle::updateDifficultylabel(GameConfig::Difficulty diff)
{
    if(diff == GameConfig::EASY) ui->difficultylabel->setText("简单");
    else if(diff == GameConfig::MEDIUM) ui->difficultylabel->setText("中等");
    else if(diff == GameConfig::HARD) ui->difficultylabel->setText("困难");
}

void GameTitle::updateRemaininglabel(int num)
{
    ui->remaininglabel->setText(QString::number(num));
}

void GameTitle::GameTitleInit()
{
    setFixedHeight(GameConfig::titlePixel());
    setFixedWidth(GameConfig::cellPixel() * GameConfig::colCount());

    // ui->settingBtn->setText(QStringLiteral(""));
    ui->settingBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    ui->settingBtn->setFixedSize(30, 30);
    connect(ui->settingBtn, &QPushButton::clicked, this, [=](){
        QMenu menu(this);
        QAction *easy = menu.addAction(QStringLiteral("简单 (9x9, 10雷)"));
        QAction *medium = menu.addAction(QStringLiteral("中等 (16x16, 40雷)"));
        QAction *hard = menu.addAction(QStringLiteral("高级 (16x30, 99雷)"));
        QAction *chosen = menu.exec(ui->settingBtn->mapToGlobal(QPoint(0, ui->settingBtn->height())));
        if(!chosen) return;
        if(chosen == easy) difficultyChangRequested(GameConfig::EASY);
        else if(chosen == medium) difficultyChangRequested(GameConfig::MEDIUM);
        else if(chosen == hard) difficultyChangRequested(GameConfig::HARD);
    });

    updateDifficultylabel(GameConfig::GetDifficulty());
    updateRemaininglabel(GameConfig::mineCount());
}
