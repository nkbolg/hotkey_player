#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractNativeEventFilter>
class QSlider;
class QMediaPlayer;
class QLabel;
class QAction;

class MainWindow : public QMainWindow, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
private slots:
    void onFileOpen();

private:
    bool activateHotkey(uint hotkeyId) const;

    QSlider *slider;
    QMediaPlayer *player;
    QLabel *duration;
    QAction *actionPause;
    QAction *backw;
    QString wholeTime;

//    const uint pauseId = 0;
//    const uint backwId = 1;
    enum ids
    {
        pauseId,
        backwId
    };

    void regiserHotkey(int id, uint mod, uint key);
};

#endif // MAINWINDOW_H
