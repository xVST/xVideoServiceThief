#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

namespace Ui
{
    class MainWindow;
}

class Working;
class PluginTest;
class PluginTester;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
	Ui::MainWindow *ui;
	PluginTester *tester;
	Working *working;
	QTreeWidgetItem* treeItemFromPluginTest(PluginTest *test);
	void reloadTests();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
	void on_toolBtnSelectPluginsDir_clicked();
	void on_toolBtnSelectTest_clicked();
	void on_actionRun_selected_test_triggered();
	void on_actionStop_triggered();
	void on_actionRun_again_Failed_tests_triggered();
	void on_actionRun_triggered();
	void pluginTestAdded(PluginTest *test);
	void pluginTestRunning(PluginTest *test);
	void pluginTestFinished(PluginTest *test);
	void workingStarted();
	void workingProgress(int pos, int max, QString information);
	void workingFinished(int ok, int failed);
};

#endif // MAINWINDOW_H
