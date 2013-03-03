/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2012 Xesc & Technology
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
* Program URL   : http://xviservicethief.sourceforge.net/
*
*/

#ifndef __UPDATES_H__
#define __UPDATES_H__

#include <QtWidgets>

#ifdef Q_OS_WIN32
#include <windows.h>
static const QString XUPDATER_PATH = "/bin/xUpdater.exe";	//!< xUpdater app (win32)
#else
#ifdef Q_OS_MACX
static const QString XUPDATER_PATH = "/tools/xUpdater";		//!< xUpdater app (MacOSX)
#else
static const QString XUPDATER_PATH = "/bin/xUpdater";		//!< xUpdater app (unix)
#endif
#endif
static const QString XUPDATER_FILE 	= "/xVST.update";			//!< update script file
static const QString XUPDATER_DWN_FILE 	= "/xVST.update.%1";	//!< downloaded update file

enum UpdateState
{
	usWaiting,
	usChecking,
	usDownloading,
	usInstalling
};

class Http;

/*! Update item */
class Update : public QObject
{
	private:
		QString caption;		//!< Update caption
		QString version;		//!< Update version
		int size;				//!< Update file size
		QString installTo;		//!< Where to find and install the new update
		QString url;			//!< Where to download this update
		QString minVersion;		//!< Min version needed to install this update
		QString minVersionUrl;	//!< Min version url
		bool packed;			//!< Flag for know if this update is packed
		bool obligatory;		//!< Flag for know if this update must be downloaed (if don't exists, it is downloaded anyway)
		bool checked;			//!< Selected to update
		bool error;				//!< An error ocurred during download process
		bool isRelativePath;	//!< Flag for know if the "installTo" property is a full or relative path
	public:
		/*! Set the update caption */
		void setCaption(QString value);
		/*! Set the update version */
		void setVersion(QString value);
		/*! Set the update file size */
		void setSize(int value);
		/*! Set where to install this update */
		void setInstallTo(QString value, bool isRelativePath = true);
		/*! Set the update url */
		void setUrl(QString value);
		/*! Set if is update is packed */
		void setPacked(bool value);
		/*! Set if this update is obligatory */
		void setObligatory(bool value);
		/*! Set the min version needed to install this update */
		void setMinVersion(QString value);
		/*! Set the min version url */
		void setMinVersionUrl(QString value);
		/*! Set as selected */
		void setChecked(bool value);
		/*! Set it as error */
		void setError(bool value);
		/*! Get the update caption */
		QString getCaption();
		/*! Get the update version */
		QString getVersion();
		/*! Get the update file size */
		int getSize();
		/*! Get the install path */
		QString getInstallTo();
		/*! Get the update url */
		QString getUrl();
		/*! Get if this update is packed */
		bool isPacked();
		/*! Get if this update is obligatory */
		bool isObligatory();
		/*! Get the min version needed to install this update */
		QString getMinVersion();
		/*! Get the min version message */
		QString getMinVersionUrl();
		/*! Get if is selected */
		bool isChecked();
		/*! Get if has an error */
		bool hasErrors();
		/*! Get if the "installTo" is a relative path */
		bool hasRelativePath();
};

/*! Check and download program updates */
class Updates : public QThread
{
Q_OBJECT
	private:
		QString appPath;				//!< Application path
		QList<Update *>* updateList;	//!< List with all aviable updates
		Http *http;						//!< download class
		QString updateURL;				//!< Where to get the update file
		UpdateState updateState;		//!< Current download state
		bool cancelled;					//!< Cancelled
		int currentItem;				//!< Current download item index
		int totalToDownload;			//!< Total size to download (bytes)
		int totalDownloaded;			//!< Total size downloaded (bytes)
		int currentDownloaded;			//!< Current size downloaded (bytes)
		/*! Parse a block */
		void parseBlock(QString block);
		/*! Parse the downloaded update file */
		void pareseUpdateFile(QString updateFile);
		/*! Determine if has updates */
		bool hasUpdates();
		/*! Clear update items */
		void clear();
		/*! Install updates */
		void buildInstalScript();
		/*! Get the total size to download */
		void getTotalSizeToDownload();
		/*! Get the first update file to download */
		int getFirstUpdateToDownload();
		/*! Get next update to download */
		void getNextUpdateToDownload();
		/*! Build the DMG script installer (MacOSX only) */
		QStringList buildDMGScript(int n);
		/*! Thread executation */
		void run();
		/*! Initialize the Http object (if is needed) */
		void initializeHttp();
		/*! Deinitalize the Http object (if is needed) */
		void deinitHttp();
	public:
		/*! Class constructor */
		Updates(QString appPath);
		/*! Class destructor */
		~Updates();
		/*! Check for update */
		void checkForUpdates(QString url);
		/*! Download updates */
		void downloadUpdates();
		/*! Run the xUpdater and install the updates */
		void installUpdates();
		/*! Cancel the current process */
		void cancel();
		/*! Get an update item */
		Update* getUpdateItem(const int index);
		/*! Get updates count */
		int getUpdatesCount();
		/*! Get total downloaded */
		int getCurrentDownloaded();
		/*! Get total to download */
		int getTotalToDownload();
		/*! Check if the xUpdater is installed */
		static bool canUpdate(QString searchPath);
	private slots:
		/*! When the http read data */
		void downloadEvent(int pos, int max);
		/*! When the http finish a download */
		void downloadFinished(const QFileInfo destFile);
		/*! When the http raise an error */
		void downloadError(int error);
	signals:
		/*! After check for updates */
		void updatesChecked(bool hasUpdates);
		/*! When updates need to display a "special message" */
		void updateMessage(QString version, QString url);
		/*! Update proces cancelled */
		void updatesCancelled();
		/*! On progress update */
		void progressCheckUpdate(int progress);
		/*! Downloading update */
		void downloadingUpdate(int updateIndex, int pogress, int totalProgress);
		/*! Update item Download finished */
		void downloadUpdateFinished(int updateIndex);
		/*! Update item Download error  */
		void downloadUpdateError(int updateIndex);
		/*! Downloads finished */
		void downloadsFinished();
		/*! When its ready to install updates */
		void readyToInstallUpdates();
		/*! When updates finished but it can't install anything... */
		void failedToInstallUpdates();
};

#endif // __UPDATES_H__
