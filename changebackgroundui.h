/***************************************************************************
 *   Copyright (C) 2005 by jhwu                                            *
 *   jhwu@redflag-linux.com                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA           *
 ***************************************************************************/

#ifndef CHANGEBACKGROUND_H
#define CHANGEBACKGROUND_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QRadioButton;
class QURLRequester;
class QColorButton;
class QComboBox;

class ChangeBackgroundUI : public QDialog {
   Q_OBJECT

public:
   ChangeBackgroundUI ( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
   ~ChangeBackgroundUI();

   QPushButton* buttonOk;
   QPushButton* buttonCancel;
   QRadioButton* rbPixmap;
   QURLRequester* picURL;
   QRadioButton* rbColor;
   QColorButton* colorButton;
   QComboBox* cbScaleMode;

public slots:
   virtual void typeChanged();

protected:

protected slots:
   virtual void languageChange();

};

#endif // CHANGEBACKGROUND_H
