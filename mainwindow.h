#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void on_pushButtonAdd_clicked();

	void on_pushButtonPlay_clicked();

    void on_horizontalSliderVolume_valueChanged(int value);

private:
    Ui::MainWindow *ui;
	QMediaPlayer* m_player;
};
#endif // MAINWINDOW_H
