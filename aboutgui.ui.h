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

#include <qpixmap.h>
#include <qlabel.h>
#include <qimage.h>

#ifndef _WIN32
#define  rfmin_logo_xpm "/usr/share/apps/llk/rfmin_logo.xpm"
#else
#define  rfmin_logo_xpm "rfmin_logo.xpm"
#endif

void AboutGUI::setAboutInfo ( const QString &name, const QString &version ) {
   logo_lbl->setPixmap ( QPixmap ( rfmin_logo_xpm ) );
   logo_lbl->setScaledContents ( TRUE );
   setCaption ( tr ( "About %1" ).arg ( name ) );
   productname_lbl->setText ( name );
   version_lbl->setText ( version );
}
