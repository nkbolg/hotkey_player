#include "mainwindow.h"

#include <Windows.h>
#include <Shellapi.h>

#include <QMenuBar>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>

#include <QMediaPlayer>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      slider(new QSlider(Qt::Horizontal)),
      player(new QMediaPlayer),
      duration(new QLabel("-- / --"))
{
    QToolBar *toolbar = this->addToolBar("Player actions");
    QAction *act = toolbar->addAction("Open");
    connect(act, &QAction::triggered, this, &MainWindow::onFileOpen);

    actionPause = toolbar->addAction("Pause");
    connect(actionPause, &QAction::triggered, [player = this->player]{
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
    });

    connect(player, &QMediaPlayer::durationChanged, [this](qint64 dur){
        int secs = dur/1000;
        int mins = secs / 60;
        secs %= 60;
        this->wholeTime = QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
        this->slider->setMinimum(0);
        this->slider->setMaximum(dur);
    });

    connect(player, &QMediaPlayer::positionChanged, [this](qint64 pos){
        int secs = pos/1000;
        int mins = secs / 60;
        secs %= 60;
        this->duration->setText(QString("%1:%2 / ").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0')) + this->wholeTime);
        this->slider->setValue(pos);
    });

    backw = toolbar->addAction("Backward");
    connect(backw, &QAction::triggered, [this]{
        int step = 4000;
        auto pos = this->player->position();
        if (pos < step) {
            pos = step;
        }
        this->player->setPosition(pos - step);
    });

    connect(slider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);

    QWidget *mainWidg = new QWidget;


    QHBoxLayout *sliderLay = new QHBoxLayout;
    sliderLay->addWidget(slider);
    sliderLay->addWidget(duration);

    QVBoxLayout *lay = new QVBoxLayout;
    lay->addLayout(sliderLay);
    mainWidg->setLayout(lay);
    setCentralWidget(mainWidg);

    this->setWindowTitle("Dumb mp3 player");


    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    regiserHotkey(pauseId, MOD_ALT, 0x51);//Q
    regiserHotkey(backwId, MOD_ALT, 0x57);//W
}

MainWindow::~MainWindow()
{
    CoUninitialize();
}

void MainWindow::regiserHotkey(int id, uint mod, uint key)
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
    player->setMedia(QUrl::fromLocalFile(fname));
    player->play();
}

bool MainWindow::activateHotkey(uint hotkeyId) const
{
    switch (hotkeyId) {
    case pauseId:
        this->actionPause->trigger();
        return true;
    case backwId:
        this->backw->trigger();
        return true;
    default:
        qDebug () << "Switch def";
        return false;
    }
}

bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    const QString winMsgStr = "windows_generic_MSG";
    if (winMsgStr == eventType) // windows_dispatcher_MSG
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            return activateHotkey(msg->wParam);
        }
    }
    return false;
}
