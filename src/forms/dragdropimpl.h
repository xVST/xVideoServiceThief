/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2018 Xesc & Technology
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with xVideoServiceThief. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: Xesc <xeskuu.xvst@gmail.com>
* Program URL   : http://www.xvideothief.com/
*
*/

#ifndef DRAGDROPIMPL_H
#define DRAGDROPIMPL_H

#include <QtWidgets>

#include "ui_dragdrop.h"

class ProgramOptions;
class VideoListController;

class QLabelClickeable : public QLabel
{
Q_OBJECT
	protected:
		/*! Override double click event */
		void mouseDoubleClickEvent(QMouseEvent * event);
	public:
		/*! Class constructor */
		QLabelClickeable(QWidget* parent = 0, Qt::WindowFlags f = 0) : QLabel(parent, f) {}
	signals:
		/*! On doublecick event */
		void doubleClicked();
};

class DragDropImpl : public QWidget, public Ui::DragDrop
{
Q_OBJECT
	private:
		QWidget *parent;				//!< Parent form
		QLabelClickeable *imgDragDrop;	//!< Drag and drop image
		QMenu *dragDropMenu;			//!< Drag and drop menu
		QMenu *alphaBlendMenu;			//!< Alpha blend menu
		ProgramOptions *programOptions;	//!< Program Options reference
		VideoListController *videoList;	//!< Video list controller reference
		QShortcut *shortCurtPasteURL;	//!< Paste from clipboard Shortcut
		void setInitialPos();
		void createDragDropMenu();
		void setAlphaBlend(float);
		void dragEnterEvent(QDragEnterEvent *event);
		void dropEvent(QDropEvent *event);
		void closeEvent (QCloseEvent *event);
		void contextMenuEvent(QContextMenuEvent * event);
		void addVideo(QString URL);
	public:
		DragDropImpl(ProgramOptions *programOptions, VideoListController *videoList,
					QWidget * parent = 0, Qt::WindowFlags f = 0);
		~DragDropImpl();
		void addVideoURL(QString URL);
	private slots:
		void pasteURLfromClipboardClicked();
		void displayMainWindowClicked();
		void alphaBlendValueClicked();
		void removeServiceIcon();
};
#endif
