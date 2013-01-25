#ifndef SEARCHVIDEOSCUSTOMIZEIMPL_H
#define SEARCHVIDEOSCUSTOMIZEIMPL_H

#include "ui_searchvideoscustomize.h"

class SearchVideosCustomizeImpl : public QDialog, private Ui::SearchVideosCustomizeImpl
{
Q_OBJECT
	private:
		QStringList customServices;
		void fillServices();
		void createActionsMenu();
	public:
		SearchVideosCustomizeImpl(QStringList customServices, QWidget *parent = 0, Qt::WFlags f = 0);
		QStringList getCustomServices();
	private slots:
		void actCheckAllClicked();
		void actUncheckAllClicked();
		void actInvertSelectionClicked();
};

#endif // SEARCHVIDEOSCUSTOMIZEIMPL_H
