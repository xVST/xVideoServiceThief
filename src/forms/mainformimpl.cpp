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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with xVideoServiceThief. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: Xesc <xeskuu.xvst@gmail.com>
* Program URL   : http://www.xvideothief.com/
*
*/

#include "mainformimpl.h"

#include "loadingimpl.h"
#include "welcomedonate.h"
#include "dragdropimpl.h"
#include "completedpopupimpl.h"
#include "addvideoimpl.h"
#include "informationimpl.h"
#include "optionsimpl.h"
#include "checkupdatesimpl.h"
#include "bugreportimpl.h"
#include "searchvideosimpl.h"
#include "winvistadownloadsmsgimpl.h"
#include "customdownloadtitleimpl.h"
#include "whatsnewimpl.h"
#include "adultspermissionimpl.h"

#include "../tools.h"
#include "../options.h"
#include "../updates.h"
#include "../searchvideos.h"
#include "../progressbardelegate.h"
#include "../videolistcontroller.h"
#include "../videoinformation.h"
#include "../sessionmanager.h"
#include "../programversion.h"
#include "../checkupdatesworker.h"

#ifdef Q_OS_MACX
#include "../mac_only/qmactoolbarext.h"
#endif

#include <QtSingleApplication>

MainFormImpl::MainFormImpl(QWidget * parent, Qt::WindowFlags f)
	: QMainWindow(parent, f)
{
	setupUi(this);
	// set caption
	setWindowTitle(QString(windowTitle()).arg(PROGRAM_VERSION));
	// hide embeded check updates
    lblCheckForUpdatesLabel->hide();
    pbrCheckingForUpdates->hide();
    spbCancelCheckForUpdates->hide();
    // load options
    programOptions = ProgramOptions::instance();
    programOptions->load();
    // configure UI
    if (programOptions->getHideDownloadInformationBox()) frmInformation->hide();
    if (programOptions->getHideConvertInformationBox()) convertWidget->hide();
    if (programOptions->getHideDownloadsBox()) downloadsWidget->hide();
    // resize add button to an optimum visualization on macosx
#ifdef Q_OS_MACX
    btnAddVideo->setMinimumWidth(158);
    frameHeader->layout()->setContentsMargins(4, 4, 6, 4);
    setMinimumSize(780, 540);
    QFont updatesFont = lblCheckForUpdatesLabel->font();
    updatesFont.setPointSize(10);
    lblCheckForUpdatesLabel->setFont(updatesFont);
	// change lsvDownloadList alternateBase color (better look&feel)
	QPalette palette = lsvDownloadList->palette();
	QColor color("#efefef");
	palette.setColor(QPalette::Disabled, QPalette::AlternateBase, color);
	palette.setColor(QPalette::Active, QPalette::AlternateBase, color);
	palette.setColor(QPalette::Inactive, QPalette::AlternateBase, color);
	lsvDownloadList->setPalette(palette);
	// add preferences item Application menu
	menuBar()->addMenu("<Application>")->addAction(actOptions);
	menuBar()->addMenu("<Application>")->addAction(actAbout);
	connect(actOptions, SIGNAL(triggered()), this, SLOT(moreOptionsClicked()));
	connect(actAbout, SIGNAL(triggered()), this, SLOT(informationClicked()));
	// add MenuBar for MacOSX
	// File menu
	QMenu *fileMenu = menuBar()->addMenu(tr("File"));
	fileMenu->addAction(actAddVideo);
	fileMenu->addAction(actDeleteVideo);
	fileMenu->addAction(actRenameVideo);
	fileMenu->addSeparator();
	fileMenu->addAction(actResetState);
	fileMenu->addSeparator();
	fileMenu->addAction(actMoveUP);
	fileMenu->addAction(actMoveDOWN);
	fileMenu->addSeparator();
	fileMenu->addAction(actClearList);
	fileMenu->addAction(actClearCompleted);
	// Controllers menu
	QMenu *controlsMenu = menuBar()->addMenu(tr("Controllers"));
	controlsMenu->addAction(actPlayVideo);
	controlsMenu->addAction(actViewErrorMessage);
	controlsMenu->addSeparator();
	controlsMenu->addAction(actStartDownload);
	controlsMenu->addAction(actPauseResumeDownload);
	controlsMenu->addAction(actCancelDownload);
	// Tools menu
	QMenu *toolsMenu = menuBar()->addMenu(tr("Tools"));
	toolsMenu->addAction(actSearchVideos);
	toolsMenu->addAction(actDragDrop);
	toolsMenu->addAction(actUpdates);
	toolsMenu->addSeparator();
	toolsMenu->addAction(actOpenDownloadDir);
	toolsMenu->addSeparator();
	toolsMenu->addAction(actDownloadVideosAutomatically);
	toolsMenu->addAction(actConvertVideos);
	toolsMenu->addAction(actDisplayPopup);
	// Help menu
	QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
	helpMenu->addAction(actOnlineHelp);
	// connect menu signals
	connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(menuBarAboutToShow()));
	connect(controlsMenu, SIGNAL(aboutToShow()), this, SLOT(menuBarAboutToShow()));
	connect(toolsMenu, SIGNAL(aboutToShow()), this, SLOT(menuBarAboutToShow()));
	connect(helpMenu, SIGNAL(aboutToShow()), this, SLOT(menuBarAboutToShow()));
    // is the moder UI selected?
    if ( ! programOptions->getClassicUI())
    {
        // hide top header and buttons menu
        frameHeader->hide();
        buttonsMenuLayout->hide();

        // re-align and change margins to adapt the new layout
        QGridLayout *grid = dynamic_cast<QGridLayout *>(centralWidget()->layout());
        grid->setHorizontalSpacing(0);
        grid->setVerticalSpacing(6);
        grid->setContentsMargins(0, 0, 0, frmInformation->isHidden() && convertWidget->isHidden() && downloadsWidget->isHidden() ? 0 : 9);
        lsvDownloadList->setFrameShape(QFrame::NoFrame);
        frmInformation->setFrameShape(QFrame::NoFrame);
        frmInformation->setStyleSheet("");
        frmInformation->layout()->setContentsMargins(9, 0, 9, 0);
        convertWidget->layout()->setContentsMargins(9, 0, 9, 0);
        downloadsWidget->layout()->setContentsMargins(9, 0, 9, 0);

        // configure the toolbar
        toolBar = new QMacToolBarExt(this);
        toolBar->setAsOnlyIcon();

        // add video items
        toolBarItemAdd = toolBar->addButtonItem(QIcon(":/buttons/images/film_add.svg"), tr("Add video"));
        connect(toolBarItemAdd, SIGNAL(activated()), this, SLOT(addVideoClicked()));
        toolBarItemRename = toolBar->addButtonItem(QIcon(":/buttons/images/film_edit.svg"), tr("Rename video"));
        connect(toolBarItemRename, SIGNAL(activated()), this, SLOT(renameVideoClicked()));
        toolBarItemDelete = toolBar->addButtonItem(QIcon(":/buttons/images/film_delete.svg"), tr("Delete video"));
        connect(toolBarItemDelete, SIGNAL(activated()), this, SLOT(deleteVideoClicked()));

        // add separation
        toolBar->addStandardItem(QMacToolBarItem::FlexibleSpace);

        // add download items
        toolBarItemStart = toolBar->addButtonItem(QIcon(":/buttons/images/control_play.svg"), tr("Start"));
        connect(toolBarItemStart, SIGNAL(activated()), this, SLOT(startDownloadVideoClicked()));
        toolBarItemPause = toolBar->addButtonItem(QIcon(":/buttons/images/control_pause.svg"), tr("Pause"));
        connect(toolBarItemPause, SIGNAL(activated()), this, SLOT(pauseResumeDownloadVideoClicked()));
        toolBarItemStop = toolBar->addButtonItem(QIcon(":/buttons/images/control_stop.svg"), tr("Stop"));
        connect(toolBarItemStop, SIGNAL(activated()), this, SLOT(cancelDownloadVideoClicked()));

        // add separation
        toolBar->addSeparator();

        toolBarItemClearList = toolBar->addButtonItem(QIcon(":/buttons/images/removeAll.svg"), tr("Clear list"));
        connect(toolBarItemClearList, SIGNAL(activated()), this, SLOT(clearListClicked()));
        toolBarItemClearCompleted = toolBar->addButtonItem(QIcon(":/buttons/images/removeCompleted.svg"), tr("Clear completed"));
        connect(toolBarItemClearCompleted, SIGNAL(activated()), this, SLOT(clearCompletedClicked()));

        // add separation
        toolBar->addStandardItem(QMacToolBarItem::FlexibleSpace);

        // add items
        toolBarItemSearch = toolBar->addButtonItem(QIcon(":/buttons/images/search.svg"), tr("Search videos"));
        connect(toolBarItemSearch, SIGNAL(activated()), this, SLOT(searchVideosClicked()));
        toolBarItemDragDrop = toolBar->addButtonItem(QIcon(":/buttons/images/application_link.svg"), tr("Drag&Drop"));
        connect(toolBarItemDragDrop, SIGNAL(activated()), this, SLOT(dragDropClicked()));
        toolBarItemUpdate = toolBar->addButtonItem(QIcon(":/buttons/images/updates_small.svg"), tr("Updates"));
        connect(toolBarItemUpdate, SIGNAL(activated()), this, SLOT(updatesClicked()));
        toolBarItemHelp = toolBar->addButtonItem(QIcon(":/buttons/images/book_open.svg"), tr("Help"));
        connect(toolBarItemHelp, SIGNAL(activated()), this, SLOT(onlineHelpClicked()));
        toolBarItemInfo = toolBar->addButtonItem(QIcon(":/buttons/images/information.svg"), tr("Info"));
        connect(toolBarItemInfo, SIGNAL(activated()), this, SLOT(informationClicked()));

        // attach the toolbar
        this->window()->winId(); // create window->windowhandle()
        toolBar->attachToWindow(this->window()->windowHandle());
    }
#endif
	// init program options
	lastOptionsPage = 0;
    // setu-up the main form
	centerWindow();
	// init main objects (program core)
	videoList = new VideoListController(programOptions);
	// set-up the lsvDownloadList widget
	QStringList headers;
	headers << tr(" Video ") << tr(" Size ") << tr(" Progress ") << tr(" Time ") << tr(" Speed ");
	// add the headers
	lsvDownloadList->setHeaderLabels(headers);
	// add the progress bar
	lsvDownloadList->setItemDelegate(new ProgressBarDelegate(this, videoList));
	// change headers sizes
	QFontMetrics fm = fontMetrics();
	QHeaderView *header = lsvDownloadList->header();
	// configure resize mode
	header->setHighlightSections(false);
	header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
	// set sizes
	header->resizeSection(1, qMax(fm.width(headers.at(1)), fm.width(" 1024 bytes  ")));
	header->resizeSection(2, qMax(fm.width(headers.at(2)), 130));
	header->resizeSection(3, qMax(fm.width(headers.at(3)), fm.width(" 24h 59m 59s  ")));
	header->resizeSection(4, qMax(fm.width(headers.at(4)), fm.width(" 999,99 KB/sec  ")));
	// set header text aligment
	QTreeWidgetItem * headerItem = lsvDownloadList->headerItem();
	headerItem->setTextAlignment(1, Qt::AlignRight   | Qt::AlignVCenter);
	headerItem->setTextAlignment(2, Qt::AlignHCenter | Qt::AlignVCenter);
	headerItem->setTextAlignment(3, Qt::AlignHCenter | Qt::AlignVCenter);
	headerItem->setTextAlignment(4, Qt::AlignHCenter | Qt::AlignVCenter);
	// configure the TrayIcon
	createTrayIcon();
	// init the completed popup
	completedPopup = new CompletedPopupImpl(trayIcon, this);
	// start the drag & drop window
    dragDropForm = new DragDropImpl(programOptions, videoList, this);
    // assign the header image
    imgHeader->setPixmap(scaledQPixmap(QPixmap(":/header/images/header.svg"), QSize(238, 100)));
    // connect buttons (header)
    connect(spbSearchVideos, SIGNAL(clicked()), this, SLOT(searchVideosClicked())); //spb Search Videos (clicked)
    connect(spbDragDrop, SIGNAL(clicked()), this, SLOT(dragDropClicked())); //spb Drag&Drop (clicked)
    connect(spbUpdates, SIGNAL(clicked()), this, SLOT(updatesClicked())); //spb Updates (clicked)
    connect(spbHelp, SIGNAL(clicked()), this, SLOT(onlineHelpClicked())); //spb Online Help (clicked)
    connect(spbInformation, SIGNAL(clicked()), this, SLOT(informationClicked())); //spb Information (clicked)
	// connect buttons directories
	connect(spbSelectDownloadDir, SIGNAL(clicked()), this, SLOT(selectDownloadDirClicked())); //spb Open Download directory (clicked)
	connect(spbOpenDownloadDir, SIGNAL(clicked()), this, SLOT(openDownloadDirClicked())); //spb Open Download directory (clicked)
	// connect actions
	connect(actDragDrop, SIGNAL(triggered()), this, SLOT(dragDropClicked())); // actDragDrop (clicked)
	connect(actUpdates, SIGNAL(triggered()), this, SLOT(updatesClicked())); // actUpdates (clicked)
	connect(actOnlineHelp, SIGNAL(triggered()), this, SLOT(onlineHelpClicked())); // actOnlineHelp (clicked)
	connect(actInformation, SIGNAL(triggered()), this, SLOT(informationClicked())); // actInformation (clicked)
	connect(actRestoreApp, SIGNAL(triggered()), this, SLOT(restoreAppClicked())); // actRestoreApp (clicked)
	connect(actCloseApp, SIGNAL(triggered()), this, SLOT(closeAppClicked())); // actCloseApp (clicked)
	connect(actMoreOptions, SIGNAL(triggered()), this, SLOT(moreOptionsClicked())); // actMoreOptions (clicked)
	connect(actAddVideo, SIGNAL(triggered()), this, SLOT(addVideoClicked())); // actAddVideo (clicked)
	connect(actDeleteVideo, SIGNAL(triggered()), this, SLOT(deleteVideoClicked())); // actDeleteOptions (clicked)
	connect(actRenameVideo, SIGNAL(triggered()), this, SLOT(renameVideoClicked())); // actRenameOptions (clicked)
	connect(actStartDownload, SIGNAL(triggered()), this, SLOT(startDownloadVideoClicked())); // actStartDownload (clicked)
	connect(actStartDownloadAgain, SIGNAL(triggered()), this, SLOT(startDownloadVideoAgainClicked())); // actStartDownloadAgain (clicked)
	connect(actPauseResumeDownload, SIGNAL(triggered()), this, SLOT(pauseResumeDownloadVideoClicked())); // actPauseResumeDownload (clicked)
	connect(actCancelDownload, SIGNAL(triggered()), this, SLOT(cancelDownloadVideoClicked())); // actCancelDownload (clicked)
	connect(actClearList, SIGNAL(triggered()), this, SLOT(clearListClicked())); // actClearList (clicked)
	connect(actClearCompleted, SIGNAL(triggered()), this, SLOT(clearCompletedClicked())); // actClearCompleted (clicked)
	connect(actPlayVideo, SIGNAL(triggered()), this, SLOT(playVideoClicked())); // actPlayVideo (clicked)
	connect(actMoveUP, SIGNAL(triggered()), this, SLOT(moveItemUpClicked())); // actMoveUP (clicked)
	connect(actMoveDOWN, SIGNAL(triggered()), this, SLOT(moveItemDownClicked())); // actMoveDOWN (clicked)
	connect(actResetState, SIGNAL(triggered()), this, SLOT(resetStateClicked())); // actResetState (clicked)
	connect(actStayOnTop, SIGNAL(triggered()), this, SLOT(stayOnTopClicked())); // actStayOnTop (clicked)
	connect(actMinimizeToSystemTray, SIGNAL(triggered()), this, SLOT(minimizeToSystemTrayClicked())); // actMinimizeToSystemTray (clicked)
	connect(actViewErrorMessage, SIGNAL(triggered()), this, SLOT(viewErrorMessageClicked())); // actViewErrorMessage (clicked)
	connect(actOpenDownloadDir, SIGNAL(triggered()), this, SLOT(openDownloadDirClicked())); //actOpenDownloadDir (clicked)
	connect(actSearchVideos, SIGNAL(triggered()), this, SLOT(searchVideosClicked())); //actSearchVideos (clicked)
	// edtDownloadDir
	connect(edtDownloadDir, SIGNAL(editingFinished()), this, SLOT(edtDownloadDirChanged()));
	// connect buttons
    connect(btnAddVideo, SIGNAL(clicked()), this, SLOT(addVideoClicked())); //btnAddVideo (clicked)
    connect(btnDeleteVideo, SIGNAL(clicked()), this, SLOT(deleteVideoClicked())); //btnAddVideo (clicked)
    connect(btnStartDownload, SIGNAL(clicked()), this, SLOT(startDownloadVideoClicked())); //btnAddVideo (clicked)
    connect(btnPauseResumeDownload, SIGNAL(clicked()), this, SLOT(pauseResumeDownloadVideoClicked())); //btnPauseResumeDownload (clicked)
    connect(btnCancelDownload, SIGNAL(clicked()), this, SLOT(cancelDownloadVideoClicked())); //btnAddVideo (clicked)
    connect(btnMoreOptions, SIGNAL(clicked()), this, SLOT(moreOptionsClicked())); //btnMoreOptions (clicked)
    connect(btnClearList, SIGNAL(clicked()), this, SLOT(clearListClicked())); //btnMoreOptions (clicked)
    connect(btnClearCompleted, SIGNAL(clicked()), this, SLOT(clearCompletedClicked())); //btnMoreOptions (clicked)
	// tray icon menu events
	connect(trayIconMenu, SIGNAL(aboutToShow()), this, SLOT(trayIconAboutToShow()));
	connect(actDownloadVideosAutomatically, SIGNAL(toggled(bool)), this, SLOT(optionSelected(bool)));
	connect(actConvertVideos, SIGNAL(toggled(bool)), this, SLOT(optionSelected(bool)));
	connect(actDisplayPopup, SIGNAL(toggled(bool)), this, SLOT(optionSelected(bool)));
	// connect check boxs
	connect(chbDownloadVideosAuto, SIGNAL(stateChanged(int)), this, SLOT(downloadAutomaticallyStateChanged(int)));
	connect(chbConvertVideos, SIGNAL(stateChanged(int)), this, SLOT(convertVideosStateChanged(int)));
	// VideoListController signals
	connect(videoList, SIGNAL(videoAdded(VideoItem *)), this, SLOT(videoAdded(VideoItem *))); //onVideList added
	connect(videoList, SIGNAL(videoDeleted(VideoItem *)), this, SLOT(videoDeleted(VideoItem *))); //onVideoList deleted
	connect(videoList, SIGNAL(videoRenamed(VideoItem *)), this, SLOT(videoRenamed(VideoItem *))); //onVideoList renamed
	connect(videoList, SIGNAL(videoUpdated(VideoItem *)), this, SLOT(videoUpdated(VideoItem *))); //onVideoList updated
	connect(videoList, SIGNAL(videoError(VideoItem *)), this, SLOT(videoError(VideoItem *))); //onVideoList error
	connect(videoList, SIGNAL(videoMoved(int, int)), this, SLOT(videoMoved(int, int))); //onVideList item moved
	// lsvDownloadList
	connect(lsvDownloadList, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(videoItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(lsvDownloadList, SIGNAL(itemSelectionChanged()), this, SLOT(videoItemSelectionChanged()));
	connect(lsvDownloadList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(videoListContextMenu(const QPoint &)));
	// options
	connect(programOptions, SIGNAL(optionsLoadBefore()), this, SLOT(optionsDidSomething()));
	connect(programOptions, SIGNAL(optionsLoadAfter()), this, SLOT(optionsDidSomething()));
	connect(programOptions, SIGNAL(optionsSaveBefore()), this, SLOT(optionsDidSomething()));
	connect(programOptions, SIGNAL(optionsSaveAfter()), this, SLOT(optionsDidSomething()));
	connect(programOptions, SIGNAL(optionsUpdated()), this, SLOT(optionsDidSomething()));
	// tray icon events
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
	// update visual options
	updateVisualOptions();
	// set the "stay on top" flag
	setStayOnTopFlag();
	// session manager
	sessionManager = new SessionManager(programOptions);
	sessionManager->loadSession(videoList);
	// forms lockers
	addForm_active = false;
	optionsForm_active = false;
	infoForm_active = false;
	// shortcut paste
	shortCutPasteURL = new QShortcut(QKeySequence(QKeySequence::Paste), this);
	connect(shortCutPasteURL, SIGNAL(activated()), this, SLOT(pasteURLfromClipboardClicked()));
	// shortcut delete
	shortCutDeleteVideo = new QShortcut(QKeySequence(QKeySequence::Delete), this);
	connect(shortCutDeleteVideo, SIGNAL(activated()), this, SLOT(deleteVideoClicked()));
	// update information
	updateListInformation();
	// set the support project link
    imgPaypal->setText("<a href=\"http://www.xvideothief.com/index.php?action=make_donation\"><img src=\":/buttons/images/support_button_main.png\" /></a>");
	// display welcome donate
	displayWelcomeMessage();
	// display adults permission
	displayAdultSitesPermission();
	// display whats new window
	displayWhatsNewMessage();
	// display windows downloads directory migrator
	displayWinVistaDownloads();
	// updater timer
	QTimer::singleShot(250, this, SLOT(checkForUpdates()));
	// fix MacOSX bug with alternating color rows and size when the list is empty
#ifdef Q_OS_MACX
	// if list is empty, then add and delete a temporal item
	if (lsvDownloadList->topLevelItemCount() == 0)
	{
		QTreeWidgetItem *tmpItem = new QTreeWidgetItem(lsvDownloadList);
		tmpItem->setSizeHint(0, QSize(18, 18));
		delete tmpItem;
	}
#endif
}

MainFormImpl::~MainFormImpl()
{
#ifdef Q_OS_MACX
	// save the window state (MacOSX bug fix)
	if (windowState() == Qt::WindowMaximized && width() == qApp->desktop()->availableGeometry(this).width())
#else
	// save the window state
	if (windowState() == Qt::WindowMaximized)
#endif
		programOptions->setMainWinowMaximized(true);
	else
	{
		programOptions->setMainWinowMaximized(false);
		programOptions->setMainWindowHeight(height());
		programOptions->setMainWindowWidth(width());
	}

	// delete shortcuts
	delete shortCutPasteURL;
	delete shortCutDeleteVideo;

	// delete the session manager
	delete sessionManager;

	// delete forms
	delete dragDropForm;
	delete completedPopup;

	// delete program options
	ProgramOptions::destroyInstance();

	// delete main objects
	delete videoList;

	// delete tray icon
	delete trayIconMenu;
	delete trayIcon;

	// remove previews
	SearchVideos::removePreviews();
}

void MainFormImpl::centerWindow()
{
	QDesktopWidget *desktop = QApplication::desktop();

	// set the window maximized
	if (programOptions->getMainWinowMaximized())
		setWindowState(Qt::WindowMaximized);
	else // only if this window is not maximized
		// set up the window height and width
		if (programOptions->getMainWindowWidth() != 0 && programOptions->getMainWindowHeight() != 0)
			resize(programOptions->getMainWindowWidth(), programOptions->getMainWindowHeight());

	int screenWidth, width;
	int screenHeight, height;
	int x, y;
	QSize windowSize;

	screenWidth = desktop->screenGeometry(desktop->primaryScreen()).width();
	screenHeight = desktop->screenGeometry(desktop->primaryScreen()).height();

	windowSize = size();
	width = windowSize.width();
	height = windowSize.height();

	x = (screenWidth - width) / 2;
	y = (screenHeight - height) / 2;
	y -= 50;

	move(x, y);
}

void MainFormImpl::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::WindowStateChange)
		if (isMinimized() && programOptions->getMinimizeToSystemTray())
		{
			QTimer::singleShot(0, this, SLOT(hide()));
			event->ignore();
			return;
		}
	QMainWindow::changeEvent(event);
}

void MainFormImpl::closeEvent(QCloseEvent *event)
{
	if (videoList->isWorking())
	{
		if (native_alert(this, QMessageBox::Question, tr("Closing..."), tr("xVideoServiceThief is working, do you wish Pause the current work?"),
			tr("Yes"), tr("No")) == 0)
		{
			videoList->stop();
			videoList->pauseAllDownloads();
			videoList->cancelConversion();
			event->accept();
		}
		else
			event->ignore();
	}
	// closing?
	if (event->isAccepted())
		sessionManager->saveSession(videoList);
}

void MainFormImpl::createTrayIcon()
{
	// create the Tray Icon context menu
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(actRestoreApp);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(actDragDrop);
	trayIconMenu->addAction(actAddVideo);
	trayIconMenu->addAction(actDeleteVideo);
	trayIconMenu->addAction(actRenameVideo);
	trayIconMenu->addAction(actSearchVideos);
	trayIconMenu->addAction(actOpenDownloadDir);
	trayIconMenu->addSeparator();
	trayIconMenuOptions = trayIconMenu->addMenu(tr("Options"));
	trayIconMenuOptions->addAction(actStayOnTop);
	trayIconMenuOptions->addAction(actMinimizeToSystemTray);
	trayIconMenuOptions->addSeparator();
	trayIconMenuOptions->addAction(actDownloadVideosAutomatically);
	trayIconMenuOptions->addAction(actConvertVideos);
	trayIconMenuOptions->addAction(actDisplayPopup);
	trayIconMenuOptions->addSeparator();
	trayIconMenuOptions->addAction(actMoreOptions);
	trayIconMenu->addAction(actUpdates);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(actOnlineHelp);
	trayIconMenu->addAction(actInformation);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(actCloseApp);

	// set defaults
	trayIconMenu->setDefaultAction(actRestoreApp);

	// create the Tray Icon object
    lastTrayIconStr = ":/icons/images/tray_icon_off.svg";
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(lastTrayIconStr));
	trayIcon->setToolTip(this->windowTitle());
	trayIcon->show();
}

// buttons and actions events
void MainFormImpl::dragDropClicked()
{
	dragDropForm->show();
	hide();
}

void MainFormImpl::updatesClicked()
{
	if (videoList->isWorking())
	{
		native_alert(this, QMessageBox::Information, tr("Updates"),
					 tr("Another process is currently working, please stop it or wait until the end of process."), tr("Ok"));
		return;
	}

    spbUpdates->setEnabled(false);
	actUpdates->setEnabled(false);

	if (!isVisible()) restoreAppClicked();

	CheckUpdatesImpl checkUpdatesForm(programOptions, this, Qt::Sheet);
	checkUpdatesForm.exec();

    spbUpdates->setEnabled(true);
	actUpdates->setEnabled(true);
}

void MainFormImpl::onlineHelpClicked()
{
	QDesktopServices::openUrl(QUrl("http://www.xvideothief.com/index.php?action=onlinehelp"));
}

void MainFormImpl::informationClicked()
{
	if (infoForm_active) return;

	infoForm_active = true;
    spbInformation->setEnabled(false);
	actInformation->setEnabled(false);

	InformationImpl informationForm(programOptions, videoList->getVideoInformation(), this);
	informationForm.exec();

    spbInformation->setEnabled(true);
	actInformation->setEnabled(true);
	infoForm_active = false;
}

void MainFormImpl::restoreAppClicked()
{
	dragDropForm->hide();
	showNormal();
}

void MainFormImpl::selectDownloadDirClicked()
{
	QString s = QFileDialog::getExistingDirectory(this, tr("Select the download directory:"),
				edtDownloadDir->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	// if is emtpy, cancel the proces
	if (s.isEmpty()) return;
	edtDownloadDir->setText(QDir::toNativeSeparators(s));
}

void MainFormImpl::openDownloadDirClicked()
{
	openDirectory(edtDownloadDir->text());
}

void MainFormImpl::closeAppClicked()
{
	close();
}

void MainFormImpl::addVideoClicked()
{
	if (addForm_active) return;

	addForm_active = true;
	actAddVideo->setEnabled(false);
    btnAddVideo->setEnabled(false);

	if (!isVisible()) restoreAppClicked();

	AddVideoImpl addVideoForm(programOptions, videoList->getVideoInformation(), this, Qt::Sheet);
	if (showModalDialog(&addVideoForm) == QDialog::Accepted)
	{
		// is a user custom download?
		if (addVideoForm.isCustomDownload())
			// user want override the current conversion config
			if (addVideoForm.chbOverrideConversion->isChecked() && chbConvertVideos->isEnabled())
				videoList->addVideo(addVideoForm.getSelectedURLs().first(), addVideoForm.getCustomDownloadTitle(), addVideoForm.getOverridedConversionConfig());
			else // user DON'T want override the current conversion config
				videoList->addVideo(addVideoForm.getSelectedURLs().first(), addVideoForm.getCustomDownloadTitle());
		else // standard add (not custom download)
			foreach (QString url, addVideoForm.getSelectedURLs())
				// user want override the current conversion config
				if (addVideoForm.chbOverrideConversion->isChecked() && chbConvertVideos->isEnabled())
					videoList->addVideo(url, addVideoForm.getOverridedConversionConfig());
				else // user DON'T want override the current conversion config
					videoList->addVideo(url);
	}

	actAddVideo->setEnabled(true);
    btnAddVideo->setEnabled(true);
	addForm_active = false;
}

void MainFormImpl::deleteVideoClicked()
{
	// remove all the items which are not possible to delete
	foreach (VideoItem *videoItem, getSelectedVideoItems())
		if (!videoItem->isRemovable())
			getQTreeWidgetItemByVideoItem(videoItem)->setSelected(false);

	// get the items to delete
	QList<VideoItem *> videoItems = getSelectedVideoItems();

	// ask before delete the video(s)
	if (videoItems.count() == 1) // only one item
	{
		if (native_alert(this, QMessageBox::Question, tr("Delete download"), tr("Wish you delete the download of <b>%1</b>?").arg(videoItems.first()->getDisplayLabel()),
			tr("Yes"), tr("No")) != 0) return;
	}
	else // more than 1 item
	{
		if (native_alert(this, QMessageBox::Question, tr("Delete downloads"), tr("Wish you delete the <b>%1</b> selected downloads?").arg(videoItems.count()),
			tr("Yes"), tr("No")) != 0) return;
	}

	// delete all selected items
	foreach (VideoItem *videoItem, getSelectedVideoItems())
	{
		if (videoItem->isRemovable())
			videoList->deleteVideo(videoItem, true);
		else // unselect this item
			getQTreeWidgetItemByVideoItem(videoItem)->setSelected(false);
	}
}

void MainFormImpl::renameVideoClicked()
{
	// rename all selected items
	foreach (VideoItem *videoItem, getSelectedVideoItems())
	{
		CustomDownloadTitleImpl renameVideoForm(this, Qt::Sheet);
		// set the current title
		renameVideoForm.edtTitle->setText(videoItem->getDisplayLabel());
		// display custom title window
		if (showModalDialog(&renameVideoForm) == QDialog::Accepted)
			videoList->renameVideo(videoItem, renameVideoForm.edtTitle->text());
	}
}

void MainFormImpl::startDownloadVideoAgainClicked()
{
	// download all selected items
	foreach (VideoItem *videoItem, getSelectedVideoItems())
	{
		if (videoItem->isCompleted())
		{
			videoList->startDownload(videoItem);
		}
	}
	// update visual controls
	updateVisualControls();
}

void MainFormImpl::startDownloadVideoClicked()
{
	// download all selected items
	foreach (VideoItem *videoItem, getSelectedVideoItems())
		if (videoItem->isDownloadable())
			videoList->startDownload(videoItem);
		else // deselect the item
			getQTreeWidgetItemByVideoItem(videoItem)->setSelected(false);
	// update visual controls
	updateVisualControls();
}

void MainFormImpl::pauseResumeDownloadVideoClicked()
{
	// download all selected items
	foreach (VideoItem *videoItem, getSelectedVideoItems())
	{
		// is ready, and we want pause it before start download
		if (videoItem->isDownloading() || videoItem->isResuming() || videoItem->isReady())
			videoList->pauseDownload(videoItem);
		else if (videoItem->isAnyKindOfPaused())
			videoList->resumeDownload(videoItem);
		else // deselect this item
			getQTreeWidgetItemByVideoItem(videoItem)->setSelected(false);
	}
	// update visual controls
	updateVisualControls();
}

void MainFormImpl::cancelDownloadVideoClicked()
{
	// remove all the items which are not possible to cancel
	foreach (VideoItem *videoItem, getSelectedVideoItems())
		if (!videoItem->isDownloading())
			getQTreeWidgetItemByVideoItem(videoItem)->setSelected(false);
	// get the items to cancel
	QList<VideoItem *> videoItems = getSelectedVideoItems();
	// display cancel confirmation
	if (!videoItems.isEmpty())
	{
		if (videoItems.count() == 1) // only one item
		{
			if (native_alert(this, QMessageBox::Question, tr("Cancel download"), tr("Wish you cancel the download of <b>%1</b>?").arg(videoItems.first()->getDisplayLabel()),
				tr("Yes"), tr("No")) != 0) return;
		}
		else // more than 1 item
		{
			if (native_alert(this, QMessageBox::Question, tr("Cancel downloads"), tr("Wish you cancel the <b>%1</b> selected downloads?").arg(videoItems.count()),
				tr("Yes"), tr("No")) != 0) return;
		}
		// cancel all selected items
		foreach (VideoItem *videoItem, videoItems)
			if (videoItem->isDownloading())
				videoList->cancelDownload(videoItem);
		// update visual controls
		updateVisualControls();
	}
}

void MainFormImpl::moreOptionsClicked()
{
	if (optionsForm_active) return;

	optionsForm_active = true;
	actMoreOptions->setEnabled(false);
    btnMoreOptions->setEnabled(false);

	if (!isVisible()) restoreAppClicked();

	OptionsImpl optionsForm(programOptions, sessionManager, videoList, lastOptionsPage, this);

	if (optionsForm.exec() == QDialog::Accepted)
	{
		updateVisualOptions();
		lastOptionsPage = optionsForm.getLastPageViewed();
	}

	actMoreOptions->setEnabled(true);
    btnMoreOptions->setEnabled(true);
	optionsForm_active = false;
}

void MainFormImpl::clearListClicked()
{
	if (native_alert(this, QMessageBox::Question, tr("Clear list"), tr("Are you sure to clear the downloads list?"), tr("Yes"), tr("No")) == 0)
	{
		videoList->clear(true);
		// update controls
		updateVisualControls();
	}
}

void MainFormImpl::clearCompletedClicked()
{
	videoList->deleteCompletedItems();
}

void MainFormImpl::resetStateClicked()
{
	QList<VideoItem *> videoItems = getSelectedVideoItems();
	// reset all items
	foreach (VideoItem *videoItem, videoItems)
		if (videoItem->isResetable()) // reset item
			videoItem->setAsNULL();
		else // deselect item
			getQTreeWidgetItemByVideoItem(videoItem)->setSelected(false);
}

void MainFormImpl::moveItemUpClicked()
{
	QList<VideoItem *> videoItems = getSelectedVideoItems();
	// only one item can be moved
	if (videoItems.count() == 1)
		videoList->moveUP(videoItems.first());
}

void MainFormImpl::moveItemDownClicked()
{
	QList<VideoItem *> videoItems = getSelectedVideoItems();
	// only one item can be moved
	if (videoItems.count() == 1)
		videoList->moveDOWN(videoItems.first());
}

void MainFormImpl::playVideoClicked()
{
	QList<VideoItem *> videoItems = getSelectedVideoItems();
	// only one item can be played
	if (videoItems.count() == 1)
		if (videoItems.first()->isCompleted() && QFile::exists(videoItems.first()->getVideoFileSavedTo()))
			openDirectory(videoItems.first()->getVideoFileSavedTo());
}

void MainFormImpl::stayOnTopClicked()
{
	programOptions->setStayOnTop(actStayOnTop->isChecked());
	// update the stay on top flag
	setStayOnTopFlag();
}

void MainFormImpl::minimizeToSystemTrayClicked()
{
	programOptions->setMinimizeToSystemTray(actMinimizeToSystemTray->isChecked());
}

void MainFormImpl::viewErrorMessageClicked()
{
	QList<VideoItem *> videoItems = getSelectedVideoItems();
	// only one item error can be displayed
	if (videoItems.count() == 1)
		native_alert(this, QMessageBox::Information, tr("Error message"),
					 tr("This video has the following error:<br><br><b>%1</b>").arg(videoItems.first()->getErrorMessage()),
					 tr("Ok"));
}

void MainFormImpl::searchVideosClicked()
{
#ifdef Q_OS_MACX
	SearchVideosImpl *searchVideos = new SearchVideosImpl(this, Qt::Window);
#else // linux and windows
	SearchVideosImpl *searchVideos = new SearchVideosImpl(this);
#endif
	searchVideos->show();
}

void MainFormImpl::downloadAutomaticallyStateChanged(int state)
{
	programOptions->setCanSendUpdateSignal(false);
	programOptions->setDownloadAutomatically(state == Qt::Checked);
	programOptions->setCanSendUpdateSignal(true);
	// update schedule image
	imgSchedule->setVisible(programOptions->getDownloadAutomatically() && programOptions->getScheduleEnabled());
}

void MainFormImpl::edtDownloadDirChanged()
{
	programOptions->setDownloadDir(edtDownloadDir->text());
}

void MainFormImpl::convertVideosStateChanged(int state)
{
	programOptions->setCanSendUpdateSignal(false);
	programOptions->setConvertVideos(state == Qt::Checked);
	programOptions->setCanSendUpdateSignal(true);
}

// tray icon menu events
void MainFormImpl::trayIconAboutToShow()
{
	updateVisualOptions();
}

// tray icon menu events
void MainFormImpl::optionSelected(bool checked)
{
	if (sender() == actDownloadVideosAutomatically)
		programOptions->setDownloadAutomatically(checked);
	else if (sender() == actConvertVideos)
		programOptions->setConvertVideos(checked);
	else if (sender() == actDisplayPopup)
		programOptions->setDisplayPopup(checked);
}

void MainFormImpl::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::DoubleClick)
		restoreAppClicked();
}

// VideoList events
void MainFormImpl::videoAdded(VideoItem *videoItem)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(lsvDownloadList);

	item->setData(0, Qt::UserRole, videoItem->getID());
	item->setData(2, Qt::UserRole, videoItem->getID());

	item->setTextAlignment(1, Qt::AlignRight   | Qt::AlignVCenter);
	item->setTextAlignment(2, Qt::AlignHCenter | Qt::AlignVCenter);
	item->setTextAlignment(3, Qt::AlignHCenter | Qt::AlignVCenter);
	item->setTextAlignment(4, Qt::AlignHCenter | Qt::AlignVCenter);

	if (videoItem->isCustomDownload()) // get the "custom video" icon
        item->setIcon(0, QPixmap(":/services/images/services/custom_video.svg"));
	else // get service icon
		item->setIcon(0, QIcon(videoList->getVideoInformation()->getHostImage(videoItem->getURL())));

	item->setSizeHint(0, QSize(18,18));

	videoUpdated(videoItem);

	// save the current session...
	sessionManager->saveSession(videoList);
}

void MainFormImpl::videoDeleted(VideoItem *videoItem)
{
	delete getQTreeWidgetItemByVideoItem(videoItem);
    lsvDownloadList->setCurrentItem(nullptr);
	updateVisualControls();
}

void MainFormImpl::videoRenamed(VideoItem *videoItem)
{
	QTreeWidgetItem *item = getQTreeWidgetItemByVideoItem(videoItem);
	item->setText(0, videoItem->getDisplayLabel());
	item->setToolTip(0, videoItem->getDisplayLabel());
}

void MainFormImpl::videoUpdated(VideoItem *videoItem)
{
	QTreeWidgetItem *item = getQTreeWidgetItemByVideoItem(videoItem);

	item->setText(0, videoItem->getDisplayLabel());
	item->setText(1, videoItem->getDisplaySize());
	item->setText(2, videoItem->getDisplayProgress());
	item->setText(3, videoItem->getDisplayTimeRemaining());
	item->setText(4, videoItem->getDisplayDownloadSpeed());

	// set hints
	item->setToolTip(0, videoItem->getDisplayLabel());
	// if is being converted, then display the "%"
	if (videoItem->isDownloading() || videoItem->isConverting())
		item->setToolTip(2, videoItem->getVideoStateAsString() + " (" + videoItem->getDisplayProgress() + ")");
	// if has errors, then display the error message
	else if (videoItem->hasErrors())
		item->setToolTip(2, videoItem->getErrorMessage());
	// do not display anything...
	else
		item->setToolTip(2, "");

	// if this video need a login, then display a "lock image"
	if (videoItem->needLogin())
        item->setIcon(2, QIcon(":/buttons/images/lock.svg"));

	// display completed popup
	if (videoItem->isCompleted())
	{
		// add to log
		sessionManager->addToLog(videoItem);
		// display popup
		if (programOptions->getDisplayPopup())
			completedPopup->displayPopup(item->icon(0),
										 videoItem->getVideoInformation().title,
										 videoItem->getVideoFileSavedTo());
	}

	// update the visual controls
	updateVisualControls();
}

void MainFormImpl::videoError(VideoItem *videoItem)
{
    if (videoItem == nullptr) return;
	if (videoItem->isReported() || !videoItem->hasErrors()) return;

	// mark as reported
	videoItem->setAsReported();

	// error form
	if (programOptions->getDisplayBugReport())
	{
		// display only the "Bug report" only if is a connection error
		if (videoItem->hasAnImportantError())
		{
			// display the main form if it is not visible
			if (!isVisible()) restoreAppClicked();

			// update tray icon
            QString trayIconStr = ":/icons/images/film_error.svg";
			trayIcon->setIcon(QIcon(trayIconStr));
			lastTrayIconStr = trayIconStr;

			// display error report form
			BugReportImpl errorReport(programOptions);
			errorReport.fillErrorInfo(videoItem, videoList->getVideoInformation());
			errorReport.exec();
		}
	}

	// update the visual controls
	updateVisualControls();
}

void MainFormImpl::videoMoved(int from, int to)
{
	QTreeWidgetItem *item;
	QTreeWidgetItem *itemFrom = lsvDownloadList->topLevelItem(from)->clone();
	QTreeWidgetItem *itemTo = lsvDownloadList->topLevelItem(to)->clone();

	// FROM ITEM
	item = lsvDownloadList->topLevelItem(from);
	item->setData(0, Qt::UserRole, itemTo->data(0, Qt::UserRole));
	item->setData(2, Qt::UserRole, itemTo->data(2, Qt::UserRole));
	item->setIcon(0, itemTo->icon(0));
	item->setText(0, itemTo->text(0));
	item->setText(1, itemTo->text(1));
	item->setText(2, itemTo->text(2));
	item->setText(3, itemTo->text(3));
	item->setText(4, itemTo->text(4));
	item->setToolTip(0, itemTo->toolTip(0));
	item->setToolTip(2, itemTo->toolTip(2));

	// TO ITEM
	item = lsvDownloadList->topLevelItem(to);
	item->setData(0, Qt::UserRole, itemFrom->data(0, Qt::UserRole));
	item->setData(2, Qt::UserRole, itemFrom->data(2, Qt::UserRole));
	item->setIcon(0, itemFrom->icon(0));
	item->setText(0, itemFrom->text(0));
	item->setText(1, itemFrom->text(1));
	item->setText(2, itemFrom->text(2));
	item->setText(3, itemFrom->text(3));
	item->setText(4, itemFrom->text(4));
	item->setToolTip(0, itemFrom->toolTip(0));
	item->setToolTip(2, itemFrom->toolTip(2));

	// select the "to item"
	lsvDownloadList->setCurrentItem(lsvDownloadList->topLevelItem(to));

	// destroy temporal items
	delete itemFrom;
	delete itemTo;
}

// shortcut signals
void MainFormImpl::pasteURLfromClipboardClicked()
{
	QStringList urls = QApplication::clipboard()->text().trimmed().split("\n", QString::SkipEmptyParts);
	// remove duplicated urls
	urls.removeDuplicates();
	// add each detected url
	foreach (QString url, urls) addVideo(url);
}

// updates
void MainFormImpl::checkForUpdates()
{
	bool forceCheckUpdates = !videoList->getVideoInformation()->hasPlugins();

	// cehck parameters
	if (!forceCheckUpdates)
		for (int n = 1; n < qApp->arguments().count(); n++)
		{
			if (qApp->arguments().at(n) == "-forceCheckUpdates")
			{
				forceCheckUpdates = true;
				break;
			}
		}

	// check updates?
	if (programOptions->getCheckForUpdatesOnStartup() || forceCheckUpdates)
		// check if the xUpdater is installed (can install updates?)
		if (!Updates::canUpdate(programOptions->getToolsPath())) //getOptionsPath()))
		{
			actUpdates->setEnabled(false);
            spbUpdates->setEnabled(false);
			// running the app for 1st time? then display this warning message
			if (programOptions->getFirstTime())
				native_alert(this, QMessageBox::Information, tr("Updates"),
							 tr("xUpdater application is missing.<br><br>Reinstall xVideoServiceThief if you want update automatically the program."),
							 tr("Ok"));
			// start the download list
			videoList->start();
			// ok, no more first time
			programOptions->setFirstTime(false);
		}
		else // can check for updates
			checkUpdates(forceCheckUpdates);
	else // start the download list, and do not check for updates
		videoList->start();
}

void MainFormImpl::checkUpdatesWorkerFinished(bool hasUpdates, bool /*closedByButton*/)
{
    spbUpdates->setEnabled(true);
	actUpdates->setEnabled(true);

    lblCheckForUpdatesLabel->hide();
    pbrCheckingForUpdates->hide();
    spbCancelCheckForUpdates->hide();

	// if no updates are ready then, start the main loop of video List
	if (!hasUpdates)
		videoList->start();

	delete checkUpdatesWorker;
}

void MainFormImpl::beforeDisplayUpdateCenter()
{
	if (!isVisible()) restoreAppClicked();
}

// MacOSX menuBar signals
void MainFormImpl::menuBarAboutToShow()
{
	updateVisualControls();
}

// Single application instance - messages from another new instance
void MainFormImpl::SingleApplicationHandleMessage(const QString& message)
{
	QStringList URLs = message.split("\n", QString::SkipEmptyParts);
	// add url's
	foreach (QString URL, URLs)
	{
		videoList->addVideo(URL);
	}
}

// lsvDownloadList functions
QTreeWidgetItem* MainFormImpl::getQTreeWidgetItemByVideoItem(VideoItem *videoItem)
{
	for (int i = 0; i < lsvDownloadList->topLevelItemCount(); ++i)
	{
		QVariant variant = lsvDownloadList->topLevelItem(i)->data(0, Qt::UserRole);
		if (videoList->getVideoItemFromQVAriant(variant) == videoItem)
			return lsvDownloadList->topLevelItem(i);
	}
    return nullptr;
}

QList<VideoItem *> MainFormImpl::getSelectedVideoItems()
{
	QList<VideoItem *> results;
	// get all video items
	foreach (QTreeWidgetItem *treeItem, lsvDownloadList->selectedItems())
		results.append(getVideoItemByQTreeWidgetItem(treeItem));
	// return the list of videos
	return results;
}

VideoItem* MainFormImpl::getVideoItemByQTreeWidgetItem(QTreeWidgetItem* treeItem)
{
    if (treeItem != nullptr)
		return videoList->getVideoItemFromQVAriant(treeItem->data(0, Qt::UserRole));
	else
        return nullptr;
}

// options controls
void MainFormImpl::updateVisualOptions()
{
	// update internal options
	videoList->updateOptions();

	// actions
	actDownloadVideosAutomatically->setChecked(programOptions->getDownloadAutomatically());
	actConvertVideos->setEnabled(QFile::exists(programOptions->getFfmpegLibLocation()));
	actConvertVideos->setChecked(actConvertVideos->isEnabled() ? programOptions->getConvertVideos() : false);
	actDisplayPopup->setChecked(programOptions->getDisplayPopup());

	// other action options
	actStayOnTop->setChecked(programOptions->getStayOnTop());
	actMinimizeToSystemTray->setChecked(programOptions->getMinimizeToSystemTray());

	// controls
	chbDownloadVideosAuto->setChecked(programOptions->getDownloadAutomatically());
	chbConvertVideos->setEnabled(QFile::exists(programOptions->getFfmpegLibLocation()));
	chbConvertVideos->setChecked(chbConvertVideos->isEnabled() ? programOptions->getConvertVideos() : false);
	edtDownloadDir->setText(programOptions->getDownloadDir());

	// schedule image
	imgSchedule->setVisible(programOptions->getDownloadAutomatically() && programOptions->getScheduleEnabled());
}

void MainFormImpl::checkUpdates(bool forceCheckUpdates)
{
	QDate nextUpdate = programOptions->getLastUpdate().addDays(programOptions->getCheckForUpdatesEvery());

	if (nextUpdate <= QDate::currentDate() || forceCheckUpdates)
	{
        spbUpdates->setEnabled(false);
		actUpdates->setEnabled(false);

        lblCheckForUpdatesLabel->show();
        pbrCheckingForUpdates->show();
        spbCancelCheckForUpdates->show();

        checkUpdatesWorker = new CheckUpdatesWorker(programOptions, this, lblCheckForUpdatesLabel, pbrCheckingForUpdates,
                                                    spbCancelCheckForUpdates, false);
		connect(checkUpdatesWorker, SIGNAL(finished(bool, bool)), this, SLOT(checkUpdatesWorkerFinished(bool, bool)));
		connect(checkUpdatesWorker, SIGNAL(beforeDisplayUpdateCenter()), this, SLOT(beforeDisplayUpdateCenter()));
		checkUpdatesWorker->checkUpdates();
	}
	else // no updates, start the main loop of video List
		videoList->start();
}

void MainFormImpl::displayWelcomeMessage()
{
	// check if is possible display the welcome message
	if (programOptions->getDisplayWelcomeMessage())
	{
		LoadingImpl::instance()->closeLoading();
		// display welcome window
		WelcomeDonate welcome(this);
		welcome.exec();
		// continue displaying this message?
		programOptions->setDisplayWelcomeMessage(!welcome.getDisplayAgain());
	}
}

void MainFormImpl::displayAdultSitesPermission()
{
	if ( ! programOptions->getAdultsSitePermissionAsked())
	{
		LoadingImpl::instance()->closeLoading();
		// display welcome window
		AdultsPermissionImpl adultsPermission(this);
		programOptions->setAdultSitesAreAllowed(!adultsPermission.exec());
		// uninstall the adult sites if needed
		if ( ! programOptions->getAdultSitesAreAllowed())
		{
			videoList->getVideoInformation()->removeAdultPlugins(true);
		}
		// ok, don't ask it again
		programOptions->setAdultsSitePermissionAsked(true);
	}
}

void MainFormImpl::displayWinVistaDownloads()
{
#ifdef Q_OS_WIN32
	// only is needed on windows vista and we want check it
	if (isWindowsVista() && programOptions->getDisplayDownloadsMigrator())
	{
		// creates the migrator window
		WinVistaDownloadsMsgImpl downloadsMsg(this);
		// display the migrator window if is needed
		if (downloadsMsg.isInProgramFiles())
		{
			// close the loading window
			LoadingImpl::instance()->closeLoading();
			// display message
			if (downloadsMsg.exec() == QDialog::Accepted)
			{
				edtDownloadDir->setText(downloadsMsg.getDownloadsDir());
				edtDownloadDirChanged();
			}
		}
		// continue displaying this message?
		programOptions->setDisplayDownloadsMigrator(!downloadsMsg.getDisplayAgain());
	}
#endif
}

void MainFormImpl::displayWhatsNewMessage()
{
	// display the whats new window only if curr. version and latest are different
	if (programOptions->getLatestVersionExecuted() != PROGRAM_VERSION_SHORT)
	{
		// close the loading window
		LoadingImpl::instance()->closeLoading();
		// display whats new window
		WhatsNewImpl whatsNew(this);
		whatsNew.exec();
	}
	// update the latest version with current version
	programOptions->setLatestVersionExecuted(PROGRAM_VERSION_SHORT);
}

// stay on top control
void MainFormImpl::setStayOnTopFlag()
{
	if (programOptions->getStayOnTop())
		setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	else
		setWindowFlags(windowFlags() & (~ Qt::WindowStaysOnTopHint));

	// refresh window
	show();
}

// Information window

void MainFormImpl::updateListInformation()
{
	lblVideosCount->setText(tr("<b>Videos count:</b> %1 <span style=\"color:#666666;\">(%2 completed)</span>")
		.arg(videoList->getVideoItemCount(true))
		.arg(videoList->getCompletedItemsCount()));

	lblDownloadingCount->setText(tr("<b>Downloading:</b> %1 of %2")
		.arg(videoList->getActiveDownloadsCount())
		.arg(videoList->getDownloableVideosCount()));

	lblDownloadSpeed->setText(tr("<b>Download speed:</b> %1")
		.arg(fileSizeToString(videoList->getTotalDownloadSpeed()) + tr("/sec")));
}

// visual controls
void MainFormImpl::updateVisualControls()
{
	QList<VideoItem *> videoItems = getSelectedVideoItems();

	if (videoItems.isEmpty()) // no items selected
	{
        btnDeleteVideo->setEnabled(false);
        btnStartDownload->setEnabled(false);
        btnPauseResumeDownload->setEnabled(false);
        btnCancelDownload->setEnabled(false);

        btnDeleteVideo->setText(tr("Delete video"));
        btnStartDownload->setText(tr("Start download"));
        btnPauseResumeDownload->setText(tr("Pause download"));
        btnCancelDownload->setText(tr("Cancel download"));

		actPlayVideo->setEnabled(false);
		actPlayVideo->setVisible(true);
		actViewErrorMessage->setVisible(false);
		actResetState->setEnabled(false);
		actStartDownloadAgain->setEnabled(false);

		actMoveUP->setEnabled(false);
		actMoveDOWN->setEnabled(false);
	}
	else if (videoItems.count() == 1) // one video item selected
	{
		VideoItem *videoItem = videoItems.first();

        btnDeleteVideo->setEnabled(videoItem->isRemovable());
        btnStartDownload->setEnabled(videoItem->isDownloadable());
        btnPauseResumeDownload->setEnabled(videoItem->isPauseable() || videoItem->isAnyKindOfPaused());
        btnCancelDownload->setEnabled(videoItem->isDownloading());

        btnDeleteVideo->setText(tr("Delete video"));
        btnStartDownload->setText(tr("Start download"));
        btnPauseResumeDownload->setText(videoItem->isAnyKindOfPaused() ? tr("Resume download") : tr("Pause download"));
        btnCancelDownload->setText(tr("Cancel download"));
		actRenameVideo->setText(tr("Rename video"));
		actResetState->setText(tr("Reset state"));

		actPlayVideo->setEnabled(videoItem->isCompleted());
		actPlayVideo->setVisible(!videoItem->hasErrors());
		actViewErrorMessage->setVisible(videoItem->hasErrors());
		actResetState->setEnabled(videoItem->isResetable());
		actStartDownloadAgain->setEnabled(videoItem->isCompleted());

		// update move up/down actions
		actMoveUP->setEnabled(videoList->getVideoItem(0) != videoItem);
		actMoveDOWN->setEnabled(videoList->getVideoItem(videoList->getVideoItemCount() - 1) != videoItem);
	}
	else // two or more items selected
	{
		bool canDelete = false;
		bool canStart = false;
		bool canPause = false;
		bool canResume = false;
		bool canCancel = false;
		bool canReset = false;
		bool canDownloadAgain = false;
		// check each video item and update the control flags
		foreach (VideoItem *videoItem, videoItems)
		{
			if (videoItem->isRemovable()) canDelete = true;
			if (videoItem->isDownloadable()) canStart = true;
			if (videoItem->isPauseable()) canPause = true;
			if (videoItem->isAnyKindOfPaused()) canResume = true;
			if (videoItem->isDownloading()) canCancel = true;
			if (videoItem->isResetable()) canReset = true;
			if (videoItem->isCompleted()) canDownloadAgain = true;
		}

        btnDeleteVideo->setEnabled(canDelete);
        btnStartDownload->setEnabled(canStart);
        btnPauseResumeDownload->setEnabled((canPause && !canResume) || (!canPause && canResume));
        btnCancelDownload->setEnabled(canCancel);

        btnDeleteVideo->setText(tr("Delete videos"));
        btnStartDownload->setText(tr("Start downloads"));
		bool pauseText = (canPause && !canResume) || (canPause == canResume);
        btnPauseResumeDownload->setText(pauseText ? tr("Pause downloads") : tr("Resume downloads"));
        btnCancelDownload->setText(tr("Cancel downloads"));
		actRenameVideo->setText(tr("Rename videos"));
		actResetState->setText(tr("Reset states"));

		actPlayVideo->setEnabled(false);
		actPlayVideo->setVisible(true);
		actViewErrorMessage->setVisible(false);
		actResetState->setEnabled(canReset);
		actResetState->setEnabled(canDownloadAgain);

		// update move up/down actions
		actMoveUP->setEnabled(false);//(videoList->getVideoItem(0) != videoItem);
		actMoveDOWN->setEnabled(false);//(videoList->getVideoItem(videoList->getVideoItemCount() - 1) != videoItem);
	}

    btnClearList->setEnabled(!videoList->isWorking() && videoList->getVideoItemCount(true) > 0);
    btnClearCompleted->setEnabled(videoList->getCompletedItemsCount() > 0);

	// update actions
    actDeleteVideo->setEnabled(btnDeleteVideo->isEnabled());
    actDeleteVideo->setText(btnDeleteVideo->text());
    actRenameVideo->setEnabled(btnDeleteVideo->isEnabled());
    actStartDownload->setEnabled(btnStartDownload->isEnabled());
    actStartDownload->setText(btnStartDownload->text());
    actPauseResumeDownload->setEnabled(btnPauseResumeDownload->isEnabled());
    actPauseResumeDownload->setText(btnPauseResumeDownload->text());
    actCancelDownload->setEnabled(btnCancelDownload->isEnabled());
    actCancelDownload->setText(btnCancelDownload->text());
    actClearList->setEnabled(btnClearList->isEnabled());
    actClearCompleted->setEnabled(btnClearCompleted->isEnabled());

	// update tray icon
    QString trayIconStr = videoList->isWorking() ? ":/icons/images/tray_icon_on.svg" : ":/icons/images/tray_icon_off.svg";
	if (lastTrayIconStr != trayIconStr)
		trayIcon->setIcon(QIcon(trayIconStr));
	lastTrayIconStr = trayIconStr;
	// set the tray icon tool tip
	QString trayIconToolTip = this->windowTitle();
	// set the current download video
	if (videoList->isDownloading())
	{
		// get all current downloads
		for (int n = 0; n < videoList->getVideoItemCount(); n++)
		{
			VideoItem *videoItem = videoList->getVideoItem(n);
			// is it downloading?
			if (videoItem->isDownloading())
				trayIconToolTip += "\n" + tr("- Downloading: %1 (%2, %3)").arg(videoItem->getDisplayLabel()).arg(videoItem->getDisplayDownloadSpeed()).arg(videoItem->getDisplayProgress());
		}
	}
	// set the current convertion video
	if (videoList->isConverting())
	{
		VideoItem *videoItem = videoList->getCurrentConvertingVideo();
        if (videoItem != nullptr)
			trayIconToolTip += "\n" + tr("- Converting: %1 (%2)").arg(videoItem->getDisplayLabel()).arg(videoItem->getDisplayProgress());
	}
	// set the tooltip
	trayIcon->setToolTip(trayIconToolTip);
	// update information
	updateListInformation();
    // update mac toolbar items
#ifdef Q_OS_MACX
    toolBar->setButtonItemSelectable(toolBarItemAdd, actAddVideo->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemRename, actRenameVideo->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemDelete, actDeleteVideo->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemStart, actStartDownload->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemPause, actPauseResumeDownload->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemStop, actStartDownload->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemClearList, actClearList->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemClearCompleted, actClearCompleted->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemSearch, actSearchVideos->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemDragDrop, actDragDrop->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemUpdate, actUpdates->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemHelp, actOnlineHelp->isEnabled());
    toolBar->setButtonItemSelectable(toolBarItemInfo, actInformation->isEnabled());
#endif
}

// lsvDownloadList signals
void MainFormImpl::videoItemDoubleClicked(QTreeWidgetItem *, int)
{
	playVideoClicked();
}

void MainFormImpl::videoItemSelectionChanged()
{
	updateVisualControls();
}

void MainFormImpl::videoListContextMenu(const QPoint & pos)
{
	QTreeWidgetItem *item = lsvDownloadList->itemAt(pos);
	VideoItem *videoItem = getVideoItemByQTreeWidgetItem(item);

    if (item != nullptr && videoItem != nullptr)
	{
		QMenu *videoItemMenu = new QMenu(this);

		// the default video is the "View error message" cuz has an error
		if (videoItem->hasErrors())
		{
			videoItemMenu->addAction(actViewErrorMessage);
			// set defaut
			videoItemMenu->setDefaultAction(actViewErrorMessage);
		}
		else // the default action is the "PLAY video"
		{
			videoItemMenu->addAction(actPlayVideo);
			// set defaut
			videoItemMenu->setDefaultAction(actPlayVideo);
		}
		// add the rest of actions
		videoItemMenu->addSeparator();
		videoItemMenu->addAction(actAddVideo);
		videoItemMenu->addAction(actDeleteVideo);
		videoItemMenu->addAction(actRenameVideo);
		videoItemMenu->addSeparator();
		videoItemMenu->addAction(actStartDownload);
		videoItemMenu->addAction(actPauseResumeDownload);
		videoItemMenu->addAction(actCancelDownload);
		videoItemMenu->addSeparator();
		videoItemMenu->addAction(actMoveUP);
		videoItemMenu->addAction(actMoveDOWN);
		videoItemMenu->addSeparator();
		videoItemMenu->addAction(actResetState);
		videoItemMenu->addAction(actStartDownloadAgain);
		videoItemMenu->addSeparator();
		videoItemMenu->addAction(actClearList);
		videoItemMenu->addAction(actClearCompleted);

		// display popup
		videoItemMenu->exec(QCursor::pos());
		// destrueix el submenu
		delete videoItemMenu;
	}
	else
	{
        lsvDownloadList->setCurrentItem(nullptr);
		updateVisualControls();

		QMenu *videoItemMenu = new QMenu(this);
		videoItemMenu->addAction(actAddVideo);
		videoItemMenu->addSeparator();
		videoItemMenu->addAction(actClearList);
		videoItemMenu->addAction(actClearCompleted);
		// display popup
		videoItemMenu->exec(QCursor::pos());
		// destrueix el submenu
		delete videoItemMenu;
	}
}

// options signals
void MainFormImpl::optionsDidSomething()
{
	updateVisualOptions();
}

// drop functions
void MainFormImpl::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/plain"))
		event->acceptProposedAction();
}

void MainFormImpl::dropEvent(QDropEvent *event)
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

bool MainFormImpl::canAddThisVideo(QString URL)
{
	if (videoList->isAlreadyAdded(URL))
	{
		native_alert(this, QMessageBox::Information, tr("Already added"), tr("You already added this video. Check your downloads list."), tr("Ok"));
		// can't add this video
		return false;
	}
	else // yes, can add it
		return true;
}

void MainFormImpl::addVideo(QString URL)
{
	// init vars
	bool ok = videoList->getVideoInformation()->isValidHost(URL) && canAddThisVideo(URL);
	// can be added?
	if (ok)	videoList->addVideo(URL);
}
