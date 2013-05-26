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

#include "updates.h"

#include "pakunpak/pakunpak.h"
#include "programversion.h"
#include "languages.h"
#include "videoinformation.h"
#include "http.h"
#include "tools.h"

// Updates class

Updates::Updates(QString appPath)
{
	updateState = usWaiting;
	cancelled = false;
#ifdef Q_OS_MACX
	this->appPath = appPath + "/../Resources";
#else
	this->appPath = appPath;
#endif
	currentDownloaded = 0;
	totalDownloaded = 0;
	totalToDownload = 0;
	// init update classes
	updateList = new QList<Update *>;
	// init http object
	http = NULL;
}

Updates::~Updates()
{
	clear();
	// destroy the possible active http object
	deinitHttp();
	// remove the updates list
	delete updateList;
}

void Updates::parseBlock(QString block)
{
	if (!block.isEmpty())
	{
		int n = 0;
		QString fileBlock = "";
		// get file blocks
		do
		{
			// get current update file
			fileBlock = copyBetween(block, QString("#FILE:%1").arg(n), QString("#END:%1").arg(n));
			// have something to analayze?
			if (!fileBlock.isEmpty())
			{
				updateList->append(new Update);
				Update *update = updateList->last();
				// assign info
				update->setCaption(copyBetween(fileBlock, "caption=\"", "\""));
				update->setVersion(copyBetween(fileBlock, "version=\"", "\""));
				update->setSize(copyBetween(fileBlock, "size=\"", "\"").toInt());
				update->setInstallTo(copyBetween(fileBlock, "installTo=\"", "\""));
				update->setUrl(copyBetween(fileBlock, "url=\"", "\""));
				update->setPacked(copyBetween(fileBlock, "packed=\"", "\"").toLower() == "true");
				update->setObligatory(copyBetween(fileBlock, "obligatory=\"", "\"").toLower() == "true");
				update->setMinVersion(copyBetween(fileBlock, "minVersion=\"", "\""));
				update->setMinVersionUrl(copyBetween(fileBlock, "minVersionUrl=\"", "\""));
				update->setChecked(true);
				update->setError(false);
			}
			// next update file
			n++;
			// continue?
		}
		while (!fileBlock.isEmpty());
	}
}

void Updates::pareseUpdateFile(QString updateFile)
{
	if (cancelled) return;
	// parse current OS updates
	parseBlock(copyBetween(updateFile, "#" + CURRENT_OS + "_" + COMPILATION_MODE + "{", "}"));
	// parse common updates (OS independent)
	parseBlock(copyBetween(updateFile, "#COMMON{", "}"));
}

bool Updates::hasUpdates()
{
	if (cancelled) return false;

	for (int n = getUpdatesCount() - 1; n >= 0; n--)
	{
		bool deleteUpdate = false;
		Update *update = updateList->at(n);
		// what type of file is?
		if (update->getInstallTo().toLower() == "this")
		{
			deleteUpdate = compareVersions(PROGRAM_VERSION, update->getVersion()) != 1;
		}
		else
		{
			if (QFile::exists(appPath + update->getInstallTo()) || update->isObligatory())
			{
				QFileInfo fileInf(update->getInstallTo());
				// language files
				if (fileInf.completeSuffix() == "language")
				{
					Language *language = LanguageManager::getLanguageInfo(appPath + update->getInstallTo());
					// have info?
					if (language != NULL)
						deleteUpdate = compareVersions(language->getVersion(), update->getVersion()) != 1;
				}
				// plugin files
				else if (fileInf.completeSuffix() == "js")
				{
					VideoInformation *vidInf = VideoInformation::instance();
					VideoInformationPlugin *plugin = vidInf->getRegisteredPlugin(fileInf.fileName(), true);
					// have info?
					if (plugin != NULL)
						deleteUpdate = compareVersions(plugin->getVersion(), update->getVersion()) != 1;
				}
				else // "unknonw file"
					deleteUpdate = true;
			}
			else // we don't have this "item" installed...
				deleteUpdate = true;
		}
		// can delete this update?
		if (deleteUpdate)
		{
			delete updateList->at(n);
			updateList->removeAt(n);
		}
	}
	return getUpdatesCount() > 0;
}

void Updates::clear()
{
	while (!updateList->isEmpty())
		updateList->removeFirst();
}

void Updates::buildInstallScript()
{
	QFile updateScriptFile(QDir::tempPath() + XUPDATER_FILE);
	if (updateScriptFile.open(QFile::WriteOnly | QFile::Truncate))
	{
		//QTextStream updateScript(&updateScriptFile);
		QStringList updateScript;
		// write the info vars
		updateScript << "#MAIN_APP=" + QCoreApplication::applicationFilePath()
					 << "#PARAMETERS=-forceCheckUpdates"
					 << "#RESTART=true";	
		// add updates
		for (int n = 0; n < getUpdatesCount(); n++)
		{
			Update *update = updateList->at(n);
			// check if is "this" reference
			if (update->getInstallTo().toLower() == "this")
			{
#ifdef Q_OS_LINUX
				// on linux the install to must be an absolute path
				update->setInstallTo(QCoreApplication::applicationFilePath(), false);
#else // mac & win
				QFileInfo appExe(QCoreApplication::applicationFilePath());
				update->setInstallTo("/" + appExe.fileName());
#endif
#ifdef Q_OS_MACX
				// special MacOSX update (DMG)
				if (update->isChecked() && update->isPacked())
				{
					// build DMG script installer
					updateScript.append(buildDMGScript(n));
					// next update item
					continue;
				}
#endif
			}
			// configure appPath (temporary for this update item)
			QString appPath = update->hasRelativePath() ? this->appPath : "";
			// is checked?
			if (update->isChecked() && !update->hasErrors())
				if (!update->isPacked())
					// block id
					updateScript << QString(":install_file_%1").arg(n)
					// create dirs if is needed
								 << QString("mkdirs \"%1\"")
										.arg(extractFilePath(appPath + update->getInstallTo()))
								 << QString("if copy \"%1\" \"%2\"")
										.arg(QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(n))	// downloaded file
										.arg(appPath + update->getInstallTo()) // destination file
					// if the downloaded file has been copied, then delete it and jump to the next file
								 << QString("del \"%1\"").arg(QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(n))
					// if isn't the last file, go to next update file
								 << (n < getUpdatesCount() - 1 ? QString("goto install_file_%1").arg(n + 1) : "goto finish_update")
					// else condition (file has not been copied)
								 << "else"
								 << QString("goto install_file_%1").arg(n)
								 << "end";
				else // is a packed update
				{
					// added for convenience
					updateScript << QString(":install_file_%1").arg(n)
								 << QString("goto install_file_%1_0").arg(n);
					// extract files from the packed update
					Unpacker *unpacker = new Unpacker;
					unpacker->extractPackage(QString(QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(n)).toStdString(),
											 QDir::tempPath().toStdString() + "/",
											 false);
					// add files into the script
					int i;
					for (i = 0; i < unpacker->getExtractedFilesCount(); i++)
					{
						// block id
						updateScript << QString(":install_file_%1_%2").arg(n).arg(i)
						// copy the downloaded file
									 << QString("if copy \"%1\" \"%2\"")
											.arg(QString::fromStdString(unpacker->getExtractedFileName(i)))	// unpacked file
											.arg(QFileInfo(appPath + update->getInstallTo()).absolutePath() +
														   "/" + QString::fromStdString(unpacker->getExtractedFileName(i, true))) // destination file
						// if the downloaded file has been copied, then delete it and jump to the next file
									 << QString("del \"%1\"").arg(QString::fromStdString(unpacker->getExtractedFileName(i)))
						// next update
									 << QString("goto install_file_%1_%2").arg(n).arg(i + 1)
						// else condition (file has not been copied)
									 << "else"
									 << QString("goto install_file_%1_%2").arg(n).arg(i)
									 << "end";
					}
					// delete the downloaded pack
					updateScript << QString(":install_file_%1_%2").arg(n).arg(i)
								 << QString("del \"%1\"").arg(QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(n))
								 << (n < getUpdatesCount() - 1 ? QString("goto install_file_%1").arg(n + 1) : "goto finish_update");

					delete unpacker;
				}
			else
				// empty block id
				updateScript << QString(":install_file_%1").arg(n)
							 << (n < getUpdatesCount() - 1 ? QString("goto install_file_%1").arg(n + 1) : "goto finish_update");
		}
		// finish update block
		updateScript << ":finish_update"
					 << QString("del \"%1\"").arg(QDir::tempPath() + XUPDATER_FILE);
		// replace dirs separators
#ifdef Q_OS_WIN32
		updateScript.replaceInStrings("/", "\\");
#else
		updateScript.replaceInStrings("\\", "/");
		updateScript.replaceInStrings("/\"", "\\\"");
#endif
		// save to disc
		QTextStream updateScriptOut(&updateScriptFile);
		foreach (QString line, updateScript)
			updateScriptOut << line << "\n";
	}
}

void Updates::getTotalSizeToDownload()
{
	currentDownloaded = 0;
	totalDownloaded = 0;
	totalToDownload = 0;
	// calcule
	for (int n = 0; n < getUpdatesCount(); n++)
	{
		Update *update = updateList->at(n);
		// is checked?
		if (update->isChecked())
			totalToDownload += update->getSize();
	}
}

void Updates::run()
{
	switch (updateState)
	{
		case usWaiting:
			break;

		case usChecking:
		{
			// create the http object
			initializeHttp();
			// analyze the update file
			pareseUpdateFile(http->downloadWebpage(QUrl(updateURL), true));
			// destroy the http object
			deinitHttp();

			// cancelled?
			if (!cancelled)
			{
				// wait 650 miliseconds
				msleep(650);
				// check versions
				bool hasUpdates = this->hasUpdates();
				// if has updates, then check if has an special update message
				if (hasUpdates)
				{
					// find if for special updates (where the minVersion > programVersion)
					for (int n = 0; n < updateList->count(); n++)
						if (!updateList->at(n)->getMinVersion().isEmpty())
							if (compareVersions(PROGRAM_VERSION_SHORT, updateList->at(n)->getMinVersion()) == 1)
								if (!updateList->at(n)->getMinVersionUrl().isEmpty())
								{
									emit updateMessage(updateList->at(n)->getVersion(), updateList->at(n)->getMinVersionUrl());
									return;
								}
					// if we are there, then no custom messages found
					emit updatesChecked(true);
				}
				else // no updates
					emit updatesChecked(false);
			}
			else // yes, cancelled
				emit updatesCancelled();
			break;
		}

		case usDownloading:
		{
			currentItem = getFirstUpdateToDownload();
			int lastItem = -1;
			getTotalSizeToDownload();
			// create the http object
			initializeHttp();
			// download updates
			while (!cancelled && updateState == usDownloading && currentItem != -1)
			{
				if (lastItem != currentItem)
				{
					lastItem = currentItem;
					// disc file
					QString fileName = QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(currentItem);
					// if this file already exsits, delete it
					if (QFile::exists(fileName))
						QFile::remove(fileName);
					// start to download the current update
					http->download(QUrl(updateList->at(currentItem)->getUrl()),
						   QDir::tempPath(), QString(XUPDATER_DWN_FILE).arg(currentItem));
				}
				// process events
				qApp->processEvents();
				// wait 100 miliseconds (prevent 100% cpu)
				msleep(100);				
			}
			// destroy the http object
			deinitHttp();
			// install updates
			if (!cancelled && updateState == usInstalling && currentItem != -1)
				run();
			else // process cancelled?
				if (cancelled) // yes
				{
					// delete all temporal files
					for (int n = 0; n < getUpdatesCount(); n++)
						if (QFile::exists(QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(currentItem)))
							QFile::remove(QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(currentItem));

					emit updatesCancelled();
				}
			break;
		}

		case usInstalling:
			// init install flags
			int readyToInstall = 0;
			// check if we have something to update (may be are all errors)
			for (int n = 0; n < updateList->count(); n++)
				if (updateList->at(n)->isChecked() && !updateList->at(n)->hasErrors())
					readyToInstall++;
			// check ready to install count
			if (readyToInstall > 0)
			{
				// make the install file
				buildInstallScript();
				// send the ok signal
				emit readyToInstallUpdates();
			}
			else // ops, failed
				emit failedToInstallUpdates();
			break;
	}
}

void Updates::checkForUpdates(QString url)
{
	updateState = usChecking;
	updateURL = url;
	this->start();
}

void Updates::downloadUpdates()
{
	updateState = usDownloading;
	this->start();
}

#ifdef Q_OS_WIN32
// special function used to execute the updater with administrator rights
BOOL RunElevated(HWND hwnd, LPCTSTR pszPath, LPCTSTR pszParameters = NULL, LPCTSTR pszDirectory = NULL)
{
	QString sVerb = isWindowsVista() ? "runas" : "";

	SHELLEXECUTEINFO shex;

    memset(&shex, 0, sizeof(shex));

    shex.cbSize       = sizeof(SHELLEXECUTEINFO);
    shex.fMask        = 0;
    shex.hwnd         = hwnd;
    shex.lpVerb       = LPWSTR(sVerb.utf16());
    shex.lpFile       = pszPath;
    shex.lpParameters = pszParameters;
    shex.lpDirectory  = pszDirectory;
    shex.nShow        = SW_HIDE;

    return ::ShellExecuteEx(&shex);
}
#endif

void Updates::installUpdates()
{
#ifdef Q_OS_WIN32
	// init wchar_t vars
	QString sApp = QDir::toNativeSeparators(appPath + XUPDATER_PATH);
	QString sParams = QDir::toNativeSeparators(QDir::tempPath() + XUPDATER_FILE);
	QString sPath = QDir::toNativeSeparators(appPath);
	// execute with admin rights
	RunElevated(NULL, LPWSTR(sApp.utf16()), LPWSTR(sParams.utf16()), LPWSTR(sPath.utf16()));
#else
	QProcess::startDetached(appPath + XUPDATER_PATH, QStringList() << QDir::toNativeSeparators(QDir::tempPath() + XUPDATER_FILE));
#endif
}

void Updates::cancel()
{
	updateState = usWaiting;
	cancelled = true;
	// maybe the http object is already destroyed.
	if(http) http->cancel();
}

Update* Updates::getUpdateItem(const int index)
{
	if (index >= 0 && index < getUpdatesCount())
		return updateList->at(index);
	else
		return NULL;
}

int Updates::getUpdatesCount()
{
	return updateList->count();
}

int Updates::getCurrentDownloaded()
{
	return currentDownloaded;
}

int Updates::getTotalToDownload()
{
	return totalToDownload;
}

int Updates::getFirstUpdateToDownload()
{
	for (int n = 0; n < getUpdatesCount(); n++)
		if (updateList->at(n)->isChecked())
			return n;

	return -1;
}

void Updates::getNextUpdateToDownload()
{
	// next
	currentItem++;
	// get next file to download
	while (currentItem < getUpdatesCount())
	{
		if (updateList->at(currentItem)->isChecked())
			return;
		//next
		currentItem++;
	}
	// if we are here, then send the "END" signal
	emit downloadsFinished();
	// set as installing state
	updateState = usInstalling;
}

QStringList Updates::buildDMGScript(int n)
{
	QStringList result;
	// get bundle path
	QString bundlePath = QString("/%1.app").arg(copyBetween(QCoreApplication::applicationFilePath(), "/", ".app"));
	QString DMG = QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(n);
	QString mountPath = QDir::tempPath() + QString(XUPDATER_DWN_FILE).arg(n) + ".mount";
	// mount DMG image
	result << QString("exec \"hdiutil mount \\\"%1\\\" -mountpoint \\\"%2\\\" -noverify -noautoopen\" wait")
			.arg(DMG)
			.arg(mountPath);
	// copy from DMG to app bundle
	result << QString("exec \"cp -R \\\"%1\\\" \\\"%2\\\"\" wait")
			.arg(mountPath + "/xVideoServiceThief.app/")
			.arg(bundlePath + "/");
	// unmount DMG image
	result << QString("exec \"hdiutil unmount \\\"%1\\\" -force\" wait").arg(mountPath);
	// delete the downloaded DMG
	result << QString("del \"%1\"").arg(DMG);
	// if isn't the last file, go to next update file
	result << (n < getUpdatesCount() - 1 ? QString("goto install_file_%1").arg(n + 1) : "goto finish_update");
	// return the dmg install script
	return result;
}

void Updates::initializeHttp()
{
	if (http) return;
	// register the QFileInfo metatype (needed for threaded execution)
	qRegisterMetaType<QFileInfo>("QFileInfo");
	// create this new http object
	http = new Http();
	// connect essential signals
	connect(http, SIGNAL(downloadEvent(int, int)), this, SLOT(downloadEvent(int, int)));
	connect(http, SIGNAL(downloadFinished(const QFileInfo)), this, SLOT(downloadFinished(const QFileInfo)));
	connect(http, SIGNAL(downloadError(int)), this, SLOT(downloadError(int)));
}

void Updates::deinitHttp()
{
	if (http) delete http;
	http = NULL;
}

bool Updates::canUpdate(QString searchPath)
{
	return QFile::exists(searchPath + "/.." + XUPDATER_PATH);
}

void Updates::downloadEvent(int pos, int max)
{
	if (updateState == usChecking)
		emit progressCheckUpdate(static_cast<int>(calculePercent(pos, max)));
	else
	{
		currentDownloaded = totalDownloaded + pos;

		emit downloadingUpdate(currentItem,
							   static_cast<int>(calculePercent(pos, max)),
							   static_cast<int>(calculePercent(currentDownloaded, totalToDownload)));
	}
}

void Updates::downloadFinished(const QFileInfo /*destFile*/)
{
	if (updateState == usDownloading)
	{
		// update total downloaded
		totalDownloaded += updateList->at(currentItem)->getSize();
		// emit the signle end singal
		emit downloadUpdateFinished(currentItem);
		// get the next update to download (if return true = more items found
		getNextUpdateToDownload();
	}
}

void Updates::downloadError(int /*error*/)
{
	if (updateState == usDownloading)
	{
		// update total downloaded
		totalDownloaded += updateList->at(currentItem)->getSize();
		// mark this current item as error
		updateList->at(currentItem)->setError(true);
		// advertise about this error
		emit downloadUpdateError(currentItem);
		// get the next update to download (if return true = more items found
		getNextUpdateToDownload();
	}
}

// Update class

void Update::setCaption(QString value)
{
	caption = value;
}

void Update::setVersion(QString value)
{
	version = value;
}

void Update::setSize(int value)
{
	size = value;
}

void Update::setInstallTo(QString value, bool isRelativePath)
{
	installTo = value;
	this->isRelativePath = isRelativePath;
}

void Update::setUrl(QString value)
{
	url = value;
}

void Update::setPacked(bool value)
{
	packed = value;
}

 void Update::setObligatory(bool value)
{
	obligatory = value;
}

void Update::setMinVersion(QString value)
{
	minVersion = value;
}

void Update::setMinVersionUrl(QString value)
{
	minVersionUrl = value;
}

void Update::setChecked(bool value)
{
	checked = value;
}

void Update::setError(bool value)
{
	error = value;
}

QString Update::getCaption()
{
	return caption;
}

QString Update::getVersion()
{
	return version;
}

int Update::getSize()
{
	return size;
}

QString Update::getInstallTo()
{
	return installTo;
}

QString Update::getUrl()
{
	return url;
}

bool Update::isPacked()
{
	return packed;
}

bool Update::isObligatory()
{
	return obligatory;
}

QString Update::getMinVersion()
{
	return minVersion;
}

QString Update::getMinVersionUrl()
{
	return minVersionUrl;
}

bool Update::isChecked()
{
	return checked;
}

bool Update::hasErrors()
{
	return error;
}

bool Update::hasRelativePath()
{
	return isRelativePath;
}
