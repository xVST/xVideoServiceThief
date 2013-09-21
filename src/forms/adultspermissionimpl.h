#ifndef ADULTSPERMISSIONIMPL_H
#define ADULTSPERMISSIONIMPL_H

#include <QtWidgets>

#include "ui_adultspermission.h"

class AdultsPermissionImpl : public QDialog, public Ui::AdultsPermission
{
Q_OBJECT
	public:
		AdultsPermissionImpl(QWidget *parent = 0, Qt::WindowFlags f = 0);
};

#endif // ADULTSPERMISSIONIMPL_H
