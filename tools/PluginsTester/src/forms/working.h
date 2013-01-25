#ifndef WORKING_H
#define WORKING_H

#include "ui_working.h"

class PluginTester;

class Working : public QDialog, private Ui::Working
{
Q_OBJECT
Q_DISABLE_COPY(Working)
	private:
		PluginTester *tester;
	public:
		explicit Working(PluginTester *tester, QWidget *parent = 0);
	private slots:
		void on_pushButton_clicked();
		void workingProgress(int pos, int max, QString information);
		void workingFinished(int ok, int failed);
};

#endif // WORKING_H
