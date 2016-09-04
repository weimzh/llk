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

#ifndef _HIGHSCORE_H_
#define _HIGHSCORE_H_

#include <qvaluelist.h>
#include <qdatastream.h>

#include "highscorebase.h"

#define HIGHSCORE_MAX 10

class Record {
public:
   Record() {
      rank = 100, score = 0, name = "unnamed", level = 0, difficulty = 0;
   }
   friend QDataStream & operator << ( QDataStream & ds, const Record & re ) {
      ds << re.rank << re.score << re.name << re.level << re.difficulty;
      return ds;
   }
   friend QDataStream & operator >> ( QDataStream & ds, Record & re ) {
      ds >> re.rank >> re.score >> re.name >> re.level >> re.difficulty;
      return ds;
   }
//  bool operator== (Record & re)
//  {
//   return (re.rank==rank && re.score==score && re.level==level && re.difficulty==difficulty && re.name==name);
//  }

public:
   int rank;
   int score;
   QString name;
   int level;
   int difficulty;
};

class HighScore: public HighScoreBase {
public:
   HighScore ( QWidget *parent = 0, const char *name = 0, bool modal = TRUE, WFlags f = 0 );
   ~HighScore();
   void readRecord();
   void writeRecord();
   void refresh ( int difficulty );
   bool isHighScore ( Record );
   int insertRecord ( Record );

public:
   QValueList<Record> easy;
   QValueList<Record> normal;
   QValueList<Record> hard;

};

#endif //_HIGHSCORE_H_
