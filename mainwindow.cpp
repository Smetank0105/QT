#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	//          Buttons style:
	ui->pushButtonAdd->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
	ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
	ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->horizontalSliderVolume->setRange(0, 100);

	//          Player init:
	m_player = new QMediaPlayer();
	m_player->setVolume(70);
	ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
	ui->horizontalSliderVolume->setValue(m_player->volume());

    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
    connect(ui->horizontalSliderTime, &QSlider::sliderMoved, this, &MainWindow::on_horizontalSliderTime_sliderMoved);

    //          Playlist init:
    m_plist_model = new QStandardItemModel(this);
    this->ui->tableViewPList->setModel(m_plist_model);
    m_plist_model->setHorizontalHeaderLabels(QStringList() << "Audio Track" << "File Path" << "Duration");
    this->ui->tableViewPList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->tableViewPList->setSelectionBehavior(QAbstractItemView::SelectRows);

    this->ui->tableViewPList->hideColumn(1);
    int duration_width = 64;
    this->ui->tableViewPList->setColumnWidth(2,duration_width);
    this->ui->tableViewPList->setColumnWidth(0,this->ui->tableViewPList->width() - duration_width*1.7);

    m_plist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_plist);

    connect(this->ui->pushButtonPrev, &QPushButton::clicked, this->m_plist, &QMediaPlaylist::previous);
    connect(this->ui->pushButtonNext, &QPushButton::clicked, this->m_plist, &QMediaPlaylist::next);
    connect(this->m_plist, &QMediaPlaylist::currentIndexChanged, this->ui->tableViewPList, &QTableView::selectRow);
    connect(this->ui->tableViewPList, &QTableView::doubleClicked,
            [this](const QModelIndex& index){m_plist->setCurrentIndex(index.row());});
    connect(this->m_player, &QMediaPlayer::currentMediaChanged,
            [this](const QMediaContent& media)
            {
                    this->ui->labelFilename->setText(media.canonicalUrl().toString());
                    this->setWindowTitle(this->ui->labelFilename->text().split('/').last());
            });
}

MainWindow::~MainWindow()
{
    delete m_plist_model;
    delete m_plist;
	delete m_player;
    delete ui;
}

void MainWindow::LoadFileToPList(const QString &filename)
{
    m_plist->addMedia(QUrl(filename));
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QDir(filename).dirName()));
    items.append(new QStandardItem(filename));
    m_plist_model->appendRow(items);
}

void MainWindow::on_pushButtonAdd_clicked()
{
    /*QString file = QFileDialog::getOpenFileName
	(
		this,
		"Open file",
        "D:\\musik\\",
		"Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac (*.flac)"
	);
	ui->labelFilename->setText(QString("File: ").append(file));
	this->m_player->setMedia(QUrl(file));
    this->m_player->play();*/

    QStringList files = QFileDialog::getOpenFileNames
            (
                this,
                "Open file",
                "D:\\musik\\",
                "Audio files (*.mp3 *.flac *.flacc);; mp3 (*.mp3);; Flac (*.flac *.flacc)"
             );
    for(QString file:files)
    {
        LoadFileToPList(file);
    }
}

void MainWindow::on_pushButtonPlay_clicked()
{
	this->m_player->play();
}


void MainWindow::on_horizontalSliderVolume_valueChanged(int value)
{
    this->m_player->setVolume(value);
    ui->labelVolume->setText(QString("Volume: ").append((QString::number(m_player->volume()))));
}

void MainWindow::on_pushButtonPause_clicked()
{
    m_player->state() == QMediaPlayer::State::PausedState ? m_player->play() : this->m_player->pause();
}

void MainWindow::on_pushButtonStop_clicked()
{
    this->m_player->stop();
}

void MainWindow::on_pushButtonMute_clicked()
{
    this->m_player->setMuted(!m_player->isMuted());
    ui->pushButtonMute->setIcon(style()->standardIcon(m_player->isMuted()?QStyle::SP_MediaVolumeMuted:QStyle::SP_MediaVolume));
}

void MainWindow::on_durationChanged(qint64 duration)
{
    this->ui->horizontalSliderTime->setRange(0,duration);
    this->ui->labelDuration->setText(QTime::fromMSecsSinceStartOfDay(duration).toString("hh:mm:ss"));
}

void MainWindow::on_positionChanged(qint64 position)
{
    ui->labelPosition->setText(QString(QTime::fromMSecsSinceStartOfDay(position).toString("hh:mm:ss")));
    ui->horizontalSliderTime->setValue(position);
}

void MainWindow::on_horizontalSliderTime_sliderMoved(qint64 position)
{
    this->m_player->setPosition(position);
}

