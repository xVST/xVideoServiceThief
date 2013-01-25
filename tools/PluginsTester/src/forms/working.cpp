#include "working.h"

#include "../plugintester.h"

Working::Working(PluginTester *tester, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
	setWindowFlags(Qt::Sheet);
	this->tester = tester;
	connect(tester, SIGNAL(workingProgress(int, int, QString)), this, SLOT(workingProgress(int, int, QString)));
	connect(tester, SIGNAL(workingFinished(int, int)), this, SLOT(workingFinished(int, int)));
}

void Working::workingProgress(int pos, int max, QString information)
{
	lblInformation->setText(information);
	pgbInformation->setMaximum(max);
	pgbInformation->setValue(pos);
}

void Working::workingFinished(int, int)
{
	accept();
}

void Working::on_pushButton_clicked()
{
	tester->stopTest();
}
