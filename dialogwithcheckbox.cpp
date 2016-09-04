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

#include <qlabel.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>

#include "dialogwithcheckbox.h"

/*
 *  Constructs a DialogWithCheckBox as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DialogWithCheckBox::DialogWithCheckBox ( QWidget* parent, const QString &caption, const QString &text, const QString &dontShowAgainName, const QString &button0text )
      : QDialog ( parent, "dialog_with_check_box", true ) {
   if ( caption.isEmpty() == false )
      setCaption ( caption );

   QVBoxLayout *vbox = new QVBoxLayout ( this );
   label = new QLabel ( text, this, "label" );
   label->setAlignment ( int ( QLabel::WordBreak | QLabel::AlignVCenter ) );
   vbox->addWidget ( label );

   cbShowAgain = new QCheckBox ( tr ( "Don't show this message again" ), this, "cbShowAgain" );
   if ( dontShowAgainName.isEmpty() == false )
      cbShowAgain->setText ( dontShowAgainName );
   vbox->addWidget ( cbShowAgain );

   pbOk = new QPushButton ( tr ( "&Ok" ), this, "pbOk" );
   pbOk->setFixedSize ( 70, 24 );
   if ( button0text.isEmpty() == false )
      pbOk->setText ( button0text );
   connect ( pbOk, SIGNAL ( clicked() ), this, SLOT ( accept() ) );
   vbox->addWidget ( pbOk );

   resize ( QSize ( 320, 160 ).expandedTo ( minimumSizeHint() ) );

   clearWState ( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DialogWithCheckBox::~DialogWithCheckBox() {
   // no need to delete child widgets, Qt does it all for us
}

void DialogWithCheckBox::setText ( const QString &str ) {
   label->setText ( str );
}

