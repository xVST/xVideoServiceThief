/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2014 Xesc & Technology
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

#include "serviceskeychain.h"

#include "forms/loginpromptimpl.h"

#include "videoinformation.h"
#include "tools.h"

// ServicesKeyChainItem

ServicesKeyChainItem::ServicesKeyChainItem(QString serviceID, ServiceLoginInformation loginInfo, bool serializable)
{
	this->serviceID = serviceID;
	this->loginInfo = loginInfo;
	this->serializable = serializable;
}

QString ServicesKeyChainItem::getServiceID()
{
	return serviceID;
}

ServiceLoginInformation ServicesKeyChainItem::getLoginInfo()
{
	return loginInfo;
}

bool ServicesKeyChainItem::isSerializable()
{
	return serializable;
}

// ServicesKeyChain class

ServicesKeyChain::ServicesKeyChain()
{
	serviceLoginSession = new QList<ServicesKeyChainItem *>();
	// load our stored logins
	loadKeychainItems();
}

ServicesKeyChain::~ServicesKeyChain()
{
	delete serviceLoginSession;
}

void ServicesKeyChain::loadKeychainItems()
{
	qDebug() << "TODO: Load";
}

void ServicesKeyChain::saveKeychainItems()
{
	qDebug() << "TODO: Save";
}

ServicesKeyChainItem* ServicesKeyChain::serviceKeyChainItemByServiceID(QString serviceID)
{
	foreach (ServicesKeyChainItem *keychainItem, *serviceLoginSession)
		if (keychainItem->getServiceID() == serviceID)
			return keychainItem;
	// not found
	return nullptr;
}

void ServicesKeyChain::serviceLoginInfo(VideoInformationPlugin *videoInformationPlugin, bool lastLoginFailed, ServiceLoginInformation &result)
{
	ServicesKeyChainItem *keychainItem = serviceKeyChainItemByServiceID(videoInformationPlugin->getID());
	// is in our session?
	if (keychainItem && !lastLoginFailed)
	{
		result = keychainItem->getLoginInfo();
		return;
	}
	else // ask user about his login info
	{
		LoginPromptImpl loginPrompt(videoInformationPlugin, keychainItem ? keychainItem->getLoginInfo() : ServiceLoginInformation(),
									lastLoginFailed, qApp->activeWindow(), Qt::Sheet);
		// display login prompt
		if (showModalDialog(&loginPrompt) == QDialog::Accepted)
		{
			keychainItem = new ServicesKeyChainItem(videoInformationPlugin->getID(), loginPrompt.getServiceLoginInfo(), loginPrompt.getRememberPassword());
			// add it into our current session?
			if (keychainItem->isSerializable() || loginPrompt.getStayLoggedInSession())
			{
				serviceLoginSession->append(keychainItem);
				// save our session
				saveKeychainItems();
			}
			// return login info
			result = ServiceLoginInformation();
			return;
		}
	}
	// no login info found
	result = ServiceLoginInformation();
}

/*
ServiceLoginInformation ServicesKeyChain::serviceLoginInfo(VideoInformationPlugin *videoInformationPlugin, bool lastLoginFailed)
{
	ServicesKeyChainItem *keychainItem = serviceKeyChainItemByServiceID(videoInformationPlugin->getID());
	// is in our session?
	if (keychainItem && !lastLoginFailed)
	{
		return keychainItem->getLoginInfo();
	}
	else // ask user about his login info
	{
		LoginPromptImpl loginPrompt(videoInformationPlugin, keychainItem ? keychainItem->getLoginInfo() : ServiceLoginInformation(),
									 lastLoginFailed, qApp->activeWindow(), Qt::Sheet);
		// display login prompt
		if (showModalDialog(&loginPrompt) == QDialog::Accepted)
		{
			keychainItem = new ServicesKeyChainItem(videoInformationPlugin->getID(),
													loginPrompt.getServiceLoginInfo(),
													loginPrompt.getRememberPassword());
			// add it into our current session?
			if (keychainItem->isSerializable() || loginPrompt.getStayLoggedInSession())
			{
				serviceLoginSession->append(keychainItem);
				// save our session
				saveKeychainItems();
			}
			// return login info
			return keychainItem->getLoginInfo();
		}
	}
	// no login info found
	return ServiceLoginInformation();
}
*/

/*
void ServicesKeyChain::serviceLoginInfoThreadSafe(VideoInformationPlugin *videoInformationPlugin, bool lastLoginFailed, ServiceLoginInformation &loginInfo)
{
	QMetaObject::invokeMethod(this, SLOT(serviceLoginInfo(VideoInformationPlugin *, bool)),
							  Q_ARG(VideoInformationPlugin, videoInformationPlugin), Q_ARG(bool, lastLoginFailed));
}
*/
