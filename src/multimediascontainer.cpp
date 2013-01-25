#include "multimediascontainer.h"

#include "forms/multimediasdetectedimpl.h"

#include "tools.h"

// MultiMediasContainerItem

MultiMediasContainerItem::MultiMediasContainerItem(QString title, QString url, QStringList userInfo)
{
	this->title = title;
	this->url = url;
	this->userInfo = userInfo;
}

QString MultiMediasContainerItem::getTitle()
{
	return this->title;
}

QString MultiMediasContainerItem::getURL()
{
	return this->url;
}

QStringList MultiMediasContainerItem::getUserInfo()
{
	return this->userInfo;
}

// MultiMediasContainer

MultiMediasContainer::MultiMediasContainer()
{
	multiDownloadsItems = new QList<MultiMediasContainerItem *>();
}

MultiMediasContainer::MultiMediasContainer(const MultiMediasContainer &other) : QObject()
{
	multiDownloadsItems = new QList<MultiMediasContainerItem *>();
	// copy all items from "other"
	for (int n = 0; n < other.multiDownloadsItems->count(); n++)
	{
		MultiMediasContainerItem *item = other.multiDownloadsItems->at(n);
		addMediaURL(item->getTitle(), item->getURL(), item->getUserInfo());
	}
}

MultiMediasContainer::~MultiMediasContainer()
{
	delete multiDownloadsItems;
}

MultiMediasContainer &MultiMediasContainer::operator=(const MultiMediasContainer &other)
{
	multiDownloadsItems->clear();
	// copy all items from "other"
	for (int n = 0; n < other.multiDownloadsItems->count(); n++)
	{
		MultiMediasContainerItem *item = other.multiDownloadsItems->at(n);
		addMediaURL(item->getTitle(), item->getURL(), item->getUserInfo());
	}
	// return my self updated
	return *this;
}

void MultiMediasContainer::askItemsToDownload()
{
	MultiMediasDetectedImpl multiMediasPrompt(qApp->activeWindow(), Qt::Sheet);
	// display login prompt
	if (showModalDialog(&multiMediasPrompt) == QDialog::Accepted)
	{
		// hola
	}
}

void MultiMediasContainer::addMediaURL(QString title, QString url, QStringList userInfo)
{
	multiDownloadsItems->append(new MultiMediasContainerItem(title, url, userInfo));
}
