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

#include "changebackgroundui.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include "qcolorbutton.h"
#include "myfiledialog.h"
#include "qurlrequester.h"
#include "changebackgroundui.ui.h"

/*
 *  Constructs a ChangeBackgroundUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ChangeBackgroundUI::ChangeBackgroundUI ( QWidget* parent, const char* name, bool modal, WFlags fl )
      : QDialog ( parent, name, modal, fl ) {
   if ( !name )
      setName ( "changeBackground" );
   setMinimumSize ( QSize ( 310, 125 ) );
   setMaximumSize ( QSize ( 310, 125 ) );
   setSizeGripEnabled ( FALSE );

   buttonOk = new QPushButton ( this, "buttonOk" );
   buttonOk->setGeometry ( QRect ( 125, 95, 90, 32 ) );
   buttonOk->setAutoDefault ( TRUE );
   buttonOk->setDefault ( TRUE );

   buttonCancel = new QPushButton ( this, "buttonCancel" );
   buttonCancel->setGeometry ( QRect ( 220, 95, 90, 32 ) );
   buttonCancel->setAutoDefault ( TRUE );

   rbPixmap = new QRadioButton ( this, "rbPixmap" );
   rbPixmap->setGeometry ( QRect ( 5, 5, 110, 24 ) );
   rbPixmap->setChecked ( TRUE );

   picURL = new QURLRequester ( this, "picURL" );
   picURL->setGeometry ( QRect ( 25, 30, 200, 30 ) );

   rbColor = new QRadioButton ( this, "rbColor" );
   rbColor->setGeometry ( QRect ( 5, 65, 110, 32 ) );

   colorButton = new QColorButton ( this, "colorButton" );
   colorButton->setGeometry ( QRect ( 134, 65, 60, 32 ) );

   cbScaleMode = new QComboBox ( FALSE, this, "cbScaleMode" );
   cbScaleMode->setGeometry ( QRect ( 235, 33, 60, 32 ) );
   languageChange();
   resize ( QSize ( 310, 125 ).expandedTo ( minimumSizeHint() ) );
   clearWState ( WState_Polished );

   // signals and slots connections
   connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( reject() ) );
   connect ( buttonOk, SIGNAL ( clicked() ), this, SLOT ( accept() ) );
   connect ( rbPixmap, SIGNAL ( clicked() ), this, SLOT ( typeChanged() ) );
   connect ( rbColor, SIGNAL ( clicked() ), this, SLOT ( typeChanged() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ChangeBackgroundUI::~ChangeBackgroundUI() {
   // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ChangeBackgroundUI::languageChange() {
   setCaption ( tr ( "Change Background" ) );
   buttonOk->setText ( tr ( "&OK" ) );
   buttonOk->setAccel ( QKeySequence ( QString::null ) );
   buttonCancel->setText ( tr ( "&Cancel" ) );
   buttonCancel->setAccel ( QKeySequence ( QString::null ) );
   rbPixmap->setText ( tr ( "Use a pixmap:" ) );
   rbColor->setText ( tr ( "Use back color:" ) );
   colorButton->setText ( QString::null );
   cbScaleMode->clear();
   cbScaleMode->insertItem ( tr ( "Center" ) );
   cbScaleMode->insertItem ( tr ( "Scale" ) );
   cbScaleMode->insertItem ( tr ( "Tile" ) );
   cbScaleMode->setCurrentItem ( 1 );
}

