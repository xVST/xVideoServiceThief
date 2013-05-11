#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT
	private:
		Ui::MainWindow *ui;
		void testPluginVideoInformation(bool debug);
		void testPluginSearchVideos(bool debug);
		QString generateBinaryArray(QString iconPath);
		void generateIconForDir(QString dir);
	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();
	private slots:
		void on_toolBtnPluginIconPath_clicked();
		void on_btnAutoGenerateIcons_clicked();
		void on_toolBtnPluginIcon_clicked();
		void on_toolBtnPluginSearchVideos_clicked();
		void on_toolBtnPluginVideoInformation_clicked();
		void on_btnDebugSearchVideos_clicked();
		void on_btnTestSearchVideos_clicked();
		void on_btnGenerateIcon_clicked();
		void on_btnDebugVideoInformation_clicked();
		void on_btnTestVideoInformation_clicked();
};

#endif // MAINWINDOW_H
