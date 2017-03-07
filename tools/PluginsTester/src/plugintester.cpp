#include "plugintester.h"

#include "../../Common/xvst-classes.h"

PluginTester::PluginTester()
{
	tests = new QList<PluginTest *>();
}

PluginTester::~PluginTester()
{
	removeTests();
	delete tests;
}

void PluginTester::loadTest(QString testFilePath)
{
	QFile testFile(testFilePath);
	//
	if (testFile.exists())
	{
		// clear previous info
		removeTests();
		// open file
		testFile.open(QIODevice::ReadOnly);
		QXmlStreamReader xml(testFile.readAll());
		PluginTest *test = NULL;
		removeTests();
		// parse xml and run tests
		while (!xml.atEnd())
		{
			xml.readNext();
			// parse document
			if (xml.isStartElement() && xml.name() == "test")
				test = new PluginTest();
			else if (xml.isStartElement() && xml.name() == "plugin")
			{
				test->setPluginFile(xml.attributes().value("file").toString());
				test->setPluginTitle(xml.attributes().value("title").toString());
			}
			else if (xml.isStartElement() && xml.name() == "video")
			{
				test->setUrl(xml.attributes().value("url").toString());
				test->setExpectedCaption(xml.attributes().value("caption").toString());
				test->setExpectedSize(QString(xml.attributes().value("size").toString()).toInt());
			}
			else if (xml.isEndElement() && xml.name() == "test")
			{
				tests->push_back(test);
				// send add signal
				emit pluginTestAdded(test);
			}
		}
	}
}

void PluginTester::runTest()
{
	if (!isRunning())
	{
		onlyFailed = false;
		onlySelected = -1;
		start();
	}
}

void PluginTester::runAgainFailedTests()
{
	if (!isRunning())
	{
		onlyFailed = true;
		onlySelected = -1;
		start();
	}
}

void PluginTester::runSelectedTest(int index)
{
	if (!isRunning())
	{
		onlyFailed = false;
		onlySelected = index;
		start();
	}
}

void PluginTester::stopTest()
{
	stopFlag = true;
}

void PluginTester::run()
{
	stopFlag = false;
    failed = 0;

	emit workingStarted();

	for (int n = 0; n < tests->count(); n++)
	{
		PluginTest *test = tests->at(n);
		//
        if ((onlyFailed && test->isTestOk()) || (onlySelected != -1 && n != onlySelected)) continue;
		//
		emit pluginTestRunning(test);
		// get video information
        emit workingProgress(n * 2, tests->count() * 2, QString("Getting <b>%1</b> video information... [1/2]").arg(test->getPluginTitle()));
		VideoInformationPlugin *plugin = new VideoInformationPlugin(NULL, pluginsDir + test->getPluginFile());
		VideoDefinition  vd = plugin->getVideoInformation(test->getUrl());
		// try to download video
		emit workingProgress(n * 2 + 1, tests->count() * 2, QString("Testing <b>%1</b> video download... [2/2]").arg(test->getPluginTitle()));
		Http *http = new Http();
		if (!vd.cookies.isEmpty()) http->addCookies(vd.cookies);
		if (!vd.headers.isEmpty()) http->addHeaderParameters(vd.headers);
		int contentLength = copyBetween(http->head(QUrl(vd.URL)), "Content-Length: ", "\n").toInt();
		delete http;
		// update test info
		test->setCaption(vd.title);
		test->setFlvUrl(vd.URL);
		test->setSize(contentLength);
		// increase failed tests
		if (!test->isTestOk()) failed++;
		// destroy plugin
		delete plugin;
		// finished
		emit pluginTestFinished(test);
		// small pause, prevent 100% cpu
		if (stopFlag) n = tests->count(); else sleep(1);
	}

	emit workingFinished(tests->count() - failed, failed);
}

void PluginTester::setPluginsDir(QString pluginsDir)
{
	this->pluginsDir = pluginsDir + "/";
}

void PluginTester::removeTests()
{
	while (!tests->empty())
		delete tests->takeFirst();
}

// PluginTest class

PluginTest::PluginTest()
{
	pluginTitle = "";
	pluginFile = "";
	url = "";
	caption = "";
	expectedCaption = "";
	flvUrl = "";
	size = 0;
	expectedSize = 0;
}

void PluginTest::setPluginTitle(QString value)
{
	pluginTitle = value;
}

QString PluginTest::getPluginTitle()
{
	return pluginTitle;
}

void PluginTest::setPluginFile(QString value)
{
	pluginFile = value;
}

QString PluginTest::getPluginFile()
{
	return pluginFile;
}

void PluginTest::setUrl(QString value)
{
	url = value;
}

QString PluginTest::getUrl()
{
	return url;
}

void PluginTest::setCaption(QString value)
{
	caption = value;
}

QString PluginTest::getCaption()
{
	return caption;
}

void PluginTest::setExpectedCaption(QString value)
{
	expectedCaption = value;
}

QString PluginTest::getExpectedCaption()
{
	return expectedCaption;
}

void PluginTest::setFlvUrl(QString value)
{
	flvUrl = value;
}

QString PluginTest::getFlvUrl()
{
	return flvUrl;
}

void PluginTest::setSize(int value)
{
	size = value;
}

int PluginTest::getSize()
{
	return size;
}

void PluginTest::setExpectedSize(int value)
{
	expectedSize = value;
}

int PluginTest::getExpectedSize()
{
	return expectedSize;
}

bool PluginTest::isCaptionOk()
{
	return caption == expectedCaption;
}

bool PluginTest::isFlvOk()
{
	return size == expectedSize && !flvUrl.isEmpty();
}

bool PluginTest::isTestOk()
{
	return isCaptionOk() && isFlvOk();
}
