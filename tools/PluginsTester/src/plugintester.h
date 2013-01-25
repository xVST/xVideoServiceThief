#ifndef PLUGINTESTER_H
#define PLUGINTESTER_H

#include <QtCore>

class PluginTest : public QObject
{
private:
	QString pluginTitle;
	QString pluginFile;
	QString url;
	QString caption;
	QString expectedCaption;
	QString flvUrl;
	int size;
	int expectedSize;
public:
	PluginTest();
	//
	void setPluginTitle(QString value);
	QString getPluginTitle();
	void setPluginFile(QString value);
	QString getPluginFile();
	void setUrl(QString value);
	QString getUrl();
	void setCaption(QString value);
	QString getCaption();
	void setExpectedCaption(QString value);
	QString getExpectedCaption();
	void setFlvUrl(QString value);
	QString getFlvUrl();
	void setSize(int value);
	int getSize();
	void setExpectedSize(int value);
	int getExpectedSize();
	//
	bool isCaptionOk();
	bool isFlvOk();
	bool isTestOk();
};

class PluginTester : public QThread
{
	Q_OBJECT
private:
	QList<PluginTest *> *tests;
	QString pluginsDir;
	bool stopFlag;
	bool onlyFailed;
	int onlySelected;
	int failed;
	void removeTests();
	void run();
public:
    PluginTester();
	~PluginTester();
	//
	void loadTest(QString testFilePath);
	void runTest();
	void runAgainFailedTests();
	void runSelectedTest(int index);
	void stopTest();
	void setPluginsDir(QString pluginsDir);
signals:
	void pluginTestAdded(PluginTest *test);
	void pluginTestRunning(PluginTest *test);
	void pluginTestFinished(PluginTest *test);
	void workingStarted();
	void workingProgress(int pos, int max, QString information);
	void workingFinished(int ok, int failed);
};

#endif // PLUGINTESTER_H
