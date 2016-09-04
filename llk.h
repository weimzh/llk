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

#ifndef _LLK_H_
#define _LLK_H_

// Should this get the whole HAVE_SYS_TIME_H TIME_WITH_SYS_TIME treatment?
#include <time.h>

#include "mainApp.h"
#include "board.h"
#include "highscore.h"
#include "qconfig.h"
#include <qaction.h>
#include "llkrule.h"
#include "dialogwithcheckbox.h"

class AboutGUI;

class App : public mainWindows {
   Q_OBJECT
public:
   App();
   ~App();


private slots:
   virtual void slotHintRifflePointsChanged();
   virtual void slotEndOfGame();
   virtual void enableItems();

   virtual void newGame();
   virtual void quitGame();
   virtual void restartGame();
   virtual void pause();
   virtual void undo();
   virtual void redo();
   virtual void hint();
   void changeLevel ( QAction* );
   virtual void hallOfFame();
   void showToolbar ( bool as );
   virtual void slotLoad();
   virtual void slotSave();
   void slotAboutQt();
   void slotAboutLlk();
   void slotActionGameRule();

private:
   void lockMenus ( bool );
   bool getPlayerName();

   void readHighscore();
   void writeHighscore();
   void insertHighscore ( Record );
   void showHighscore ( int difficulty, int rank );


private:
   QConfig *config;
   QString lastPlayerName;
   Board *b;
   HighScore *highscore;
   gameRule *rule;
   AboutGUI *aboutGui;
   DialogWithCheckBox *dialog;
};

#endif // _LLK_H_
