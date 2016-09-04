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

#ifndef _DIALOGWITHCHECKBOX_H_
#define _DIALOGWITHCHECKBOX_H_

#include <qdialog.h>

class QLabel;
class QPushButton;
class QCheckBox;

class DialogWithCheckBox : public QDialog {
   Q_OBJECT
public:
   DialogWithCheckBox ( QWidget* parent, const QString &caption, const QString &text, const QString &dontShowAgainName = QString::null, const QString &button1text = QString::null );
   ~DialogWithCheckBox();

   void setText ( const QString& );

public:
   QLabel* label;
   QPushButton* pbOk;
   QCheckBox* cbShowAgain;

};

#endif //_DIALOGWITHCHECKBOX_H_