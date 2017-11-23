#include "youtubedl.h"

YoutubeDL::YoutubeDL(QString appPath, QString workingDir)
{
	setObjectName("YoutubeDL");
	// init internal vars
	init();
    // set up the RTMP app path
    this->appPath = appPath + "/" + YOUTUBE_DL_APP_PATH;
	// create a new qprocess
	process = new QProcess();
	// connect signals
//	connect(process, SIGNAL(started()), this, SLOT(started()));
//	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
//	connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
//	connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
	// cofnigure process
	process->setWorkingDirectory(workingDir);
}

YoutubeDL::~YoutubeDL()
{
    delete process;
}

void YoutubeDL::init()
{
}

QJsonDocument YoutubeDL::getVideoInformation(const QString URL)
{
    QStringList arguments = QStringList() << "-j" << URL;
    // execute command
    process->start(appPath, arguments);
    process->waitForFinished();
    QString output(process->readAllStandardOutput());
    // parse the json
    return QJsonDocument::fromJson(output.toUtf8());
}
