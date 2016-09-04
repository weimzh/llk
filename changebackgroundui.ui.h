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

#include <qradiobutton.h>


void ChangeBackgroundUI::typeChanged() {
   const QObject * send = sender();
   Q_ASSERT ( send );
   if ( send == rbPixmap ) {
      if ( rbPixmap->isChecked() == FALSE )
         rbPixmap->setChecked ( TRUE );
      if ( rbColor->isChecked() == TRUE )
         rbColor->setChecked ( FALSE );
   } else {
      if ( rbPixmap->isChecked() == TRUE )
         rbPixmap->setChecked ( FALSE );
      if ( rbColor->isChecked() == FALSE )
         rbColor->setChecked ( TRUE );
   }
}
