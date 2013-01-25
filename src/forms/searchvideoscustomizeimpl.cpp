#include "searchvideoscustomizeimpl.h"

#include "../videoinformation.h"

SearchVideosCustomizeImpl::SearchVideosCustomizeImpl(QStringList customServices, QWidget *parent, Qt::WFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	// copy custom services
	this->customServices = customServices;
	// prepare form
	fillServices();
	createActionsMenu();
	// add an space
	spbActions->setText(spbActions->text() + " ");
	// connect signals
	connect(actCheckAll, SIGNAL(triggered()), this, SLOT(actCheckAllClicked()));
	connect(actUncheckAll, SIGNAL(triggered()), this, SLOT(actUncheckAllClicked()));
	connect(actInvertSelection, SIGNAL(triggered()), this, SLOT(actInvertSelectionClicked()));
}

void SearchVideosCustomizeImpl::fillServices()
{
	QList<VideoInformationPlugin*> searchEngines = VideoInformation::instance()->getAllSearchPlugins();
	// add them to QTreeWidget
	for (int n = 0; n < searchEngines.count(); n++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(lsvServices);
		item->setIcon(0, *(searchEngines.at(n)->getIcon()));
		item->setText(0, searchEngines.at(n)->getCaption());
		item->setData(0, Qt::UserRole, QVariant(searchEngines.at(n)->getID()));
		item->setCheckState(0, customServices.indexOf(searchEngines.at(n)->getID(), 0) > -1 ? Qt::Checked : Qt::Unchecked);
		item->setHidden(VideoInformation::instance()->getBlockAdultContent() && searchEngines.at(n)->hasAdultContent());
	}
}

void SearchVideosCustomizeImpl::createActionsMenu()
{
	QMenu *menu = new QMenu(this);
	// add actions
	menu->addAction(actCheckAll);
	menu->addAction(actUncheckAll);
	menu->addSeparator();
	menu->addAction(actInvertSelection);
	// assign it to spbAction
	spbActions->setMenu(menu);
}

QStringList SearchVideosCustomizeImpl::getCustomServices()
{
	customServices.clear();
	// add checked items
	for (int n = 0; n < lsvServices->topLevelItemCount(); n++)
		if (lsvServices->topLevelItem(n)->checkState(0) == Qt::Checked)
			customServices.append(lsvServices->topLevelItem(n)->data(0, Qt::UserRole).toString());
	// return services
	return QStringList(customServices);
}

void SearchVideosCustomizeImpl::actCheckAllClicked()
{
	for (int n = 0; n < lsvServices->topLevelItemCount(); n++)
		lsvServices->topLevelItem(n)->setCheckState(0, Qt::Checked);
}

void SearchVideosCustomizeImpl::actUncheckAllClicked()
{
	for (int n = 0; n < lsvServices->topLevelItemCount(); n++)
		lsvServices->topLevelItem(n)->setCheckState(0, Qt::Unchecked);
}

void SearchVideosCustomizeImpl::actInvertSelectionClicked()
{
	for (int n = 0; n < lsvServices->topLevelItemCount(); n++)
		if (lsvServices->topLevelItem(n)->checkState(0) == Qt::Checked)
			lsvServices->topLevelItem(n)->setCheckState(0, Qt::Unchecked);
		else
			lsvServices->topLevelItem(n)->setCheckState(0, Qt::Checked);
}
