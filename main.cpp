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

#ifdef __APPLE__
#include <stdint.h>
extern "C" extern int _NSGetExecutablePath(char *buf, uint32_t *bufsize);
#endif

#include <qapplication.h>
#include <qtranslator.h>
#include <qlocale.h>

#include "llk.h"
#include "general.h"

int main ( int argc, char **argv ) {
#ifdef __APPLE__
    // Hack for OS X Mavericks or later
    char buf[4096];
    uint32_t size = 4096;
    _NSGetExecutablePath(buf, &size);

    char *p = &buf[strlen(buf) - 1];
    while (*p != '/') {
        p--;
    }
    *p = '\0';
    strcat(buf, "/../../..");
    printf("%s\n", buf);
    chdir(buf);
#endif
   QApplication qapp ( argc, argv );
   qapp.setStyle ( "plastik" );

   QString locale  = QLocale::system().name();

   // translation file for Qt
   QTranslator qt ( 0 );
   qt.load ( QString ( "qt_" ) + locale, "." );
   qapp.installTranslator ( &qt );

   // translation file for application strings
   QTranslator myapp ( 0 );
   myapp.load ( LLK_INSTALL_DIR + QString ( "/llk_" ) + locale, "." );
   qapp.installTranslator ( &myapp );

   App *llk = new App();
   llk->show();
   qapp.setMainWidget ( llk );
   return qapp.exec();
}
