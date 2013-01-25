#include "customdownloadtitleimpl.h"

CustomDownloadTitleImpl::CustomDownloadTitleImpl(QWidget *parent, Qt::WFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	// resize form if is needed
#ifndef Q_OS_WIN32
	resize(472, 162);
#endif
	// connect signals
	connect(edtTitle, SIGNAL(textChanged(QString)), this, SLOT(edtTitleTextChanged(QString)));
}

void CustomDownloadTitleImpl::edtTitleTextChanged(QString text)
{
	btnOk->setEnabled(text.trimmed() != "");
}
