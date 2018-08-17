/*
 *
 * This file is part of xVideoServiceThief,
 * an open-source cross-platform Video service download
 *
 * Copyright (C) 2007 - 2018 Xesc & Technology
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xVideoServiceThief. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact e-mail: Xesc <xeskuu.xvst@gmail.com>
 * Program URL : http://www.xvideothief.com/
 *
 */

#include "dragdropimpl.h"

#include "customalphablendimpl.h"

#include "../tools.h"
#include "../options.h"
#include "../videolistcontroller.h"
#include "../videoinformation.h"

DragDropImpl::DragDropImpl(ProgramOptions *programOptions, VideoListController *videoList,
						   QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setupUi(this);
	// create drag and drop image
	imgDragDrop = new QLabelClickeable(this);
	imgDragDrop->setObjectName(QString::fromUtf8("imgDragDrop"));
    imgDragDrop->setGeometry(QRect(0, 0, 120, 80));
	imgDragDrop->setAcceptDrops(true);
    imgDragDrop->setPixmap(scaledQPixmap(QPixmap(":/header/images/DragDropWindow.svg"), imgDragDrop->size()));
	imgDragDrop->setAlignment(Qt::AlignCenter);
	// connect doubleClick signal
	connect(imgDragDrop, SIGNAL(doubleClicked()), this, SLOT(displayMainWindowClicked())); // on double click, display main window
	// save the parent window
	this->parent = parent;
	this->programOptions = programOptions;
	this->videoList = videoList;
	// set up the window position
	setInitialPos();
	// set alpha blend
	if (programOptions->getDragDropAlphaBlend())
		setAlphaBlend(static_cast<float>(programOptions->getDragDropAlphaBlendValue()) / 100);
	// set window flags
#ifdef Q_OS_MACX
	QWidget::setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
#else
	QWidget::setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
#endif
	// add the context menu
	createDragDropMenu();
	// connect actions
	connect(actPasteURLfromClipboard, SIGNAL(triggered()), this, SLOT(pasteURLfromClipboardClicked())); // actPasteURLfromClipboard (clicked)
	connect(actDisplayMainWindo, SIGNAL(triggered()), this, SLOT(displayMainWindowClicked())); // actDisplayMainWindow (clicked)
	connect(actAlphaOFF, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlphaOFF
	connect(actAlpha10, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha10
	connect(actAlpha20, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha20
	connect(actAlpha30, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha30
	connect(actAlpha40, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha40
	connect(actAlpha50, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha50
	connect(actAlpha60, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha60
	connect(actAlpha70, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha70
	connect(actAlpha80, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha80
	connect(actAlpha90, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlpha90
	connect(actAlphaCustom, SIGNAL(triggered()), this, SLOT(alphaBlendValueClicked())); // actAlphaCustom
	// shortucut config
	shortCurtPasteURL = new QShortcut(QKeySequence(QKeySequence::Paste), this);
	connect(shortCurtPasteURL, SIGNAL(activated()), this, SLOT(pasteURLfromClipboardClicked()));
}
DragDropImpl::~DragDropImpl()
{
	delete imgDragDrop;
	// save options
	programOptions->blockSignals(true);
	programOptions->setDragDropAlphaBlend(this->windowOpacity() != 1.0);
	programOptions->setDragDropAlphaBlendValue(static_cast<int>(this->windowOpacity() * 100));
	programOptions->setDragDropLeft(this->x());
	programOptions->setDragDropTop(this->y());
	programOptions->blockSignals(false);
	delete shortCurtPasteURL;
}
void DragDropImpl::addVideoURL(QString URL)
{
	addVideo(URL);
}
void DragDropImpl::setInitialPos()
{
	QDesktopWidget *desktop = QApplication::desktop();
	int screenWidth, width;
	int screenHeight, height;
	int x, y;
	QSize windowSize;
	screenWidth = desktop->width();
	screenHeight = desktop->height();
	windowSize = size();
	width = windowSize.width();
	height = windowSize.height();
	x = programOptions->getDragDropLeft() < 0 ? (screenWidth - width) / 2 : programOptions->getDragDropLeft();
	y = programOptions->getDragDropTop() < 0 ? ((screenHeight - height) / 2) - 50: programOptions->getDragDropTop();
	move(x, y);
}
void DragDropImpl::createDragDropMenu()
{
	dragDropMenu = new QMenu(this);
	dragDropMenu->addAction(actPasteURLfromClipboard);
	dragDropMenu->addSeparator();
	alphaBlendMenu = dragDropMenu->addMenu(tr("Alpha blend value"));
	alphaBlendMenu->addAction(actAlphaOFF);
	alphaBlendMenu->addSeparator();
	alphaBlendMenu->addAction(actAlpha10);
	alphaBlendMenu->addAction(actAlpha20);
	alphaBlendMenu->addAction(actAlpha30);
	alphaBlendMenu->addAction(actAlpha40);
	alphaBlendMenu->addAction(actAlpha50);
	alphaBlendMenu->addAction(actAlpha60);
	alphaBlendMenu->addAction(actAlpha70);
	alphaBlendMenu->addAction(actAlpha80);
	alphaBlendMenu->addAction(actAlpha90);
#ifndef Q_OS_MACX
	alphaBlendMenu->addSeparator();
	alphaBlendMenu->addAction(actAlphaCustom);
#endif
	dragDropMenu->addSeparator();
	dragDropMenu->addAction(actDisplayMainWindo);
}

void DragDropImpl::setAlphaBlend(float alphaBlend)
{
	if (alphaBlend > 0 && alphaBlend <= 1)
		setWindowOpacity(alphaBlend);
}

void DragDropImpl::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/plain"))
		event->acceptProposedAction();
}

void DragDropImpl::dropEvent(QDropEvent *event)
{
	// get urls
	QStringList urls = event->mimeData()->text().trimmed().split("\n", QString::SkipEmptyParts);
	// remove duplicated urls
	urls.removeDuplicates();
	// add each detected url
	foreach (QString url, urls) addVideo(url);
	// ok
	event->acceptProposedAction();
}

void DragDropImpl::closeEvent(QCloseEvent* /*event*/)
{
	if (!parent->isMinimized())
		parent->show();
}

void DragDropImpl::contextMenuEvent(QContextMenuEvent * event)
{
	dragDropMenu->exec(event->globalPos());
}

void DragDropImpl::addVideo(QString URL)
{
	// check if we already added this video
	if (videoList->isAlreadyAdded(URL))
	{
		native_alert(this, QMessageBox::Information, tr("Already added"),
					 tr("You already added this video. Check your downloads list."), tr("Ok"));
		// abort current process
		return;
	}
	// init vars
	bool ok = videoList->getVideoInformation()->isValidHost(URL);
	QString blockMsg = "";
	if (ok)
	{
		BlockedState bs = bsNotBlocked;
		ok = !videoList->getVideoInformation()->isBlockedHost(URL, bs);
		if (bs == bsBlocked)
			blockMsg = tr(" - Blocked site");
		else if (bs == bsAdultContent)
			blockMsg = tr(" - Adult content is not allowed");
	}
	imgVideoService->setPixmap(QPixmap(videoList->getVideoInformation()->getHostImage(URL)));
	imgVideoService->setToolTip(videoList->getVideoInformation()->getHostCaption(URL) + blockMsg);
	if (ok)
		videoList->addVideo(URL);
	QTimer::singleShot(5000, this, SLOT(removeServiceIcon()));
}

void DragDropImpl::pasteURLfromClipboardClicked()
{
	QStringList urls = QApplication::clipboard()->text().trimmed().split("\n", QString::SkipEmptyParts);
	// remove duplicated urls
	urls.removeDuplicates();
	// add each detected url
	foreach (QString url, urls) addVideo(url);
}

void DragDropImpl::displayMainWindowClicked()
{
	close();
}

void DragDropImpl::alphaBlendValueClicked()
{
	if (sender() == actAlphaOFF)
		setAlphaBlend(1.0);
	else if (sender() == actAlphaCustom)
	  {
		  CustomAlphaBlendImpl customAlphaBlendForm(this);
		  customAlphaBlendForm.setInitialAlphaBlend(windowOpacity());
		  if (customAlphaBlendForm.exec() == QDialog::Accepted)
			  //setAlphaBlend(customAlphaBlendForm.getAlphaBlend());
			  setAlphaBlend(2.0);
	  }
	else
	  {
		  QString senderName = sender()->objectName();
		  senderName.remove(0, 8); // delete the object name and get the alpha value
		  bool converted;
		  float alphaValue = senderName.toFloat(&converted);
		  // set the alpha value
		  if (converted)
			  setAlphaBlend(alphaValue / 100);
	  }
}

void DragDropImpl::removeServiceIcon()
{
    imgVideoService->setPixmap(QPixmap());
    imgVideoService->setToolTip("");
}

// QLabelClickeable

void QLabelClickeable::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}
//
