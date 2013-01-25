#ifndef MULTIMEDIASDETECTEDIMPL_H
#define MULTIMEDIASDETECTEDIMPL_H

#include "ui_multimediasdetected.h"

class MultiMediasDetectedImpl : public QDialog, private Ui::MultiMediasDetectedImpl
{
	Q_OBJECT

public:
	explicit MultiMediasDetectedImpl(QWidget *parent = 0, Qt::WFlags f = 0);
};

#endif // MULTIMEDIASDETECTEDIMPL_H
