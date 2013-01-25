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

#ifndef SERVICESKEYCHAIN_H
#define SERVICESKEYCHAIN_H

#include <QtGui>

struct ServiceLoginInformation
{
	QString userName;
	QString password;
	// Struct initializer
	ServiceLoginInformation()
	{
		userName = "";
		password = "";
	}
};

inline ServiceLoginInformation serviceLoginInformation(QString userName, QString password)
{
	ServiceLoginInformation result;
	// configure
	result.userName = userName;
	result.password = password;
	// return configured login info
	return result;
}

class ServicesKeyChainItem : public QObject
{
Q_OBJECT
	private:
		QString serviceID;					//!< Service linked to this keychain item
		ServiceLoginInformation loginInfo;	//!< Login information of this keychain item
		bool serializable;					//!< Flag for know if this keychain item can be stored in disk
	public:
		/*! Class constructor */
		ServicesKeyChainItem(QString serviceID, ServiceLoginInformation loginInfo, bool serializable);
		/*! Get the serviceID */
		QString getServiceID();
		/*! Get the login information */
		ServiceLoginInformation getLoginInfo();
		/*! Get if is serializabe */
		bool isSerializable();
};

class VideoInformationPlugin;

class ServicesKeyChain : public QObject
{
Q_OBJECT
	private:
		QList<ServicesKeyChainItem *> *serviceLoginSession;	//!< Stores the current session logins
		/*! Load from disk all serializable stored keychain items */
		void loadKeychainItems();
		/*! Save to disk all serializabe stored keychain items */
		void saveKeychainItems();
		/*! Get a keychain item by serviceID */
		ServicesKeyChainItem *serviceKeyChainItemByServiceID(QString serviceID);
	public:
		/*! Class constructor */
		ServicesKeyChain();
		/*! Class destructor */
		~ServicesKeyChain();
		/*! Request for service login information */
//		void serviceLoginInfo(VideoInformationPlugin *videoInformationPlugin, bool lastLoginFailed, ServiceLoginInformation &result);
	public slots:
		/*! Request for service login information */
		void serviceLoginInfo(VideoInformationPlugin *videoInformationPlugin, bool lastLoginFailed, ServiceLoginInformation &result);
};

#endif // SERVICESKEYCHAIN_H
