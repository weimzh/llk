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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qpixmap.h>

#include "tileset.h"
#include "general.h"


TileSet::TileSet() {
   srand ( time ( NULL ) );

   // loadTiles
   QImage tmpImage ( LLK_INSTALL_DIR + QString ( "/portrait.png" ) );
   QImage tmpImage1 ( LLK_INSTALL_DIR + QString ( "/cellback.png" ) );
   timeBarImg.load ( LLK_INSTALL_DIR + QString ( "/timebar.png" ) );
   if ( tmpImage.isNull() || tmpImage1.isNull() || timeBarImg.isNull() ) {
      QMessageBox::information ( 0, QString::null, QObject::tr ( "Cannot load tiles pixmap!" ), QObject::tr ( "&OK" ) );
      QApplication::exit ( 1 );
   }

   // split into individual tiles
   img1.reserve ( NTILES );
   int w = tmpImage.width() / TILES_X;
   int h = tmpImage.height() / TILES_Y;
   for ( int row = 0;row < TILES_Y;row++ )
      for ( int col = 0;col < TILES_X;col++ )
         img1.push_back ( tmpImage.copy ( col * w, row * h, w, h ) );

   img2.reserve ( 12 );
   w = tmpImage1.width() / 2;
   h = tmpImage1.height() / 6;
   for ( int row = 0;row < BACK_Y;row++ )
      for ( int col = 0;col < BACK_X;col++ )
         img2.push_back ( tmpImage1.copy ( col * w, row * h, w, h ) );

   // set tiles' back color and create image buffer -- bufImg
   changeBackColor();
}

TileSet::~TileSet() {}

QPixmap TileSet::tile ( int n ) const {
   return bufImg[n];
}

QPixmap TileSet::highlightedTile ( int n ) const {
   return bufImg[NTILES+n];
}

QPixmap TileSet::timeBar() const {
   return timeBarImg;
}

void TileSet::changeBackColor() {
   backcolor = rand() % 6 * 2;
   createBuffer();
}

void TileSet::createBuffer() {
   bufImg.clear();
   bufImg.reserve ( 2 * NTILES );

   for ( int i = 0;i < NTILES;i++ ) {
      QPixmap pix ( img2[backcolor] );
      QImage img ( img1[i] );
      bitBlt ( &pix, ( CELL_WIDTH - img.width() ) / 2, ( CELL_HEIGHT - img.height() ) / 2, &img, 0, 0, -1, -1, Qt::CopyROP );
      bufImg.push_back ( pix );
   }

   for ( int i = 0;i < NTILES;i++ ) {
      QPixmap pix ( img2[backcolor+1] );
      QImage img ( img1[i] );
      bitBlt ( &pix, ( CELL_WIDTH - img.width() ) / 2, ( CELL_HEIGHT - img.height() ) / 2, &img, 0, 0, -1, -1, Qt::CopyROP );
      bufImg.push_back ( pix );
   }
}
