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

#ifndef __BOARD__H__
#define __BOARD__H__

// Should this get the whole HAVE_SYS_TIME_H TIME_WITH_SYS_TIME treatment?
#include <time.h>

#include <qimage.h>
#include <qvaluestack.h>

//#include <krandomsequence.h>
#include <list>
#include "tileset.h"
#include "highscore.h"
#include "general.h"

#include "sound.h"

class ChangeBackgroundUI;
class QColor;
class QTimer;
class QRect;
//class QImage;
class QConfig;

struct Position {
   Position() : x ( 0 ), y ( 0 ) { }
   Position ( int _x, int _y ) : x ( _x ), y ( _y ) { }
   int x;
   int y;
};

typedef std::list<Position> Path;

enum
{
   SOUND_CANCEL = 0,
   SOUND_CLICK,
   SOUND_GAMEOVER,
   SOUND_HINT,
   SOUND_LINK,
   SOUND_NOLINK,
   SOUND_SHUFFLE,
   SOUND_WIN,
   SOUND_AWARD,
   SOUND_BOMB,

   NUM_SOUND,
};

class Move {
public:
   Move ( int _x1, int _y1, int _x2, int _y2, int _tile ) :
   x1 ( _x1 ), y1 ( _y1 ), x2 ( _x2 ), y2 ( _y2 ), tile ( _tile ) { }

   int x1, y1, x2, y2;
   int tile;
};

class Board : public QWidget {
   friend class App;
   Q_OBJECT
public:
   enum Direction{
      BuBian = 0,
      XiangXia,
      XiangZuo,
      ShangXiaFenLi,
      ZuoYouFenLi,
      ShangXiaJiZhong,
      ZuoYouJiZhong,
      ShangZuoXiaYou,
      ZuoXiaYouShang,
      XiangWaiKuoSan,
      XiangNeiJiZhong
   };

public:
   Board ( QWidget *parent = 0 );
   ~Board();

   virtual void paintEvent ( QPaintEvent* );
   virtual void mousePressEvent ( QMouseEvent* );
   virtual void resizeEvent ( QResizeEvent* );

   bool canUndo() const;
   bool canRedo() const;
   void redo();
   void undo();

   void setSize ( int x, int y );
   QSize minSize() const;
   void newGame();

   void showHint();
   bool getHint_I ( Path& p ) const;

   QConfig* getConfig();

#ifdef DEBUGGING
   void makeHintMove();
//public slots:
   void finish();
   void dumpBoard() const;
public:
#endif

   int tilesLeft() const;
   int x_tiles() const;
   int y_tiles() const;
   bool isPaused() const {
      return paused;
   }

signals:
   void markMatched();
   void changed();
   void endOfGame();
   void hintRifflePointsChanged();
   void saveFileStatusChanged ( bool );

public slots:
   bool pause();
   void slotChangeBackground();
   void slotUpdateTime();
   void slotRiffle();
   void slotChangeDirection();
   void slotSave();
   void slotLoad();

private slots:
   void undrawConnection ( bool needCheck = TRUE );
   void slotMoveTiles ( QPoint, bool );
   //second parameter indicate if row(TRUE) or column(FALSE) need check
   void slotLevelUp();
   void slotDistributeCards();

private: // functions
   void checkIfEnd();
   void setHintPoints ( int );
   void setRifflePoints ( int );

   int xOffset() const;
   int yOffset() const;

   void setField ( int x, int y, int value );
   int getField ( int x, int y ) const;
   void updateField ( int, int, bool erase = true );
   bool canMakePath ( int x1, int y1, int x2, int y2 ) const;
   bool findPath ( int x1, int y1, int x2, int y2, Path& p ) const;
   bool findSimplePath ( int x1, int y1, int x2, int y2, Path& p ) const;
   bool isTileHighlighted ( int x, int y ) const;
   void drawConnection ( int timeout );
   QPoint midCoord ( int x, int y ) const;
   void marked ( int x, int y );

   void readBackgroundSetting();
   void writeBackgroundSetting();
   bool createBackgroundImage();

   void moveTilesUp ( int, int, int );
   void moveTilesDown ( int, int, int );
   void moveTilesLeft ( int, int, int );
   void moveTilesRight ( int, int, int );

   void randomBonus();

private:

   TileSet tiles;

   QValueStack<QByteArray> pageShot;
   QValueStack<QByteArray> redoStack;

   int mark_x;
   int mark_y;
   Path connection;
   int *field;
   int _x_tiles;
   int _y_tiles;

   bool paused;
   bool finished;
   bool moveFinished;

   Direction direct;
   QValueStack<QPoint> needCheckRows;
   QValueStack<QPoint> needCheckCols;

   int score, scoreStep;
   int totalTime, remainTime;

   void *m_snd[NUM_SOUND];
   bool no_sound;

   void loadSound();
   void freeSound();
   void playSound ( int num );

public:
   ChangeBackgroundUI *dlgChangeBackground;
   int backgroundType; //0-pixmap,1-fill color
   QString backgroundPixmap;
   QImage bgImage;
   QPixmap timeBarImg;
   int bgPixmapScaleMode; //0-Center,1-Scale,2-Tile
   QColor backgroundColor;
   QConfig *config;
   QTimer *timer;
#ifndef DEBUGGING
   QTimer *drawConnectionTimer;
#endif
   QRect timeRect, scoreRect, riffleRect, hintRect;
   QRect bonusRect;
   QString bonusStr;
   int showingBonus;

   int riffleChances;
   int hintChances;
   int difficulty, level;
   Record re;
};

#endif
