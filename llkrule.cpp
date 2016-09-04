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

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include "llkrule.h"
#include "general.h"

/*
 *  Constructs a gameRule as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
gameRule::gameRule ( QWidget* parent, const char* name, bool modal, WFlags fl )
      : QDialog ( parent, name, modal, fl ) {
   if ( !name )
      setName ( "Form1" );
   setSizePolicy ( QSizePolicy ( ( QSizePolicy::SizeType ) 0, ( QSizePolicy::SizeType ) 0, 0, 0, sizePolicy().hasHeightForWidth() ) );
   setMinimumSize ( QSize ( 660, 430 ) );
   setMaximumSize ( QSize ( 660, 430 ) );
   setBaseSize ( QSize ( 660, 430 ) );
   Form1Layout = new QVBoxLayout ( this, 11, 6, "Form1Layout" );

   textLabel1 = new QLabel ( this, "textLabel1" );
   textLabel1->setPixmap ( QPixmap ( LLK_INSTALL_DIR + QString ( "/llk_help.png" ) ) );
   Form1Layout->addWidget ( textLabel1 );

   layout1 = new QHBoxLayout ( 0, 0, 6, "layout1" );

   textLabel2 = new QLabel ( this, "textLabel2" );
   layout1->addWidget ( textLabel2 );

   pushButton1 = new QPushButton ( this, "pushButton1" );
   pushButton1->setSizePolicy ( QSizePolicy ( ( QSizePolicy::SizeType ) 0, ( QSizePolicy::SizeType ) 0, 0, 0, pushButton1->sizePolicy().hasHeightForWidth() ) );
   pushButton1->setMinimumSize ( QSize ( 80, 24 ) );
   pushButton1->setMaximumSize ( QSize ( 80, 24 ) );
   layout1->addWidget ( pushButton1 );
   Form1Layout->addLayout ( layout1 );
   languageChange();
   resize ( QSize ( 660, 430 ).expandedTo ( minimumSizeHint() ) );
   clearWState ( WState_Polished );

   // signals and slots connections
   connect ( pushButton1, SIGNAL ( clicked() ), this, SLOT ( close() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
gameRule::~gameRule() {
   // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void gameRule::languageChange() {
   setCaption ( tr ( "game rule" ) );
   textLabel1->setText ( QString::null );
   textLabel2->setText ( QString::null );
   pushButton1->setText ( tr ( "&Ok" ) );
   pushButton1->setAccel ( QKeySequence ( tr ( "Alt+O" ) ) );
}
