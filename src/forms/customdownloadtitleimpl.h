#ifndef CUSTOMDOWNLOADTITLEIMPL_H
#define CUSTOMDOWNLOADTITLEIMPL_H

#include "ui_customdownloadtitle.h"

class CustomDownloadTitleImpl : public QDialog, public Ui::CustomDownloadTitleImpl
{
Q_OBJECT
	public:
		CustomDownloadTitleImpl(QWidget *parent = 0, Qt::WFlags f = 0);
	private slots:
		void edtTitleTextChanged(QString text);
};

#endif // CUSTOMDOWNLOADTITLEIMPL_H
