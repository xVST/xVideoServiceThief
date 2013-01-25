#include "mainwindow.h"

#include "ui_mainwindow.h"

#include "working.h"

#include "../../../Common/xvst-classes.h"
#include "../plugintester.h"
#include "../programsettings.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	// set http global user agent
	Http::setGlobalUserAgent("xVST-PluginsTester");

	ProgramSettings settings;
	settings.load();
	ui->edtPluginsDir->setText(settings.getPluginsDir());
	ui->edtTestFile->setText(settings.getTestingFile());

	tester = new PluginTester();

	connect(tester, SIGNAL(pluginTestAdded(PluginTest*)), this, SLOT(pluginTestAdded(PluginTest*)));
	connect(tester, SIGNAL(pluginTestRunning(PluginTest*)), this, SLOT(pluginTestRunning(PluginTest*)));
	connect(tester, SIGNAL(pluginTestFinished(PluginTest*)), this, SLOT(pluginTestFinished(PluginTest*)));
	connect(tester, SIGNAL(workingStarted()), this, SLOT(workingStarted()));
	connect(tester, SIGNAL(workingProgress(int, int, QString)), this, SLOT(workingProgress(int, int, QString)));
	connect(tester, SIGNAL(workingFinished(int, int)), this, SLOT(workingFinished(int, int)));

	if (ui->edtPluginsDir->text().isEmpty())
		on_toolBtnSelectPluginsDir_clicked();

	if (ui->edtTestFile->text().isEmpty())
		on_toolBtnSelectTest_clicked();

	reloadTests();

	working = new Working(tester, this);

	statusBar()->setSizeGripEnabled(false);
	statusBar()->addWidget(ui->lblInformation, 1);
}

MainWindow::~MainWindow()
{
	ProgramSettings settings;
	settings.setPluginsDir(ui->edtPluginsDir->text());
	settings.setTestingFile(ui->edtTestFile->text());
	settings.save();

	delete working;
	delete tester;
    delete ui;
}

QTreeWidgetItem* MainWindow::treeItemFromPluginTest(PluginTest *test)
{
	for (int n = 0; n < ui->trvTests->topLevelItemCount(); n++)
		if (ui->trvTests->topLevelItem(n)->text(0) == test->getPluginTitle())
			return ui->trvTests->topLevelItem(n);
	// not found
	return NULL;
}

void MainWindow::pluginTestAdded(PluginTest *test)
{
	QTreeWidgetItem *base, *item, *subItem, *subSubItem;

	base = new QTreeWidgetItem(ui->trvTests);
	base->setText(0, test->getPluginTitle());

	// [0]
	item = new QTreeWidgetItem(base);
	item->setText(0, "file");
	item->setText(1, test->getPluginFile());

	// [1]
	item = new QTreeWidgetItem(base);
	item->setText(0, "URL");
	item->setText(1, test->getUrl());

	// [2]
	item = new QTreeWidgetItem(base);
	item->setText(0, "Title");

	// [2][0]
	subItem = new QTreeWidgetItem(item);
	subItem->setText(0, "Current");
	subItem->setText(1, "-");

	// [2][1]
	subItem = new QTreeWidgetItem(item);
	subItem->setText(0, "Expected");
	subItem->setText(1, test->getExpectedCaption());

	// [3]
	item = new QTreeWidgetItem(base);
	item->setText(0, "FLV");

	// [3][0]
	subItem = new QTreeWidgetItem(item);
	subItem->setText(0, "URL");
	subItem->setText(1, "-");

	// [3][1]
	subItem = new QTreeWidgetItem(item);
	subItem->setText(0, "Size");

	// [3][1][0]
	subSubItem = new QTreeWidgetItem(subItem);
	subSubItem->setText(0, "Current");
	subSubItem->setText(1, "-");

	// [3][1][1]
	subSubItem = new QTreeWidgetItem(subItem);
	subSubItem->setText(0, "Expected");
	subSubItem->setText(1, QString("%1").arg(test->getExpectedSize()));
}

void MainWindow::pluginTestRunning(PluginTest *test)
{
	QTreeWidgetItem *item = treeItemFromPluginTest(test);
	if (item != NULL) item->setText(1, "[Testing...]");
}

void MainWindow::pluginTestFinished(PluginTest *test)
{
	QTreeWidgetItem *item = treeItemFromPluginTest(test);
	//
	if (item != NULL)
	{
		QFont font = item->font(0);
		font.setBold(true);
		// is test ok?
		if (test->isTestOk())
		{
			item->setText(1, "[Ok]");
			item->setFont(1, font);
		}
		else // test failed
		{
			item->setText(1, "[Failed!]");
			item->setFont(0, font);
		}
		// update info
		item->child(2)->child(0)->setText(1, test->getCaption());
		item->child(3)->child(0)->setText(1, test->getFlvUrl());
		item->child(3)->child(1)->child(0)->setText(1, QString("%1").arg(test->getSize()));
		// update info results
		if (!test->isFlvOk())
		{
			item->child(3)->setText(1, "[Failed!]");
			item->child(3)->setFont(0, font);
			// is flvurl empty?
			if (test->getFlvUrl().isEmpty())
			{
				item->child(3)->child(0)->setText(1, "Empty Url!");
				item->child(3)->child(0)->setFont(0, font);
			}
			// is size?
			if (test->getSize() != test->getExpectedSize())
			{
				item->child(3)->child(1)->setText(1,"[Failed!]");
				item->child(3)->child(1)->setFont(0, font);
			}
			else // size ok
			{
				item->child(3)->child(1)->setText(1,"[Ok]");
				item->child(3)->child(1)->setFont(1, font);
			}
		}
		else // flv ok
		{
			// flv
			item->child(3)->setText(1, "[Ok]");
			item->child(3)->setFont(1, font);
			// size flv
			item->child(3)->child(1)->setText(1,"[Ok]");
			item->child(3)->child(1)->setFont(1, font);
		}
		//
		if (!test->isCaptionOk())
		{
			item->child(2)->setText(1, "[Failed!]");
			item->child(2)->setFont(0, font);
		}
		else // caption ok
		{
			item->child(2)->setText(1, "[Ok]");
			item->child(2)->setFont(1, font);
		}
	}
}

void MainWindow::workingStarted()
{
	working->show();
}

void MainWindow::workingProgress(int, int, QString information)
{
	ui->lblInformation->setText(information);
}

void MainWindow::workingFinished(int ok, int failed)
{
	ui->lblInformation->setText(QString("Results: <b>%1 Ok</b>  -  <b>%2 Failed</b>    (Ratio: <b>%3%</b>)").
								arg(ok).arg(failed).arg(floatToStr(calculePercent(ok, ok + failed))));
}

void MainWindow::reloadTests()
{
	ui->trvTests->clear();
	tester->setPluginsDir(ui->edtPluginsDir->text());
	tester->loadTest(ui->edtTestFile->text());
}

void MainWindow::on_actionRun_triggered()
{
	tester->runTest();
}

void MainWindow::on_actionRun_again_Failed_tests_triggered()
{
	tester->runAgainFailedTests();
}

void MainWindow::on_actionStop_triggered()
{
	tester->stopTest();
}

void MainWindow::on_actionRun_selected_test_triggered()
{
	tester->runSelectedTest(ui->trvTests->currentIndex().row());
}

void MainWindow::on_toolBtnSelectTest_clicked()
{
	QString testFile = QFileDialog::getOpenFileName(this,
		"Open Testing file", ui->edtTestFile->text(), "Testing files (*.xml *.xvst-testing)");
	// set file name
	if (!testFile.isEmpty())
	{
		ui->edtTestFile->setText(testFile);
		reloadTests();
	}
}

void MainWindow::on_toolBtnSelectPluginsDir_clicked()
{
	QString pluginsDir = QFileDialog::getExistingDirectory(this,
		"Open Plugins Directory", ui->edtPluginsDir->text(), QFileDialog::ShowDirsOnly);
	// set plugins dir
	if (!pluginsDir.isEmpty())
	{
		ui->edtPluginsDir->setText(pluginsDir);
		reloadTests();
	}
}
