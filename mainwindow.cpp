#include "mainwindow.h"

#include <Windows.h>
#include <Shellapi.h>

#include <QMenuBar>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSlider>
#include <QFileInfo>
#include <QLineEdit>
#include <QMediaPlayer>
#include <QDebug>
#include <QTextDocument>
#include <QGroupBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      durationSlider(new QSlider(Qt::Horizontal)),
      durationLabel(new QLabel("-- / --")),
      volumeSlider(new QSlider(Qt::Vertical)),
      volumeLabel(new QLabel("100")),
      player(new QMediaPlayer(this))
{
    setupActions();

    QMenuBar *bar = menuBar();

    bar->addAction(actOpen);
    bar->addAction(actSettings);

    QMenu *controls = bar->addMenu("&Controls");
    controls->addActions({actionPause, actionBackw, actionForward});

    QMenu *help = bar->addMenu("&Help");
    help->addActions({actAboutMe, actAboutQt});

    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(durationSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    connect(volumeSlider, &QSlider::valueChanged, [this](int val) { this->volumeLabel->setText(QString::number(val)); });
    connect(volumeSlider, &QSlider::valueChanged, player, &QMediaPlayer::setVolume);


    durationSlider->setMinimum(0);

    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setValue(100);

    volumeSlider->setMaximumHeight(durationSlider->sizeHint().height()*2);

    QPushButton *openFileBtn = new QPushButton(QIcon(":/icons/folder.png"), "");
    connect(openFileBtn, &QPushButton::clicked, actOpen, &QAction::trigger);
    openFileBtn->setFlat(true);

    QPushButton *pauseBtn = new QPushButton(QIcon(":/icons/Play-48.png"), "");
    connect(pauseBtn, &QPushButton::clicked, actionPause, &QAction::trigger);
    connect(player, &QMediaPlayer::stateChanged, [pauseBtn](auto state){
        if (state == QMediaPlayer::PlayingState) {
            pauseBtn->setIcon(QIcon(":/icons/Pause-48.png"));
        }
        else {
            pauseBtn->setIcon(QIcon(":/icons/Play-48.png"));
        }
    });
    pauseBtn->setFlat(true);

    QPushButton *backwBtn = new QPushButton(QIcon(":/icons/Rewind-48.png"), "");
    connect(backwBtn, &QPushButton::clicked, actionBackw, &QAction::trigger);
    backwBtn->setFlat(true);

    QPushButton *forwardBtn = new QPushButton(QIcon(":/icons/Fast Forward-48.png"), "");
    connect(forwardBtn, &QPushButton::clicked, actionForward, &QAction::trigger);
    forwardBtn->setFlat(true);

    QWidget *mainWidg = new QWidget;

    QGroupBox *seekBox = new QGroupBox("Time seek");
    QGroupBox *volumeBox = new QGroupBox("Volume");

    QHBoxLayout *sliderLay = new QHBoxLayout;
    sliderLay->addWidget(durationSlider);
    sliderLay->addWidget(durationLabel);

    seekBox->setLayout(sliderLay);

    QHBoxLayout *volumeLay = new QHBoxLayout;
    volumeLay->addWidget(volumeSlider);
    volumeLay->addWidget(volumeLabel);

    volumeBox->setLayout(volumeLay);
    volumeBox->setMinimumWidth(volumeBox->sizeHint().width());

    QHBoxLayout *btnLay = new QHBoxLayout;
    btnLay->addWidget(openFileBtn);
    btnLay->addWidget(backwBtn);
    btnLay->addWidget(pauseBtn);
    btnLay->addWidget(forwardBtn);

    QHBoxLayout *boxesLay = new QHBoxLayout;
    boxesLay->addWidget(seekBox);
    boxesLay->addWidget(volumeBox);

    QVBoxLayout *lay = new QVBoxLayout;
    lay->addLayout(boxesLay);
    lay->addLayout(btnLay);
    mainWidg->setLayout(lay);
    setCentralWidget(mainWidg);

    this->setWindowTitle(appName);

    auto flags = this->windowFlags();
    if (flags | Qt::WindowMaximizeButtonHint) {
        flags ^= Qt::WindowMaximizeButtonHint;
    }
    this->setWindowFlags(flags);


    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    registerHotkey(pauseId, MOD_ALT, 0x51);//Q
    registerHotkey(backwId, MOD_ALT, 0x57);//W
    registerHotkey(fwdId, MOD_ALT, 0x45);//E
}

MainWindow::~MainWindow()
{
    UnregisterHotKey(0, pauseId);
    UnregisterHotKey(0, backwId);
    UnregisterHotKey(0, fwdId);

    CoUninitialize();
}

void MainWindow::registerHotkey(int id, uint mod, uint key) const
{
    uchar keyCode = key & 0xFF;
    BOOL res = 0;
    if (keyCode != 0) {
        res = RegisterHotKey(0, id, mod, keyCode);
    }
    if (res == 0) {
        QMessageBox::critical(0,"Error",QString("Error occured while registering new hotkey ")
                              + QString::number(id) + "!");
    }
    qDebug () << res;
}

void MainWindow::onFileOpen()
{
    QString fname = QFileDialog::getOpenFileName(this, "Open media file", QString(), "Media (*.mp3 *.wma)");
    if (!fname.length()) {
        return;
    }
    QFileInfo info(fname);
    this->setWindowTitle(info.fileName() + " - " + appName);

    player->setMedia(QUrl::fromLocalFile(fname));
    player->play();
}

void MainWindow::onPause()
{
    qDebug() << "Triggered";
    if (player->state() == QMediaPlayer::PausedState) {
        qDebug() << "Play";
        player->play();
    }
    else
    {
        qDebug() << "Pause";
        player->pause();
    }
}

void MainWindow::onDurationChanged(qint64 dur)
{
    int secs = dur/1000;
    int mins = secs / 60;
    secs %= 60;
    wholeTime = QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
    durationSlider->setMaximum(dur);
}

void MainWindow::onPositionChanged(qint64 pos)
{
    int secs = pos/1000;
    int mins = secs / 60;
    secs %= 60;
    durationLabel->setText(QString("%1:%2 / ").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0')) + this->wholeTime);
    durationSlider->setValue(pos);
}

void MainWindow::onMoveBackwards()
{
    auto pos = player->position();
    if (pos < step*1000) {
        this->player->setPosition(0);
    }
    else {
        this->player->setPosition(pos - step*1000);
    }
}

void MainWindow::onMoveForward()
{
    auto dur = player->duration();
    auto pos = player->position();
    if (dur < pos + step*1000) {
        this->player->setPosition(dur);
    }
    else {
        this->player->setPosition(pos + step*1000);
    }
}

void MainWindow::onAboutMe() const
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("About");
    aboutBox.setTextFormat(Qt::RichText);
    QString aboutTxt = QStringLiteral(
R"(
%1
Simple mp3 player with system-wide hotkeys.

Hotkeys:
    Play/Pause: Alt+Q
    Step backward: Alt+W
    Step forward: Alt+E

%2

This program is released under the MIT License.
Copyright (C) 2016-2017 nkbolg
)").arg(appName);
    QString repo_url = QStringLiteral("<a href='https://github.com/nkbolg/hotkey_player'>https://github.com/nkbolg/hotkey_player</a>");
    QString aboutHtml = Qt::convertFromPlainText(aboutTxt).arg(repo_url);
    aboutBox.setText(aboutHtml);
    aboutBox.exec();
}

void MainWindow::onSettings()
{
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle("Settings");

    QVBoxLayout *lay = new QVBoxLayout;

    QFormLayout *form = new QFormLayout;
    QLineEdit *le = new QLineEdit(QString::number(step));
    form->addRow("Step size:", le);

    QHBoxLayout *btnLay = new QHBoxLayout;
    QPushButton *btn = new QPushButton("OK");
    connect(btn, &QPushButton::clicked, &settingsDialog, &QDialog::accept);
    btnLay->addWidget(btn, 0, Qt::AlignRight);

    lay->addLayout(form);
    lay->addLayout(btnLay);

    settingsDialog.setLayout(lay);
    if (settingsDialog.exec() == QDialog::Accepted) {
        bool convertStatus = false;
        int newStep = 0;
        newStep = le->text().toInt(&convertStatus);
        if (convertStatus) {
            step = newStep;
        }
        else {
            QMessageBox::warning(this, "Warning", "Incorrect number");
        }
    }
}

bool MainWindow::activateHotkey(uint hotkeyId) const
{
    switch (hotkeyId) {
    case pauseId:
        actionPause->trigger();
        return true;
    case backwId:
        actionBackw->trigger();
        return true;
    case fwdId:
        actionForward->trigger();
        return true;
    default:
        qDebug () << "Switch def";
        return false;
    }
}

void MainWindow::setupActions()
{
    actOpen = new QAction("Open", this);
    connect(actOpen, &QAction::triggered, this, &MainWindow::onFileOpen);

    actionPause = new QAction("Pause", this);
    connect(actionPause, &QAction::triggered, this, &MainWindow::onPause);

    actionBackw = new QAction("Backward", this);
    connect(actionBackw, &QAction::triggered, this, &MainWindow::onMoveBackwards);

    actionForward = new QAction("Forward", this);
    connect(actionForward, &QAction::triggered, this, &MainWindow::onMoveForward);

    actAboutMe = new QAction("About", this);
    connect(actAboutMe, &QAction::triggered, this, &MainWindow::onAboutMe);

    actAboutQt = new QAction("About Qt", this);
    connect(actAboutQt, &QAction::triggered, [this]{QMessageBox::aboutQt(this);});

    actSettings = new QAction("Settings", this);
    connect(actSettings, &QAction::triggered, this, &MainWindow::onSettings);
}

bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    const QByteArray winMsgStr = "windows_generic_MSG";
    if (winMsgStr == eventType)
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            return activateHotkey(msg->wParam);
        }
    }
    return false;
}
