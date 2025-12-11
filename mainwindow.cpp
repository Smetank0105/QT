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
    ui->pushButtonLoop->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

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

    shuffle=false;
    loop=false;

    LoadFromFile();
}

MainWindow::~MainWindow()
{
    SaveToFile();
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

void MainWindow::SaveToFile()
{
//    QUrl file = QUrl::fromLocalFile("my_plist.m3u8");
//    m_plist->save(file, "m3u8");
    QFile file("my_plist.plist");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        for(int i = 0; i < m_plist->mediaCount(); i++)
        {
            QString path = m_plist->media(i).canonicalUrl().toString();
            out << path << endl;
        }
        file.close();
    }
}

void MainWindow::LoadFromFile()
{
//    QUrl file = QUrl::fromLocalFile("my_plist.plist");
//    m_plist->load(file, "plist");
//    for(int i =0; i < m_plist->mediaCount(); i++)
//    {
//        QUrl url = m_plist->media(i).canonicalUrl();
//        QList<QStandardItem*> items;
//        items.append(new QStandardItem(url.fileName()));
//        items.append(new QStandardItem(url.path()));
//        m_plist_model->appendRow(items);
//    }
    QFile file("my_plist.plist");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        while(!in.atEnd())
        {
            QString line = in.readLine();
            LoadFileToPList(line);
        }
        file.close();
    }
}

void MainWindow::on_pushButtonAdd_clicked()
{
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


void MainWindow::on_pushButtonShuffle_clicked()
{
//    this->m_plist_model->clear();

//    m_plist_model->setHorizontalHeaderLabels(QStringList() << "Audio Track" << "File Path" << "Duration");
//    this->ui->tableViewPList->hideColumn(1);
//    int duration_width = 64;
//    this->ui->tableViewPList->setColumnWidth(2,duration_width);
//    this->ui->tableViewPList->setColumnWidth(0,this->ui->tableViewPList->width() - duration_width*1.7);

//    this->m_plist->shuffle();

//    for(int i =0; i < m_plist->mediaCount(); i++)
//    {
//        QUrl url = m_plist->media(i).canonicalUrl();
//        QList<QStandardItem*> items;
//        items.append(new QStandardItem(url.fileName()));
//        items.append(new QStandardItem(url.path()));
//        m_plist_model->appendRow(items);
//    }

//    this->ui->tableViewPList->selectRow(m_plist->currentIndex());

    this->ui->pushButtonShuffle->setCheckable(true);
    shuffle = !shuffle;
    this->m_plist->setPlaybackMode(shuffle ? QMediaPlaylist::PlaybackMode::Random : QMediaPlaylist::PlaybackMode::Sequential);
    this->ui->pushButtonShuffle->setChecked(shuffle);
}


void MainWindow::on_checkBoxRepeate_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked) m_plist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    else m_plist->setPlaybackMode(QMediaPlaylist::Sequential);
}


void MainWindow::on_pushButtonDEL_clicked()
{
    while(ui->tableViewPList->selectionModel()->selectedRows().count())
    {
        m_plist->removeMedia(this->ui->tableViewPList->selectionModel()->selectedRows().first().row());
        m_plist_model->removeRow(this->ui->tableViewPList->selectionModel()->selectedRows().first().row());
    }
}


void MainWindow::on_pushButtonCLR_clicked()
{
    m_plist->clear();
    m_plist_model->clear();
    m_plist_model->setHorizontalHeaderLabels(QStringList() << "Audio Track" << "File Path" << "Duration");
    this->ui->tableViewPList->hideColumn(1);
    int duration_width = 64;
    this->ui->tableViewPList->setColumnWidth(2,duration_width);
    this->ui->tableViewPList->setColumnWidth(0,this->ui->tableViewPList->width() - duration_width*1.7);
}


void MainWindow::on_pushButtonAddDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory
            (
                this,
                "Open directory",
                "D:\\musik\\",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
             );
    if(!dir.isEmpty())
    {
        QDir directory(dir);
        directory.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        QStringList files = directory.entryList();
        for(QString file:files)
            LoadFileToPList(directory.filePath(file));
    }
}


void MainWindow::on_pushButtonLoop_clicked()
{
    loop = !loop;
    this->ui->pushButtonLoop->setCheckable(true);
    this->m_plist->setPlaybackMode(loop ? QMediaPlaylist::PlaybackMode::Loop : QMediaPlaylist::PlaybackMode::Sequential);
    this->ui->pushButtonLoop->setChecked(loop);
}

