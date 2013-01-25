#ifndef MULTIMEDIASCONTAINER_H
#define MULTIMEDIASCONTAINER_H

#include <QtGui>

class  MultiMediasContainerItem : public QObject
{
Q_OBJECT
	private:
		QString title;			//!< Download title
		QString url;			//!< Download URL
		QStringList userInfo;	//!< Extra user information
	public:
		/*! Class constructor */
		MultiMediasContainerItem(QString title, QString url, QStringList userInfo = QStringList());
		/*! Get title */
		QString getTitle();
		/*! Get URL */
        QString getURL();
		/*! Extra item info */
		QStringList getUserInfo();
};

class MultiMediasContainer : public QObject
{
Q_OBJECT
	private:
		QList<MultiMediasContainerItem *> *multiDownloadsItems;	//!< Stores all the detected downloads
	public:
		/*! Class constructor */
		MultiMediasContainer();
		/*! Copy class constructor */
		MultiMediasContainer(const MultiMediasContainer&);
		/*! Class destructor */
		~MultiMediasContainer();
		/*! Override = operator */
		MultiMediasContainer &operator=(const MultiMediasContainer&);
		/*! Ask to user what urls should download */
		void askItemsToDownload();
		/*! Add a new media URL */
		void addMediaURL(QString title, QString url, QStringList userInfo = QStringList());
};

#endif // MULTIMEDIASCONTAINER_H
