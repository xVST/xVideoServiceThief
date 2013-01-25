#ifndef CHANGELOGIMP_H
#define CHANGELOGIMP_H

#include "ui_changelog.h"

class ChangelogImpl : public QDialog, public Ui::ChangelogImpl
{
Q_OBJECT
	private:
		QString convertPlainTextToHtml(QString plainText);
	public:
		ChangelogImpl(QWidget *parent = 0, Qt::WFlags f = 0);
	private slots:
		void cmbVersionsInActivated(int index);
};

#endif // CHANGELOGIMP_H
