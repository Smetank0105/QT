#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QTime>
#include <QMediaPlaylist>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void LoadFileToPList(const QString& filename);
    void SaveToFile();
    void LoadFromFile();

private slots:
	void on_pushButtonAdd_clicked();

	void on_pushButtonPlay_clicked();

    void on_horizontalSliderVolume_valueChanged(int value);

    void on_pushButtonPause_clicked();

    void on_pushButtonStop_clicked();

    void on_pushButtonMute_clicked();

    void on_durationChanged(qint64 duration);

    void on_positionChanged(qint64 position);

    void on_horizontalSliderTime_sliderMoved(qint64 position);

    void on_pushButtonShuffle_clicked();

    void on_checkBoxRepeate_stateChanged(int arg1);

    void on_pushButtonDEL_clicked();

    void on_pushButtonCLR_clicked();

    void on_pushButtonAddDir_clicked();

    void on_pushButtonLoop_clicked();

private:
    Ui::MainWindow *ui;
	QMediaPlayer* m_player;
    QMediaPlaylist* m_plist;
    QStandardItemModel* m_plist_model;

    bool shuffle;
    bool loop;
};
#endif // MAINWINDOW_H
