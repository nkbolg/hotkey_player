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
    void onPause();
    void onDurationChanged(qint64 dur);
    void onPositionChanged(qint64 pos);
    void onMoveBackwards();
    void onMoveForward();
    void onAboutMe();
    void onSettings();

private:
    bool activateHotkey(uint hotkeyId) const;

    void setupActions();

    QSlider *slider;
    QMediaPlayer *player;
    QLabel *duration;
    QAction *actOpen;
    QAction *actionPause;
    QAction *actionBackw;
    QAction *actionForward;
    QAction *actAboutMe;
    QAction *actAboutQt;
    QAction *actSettings;
    QString wholeTime;

    enum ids : uint
    {
        pauseId,
        backwId,
        fwdId
    };

    void registerHotkey(int id, uint mod, uint key);

    int step = 4;
    const QString appName = QLatin1String("Dumb mp3 player");
};

#endif // MAINWINDOW_H
