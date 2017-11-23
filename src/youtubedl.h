#ifndef YOUTUBEDL_H
#define YOUTUBEDL_H

#include <QtCore>

#ifdef Q_OS_WIN32
static const QString YOUTUBE_DL_APP_PATH = "youtube-dl.exe"; //!< Youtube-dl app (win32)
#else
static const QString YOUTUBE_DL_APP_PATH = "youtube-dl"; //!< Youtube-dl app (*unix)
#endif

/*! YoutubeDL Qt5 wrapper */
class YoutubeDL : public QObject
{
Q_OBJECT
    private:
        QProcess *process;				//!< YoutubeDL process
        QString appPath;				//!< Base dir where Youtube-dl is located (ie: /users/xesk/.xvst/)
        /*! Init internal variables */
        void init();
    public:
        /*! Class constructor */
        YoutubeDL(QString appPath, QString workingDir);
        /*! Class destructor */
        ~YoutubeDL();
        /*! Get information  */
        QJsonDocument getVideoInformation(const QString URL);
    signals:
        /*! when a download started */
        void downloadStarted();
        /*! when a download finished */
        void downloadFinished(const QFileInfo destFile);
        /*! when a download has been paused */
        void downloadPaused(const QFileInfo destFile);
        /*! when a download has been resumed */
        void downloadResumed();
        /*! when a download file has been canceled */
        void downloadCanceled();
        /*! an error ocurred during the download process */
        void downloadError(int error);
        /*! when the rtmp read data */
        void downloadEvent(int pos, int max);
};

#endif // YOUTUBEDL_H
