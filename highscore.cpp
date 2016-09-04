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

#include <qfile.h>
#include <qdatastream.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <cstdlib>

#include "highscore.h"
#include "general.h"

HighScore::HighScore ( QWidget *parent, const char *name, bool modal, WFlags f )
      : HighScoreBase ( parent, name, modal, f ) {
   readRecord();

   for ( int i = 0;i < 3;i++ )
      refresh ( i );

}

HighScore::~HighScore() {
   writeRecord();
   easy.clear();
   normal.clear();
   hard.clear();
}

void HighScore::readRecord() {
   QString filename;
#ifndef _WIN32
   char *homedir = getenv ( "HOME" );
   if ( homedir ) {
      filename += homedir;
      filename += "/";
   }
#endif
   filename += ".llk_record";
   QFile file ( filename );
   if ( file.exists() == FALSE )
      return;
   if ( file.open ( IO_ReadOnly ) == FALSE ) {
      QMessageBox::warning ( this, QString::null, QObject::tr ( "Read high score record failure!" ), QObject::tr ( "&OK" ) );
      return;
   }

   QDataStream ds ( &file );
   ds >> easy;
   ds >> normal;
   ds >> hard;

   file.close();
}

void HighScore::writeRecord() {
   QString filename;
#ifndef _WIN32
   char *homedir = getenv ( "HOME" );
   if ( homedir ) {
      filename += homedir;
      filename += "/";
   }
#endif
   filename += ".llk_record";
   QFile file ( filename );
   if ( file.open ( IO_WriteOnly ) == FALSE ) {
      QMessageBox::warning ( this, QString::null, QObject::tr ( "Write high score record failure!" ), QObject::tr ( "&OK" ) );
      return;
   }

   QDataStream ds ( &file );
   ds << easy << normal << hard;

   file.flush();
   file.close();
}

void HighScore::refresh ( int difficulty ) {
   QListView * lv;
   QValueList<Record> vl;
   if ( difficulty == 0 ) {
      lv = lvEasy;
      vl = easy;
   } else if ( difficulty == 1 ) {
      lv = lvNormal;
      vl = normal;
   } else {
      lv = lvHard;
      vl = hard;
   }

   lv->clear();
   if ( vl.isEmpty() )
      return;

   QValueList<Record>::iterator it = vl.begin();
   QListViewItem *lvItem = 0;
   for ( ;it != vl.end();++it ) {
      Record re = ( *it );
      if ( lvItem == 0 )
         lvItem = new QListViewItem ( lv, QString::number ( re.rank ), QString::number ( re.score ), re.name, QString::number ( re.level ) );
      else
         lvItem = new QListViewItem ( lv, lvItem, QString::number ( re.rank ), QString::number ( re.score ), re.name, QString::number ( re.level ) );
   }
}

bool HighScore::isHighScore ( Record re ) {
   QValueList<Record> vl;
   if ( re.difficulty == 0 )
      vl = easy;
   else if ( re.difficulty == 1 )
      vl = normal;
   else
      vl = hard;

   if ( vl.count() < HIGHSCORE_MAX )
      return TRUE;
   else {
      Record tmp = vl.last();
      if ( tmp.score < re.score )
         return TRUE;
   }

   return FALSE;
}

int HighScore::insertRecord ( Record re ) {
   QValueList<Record> vl;
   if ( re.difficulty == 0 )
      vl = easy;
   else if ( re.difficulty == 1 )
      vl = normal;
   else
      vl = hard;

   if ( vl.isEmpty() == TRUE ) {
      re.rank = 1;
      vl.append ( re );
   } else {
      QValueList<Record>::iterator it = vl.begin();
      QValueList<Record>::iterator it1 = vl.begin();
      int i = 1;
      bool inserted = FALSE;
      for ( ;it != vl.end();++it, ++i ) {
         if ( inserted == FALSE ) {
            if ( ( *it ).score < re.score ) {
               ( *it ).rank++;
               re.rank = i;
               vl.insert ( it, re );
               inserted = TRUE;
            }
         } else {
            ( *it ).rank++;
         }
      }
      if ( !inserted && i <= 10 ) {
         re.rank = i;
         vl.append ( re );
      }
      while ( vl.count() > HIGHSCORE_MAX ) {
         vl.pop_back();
      }
   }

   if ( re.difficulty == 0 )
      easy = vl;
   else if ( re.difficulty == 1 )
      normal = vl;
   else
      hard = vl;

   refresh ( re.difficulty );
   writeRecord();

   return re.rank;
}
