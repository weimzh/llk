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
#include <time.h>
#include <stdlib.h>

#include <qapp.h>
#include <qrect.h>
#include <qfont.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qtimer.h>
#include <qcolor.h>
#include <qstring.h>
#include <qrect.h>
#include <qtimer.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

#include "board.h"
#include "qconfig.h"
#include "changebackgroundui.h"
#include "qcolorbutton.h"
#include "myfiledialog.h"
#include "qurlrequester.h"

#include "sound.h"

Board::Board ( QWidget *parent ) : QWidget ( parent, 0, WNoAutoErase ) {
   config = 0;
   srand ( time ( NULL ) );
   field = 0;
   paused = FALSE;
   finished = TRUE;
   moveFinished = TRUE;
   level = -1;
   hintChances = 0;
   riffleChances = 0;
   direct = BuBian;

   timeBarImg = tiles.timeBar();

   config = getConfig();

   // BEGIN setPaletteBackgroundPixmap | setPaletteBackgroundColor
   dlgChangeBackground = 0;

   readBackgroundSetting();

   if ( backgroundPixmap != QString::null ) {
      if ( bgImage.load ( backgroundPixmap ) == FALSE ) {
         backgroundPixmap = QString::null;
         backgroundType = 1;
      }
   } else {
      backgroundType = 1;
   }

   if ( backgroundType == 0 ) {
      if ( createBackgroundImage() == FALSE ) {
         backgroundPixmap = QString::null;
         backgroundType = 1;
      }
   }

   if ( backgroundType == 0 )
      setPaletteBackgroundPixmap ( QPixmap ( bgImage ) );
   else
      setPaletteBackgroundColor ( backgroundColor );
   // END setPaletteBackgroundPixmap | setPaletteBackgroundColor

   timer = new QTimer ( this );
   connect ( timer, SIGNAL ( timeout() ), SLOT ( slotUpdateTime() ) );
#ifndef DEBUGGING
   drawConnectionTimer = new QTimer ( this );
   connect ( drawConnectionTimer, SIGNAL ( timeout() ), SLOT ( undrawConnection() ) );
#endif

   timeRect.setRect ( 280, 32, 304, 32 );
   scoreRect.setRect ( 640, 0, 150, 32 );
   riffleRect.setRect ( 0, 32, 150, 32 );
   hintRect.setRect ( 150, 32, 150, 32 );
   bonusRect.setRect ( 150, 64, 500, 32 );

   loadSound ();
}

Board::~Board() {
   delete [] field;
   if ( dlgChangeBackground )
      delete dlgChangeBackground;

   if ( timer->isActive() )
      timer->stop();
   delete timer;

   freeSound();

#ifndef DEBUGGING
   if ( drawConnectionTimer->isActive() )
      drawConnectionTimer->stop();
   delete drawConnectionTimer;
#endif
}

int Board::x_tiles() const {
   return _x_tiles;
}

int Board::y_tiles() const {
   return _y_tiles;
}

void Board::setField ( int x, int y, int value ) {
   if ( x < 0 || y < 0 || x >= x_tiles() || y >= y_tiles() ) {
      qFatal ( "Attempted write to invalid field position (%d,%d)", x, y );
   }

   if ( value < 0 || value > NTILES )
      return;

   field[y * x_tiles() + x] = value;
}

int Board::getField ( int x, int y ) const {
#ifdef DEBUGGING
   if ( x < -1 || y < -1 || x > x_tiles() || y > y_tiles() ) {
      qFatal ( "Attempted read from invalid field position (%d,%d)", x, y );
   }
#endif

   if ( x < 0 || y < 0 || x >= x_tiles() || y >= y_tiles() )
      return EMPTY;

   return field[y * x_tiles() + x];
}

void Board::moveTilesUp ( int col, int row, int bottom ) {
   int rptr = row, wptr = row;
   int rc = col;
   while ( rptr < bottom ) {
      if ( getField ( rc, wptr ) != EMPTY ) {
         rptr++;
         wptr++;
      } else {
         if ( getField ( rc, rptr ) != EMPTY ) {
            setField ( rc, wptr, getField ( rc, rptr ) );
            setField ( rc, rptr, EMPTY );

            updateField ( rc, rptr );
            updateField ( rc, wptr );

            wptr++;
            rptr++;
         } else
            rptr++;
      }
   }
}

void Board::moveTilesDown ( int col, int row, int top ) {
   int rptr = row, wptr = row;
   int rc = col;
   while ( rptr >= top ) {
      if ( getField ( rc, wptr ) != EMPTY ) {
         rptr--;
         wptr--;
      } else {
         if ( getField ( rc, rptr ) != EMPTY ) {
            setField ( rc, wptr, getField ( rc, rptr ) );
            setField ( rc, rptr, EMPTY );

            updateField ( rc, rptr );
            updateField ( rc, wptr );

            wptr--;
            rptr--;
         } else
            rptr--;
      }
   }
}

void Board::moveTilesLeft ( int col, int row, int right ) {
   int rptr = col, wptr = col;
   int rc = row;
   while ( rptr < right ) {
      if ( getField ( wptr, rc ) != EMPTY ) {
         rptr++;
         wptr++;
      } else {
         if ( getField ( rptr, rc ) != EMPTY ) {
            setField ( wptr, rc, getField ( rptr, rc ) );
            setField ( rptr, rc, EMPTY );

            updateField ( rptr, rc );
            updateField ( wptr, rc );

            wptr++;
            rptr++;
         } else
            rptr++;
      }
   }

   if ( direct == XiangWaiKuoSan || direct == XiangNeiJiZhong ) {
      if ( getField ( col, row ) == 0 )
         needCheckCols.push ( QPoint ( col, row ) );
      else {
         int i = col;
         while ( getField ( i, row ) )
            i++;
         needCheckCols.push ( QPoint ( i, row ) );
         if ( ++i < right )
            needCheckCols.push ( QPoint ( i, row ) );
      }
   }
}

void Board::moveTilesRight ( int col, int row, int left ) {
   int rptr = col, wptr = col;
   int rc = row;
   while ( rptr >= left ) {
      if ( getField ( wptr, rc ) != EMPTY ) {
         rptr--;
         wptr--;
      } else {
         if ( getField ( rptr, rc ) != EMPTY ) {
            setField ( wptr, rc, getField ( rptr, rc ) );
            setField ( rptr, rc, EMPTY );

            updateField ( rptr, rc );
            updateField ( wptr, rc );

            wptr--;
            rptr--;
         } else
            rptr--;
      }
   }

   if ( direct == XiangWaiKuoSan || direct == XiangNeiJiZhong ) {
      if ( getField ( col, row ) == 0 )
         needCheckCols.push ( QPoint ( col, row ) );
      else {
         int i = col;
         while ( getField ( i, row ) )
            i--;
         needCheckCols.push ( QPoint ( i, row ) );
         if ( --i >= left )
            needCheckCols.push ( QPoint ( i, row ) );
      }
   }
}

void Board::slotMoveTiles ( QPoint p, bool isRow ) {
   switch ( direct ) {
   case XiangXia: {
      moveTilesDown ( p.x(), p.y(), 0 );
      break;
   }
   case XiangZuo: {
      moveTilesLeft ( p.x(), p.y(), x_tiles() );
      break;
   }
   case ShangXiaFenLi: {
      if ( p.y() < y_tiles() / 2 )
         moveTilesUp ( p.x(), p.y(), y_tiles() / 2 );
      else
         moveTilesDown ( p.x(), p.y(), y_tiles() / 2 );
      break;
   }
   case ZuoYouFenLi: {
      if ( p.x() < x_tiles() / 2 )
         moveTilesLeft ( p.x(), p.y(), x_tiles() / 2 );
      else
         moveTilesRight ( p.x(), p.y(), x_tiles() / 2 );
      break;
   }
   case ShangXiaJiZhong: {
      if ( p.y() < y_tiles() / 2 )
         moveTilesDown ( p.x(), p.y(), 0 );
      else
         moveTilesUp ( p.x(), p.y(), y_tiles() );
      break;
   }
   case ZuoYouJiZhong: {
      if ( p.x() < x_tiles() / 2 )
         moveTilesRight ( p.x(), p.y(), 0 );
      else
         moveTilesLeft ( p.x(), p.y(), x_tiles() );
      break;
   }
   case ShangZuoXiaYou: {
      if ( p.y() < y_tiles() / 2 )
         moveTilesLeft ( p.x(), p.y(), x_tiles() );
      else
         moveTilesRight ( p.x(), p.y(), 0 );
      break;
   }
   case ZuoXiaYouShang: {
      if ( p.x() < x_tiles() / 2 )
         moveTilesDown ( p.x(), p.y(), 0 );
      else
         moveTilesUp ( p.x(), p.y(), y_tiles() );
      break;
   }
   case XiangWaiKuoSan: {
      if ( isRow ) {
         if ( p.x() < x_tiles() / 2 )
            moveTilesLeft ( p.x(), p.y(), x_tiles() / 2 );
         else
            moveTilesRight ( p.x(), p.y(), x_tiles() / 2 );
      } else {
         if ( p.y() < y_tiles() / 2 )
            moveTilesUp ( p.x(), p.y(), y_tiles() / 2 );
         else
            moveTilesDown ( p.x(), p.y(), y_tiles() / 2 );
      }
      break;
   }
   case XiangNeiJiZhong: {
      if ( isRow ) {
         if ( p.x() < x_tiles() / 2 )
            moveTilesRight ( p.x(), p.y(), 0 );
         else
            moveTilesLeft ( p.x(), p.y(), x_tiles() );
      } else {
         if ( p.y() < y_tiles() / 2 )
            moveTilesDown ( p.x(), p.y(), 0 );
         else
            moveTilesUp ( p.x(), p.y(), y_tiles() );
      }
      break;
   }
   case BuBian:
   default:
      ;
   }
}

void Board::mousePressEvent ( QMouseEvent *e ) {
   // Calculate field position
   int pos_x = ( e->x() - xOffset() ) / CELL_WIDTH;
   int pos_y = ( e->y() - yOffset() ) / CELL_HEIGHT;

   if ( e->x() < xOffset() || e->y() < yOffset() ||
         pos_x >= x_tiles() || pos_y >= y_tiles() ) {
      pos_x = -1;
      pos_y = -1;
   }

   // Mark tile
   if ( e->button() == LeftButton ) {
      // Clear highlighted tiles
      if ( pos_x != -1 ) {
         marked ( pos_x, pos_y );
      }
   }

   if ( e->button() == RightButton ) {
      if ( mark_x != -1 ) {
         // We need to set mark_x and mark_y to -1 before calling
         // updateField() to ensure the tile is redrawn as unmarked.
         int oldmarkx = mark_x;
         int oldmarky = mark_y;
         mark_x = -1;
         mark_y = -1;
         updateField ( oldmarkx, oldmarky, false );
         playSound ( SOUND_CANCEL );
      }
   }
}

// The board is centred inside the main playing area. xOffset/yOffset provide
// the coordinates of the top-left corner of the board.
int Board::xOffset() const {
   return ( width() - ( ( GRID_WIDTH - CELL_WIDTH ) + CELL_WIDTH * x_tiles() ) ) / 2;
}

int Board::yOffset() const {
   if ( difficulty == 2 )
      return ( int ) ( ( height() - ( ( GRID_HEIGHT - CELL_HEIGHT ) + CELL_HEIGHT * y_tiles() ) ) / 1.2 );
   else
      return ( int ) ( ( height() - ( ( GRID_HEIGHT - CELL_HEIGHT ) + CELL_HEIGHT * y_tiles() ) ) / 1.5 );
}

void Board::setSize ( int x, int y ) {
   difficulty = 0;
   if ( x > 12 )
      difficulty = 1;
   if ( x > 14 )
      difficulty = 2;

   if ( field != 0 )
      delete [] field;

   field = new int[ x * y ];
   _x_tiles = x;
   _y_tiles = y;
   for ( int i = 0; i < x; i++ )
      for ( int j = 0; j < y; j++ ) {
         setField ( i, j, EMPTY );
      }

   emit changed();
}

void Board::resizeEvent ( QResizeEvent* ) {
   if ( backgroundType == 0 ) {
      if ( createBackgroundImage() )
         setPaletteBackgroundPixmap ( QPixmap ( bgImage ) );
   }
}

QSize Board::minSize() const {
   int w = ( GRID_WIDTH - CELL_WIDTH ) + CELL_WIDTH * x_tiles();
   int h = ( GRID_HEIGHT - CELL_HEIGHT ) + CELL_HEIGHT * y_tiles();
   w += 4 * GRID_WIDTH;
   h += 4 * GRID_HEIGHT;
   return QSize ( w, h );
}

void Board::newGame() {
   level = -1;
   slotLevelUp();
}

bool Board::isTileHighlighted ( int x, int y ) const {
   if ( x == mark_x && y == mark_y )
      return true;

   if ( !connection.empty() ) {
      if ( x == connection.front().x && y == connection.front().y )
         return true;

      if ( x == connection.back().x && y == connection.back().y )
         return true;
   }

   return false;
}

void Board::updateField ( int x, int y, bool erase ) {
   QRect r ( xOffset() + x * CELL_WIDTH, yOffset() + y * CELL_HEIGHT, GRID_WIDTH, GRID_HEIGHT );

   repaint ( r, erase );
}

void Board::paintEvent ( QPaintEvent *e ) {
   QRect ur = e->rect();            // rectangle to update
   QPixmap pm ( ur.size() );           // Pixmap for double-buffering
   pm.fill ( this, ur.topLeft() );     // fill with widget background

   if ( finished ) {
      bitBlt ( this, ur.topLeft(), &pm );
      return;
   }

   QPainter p ( &pm );
   p.translate ( -ur.x(), -ur.y() );   // use widget coordinate system

   p.setPen ( yellow );
   QFont font ( "URW Chancery L", 24, QFont::Bold, TRUE );
   p.setFont ( font );
   QFont font1 ( "helvetica", 16 );
   font1.setStyleHint ( QFont::SansSerif );
   font1.setWeight ( QFont::Bold );

   // BEGIN draw some panel display
   // BEGIN difficulty
   QRect re ( 0, 0, 150, 32 );
   if ( re.intersects ( ur ) ) {
      QString strDifficulty ( "Class " );
      if ( difficulty == 0 )
         strDifficulty += "Easy";
      else if ( difficulty == 1 )
         strDifficulty += "Normal";
      else
         strDifficulty += "Hard";
      p.save();
      font.setWeight ( QFont::Black );
      p.setFont ( font );
      if ( backgroundColor == white ) {
         p.setPen ( gray );
         re.moveBy ( 2, 1 );
         p.drawText ( re, Qt::AlignVCenter | Qt::AlignLeft, strDifficulty );
         re.moveBy ( -2, -1 );
      }
      p.setPen ( white );
      p.drawText ( re, Qt::AlignVCenter | Qt::AlignLeft, strDifficulty );
      p.restore();
   }
   //END difficulty
   //level
   re.moveTopLeft ( QPoint ( 150, 0 ) );
   if ( re.intersects ( ur ) ) {
      QString strLevel ( tr("Level") + ": " );
      strLevel += QString::number ( level + 1 );
      p.save();
      p.setFont ( font1 );
      p.drawText ( re, Qt::AlignVCenter | Qt::AlignLeft, strLevel );
      p.restore();
   }
   // riffle
   if ( riffleChances > 0 && riffleRect.intersects ( ur ) ) {
      QString strRiffle = QString ( tr ( "Riffle: %1" ) ).arg ( riffleChances );
      p.save();
      p.setFont ( font1 );
      p.drawText ( riffleRect, Qt::AlignVCenter | Qt::AlignLeft, strRiffle );
      p.restore();
   }
   // hint
   if ( hintChances > 0 && hintRect.intersects ( ur ) ) {
      QString strHint = QString ( tr ( "Hint: %1" ) ).arg ( hintChances );
      p.save();
      font.setPointSize ( 12 );
      p.setFont ( font1 );
      p.drawText ( hintRect, Qt::AlignVCenter | Qt::AlignLeft, strHint );
      p.restore();
   }
   // erase mode
   QRect r ( 280, 0, 300, 32 );
   if ( r.intersects ( ur ) ) {
      QString s;
      switch ( direct ) {
      case XiangXia: {
         s = tr ( "Downwards" );
         break;
      }
      case XiangZuo: {
         s = tr ( "Leftwards" );
         break;
      }
      case ShangXiaFenLi: {
         s = tr ( "Part vertically" );
         break;
      }
      case ZuoYouFenLi: {
         s = tr ( "Part horizentally" );
         break;
      }
      case ShangXiaJiZhong: {
         s = tr ( "Gather vertically" );
         break;
      }
      case ZuoYouJiZhong: {
         s = tr ( "Gather horizentally" );
         break;
      }
      case ShangZuoXiaYou: {
         s = tr ( "Above-left / Below-right" );
         break;
      }
      case ZuoXiaYouShang: {
         s = tr ( "Left-fall / Right-rise" );
         break;
      }
      case XiangWaiKuoSan: {
         s = tr ( "Spread" );
         break;
      }
      case XiangNeiJiZhong: {
         s = tr ( "Shrink" );
         break;
      }
      case BuBian:
      default:
         s = tr ( "No Change" );
      }

      p.save();
      font.setPointSize ( 18 );
      p.setFont ( font1 );
      p.drawText ( r, Qt::AlignVCenter | Qt::AlignLeft, s );
      p.restore();
   }
   if ( timeRect.intersects ( ur ) ) {
      int y = timeRect.y() + ( timeRect.height() - timeBarImg.height() ) / 2;
      p.save();
      p.drawPixmap ( timeRect.x(), y, timeBarImg );
      p.setPen ( NoPen );
      double step = 300.0 / ( double ) totalTime;
      int w = 300 - qRound ( remainTime * step );
      if ( w <= 0 ) {
         p.setBrush ( white );
         p.drawRect ( timeRect.x() + 301, y + 2, -w, 15 );
         timeRect.setWidth ( timeRect.width() - w );
      } else {
         p.setBrush ( black );
         p.drawRect ( timeRect.x() + 2 + qRound ( remainTime * step ), y + 2, w, 15 );
      }
      p.restore();
   }
   // score
   if ( scoreRect.intersects ( ur ) ) {
      p.save();
      font.setWeight ( QFont::Black );
      p.setFont ( font );
      if ( backgroundColor == white ) {
         p.setPen ( gray );
         scoreRect.moveBy ( 2, 1 );
         p.drawText ( scoreRect, Qt::AlignVCenter | Qt::AlignLeft, QString::number ( score ) );
         scoreRect.moveBy ( -2, -1 );
      }
      p.setPen ( white );
      p.drawText ( scoreRect, Qt::AlignVCenter | Qt::AlignLeft, QString::number ( score ) );
      p.restore();
   }
   // bonus
   if ( showingBonus > 0 && bonusRect.intersects ( ur ) ) {
      p.save();
      p.setFont ( font1 );
      p.drawText ( bonusRect, Qt::AlignVCenter | Qt::AlignLeft, bonusStr );
      p.restore();
   }
   // END

   if ( paused ) {
      QRect re ( rect() );
      QString s ( tr ( "Game Paused" ) );
      p.save();
      //p.setFont(KGlobalSettings::largeFont());
      p.setFont ( QFont ( "courier", 48 ) );
      p.setPen ( gray );
      re.moveBy ( 2, 1 );
      p.drawText ( re, Qt::AlignCenter, s );
      p.setPen ( white );
      re.moveBy ( -2, -1 );
      p.drawText ( re, Qt::AlignCenter, s );
      p.restore();
   } else {
      int w = CELL_WIDTH;
      int h = CELL_HEIGHT;
      for ( int i = 0; i < x_tiles(); i++ )
         for ( int j = 0; j < y_tiles(); j++ ) {
            int tile = getField ( i, j );
            if ( tile == EMPTY )
               continue;
            int xpos = xOffset() + i * w;
            int ypos = yOffset() + j * h;
            QRect r ( xpos, ypos, GRID_WIDTH, GRID_HEIGHT );
            if ( e->rect().intersects ( r ) ) {
               if ( isTileHighlighted ( i, j ) )
                  p.drawPixmap ( xpos, ypos, tiles.highlightedTile ( tile - 1 ) );
               else
                  p.drawPixmap ( xpos, ypos, tiles.tile ( tile - 1 ) );
            }
         }
   }
   p.end();
   bitBlt ( this, ur.topLeft(), &pm );
}

void Board::marked ( int x, int y ) {
   if ( moveFinished == FALSE )
      undrawConnection();

   if ( getField ( x, y ) == EMPTY ) {
      return;
   }

   if ( x == mark_x && y == mark_y ) {
      // unmark the piece
      mark_x = -1;
      mark_y = -1;
      updateField ( x, y, false );
      playSound ( SOUND_CANCEL );
      return;
   }

   if ( mark_x == -1 ) {
      mark_x = x;
      mark_y = y;
      updateField ( x, y, false );
      playSound ( SOUND_CLICK );
      return;
   }

   int fld1 = getField ( mark_x, mark_y );
   int fld2 = getField ( x, y );

   // both field same?
   if ( fld1 != fld2 ) {
      // not same, clear old mark
      int oldx = mark_x;
      int oldy = mark_y;
      mark_x = -1;
      mark_y = -1;
      updateField ( oldx, oldy, FALSE );
      playSound ( SOUND_CANCEL );
      return;
   }

   // trace
   if ( findPath ( mark_x, mark_y, x, y, connection ) ) {
      QByteArray tmp ( y_tiles() * x_tiles() );
      for ( int i = 0; i < y_tiles(); i++ )
         for ( int j = 0; j < x_tiles(); j++ )
            tmp[i * x_tiles() +j] = field[i * x_tiles() +j];

      pageShot.push ( tmp );
      redoStack.clear();
      emit changed();

      playSound ( SOUND_LINK );
      drawConnection ( DEFAULTDELAY );
      setField ( mark_x, mark_y, EMPTY );
      setField ( x, y, EMPTY );

      score += scoreStep;
      update ( scoreRect );
      remainTime += TIMEBONUS;
      if ( difficulty == 2 )
         remainTime++;
      update ( timeRect );

      needCheckRows.clear();
      needCheckCols.clear();
      switch ( direct ) {
      case XiangXia:
      case ShangXiaFenLi:
      case ZuoXiaYouShang:
      case ShangXiaJiZhong: {
         needCheckCols.push ( QPoint ( x, y ) );
         needCheckCols.push ( QPoint ( mark_x, mark_y ) );
         break;
      }
      case XiangZuo:
      case ZuoYouFenLi:
      case ShangZuoXiaYou:
      case ZuoYouJiZhong:
      case XiangWaiKuoSan:
      case XiangNeiJiZhong: {
         needCheckRows.push ( QPoint ( x, y ) );
         needCheckRows.push ( QPoint ( mark_x, mark_y ) );
         break;
      }
      case BuBian:
      default:
         ;
      }

      mark_x = -1;
      mark_y = -1;

      randomBonus();

      // game is over?
      // Must delay until after tiles fall to make this test
      // See undrawConnection GP.
   } else {
      connection.clear();
      int oldx = mark_x;
      int oldy = mark_y;
      mark_x = -1;
      mark_y = -1;
      updateField ( oldx, oldy, FALSE );
      playSound ( SOUND_NOLINK );
   }
}

// Can we make a path between two tiles with a single line?
bool Board::canMakePath ( int x1, int y1, int x2, int y2 ) const {
   if ( x1 == x2 ) {
      for ( int i = std::min ( y1, y2 ) + 1; i < std::max ( y1, y2 ); i++ )
         if ( getField ( x1, i ) != EMPTY )
            return false;

      return true;
   }

   if ( y1 == y2 ) {
      for ( int i = std::min ( x1, x2 ) + 1; i < std::max ( x1, x2 ); i++ )
         if ( getField ( i, y1 ) != EMPTY )
            return false;

      return true;
   }

   return false;
}

bool Board::findPath ( int x1, int y1, int x2, int y2, Path& p ) const {
   p.clear();

   if ( findSimplePath ( x1, y1, x2, y2, p ) )
      return true;

   // Find a path of 3 segments
   const int dx[4] = { 1, 0, -1, 0 };
   const int dy[4] = { 0, 1, 0, -1 };

   for ( int i = 0; i < 4; i++ ) {
      int newx = x1 + dx[i];
      int newy = y1 + dy[i];
      while ( newx >= -1 && newx <= x_tiles() &&
              newy >= -1 && newy <= y_tiles() &&
              getField ( newx, newy ) == EMPTY ) {
         if ( findSimplePath ( newx, newy, x2, y2, p ) ) {
            p.push_front ( Position ( x1, y1 ) );
            return true;
         }
         newx += dx[i];
         newy += dy[i];
      }
   }

   return false;
}

// Find a path of 1 or 2 segments between tiles. Returns whether
// a path was found, and if so, the path is returned via 'p'.
bool Board::findSimplePath ( int x1, int y1, int x2, int y2, Path& p ) const {
   // Find direct line (path of 1 segment)
   if ( canMakePath ( x1, y1, x2, y2 ) ) {
      p.push_back ( Position ( x1, y1 ) );
      p.push_back ( Position ( x2, y2 ) );
      return true;
   }

   // If the tiles are in the same row or column, then
   // a 'simple path' cannot be found between them
   if ( x1 == x2 || y1 == y2 )
      return false;

   // Find path of 2 segments (route A)
   if ( getField ( x2, y1 ) == EMPTY && canMakePath ( x1, y1, x2, y1 ) &&
         canMakePath ( x2, y1, x2, y2 ) ) {
      p.push_back ( Position ( x1, y1 ) );
      p.push_back ( Position ( x2, y1 ) );
      p.push_back ( Position ( x2, y2 ) );
      return true;
   }

   // Find path of 2 segments (route B)
   if ( getField ( x1, y2 ) == EMPTY && canMakePath ( x1, y1, x1, y2 ) &&
         canMakePath ( x1, y2, x2, y2 ) ) {
      p.push_back ( Position ( x1, y1 ) );
      p.push_back ( Position ( x1, y2 ) );
      p.push_back ( Position ( x2, y2 ) );
      return true;
   }

   return false;
}

void Board::drawConnection ( int timeout ) {
   if ( connection.empty() )
      return;

#ifndef DEBUGGING
   if ( drawConnectionTimer->isActive() )
      drawConnectionTimer->stop();
#endif

   // lighten the fields
   updateField ( connection.front().x, connection.front().y );
   updateField ( connection.back().x, connection.back().y );

   QPainter p;
   p.begin ( this );
   QPen pen ( QColor ( "red" ), 2 );
   pen.setJoinStyle ( Qt::RoundJoin );
   p.setPen ( pen );

   // Path.size() will always be >= 2
   Path::const_iterator pathEnd = connection.end();
   Path::const_iterator pt1 = connection.begin();
   Path::const_iterator pt2 = pt1;
   ++pt2;
   while ( pt2 != pathEnd ) {
      p.drawLine ( midCoord ( pt1->x, pt1->y ), midCoord ( pt2->x, pt2->y ) );
      ++pt1;
      ++pt2;
   }

   p.flush();
   p.end();

   moveFinished = FALSE;

#ifndef DEBUGGING
   drawConnectionTimer->start ( timeout, TRUE );
#else
   QTimer::singleShot ( timeout, this, SLOT ( undrawConnection() ) );
#endif
}

void Board::undrawConnection ( bool needCheck ) {
   // is already undrawn?
   if ( connection.empty() )
      return;

#ifndef DEBUGGING
   if ( drawConnectionTimer->isActive() )
      drawConnectionTimer->stop();
#endif

   // Redraw all affected fields
   Path oldConnection = connection;
   connection.clear();

   // Path.size() will always be >= 2
   Path::const_iterator pathEnd = oldConnection.end();
   Path::const_iterator pt1 = oldConnection.begin();
   Path::const_iterator pt2 = pt1;
   ++pt2;
   while ( pt2 != pathEnd ) {
      if ( pt1->y == pt2->y ) {
         for ( int i = std::min ( pt1->x, pt2->x ); i <= std::max ( pt1->x, pt2->x ); i++ )
            updateField ( i, pt1->y );
      } else {
         for ( int i = std::min ( pt1->y, pt2->y ); i <= std::max ( pt1->y, pt2->y ); i++ )
            updateField ( pt1->x, i );
      }
      ++pt1;
      ++pt2;
   }

   if ( needCheck == FALSE )
      return;

   if ( direct != BuBian ) {
      while ( !needCheckRows.isEmpty() )
         slotMoveTiles ( needCheckRows.pop(), TRUE );
      while ( !needCheckCols.isEmpty() )
         slotMoveTiles ( needCheckCols.pop(), FALSE );
   }
   moveFinished = TRUE;
   checkIfEnd();
}

QPoint Board::midCoord ( int x, int y ) const {
   QPoint p;
   int w = CELL_WIDTH;
   int h = CELL_HEIGHT;

   if ( x == -1 )
      p.setX ( xOffset() - ( w / 4 ) );
   else if ( x == x_tiles() )
      p.setX ( xOffset() + ( w * x_tiles() ) + ( w / 4 ) );
   else
      p.setX ( xOffset() + ( w * x ) + ( w / 2 ) );

   if ( y == -1 )
      p.setY ( yOffset() - ( w / 4 ) );
   else if ( y == y_tiles() )
      p.setY ( yOffset() + ( h * y_tiles() ) + ( w / 4 ) );
   else
      p.setY ( yOffset() + ( h * y ) + ( h / 2 ) );

   return p;
}

bool Board::canUndo() const {
#ifdef DEBUGGING
   return !pageShot.isEmpty();
#else
   return !pageShot.isEmpty() && score >= 500;
#endif
}

bool Board::canRedo() const {
   return !redoStack.isEmpty();
}

void Board::undo() {
   if ( !canUndo() )
      return;

   QByteArray tmp ( y_tiles() * x_tiles() );
   for ( int i = 0;i < y_tiles();i++ )
      for ( int j = 0;j < x_tiles();j++ )
         tmp[i * x_tiles() +j] = field[i*x_tiles() +j];
   redoStack.push ( tmp );

   tmp = pageShot.pop();
   for ( int i = 0;i < y_tiles();i++ )
      for ( int j = 0;j < x_tiles();j++ )
         field[i*x_tiles() +j] = tmp[i*x_tiles() +j];
#ifndef DEBUGGING
   score -= 500;
#endif
   update();
}

void Board::redo() {
   if ( !canRedo() )
      return;

   QByteArray tmp ( y_tiles() * x_tiles() );
   for ( int i = 0;i < y_tiles();i++ )
      for ( int j = 0;j < x_tiles();j++ )
         tmp[i*x_tiles() +j] = field[i*x_tiles() +j];
   pageShot.push ( tmp );

   tmp = redoStack.pop();
   for ( int i = 0;i < y_tiles();i++ )
      for ( int j = 0;j < x_tiles();j++ )
         field[i*x_tiles() +j] = tmp[i*x_tiles() +j];

   update();
}

void Board::showHint() {
   undrawConnection ( FALSE );

   if ( hintChances > 0 && getHint_I ( connection ) ) {
      if ( mark_x != -1 ) {
         // We need to set mark_x and mark_y to -1 before calling
         // updateField() to ensure the tile is redrawn as unmarked.
         int oldmarkx = mark_x;
         int oldmarky = mark_y;
         mark_x = -1;
         mark_y = -1;
         updateField ( oldmarkx, oldmarky, false );
      }

      playSound ( SOUND_HINT );
      drawConnection ( 1000 );
      setHintPoints ( hintChances - 1 );
      update ( hintRect );
   }
}


#ifdef DEBUGGING
void Board::makeHintMove() {
   Path p;

   if ( hintChances > 0 && getHint_I ( p ) ) {
      if ( mark_x != -1 ) {
         // We need to set mark_x and mark_y to -1 before calling
         // updateField() to ensure the tile is redrawn as unmarked.
         int oldmarkx = mark_x;
         int oldmarky = mark_y;
         mark_x = -1;
         mark_y = -1;
         updateField ( oldmarkx, oldmarky, false );
      }

      marked ( p.front().x, p.front().y );
      marked ( p.back().x, p.back().y );
   }
}

void Board::finish() {
   Path p;
   bool ready = false;

   while ( !ready && getHint_I ( p ) ) {
      mark_x = -1;
      mark_y = -1;
      if ( tilesLeft() == 2 )
         ready = true;
      marked ( p.front().x, p.front().y );
      marked ( p.back().x, p.back().y );
      qApp->processEvents();
      usleep ( 250 * 1000 );
   }
}

void Board::dumpBoard() const {
   qDebug ( "Board contents:" );
   for ( int y = 0; y < y_tiles(); ++y ) {
      QString row;
      for ( int x = 0; x < x_tiles(); ++x ) {
         int tile = getField ( x, y );
         if ( tile == EMPTY )
            row += " --";
         else
            row += QString ( "%1" ).arg ( tile, 3 );
      }
      qDebug ( "%s", row.latin1() );
   }
}
#endif

bool Board::getHint_I ( Path& p ) const {
   char done[NTILES];
   memset ( done, 0, NTILES );

   for ( int x = 0; x < x_tiles(); x++ )
      for ( int y = 0; y < y_tiles(); y++ ) {
         int tile = getField ( x, y );
         if ( tile != EMPTY && done[tile - 1] != 4 ) {
            // for all these types of tile search path's
            for ( int xx = 0; xx < x_tiles(); xx++ )
               for ( int yy = 0; yy < y_tiles(); yy++ ) {
                  if ( xx != x || yy != y ) {
                     if ( getField ( xx, yy ) == tile )
                        if ( findPath ( x, y, xx, yy, p ) ) {
                           return true;
                        }
                  }
               }
            done[tile - 1]++;
         }
      }

   return false;
}

int Board::tilesLeft() const {
   int left = 0;

   for ( int i = 0; i < x_tiles(); i++ )
      for ( int j = 0; j < y_tiles(); j++ )
         if ( getField ( i, j ) != EMPTY )
            left++;

   return left;
}

bool Board::pause() {
   paused = !paused;
   if ( paused ) {
      timer->stop();
   } else {
      timer->start ( 1000 );
   }
   update();

   return paused;
}

void Board::readBackgroundSetting() {
   QString oldGroup = config->group();
   config->setGroup ( "BACKGROUNDSETTING" );
   backgroundType = config->readNumEntry ( "BACKGROUNDTYPE", 0 );
   backgroundPixmap = config->readPathEntry ( "BACKGROUNDPIXMAP", QString::null );
   bgPixmapScaleMode = config->readNumEntry ( "BGPIXMAPSCALEMODE", 0 );
   QColor color = black;
   backgroundColor = config->readColorEntry ( "BACKGROUNDCOLOR", &color );
   config->setGroup ( oldGroup );
}

void Board::writeBackgroundSetting() {
   QString oldGroup = config->group();
   config->setGroup ( "BACKGROUNDSETTING" );
   config->writeEntry ( "BACKGROUNDTYPE", backgroundType );
   config->writePathEntry ( "BACKGROUNDPIXMAP", backgroundPixmap );
   config->writeEntry ( "BGPIXMAPSCALEMODE", bgPixmapScaleMode );
   config->writeEntry ( "BACKGROUNDCOLOR", backgroundColor );
   config->sync();
   config->setGroup ( oldGroup );
}

bool Board::createBackgroundImage() {
   if ( bgImage.load ( backgroundPixmap ) == FALSE ) {
      QMessageBox::information ( this, tr ( "Load pixmap failed" ), QString ( tr ( "Can't load pixmap:\n%1\nBackground leave unchanged." ) ).arg ( backgroundPixmap ), tr ( "&OK" ) );
      return FALSE;
   }

   QImage img ( width(), height(), 32 );
   switch ( bgPixmapScaleMode ) {
   case 0://Center
      bitBlt ( &img, ( width() - bgImage.width() ) / 2, ( height() - bgImage.height() ) / 2,
               &bgImage, 0, 0, -1, -1 );
      bgImage = img;
      break;
   case 1://Scale
      bgImage = bgImage.smoothScale ( width(), height() );
      break;
   case 2://Tile
   default:
      ;
   }
   return TRUE;
}

void Board::slotChangeBackground() {
   bool b = isPaused();
   if ( b == FALSE )
      pause();
   // BEGIN init change background dialog
   ChangeBackgroundUI *dlg;
   if ( !dlgChangeBackground ) {
      dlgChangeBackground = new ChangeBackgroundUI ( this );
      dlg = dlgChangeBackground;

      dlg->picURL->setCaption ( tr ( "Select a picture" ) );
      //dlg->picURL->setFilter("image/png image/jpeg image/x-bmp image/tiff");
      dlg->picURL->setFilter ( "Images (*.png *.xpm *.jpg *.jpeg *.tiff *.bmp)" );
      QFileDialog *fd = dlg->picURL->fileDialog();
      QHBox *hbox = new QHBox ( fd );
      Preview *preview = new Preview ( hbox );
      fd->setContentsPreviewEnabled ( true );
      fd->setContentsPreview ( hbox, preview );
      fd->setPreviewMode ( QFileDialog::Contents );
      fd->resize ( 660, 380 );
   }

   dlg = dlgChangeBackground;
   dlg->rbPixmap->setChecked ( ! ( bool ) backgroundType );
   dlg->picURL->setURL ( backgroundPixmap );
   dlg->cbScaleMode->setCurrentItem ( bgPixmapScaleMode );
   dlg->rbColor->setChecked ( ( bool ) backgroundType );
   dlg->colorButton->setColor ( backgroundColor );
   // END init change background dialog

   if ( dlg->exec() == QDialog::Accepted ) {
      int oldBackgroundType = backgroundType;
      backgroundType = ( dlg->rbPixmap->isChecked() ) ? 0 : 1;
      if ( backgroundType == 1 ) {
         backgroundColor = dlg->colorButton->color();
         setPaletteBackgroundColor ( backgroundColor );
         writeBackgroundSetting();
      } else {
         QString path = dlg->picURL->url();
         int tmpMode = dlg->cbScaleMode->currentItem();

         if ( path == backgroundPixmap && tmpMode == bgPixmapScaleMode && oldBackgroundType == backgroundType ) {
            if ( b == FALSE )
               pause();
            return;
         }

         if ( path.isEmpty() == TRUE ) {
            QMessageBox::information ( this, tr ( "Load pixmap failed" ), QString ( tr ( "Can't load pixmap:\nEmpty or illegal file name.\nBackground leave unchanged." ) ), tr ( "&OK" ) );
            if ( b == FALSE )
               pause();
            return;
         }

         int oldMode = bgPixmapScaleMode;
         QString oldBackgroundPixmap = backgroundPixmap;
         bgPixmapScaleMode = tmpMode;
         backgroundPixmap = path;
         if ( createBackgroundImage() ) {
            setPaletteBackgroundPixmap ( QPixmap ( bgImage ) );
            writeBackgroundSetting();
         } else {
            bgPixmapScaleMode = oldMode;
            backgroundPixmap = oldBackgroundPixmap;
         }
      }
   }
   if ( b == FALSE )
      pause();
}

void Board::slotUpdateTime() {
   remainTime--;
   update ( timeRect );

   if ( showingBonus > 0 )
      showingBonus--;
   else
      update ( bonusRect );

   if ( remainTime == 0 ) {
      timer->stop();
      finished = TRUE;
      update();

      re.score = score;
      re.level = level;
      re.difficulty = difficulty;

      level = -1;
      hintChances = 0;
      riffleChances = 0;

      playSound ( SOUND_GAMEOVER );
      QMessageBox::information ( this, tr ( "End of Game" ), tr ( "Time Out!\nGame Over!" ), tr ( "&OK" ) );
      emit endOfGame();
   }
}

void Board::slotChangeDirection() {
   bool ok;
   int i;
   i = QInputDialog::getInteger ( tr ( "Set Direction" ), tr ( "Type Number 0~10:" ), ( int ) direct, 0, 10, 1, &ok, this, "change_direction" );
   if ( ok && i >= 0 && i <= 10 )
      direct = ( Direction ) i;
}

void Board::slotLevelUp() {
   if ( level < 0 ) {
      finished = FALSE;
      level = 0;
      switch ( difficulty ) {
      case 1:
         riffleChances = 3;
         hintChances = 6;
         totalTime = remainTime = TOTALTIME + 40;
         break;
      case 2:
         riffleChances = 4;
         hintChances = 8;
         totalTime = remainTime = TOTALTIME;
         break;
      case 0:
      default:
         riffleChances = 2;
         hintChances = 4;
         totalTime = remainTime = TOTALTIME;
      }
      score = 0;
      scoreStep = 10;
      direct = BuBian;
      showingBonus = 0;
      tiles.changeBackColor();
      slotDistributeCards();
      timer->start ( 1000 );
      emit hintRifflePointsChanged();
   } else if ( level <= 9 ) {
      playSound ( SOUND_WIN );
      timer->stop();
      int bonusScore = ( riffleChances * 500 + hintChances * 200 + remainTime * 20 ) * ( level + 10 ) / 10;
      score += bonusScore;
      update ( scoreRect );
      QMessageBox::information ( this, tr ( "Level Up" ), QString ( tr ( "Congratulations!\nYou have finish level %1, now can upgrade to level %2.\nRemain riffle and hint points and time can gain bonus score %3." ) ).arg ( level + 1 ).arg ( level + 2 ).arg ( bonusScore ), tr ( "&OK" ) );
      level++;
      setRifflePoints ( riffleChances + 1 );
      setHintPoints ( hintChances + 1 );
      scoreStep = ( level + 1 ) * 10;
      remainTime = totalTime;
      direct = ( Direction ) level;
      showingBonus = 0;
      tiles.changeBackColor();
      slotDistributeCards();
      timer->start ( 1000 );
   } else {
      playSound ( SOUND_WIN );
      timer->stop();
      finished = TRUE;
      showingBonus = 0;
      update();
      int bonusScore = ( riffleChances * 500 + hintChances * 200 + remainTime * 20 ) * ( level + 10 ) / 10;
      score += bonusScore;
      update ( scoreRect );

      re.score = score;
      re.level = level;
      re.difficulty = difficulty;

      level = -1;
      hintChances = 0;
      riffleChances = 0;
      QMessageBox::information ( this, tr ( "You Win" ), tr ( "Congratulations! You have finish all levels!\nRemain riffle and hint points and time can gain bonus score %1." ).arg ( bonusScore ), tr ( "&OK" ) );
      emit hintRifflePointsChanged();
      emit endOfGame();
   }
}

void Board::slotDistributeCards() {
   int i, x, y, k;

   mark_x = -1;
   mark_y = -1;

   connection.clear();
   pageShot.clear();
   redoStack.clear();

   // distribute all tiles on board
   int cur_tile = 1;
   for ( x = 0; x < x_tiles(); x += 4 )
      for ( y = 0; y < y_tiles(); y++ ) {
         for ( k = 0; k < 4 && x + k < x_tiles(); k++ ) {
            setField ( x + k, y, cur_tile );
         }

         cur_tile++;
         if ( cur_tile > NTILES )
            cur_tile = 1;
      }

   // shuffle the field
   int tx = x_tiles();
   int ty = y_tiles();
   Path dummyPath;

   do {
      for ( i = 0; i < x_tiles() * y_tiles() * DEFAULTSHUFFLE; i++ ) {
         int x1 = ( int ) ( ( double ) tx * rand() / ( RAND_MAX + 1.0 ) );
         int y1 = ( int ) ( ( double ) ty * rand() / ( RAND_MAX + 1.0 ) );
         int x2 = ( int ) ( ( double ) tx * rand() / ( RAND_MAX + 1.0 ) );
         int y2 = ( int ) ( ( double ) ty * rand() / ( RAND_MAX + 1.0 ) );
         int t  = getField ( x1, y1 );
         setField ( x1, y1, getField ( x2, y2 ) );
         setField ( x2, y2, t );
      }
   } while ( !getHint_I ( dummyPath ) );

   update();
   emit changed();
}

void Board::slotRiffle() {
   if ( riffleChances <= 0 )
      return;

   if ( moveFinished == FALSE )
      undrawConnection();

   Path dummyPath;

   Position *pptr = new Position[tilesLeft() ];
   int count = 0;
   for ( int i = 0;i < x_tiles();i++ )
      for ( int j = 0;j < y_tiles();j++ ) {
         if ( getField ( i, j ) != EMPTY ) {
            pptr[count].x = i;
            pptr[count].y = j;
            ++count;
         }
      }

   do {
      for ( int i = 0; i < count; i++ ) {
         int a = ( int ) ( ( double ) ( tilesLeft() ) * rand() / ( RAND_MAX + 1.0 ) );
         int b = ( int ) ( ( double ) ( tilesLeft() ) * rand() / ( RAND_MAX + 1.0 ) );

         int f = getField ( pptr[a].x, pptr[a].y );
         setField ( pptr[a].x, pptr[a].y, getField ( pptr[b].x, pptr[b].y ) );
         setField ( pptr[b].x, pptr[b].y, f );
      }
   } while ( !getHint_I ( dummyPath ) );

   update();
   setRifflePoints ( riffleChances - 1 );
   delete [] pptr;
   pptr = 0;
   playSound ( SOUND_SHUFFLE );
}

void Board::checkIfEnd() {
   Path dummyPath;

   // game is over?
   if ( !getHint_I ( dummyPath ) ) {
      if ( riffleChances == 0 || tilesLeft() == 0 ) {
         if ( tilesLeft() > 0 ) {
            timer->stop();
            finished = TRUE;
            update();

            re.score = score;
            re.level = level;
            re.difficulty = difficulty;

            level = -1;
            hintChances = 0;
            riffleChances = 0;

            playSound ( SOUND_GAMEOVER );
            QMessageBox::information ( this, tr ( "End of Game" ), tr ( "No more moves possible!\nGame Over!" ), tr ( "&OK" ) );
            emit hintRifflePointsChanged();
            emit endOfGame();
         } else
            slotLevelUp();
      } else {
         slotRiffle();
      }
   }
}

void Board::randomBonus() {
   long i = ( long ) ( 100.0 * rand() / ( RAND_MAX + 1.0 ) );
   if ( i > 5 )
      return;

   showingBonus = 3;
   i = ( long ) ( 100.0 * rand() / ( RAND_MAX + 1.0 ) );
   if ( i > 80 ) {
      if ( i > 95 ) {
         setRifflePoints ( riffleChances + 2 );
         bonusStr = tr ( "Great lucky! Add 2 riffle points." );
         playSound ( SOUND_AWARD );
      } else if ( i > 85 ) {
         setRifflePoints ( riffleChances + 1 );
         bonusStr = tr ( "Lucky! Add 1 riffle point." );
         playSound ( SOUND_AWARD );
      } else if ( riffleChances > 0 ) {
         setRifflePoints ( riffleChances - 1 );
         bonusStr = tr ( "Too bad! Lose 1 riffle point." );
         playSound ( SOUND_BOMB );
      }
      update ( riffleRect );
   } else if ( i > 40 ) {
      if ( i > 75 ) {
         setHintPoints ( hintChances + 3 );
         bonusStr = tr ( "Great lucky! Add 3 hint points." );
         playSound ( SOUND_AWARD );
      } else if ( i > 65 ) {
         setHintPoints ( hintChances + 2 );
         bonusStr = tr ( "Lucky! Add 2 hint points." );
         playSound ( SOUND_AWARD );
      } else if ( i > 50 ) {
         setHintPoints ( hintChances + 1 );
         bonusStr = tr ( "Not bad! Add 1 hint point." );
         playSound ( SOUND_AWARD );
      } else if ( hintChances > 0 ) {
         setHintPoints ( hintChances - 1 );
         bonusStr = tr ( "Hapless! Lose 1 hint point." );
         playSound ( SOUND_BOMB );
      }
      update ( hintRect );
   } else if ( i > 10 ) {
      if ( i > 25 && remainTime < TOTALTIME - 10 ) {
         remainTime += 7;
         bonusStr = tr ( "Not bad! Bonus time 7 seconds." );
         playSound ( SOUND_AWARD );
      } else if ( remainTime > 10 ) {
         remainTime -= 7;
         bonusStr = tr ( "Hapless! Lose time 7 seconds." );
         playSound ( SOUND_BOMB );
      }
      update ( timeRect );
   } else {
      score += 2500;
      bonusStr = tr ( "Lucky! Bonus score 2500." );
      update ( scoreRect );
      playSound ( SOUND_AWARD );
   }

   update ( bonusRect );
}

void Board::setHintPoints ( int i ) {
   hintChances = i;
   emit hintRifflePointsChanged();
}

void Board::setRifflePoints ( int i ) {
   riffleChances = i;
   emit hintRifflePointsChanged();
}

void Board::slotSave() {
   QString filename;
#ifndef _WIN32
   char *homedir = getenv ( "HOME" );
   if (homedir != NULL) {
      filename += homedir;
      filename += "/";
   }
#endif
   filename += ".llk_save";
   QFile file ( filename );
   if ( file.open ( IO_WriteOnly ) == FALSE ) {
      QMessageBox::warning ( this, QString::null, tr ( "Write save file failure!" ), tr ( "&OK" ) );
      return;
   }

   QDataStream ds ( &file );
   ds << difficulty << level << riffleChances << hintChances << remainTime << score;

   QByteArray tmp ( y_tiles() * x_tiles() );
   for ( int i = 0;i < y_tiles();i++ )
      for ( int j = 0;j < x_tiles();j++ )
         tmp[i*x_tiles() +j] = field[i*x_tiles() +j];
   pageShot.push ( tmp );

   ds << pageShot;
   ds << redoStack;
   file.flush();
   file.close();

   emit saveFileStatusChanged ( TRUE );
}

void Board::slotLoad() {
   if ( finished == FALSE ) {
      int result = QMessageBox::warning ( this, QString::null, tr ( "Current game status will lose if you load a saved game .\nAre you sure to coninue?" ), tr ( "&OK" ), tr ( "&Cancel" ) );
      if ( result != 0 ) {
         return;
      }
   }
   QString filename;
#ifndef _WIN32
   char *homedir = getenv ( "HOME" );
   if (homedir != NULL) {
      filename += homedir;
      filename += "/";
   }
#endif
   filename += ".llk_save";
   QFile file ( filename );
   if ( file.exists() == FALSE ) {
      QMessageBox::warning ( this, QString::null, tr ( "Save file doesn't exist!" ), tr ( "&OK" ) );
      emit saveFileStatusChanged ( FALSE );
      return;
   }
   if ( file.open ( IO_ReadOnly ) == FALSE ) {
      QMessageBox::warning ( this, QString::null, tr ( "Open save file failure!" ), tr ( "&OK" ) );
      emit saveFileStatusChanged ( FALSE );
      return;
   }

   QDataStream ds ( &file );
   int difficulty, level, riffleChances, hintChances, remainTime, score;
   ds >> difficulty >> level >> riffleChances >> hintChances >> remainTime >> score;

   bool dataCrash = FALSE;

   if ( difficulty < 0 || difficulty > 2 ) {
      difficulty = 2;
      dataCrash = TRUE;
   }
   if ( level < 0 || level > 10 ) {
      level = 0;
      dataCrash = TRUE;
   }
   if ( remainTime <= 0 ) {
      remainTime = totalTime;
      dataCrash = TRUE;
   }
   if ( score < 0 ) {
      score = 0;
      dataCrash = TRUE;
   }

   if ( dataCrash == TRUE ) {
      int result = QMessageBox::warning ( this, tr ( "Read Save File Error!" ), tr ( "Some data read incorrectly,the save file maybe cracked.\nThese data have been replaced by Default value." ), tr ( "&OK" ), tr ( "&Cancel" ) );
      if ( result != 0 )
         return;
   }

   if ( difficulty == 0 ) {
      _x_tiles = 12, _y_tiles = 7;
      totalTime = TOTALTIME;
   } else if ( difficulty == 1 ) {
      _x_tiles = 14, _y_tiles = 8;
      totalTime = TOTALTIME;
   } else {
      difficulty = 2;
      _x_tiles = 16, _y_tiles = 9;
      totalTime = TOTALTIME + 40;
   }

   if ( this->difficulty != difficulty ) {
      if ( field )
         delete [] field;
      field = new int[_x_tiles * _y_tiles];
   }

   this->difficulty = difficulty;

   this->level = level;
   this->riffleChances = riffleChances;
   this->hintChances = hintChances;
   this->remainTime = remainTime;
   this->score = score;

   direct = ( Direction ) level;
   scoreStep = ( level + 1 ) * 10;

   pageShot.clear();
   redoStack.clear();
   ds >> pageShot;
   QByteArray tmp = pageShot.pop();
   for ( int i = 0; i < y_tiles(); i++ )
      for ( int j = 0; j < x_tiles(); j++ )
         field[i * x_tiles() + j] = tmp[i * x_tiles() + j];

   ds >> redoStack;
   finished = FALSE;

   if ( paused == FALSE )
      timer->start ( 1000 );
   update();
   emit changed();
   emit saveFileStatusChanged ( TRUE );

   file.close();
}

QConfig* Board::getConfig() {
   if ( config == 0 ) {
#if defined (__APPLE__)
      QString configPath = getenv ( "HOME" );
      config = new QConfig ( configPath + "/Library/Preferences/llkrc" );
#elif defined (_WIN32)
      config = new QConfig ( "llkrc" );
#else
      QString configPath = getenv ( "HOME" );
      config = new QConfig ( configPath + "/.kde/share/config/llkrc" );
#endif
   }

   return config;
}

void Board::loadSound() {
   if ( SOUND_OpenAudio ( 22050, 1, 1024 ) == -1 ) {
      no_sound = true;
      return;
   }
   no_sound = false;

   QString sound_file[NUM_SOUND] = {
      QString ( LLK_INSTALL_DIR ) + "/wav/cancel.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/click.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/gameover.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/hint.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/link.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/nolink.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/shuffle.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/win.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/award.wav",
      QString ( LLK_INSTALL_DIR ) + "/wav/bomb.wav"
   };

   for ( int i = 0; i < NUM_SOUND; i++ ) {
      m_snd[i] = SOUND_LoadWAV ( sound_file[i].ascii() );
   }
}

void Board::freeSound() {
   if ( no_sound ) {
      return;
   }
   for ( int i = 0; i < NUM_SOUND; i++ ) {
      SOUND_FreeWAV ( m_snd[i] );
   }
   SOUND_CloseAudio ();
}

void Board::playSound ( int num ) {
   if ( no_sound || m_snd[num] == NULL ) {
      return;
   }
   SOUND_PlayWAV ( m_snd[num] );
}
