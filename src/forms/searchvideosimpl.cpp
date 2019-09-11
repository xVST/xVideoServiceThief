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
* Program URL   : https://www.xvideothief.com/
*
*/

#include "searchvideosimpl.h"

#include "searchvideositemimpl.h"
#include "searchvideosnoresultsimpl.h"
#include "searchvideossearchblockimpl.h"
#include "searchvideoscustomizeimpl.h"

#include "../videoinformation.h"
#include "../searchvideos.h"
#include "../tools.h"

SearchVideosImpl::SearchVideosImpl(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	centerWindow();
	//
	searchVideos = new SearchVideos();
	loadingMovie = new QMovie(":/search/images/loader.gif");
	loadingMovie->start();
	//
	layoutSearchItems->setAlignment(Qt::AlignTop);
	//
	updateButons(true);
	fillSearchServices();
	stackedResults->setCurrentIndex(1);
	//
	restoreLastSearchSettings();
	//
	connect(this, SIGNAL(finished(int)), this, SLOT(finished(int)));
	connect(cmbSearchIn, SIGNAL(activated(int)), this, SLOT(cmbSearchInActivated(int)));
	connect(edtKeyWord, SIGNAL(textChanged(QString)), this, SLOT(edtKeyWordChanged(QString)));
	connect(btnSearch, SIGNAL(clicked()), this, SLOT(btnSearchClicked()));
	connect(btnPrevSearch, SIGNAL(clicked()), this, SLOT(btnPrevSearchClicked()));
	connect(btnNextSearch, SIGNAL(clicked()), this, SLOT(btnNextSearchClicked()));
	connect(searchVideos, SIGNAL(searchStarted()), this, SLOT(searchStarted()));
	connect(searchVideos, SIGNAL(addNewSearchBlock(VideoInformationPlugin*)), this, SLOT(addNewSearchBlock(VideoInformationPlugin *)));
	connect(searchVideos, SIGNAL(searchFinished()), this, SLOT(searchFinished()));
	connect(searchVideos, SIGNAL(searchResultAdded(SearchResultItem*)), this, SLOT(searchResultAdded(SearchResultItem*)));
	connect(searchVideos, SIGNAL(startedDownloadPreview(SearchResultItem*)), this, SLOT(startedDownloadPreview(SearchResultItem*)));
	connect(searchVideos, SIGNAL(finishedDownloadPreview(SearchResultItem*,bool)), this, SLOT(finishedDownloadPreview(SearchResultItem*,bool)));
}

SearchVideosImpl::~SearchVideosImpl()
{
	// save last search settings
	saveLastSearchSettings();
	// disconnect all signals
	disconnect();
	// destroy objects
	delete loadingMovie;
	delete searchVideos;
}

void SearchVideosImpl::finished(int)
{
	this->deleteLater();
}

void SearchVideosImpl::cmbSearchInActivated(int /*index*/)
{
	if (cmbSearchIn->itemData(cmbSearchIn->currentIndex(), Qt::UserRole).toString() == SEARCH_ID_CUSTOM)
	{
		SearchVideosCustomizeImpl customize(customServices, this, Qt::Sheet);
		if (showModalDialog(&customize) == QDialog::Accepted)
			customServices = customize.getCustomServices();
	}
}

void SearchVideosImpl::edtKeyWordChanged(QString text)
{
	spinBoxPage->setValue(1);
	updateButons(text.isEmpty());
}

void SearchVideosImpl::btnSearchClicked()
{
	clearResults();
	// custom search?
	if (cmbSearchIn->itemData(cmbSearchIn->currentIndex(), Qt::UserRole).toString() == SEARCH_ID_CUSTOM)
	{
		if (customServices.isEmpty())
		{
			native_alert(this, QMessageBox::Information, tr("Custom search"), tr("Please, select one or more services."), 	tr("Ok"));
			// display custom search services
			cmbSearchInActivated(cmbSearchIn->currentIndex());
			// abort process
			return;
		}
		else
			searchVideos->searchVideos(edtKeyWord->text(), spinBoxPage->value(), customServices);
	}
	else // single search
		searchVideos->searchVideos(edtKeyWord->text(), spinBoxPage->value(),
								   QStringList() << cmbSearchIn->itemData(cmbSearchIn->currentIndex(), Qt::UserRole).toString());
	updateButons(true);
}

void SearchVideosImpl::btnPrevSearchClicked()
{
	if (spinBoxPage->value() > 1)
	{
		spinBoxPage->setValue(spinBoxPage->value() - 1);
		btnSearch->click();
	}
}

void SearchVideosImpl::btnNextSearchClicked()
{
	spinBoxPage->setValue(spinBoxPage->value() + 1);
	btnSearch->click();
}

void SearchVideosImpl::searchStarted()
{
	btnSearch->setEnabled(false);
	//
	imgSearching->setMovie(loadingMovie);
	lblSearching->setText(tr("Searching \"<b>%1</b>\" (page %2)... please wait...").arg(edtKeyWord->text()).arg(spinBoxPage->value()));
	// display searching sping
	stackedResults->setCurrentIndex(0);
}

void SearchVideosImpl::addNewSearchBlock(VideoInformationPlugin *plugin)
{
	layoutSearchItems->addWidget(new SearchVideosSearchBlockImpl(this, plugin, searchVideos->getSearchSummary()));
}

void SearchVideosImpl::searchFinished()
{
	// enable buttons again
	updateButons(false);
	// display results
	stackedResults->setCurrentIndex(1);
	// start to download the preview images
	searchVideos->downloadPreviewImages();
}

void SearchVideosImpl::searchResultAdded(SearchResultItem *searchResultItem)
{
	if (searchResultItem != nullptr) // add a new widget before the strech item (top alignment)
		layoutSearchItems->addWidget(new SearchVideosItemImpl(this, searchResultItem, loadingMovie));
	else // add a new widget displaying the message: "no results found..."
		layoutSearchItems->addWidget(new SearchVideosNoResultsImpl(this, searchVideos->getKeyWords()));
}

void SearchVideosImpl::startedDownloadPreview(SearchResultItem* /*searchItem*/)
{
	// Nothing to do...
}

void SearchVideosImpl::finishedDownloadPreview(SearchResultItem *searchItem, bool /*error*/)
{
	QList<SearchVideosItemImpl*> items = getSearchVideosItemImplBySearchItem(searchItem);
	for (int n = 0; n < items.count(); n++)
		items.at(n)->reloadPreview();
}

QList<SearchVideosItemImpl*> SearchVideosImpl::getSearchVideosItemImplBySearchItem(SearchResultItem *item)
{
	if (item != nullptr)
		return this->findChildren<SearchVideosItemImpl*>(item->getVideoId());
	else
		return QList<SearchVideosItemImpl*>();
}

void SearchVideosImpl::clearResults()
{
	while (!layoutSearchItems->isEmpty())
		delete layoutSearchItems->takeAt(0)->widget();
}

void SearchVideosImpl::updateButons(bool searching)
{
	btnPrevSearch->setEnabled(!searching && spinBoxPage->value() > 1);
	btnNextSearch->setEnabled(!searching);
	btnSearch->setEnabled(!searching);
}

void SearchVideosImpl::fillSearchServices()
{
	QList<VideoInformationPlugin*> searchEngines = VideoInformation::instance()->getAllSearchPlugins();
	// has more than one item?
	if (searchEngines.count() > 1)
	{
		// if adult sites are not blocked then add these two search options
		if (!VideoInformation::instance()->getBlockAdultContent())
		{
			cmbSearchIn->addItem(tr("All standard services"), QVariant(SEARCH_ID_STANDARD));
			cmbSearchIn->addItem(tr("All adult services"), QVariant(SEARCH_ID_ADULTS));
		}
		cmbSearchIn->addItem(tr("All services"), QVariant(SEARCH_ID_ALL));
		cmbSearchIn->insertSeparator(cmbSearchIn->count());
		cmbSearchIn->addItem(tr("Custom search"), QVariant(SEARCH_ID_CUSTOM));
		cmbSearchIn->insertSeparator(cmbSearchIn->count());
	}
	// add them to combobox
	for (int n = 0; n < searchEngines.count(); n++)
		if (!VideoInformation::instance()->getBlockAdultContent() ||
                    (VideoInformation::instance()->getBlockAdultContent() && !searchEngines.at(n)->hasAdultContent()))
		{
			cmbSearchIn->addItem(*(searchEngines.at(n)->getIcon()),
								 searchEngines.at(n)->getCaption(),
								 QVariant(searchEngines.at(n)->getID()));
		}
}

void SearchVideosImpl::centerWindow()
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

	x = (screenWidth - width) / 2;
	y = (screenHeight - height) / 2;
	y -= 50;

	move(x, y);
}

void SearchVideosImpl::restoreLastSearchSettings()
{
	SearchVideosSettings lastSearchSettings;
	lastSearchSettings.load();
	// get the list of custom services
	customServices = lastSearchSettings.getPluginsIds();
	// check if the last searchIn is adults and ups is enabled
	if (lastSearchSettings.getSearchInId() == SEARCH_ID_ADULTS && VideoInformation::instance()->getBlockAdultContent())
	{
		cmbSearchIn->setCurrentIndex(0);
		return;
	}
	// get combobox option which corresponds to the searchIn
	for (int n = 0; n < cmbSearchIn->count(); n++)
		if (cmbSearchIn->itemData(n, Qt::UserRole).toString() == lastSearchSettings.getSearchInId())
		{
			cmbSearchIn->setCurrentIndex(n);
			return;
		}
	// not found, then the default option is the first item
	cmbSearchIn->setCurrentIndex(0);
}

void SearchVideosImpl::saveLastSearchSettings()
{
	SearchVideosSettings lastSearchSettings;
	// assign last search settings
	lastSearchSettings.setPluginsIds(customServices);
	lastSearchSettings.setSearchInId(cmbSearchIn->itemData(cmbSearchIn->currentIndex(), Qt::UserRole).toString());
	// save settings
	lastSearchSettings.save();
}
