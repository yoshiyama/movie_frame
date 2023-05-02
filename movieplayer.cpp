// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "movieplayer.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QLabel>
#include <QMovie>
#include <QSlider>
#include <QSpinBox>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

MoviePlayer::MoviePlayer(QWidget *parent)
    : QWidget(parent)
{
    movie = new QMovie(this);
    movie->setCacheMode(QMovie::CacheAll);

    movieLabel = new QLabel(tr("No movie loaded"));
    movieLabel->setAlignment(Qt::AlignCenter);
    movieLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    movieLabel->setBackgroundRole(QPalette::Dark);
    movieLabel->setAutoFillBackground(true);

    currentMovieDirectory = "movies";

    createControls();
    createButtons();

    connect(movie, &QMovie::frameChanged, this, &MoviePlayer::updateFrameSlider);
    connect(movie, &QMovie::stateChanged, this, &MoviePlayer::updateButtons);
    connect(fitCheckBox, &QCheckBox::clicked, this, &MoviePlayer::fitToWindow);
    connect(frameSlider, &QSlider::valueChanged, this, &MoviePlayer::goToFrame);
    connect(speedSpinBox, &QSpinBox::valueChanged,
            movie, &QMovie::setSpeed);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(movieLabel);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    updateFrameSlider();
    updateButtons();

    setWindowTitle(tr("Movie Player"));
    resize(400, 400);
}

void MoviePlayer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Movie"),
                               currentMovieDirectory);
    if (!fileName.isEmpty())
        openFile(fileName);
}

void MoviePlayer::openFile(const QString &fileName)
{
    currentMovieDirectory = QFileInfo(fileName).path();

    movie->stop();
    movieLabel->setMovie(movie);
    movie->setFileName(fileName);
    movie->start();

    updateFrameSlider();
    updateButtons();
}

void MoviePlayer::goToFrame(int frame)
{
    movie->jumpToFrame(frame);
}

void MoviePlayer::fitToWindow()
{
    movieLabel->setScaledContents(fitCheckBox->isChecked());
}

void MoviePlayer::updateFrameSlider()
{
    bool hasFrames = (movie->currentFrameNumber() >= 0);

    if (hasFrames) {
        if (movie->frameCount() > 0) {
            frameSlider->setMaximum(movie->frameCount() - 1);
        } else {
            if (movie->currentFrameNumber() > frameSlider->maximum())
                frameSlider->setMaximum(movie->currentFrameNumber());
        }
        frameSlider->setValue(movie->currentFrameNumber());


		  // フレーム番号を表示するラベルを更新する
		  QString frameNumberText = QString("Frame number: %1/%2")
					 .arg(movie->currentFrameNumber() + 1)
					 .arg(movie->frameCount());
		  frameNumberLabel->setText(frameNumberText);
    } else {
        frameSlider->setMaximum(0);
		  frameNumberLabel->setText(tr("Frame number:"));
    }
    frameLabel->setEnabled(hasFrames);
    frameSlider->setEnabled(hasFrames);
}

void MoviePlayer::updateButtons()
{
    playButton->setEnabled(movie->isValid() && movie->frameCount() != 1
                           && movie->state() == QMovie::NotRunning);
    pauseButton->setEnabled(movie->state() != QMovie::NotRunning);
    pauseButton->setChecked(movie->state() == QMovie::Paused);
    stopButton->setEnabled(movie->state() != QMovie::NotRunning);
}

void MoviePlayer::createControls()
{
    fitCheckBox = new QCheckBox(tr("Fit to Window"));

    frameLabel = new QLabel(tr("Current frame:"));

    frameSlider = new QSlider(Qt::Horizontal);
    frameSlider->setTickPosition(QSlider::TicksBelow);
    frameSlider->setTickInterval(10);

	 frameNumberLabel = new QLabel(tr("Frame number:"));

    speedLabel = new QLabel(tr("Speed:"));

    speedSpinBox = new QSpinBox;
    speedSpinBox->setRange(1, 9999);
    speedSpinBox->setValue(100);
    speedSpinBox->setSuffix(tr("%"));

    controlsLayout = new QGridLayout;
    controlsLayout->addWidget(fitCheckBox, 0, 0, 1, 2);
    controlsLayout->addWidget(frameLabel, 1, 0);
    controlsLayout->addWidget(frameSlider, 1, 1, 1, 2);
	 controlsLayout->addWidget(frameNumberLabel, 2, 0);
    controlsLayout->addWidget(speedLabel, 3, 0);
    controlsLayout->addWidget(speedSpinBox, 3, 1);
}

void MoviePlayer::createButtons()
{
    QSize iconSize(36, 36);

    openButton = new QToolButton;
    openButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    openButton->setIconSize(iconSize);
    openButton->setToolTip(tr("Open File"));
    connect(openButton, &QToolButton::clicked, this, &MoviePlayer::open);

    playButton = new QToolButton;
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playButton->setIconSize(iconSize);
    playButton->setToolTip(tr("Play"));
    connect(playButton, &QToolButton::clicked, movie, &QMovie::start);

    pauseButton = new QToolButton;
    pauseButton->setCheckable(true);
    pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    pauseButton->setIconSize(iconSize);
    pauseButton->setToolTip(tr("Pause"));
    connect(pauseButton, &QToolButton::clicked, movie, &QMovie::setPaused);

    stopButton = new QToolButton;
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setIconSize(iconSize);
    stopButton->setToolTip(tr("Stop"));
    connect(stopButton, &QToolButton::clicked, movie, &QMovie::stop);

    quitButton = new QToolButton;
    quitButton->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    quitButton->setIconSize(iconSize);
    quitButton->setToolTip(tr("Quit"));
    connect(quitButton, &QToolButton::clicked, this, &MoviePlayer::close);

    buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(openButton);
    buttonsLayout->addWidget(playButton);
    buttonsLayout->addWidget(pauseButton);
    buttonsLayout->addWidget(stopButton);
    buttonsLayout->addWidget(quitButton);
    buttonsLayout->addStretch();
}
