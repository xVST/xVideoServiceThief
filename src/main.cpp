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

#include <QtWidgets>
#include <QtSingleApplication>
//
#include "forms/loadingimpl.h"
#include "forms/mainformimpl.h"
//
#include "options.h"
#include "languages.h"
#include "http.h"
#include "programversion.h"

int main(int argc, char **argv)
{
	QtSingleApplication app("xVideoServiceThief " + PROGRAM_VERSION_SHORT, argc, argv);

	// init defaults
	bool noSplash = false;
	bool forceNewInstance = false;

	// capture the application arguments
	QString URLs;

	for (int n = 1; n < argc; n++)
	{
		QString arg = argv[n];
		// is a command?
		if (arg == "-nosplash")
			noSplash = true;
		else if (arg == "-forcenewinstance")
			forceNewInstance = true;
		else if (arg != "-forceCheckUpdates") // we assume which it is an URL to add
			URLs += arg + "\n";
	}

	// another instance is running? (and we aren't forcing a new instance)
	if (!forceNewInstance && app.sendMessage(URLs)) return 0;

	// display loading dialog
	if (!noSplash) LoadingImpl::instance()->show();

	// get language file
	ProgramOptions *programOptions = ProgramOptions::instance();
	programOptions->load();
	QString qm = LanguageManager::get_qm_languageFile(programOptions->getLanguageFile(true));

	// install translator
	QTranslator translator;
	translator.load(qm);
	app.installTranslator(&translator);

	// display loading plugins
	LoadingImpl::instance()->setMessage(LoadingImpl::tr("Loading plugins... please wait..."));
	qApp->processEvents();

	// create and display main window
	MainFormImpl win;
	win.show();

	// set instance message handler
	QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &win, SLOT(SingleApplicationHandleMessage(const QString&)));

	// set instance activation window
	app.setActivationWindow(&win, true);

	// hide loading window
	if (LoadingImpl::instance() != NULL)
	{
		LoadingImpl::instance()->setMessage(LoadingImpl::tr("Plugins loaded"));
		LoadingImpl::instance()->finished();
	}

	// set http global user agent
	Http::setGlobalUserAgent("xVST-" + PROGRAM_VERSION_SHORT);

	// has URLs to add?
	if (!URLs.isEmpty()) win.SingleApplicationHandleMessage(URLs);

	// run program
	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
	return app.exec();
}
//
