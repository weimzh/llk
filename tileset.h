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

#ifndef __IMAGEDATA__H__
#define __IMAGEDATA__H__

#include <qvaluevector.h>

#define CELL_WIDTH 40
#define CELL_HEIGHT 50
#define GRID_WIDTH 46
#define GRID_HEIGHT 56
#define TILES_X 9
#define TILES_Y 4
#define BACK_X 2
#define BACK_Y 6
#define NTILES 36

class TileSet {
public:
   TileSet();
   ~TileSet();

   QPixmap tile ( int n ) const;
   QPixmap highlightedTile ( int n ) const;
   QPixmap timeBar() const;

   void changeBackColor();
   void createBuffer();

private:
   QValueVector<QImage> img1;
   QValueVector<QImage> img2;
   QValueVector<QPixmap> bufImg;
   QPixmap timeBarImg;
   int backcolor;
};

#endif

