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

#include <qapplication.h>
#include <qmessagebox.h>
#include <qlocale.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qinputdialog.h>
#include <qiconset.h>
#include <qlistview.h>
#include <qaction.h>
#include <qstatusbar.h>
#include <qtooltip.h>
#include <qcheckbox.h>
#include <cmath>
#include <cstdlib>

#include "llk.h"
#include "aboutgui.h"

static int size_x[3] = {12, 14, 16};
static int size_y[3] = {7, 8, 9};

App::App() : mainWindows ( 0 ), config ( 0 ) {
   setFixedSize ( 800, 640 );
   setCaption ( tr ( "llk" ) );

   b = new Board ( this );
   setCentralWidget ( b );

   rule = 0;
   aboutGui = 0;

   options_background->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/colorize.png" ) ) );
   game_quit->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/exit.png" ) ) );
   game_save->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/filesave.png" ) ) );
   game_new->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/filenew.png" ) ) );
   game_load->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/revert.png" ) ) );
   game_pause->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/pause.png" ) ) );
   move_undo->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/undo.png" ) ) );
   move_redo->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/redo.png" ) ) );
   move_hint->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/wizard.png" ) ) );
   move_riffle->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/roll.png" ) ) );
   game_fame->setIconSet ( QIconSet ( LLK_INSTALL_DIR + QString ( "/icons/16x16/playlist.png" ) ) );

   connect ( b, SIGNAL ( changed() ), this, SLOT ( enableItems() ) );
   connect ( b, SIGNAL ( endOfGame() ), this, SLOT ( slotEndOfGame() ) );
   connect ( b, SIGNAL ( hintRifflePointsChanged() ), SLOT ( slotHintRifflePointsChanged() ) );
   connect ( b, SIGNAL ( saveFileStatusChanged ( bool ) ), game_load, SLOT ( setEnabled ( bool ) ) );
   connect ( move_riffle, SIGNAL ( activated() ), b, SLOT ( slotRiffle() ) );
   connect ( move_hint, SIGNAL ( activated() ), this, SLOT ( hint() ) );
   connect ( options_background, SIGNAL ( activated() ), b, SLOT ( slotChangeBackground() ) );
   connect ( options_toolbar, SIGNAL ( toggled ( bool ) ), this , SLOT ( showToolbar ( bool ) ) );
   connect ( AboutqtAction, SIGNAL ( activated() ), this, SLOT ( slotAboutQt() ) );
   connect ( AboutllkAction, SIGNAL ( activated() ), this, SLOT ( slotAboutLlk() ) );
   connect ( gameRuleAction, SIGNAL ( activated() ), this, SLOT ( slotActionGameRule() ) );

   options_levelAction->setToggleAction ( true );
   options_levelNormalAction->setToggleAction ( true );
   options_levelHardAction->setToggleAction ( true );

   // load default settings
   config = b->getConfig();
   config->setGroup ( QString::null );
   bool showRRDialog = config->readBoolEntry ( "ShowRRDialog", true );

   // check desktop geometry
   QDesktopWidget *d = QApplication::desktop();
   int w = d->width();     // returns desktop width
   int h = d->height();    // returns desktop height
   if ( w <= 800 || h <= 600 ) {
      if ( showRRDialog == true ) {
         b->pause();
         dialog = new DialogWithCheckBox ( this, tr ( "Resolution too low" ), tr ( "In order to play llk normally, resolution ratio greater than or equal 1024X768 is expected." ) );
         dialog->exec();
         bool bs = dialog->cbShowAgain->isChecked();
         config->writeEntry ( "ShowRRDialog", !bs );
         delete dialog;
         dialog = 0;
         b->pause();
      }
   }

   lastPlayerName = config->readEntry ( "PlayerName", QString::null );
   config->setGroup ( QString::null );
   bool showToolBar = config->readBoolEntry ( "ShowToolBar", true );

   if ( showToolBar == FALSE ) {
      options_toolbar->setOn ( false );
      toolBar->hide();
   } else {
      options_toolbar->setOn ( true );
      toolBar->show();
   }

   int i = config->readNumEntry ( "Level", 0 );
   QAction *tmp;
   if ( i == 0 )
      tmp = options_levelAction;
   else if ( i == 1 )
      tmp = options_levelNormalAction;
   else
      tmp = options_levelHardAction;

   tmp->toggle();
   changeLevel ( tmp ); // initiates new game
   enableItems();

   highscore = new HighScore ( this );

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
   if ( file.exists() == FALSE )
      game_load->setEnabled ( FALSE );
   else if ( file.open ( IO_ReadOnly ) == FALSE )
      game_load->setEnabled ( FALSE );
   QActionGroup *grp = new QActionGroup ( this );
   grp->add ( options_levelAction );
   grp->add ( options_levelNormalAction );
   grp->add ( options_levelHardAction );
   connect ( grp, SIGNAL ( selected ( QAction* ) ), this, SLOT ( changeLevel ( QAction* ) ) );
   grp->setExclusive ( TRUE );

   statusBar() ->hide();
}

App::~App() {
   config->sync();
   delete b;
   delete highscore;
   delete rule;
   delete aboutGui;
}

void App::hallOfFame() {
   bool t = b->isPaused();
   if ( t == FALSE )
      b->pause();

   showHighscore ( b->difficulty, -1 );

   if ( t == FALSE )
      b->pause();
}

void App::newGame() {
   game_pause->setEnabled ( TRUE );
   game_pause->setOn ( FALSE );

   game_save->setEnabled ( TRUE );
   move_hint->setEnabled ( TRUE );
   move_riffle->setEnabled ( TRUE );
   move_undo->setEnabled ( FALSE );
   move_redo->setEnabled ( FALSE );

   b->newGame();
   if ( b->isPaused() ) {
      b->pause();
      game_pause->setEnabled ( true );
   }
   game_save->setEnabled ( TRUE );

}

void App::quitGame() {
   delete this;
   qApp->quit();
}

void App::restartGame() {
   b->setUpdatesEnabled ( FALSE );
   while ( b->canUndo() )
      b->undo();
   b->setUpdatesEnabled ( TRUE );
   b->update();
   enableItems();
}

void App::pause() {
   bool paused = b->pause();
   lockMenus ( paused );
}

void App::undo() {
   if ( b->canUndo() ) {
      b->undo();
      enableItems();
   }
}

void App::redo() {
   if ( b->canRedo() )
      b->redo();
   enableItems();
}

void App::hint() {
#ifdef DEBUGGING
   b->makeHintMove();
#else
   b->showHint();
#endif
   enableItems();
}

void App::changeLevel ( QAction* /*ac*/ ) {
   int index = 0;
   if ( options_levelAction->isOn() ) {
      index = 0;
   }
   if ( options_levelNormalAction->isOn() ) {
      index = 1;
   }
   if ( options_levelHardAction->isOn() ) {
      index = 2;
   }
   config->writeEntry ( "Level", index );
   b->setSize ( size_x[index], size_y[index] );
   newGame();
}

void App::lockMenus ( bool /*lock*/ ) {
   bool tmp = game_load->isEnabled();
   options_background->setEnabled ( TRUE );
   game_load->setEnabled ( tmp );
   options_toolbar->setEnabled ( TRUE );
   options_level->setEnabled ( TRUE );

   enableItems();

}
void App::enableItems() {
   if ( !b->isPaused() ) {
      move_undo->setEnabled ( b->canUndo() );
      move_redo->setEnabled ( b->canRedo() );
      move_hint->setEnabled ( b->hintChances > 0 );
      move_riffle->setEnabled ( b->riffleChances > 0 );
   } else {
      move_undo->setEnabled ( false );
      move_redo->setEnabled ( false );
      move_hint->setEnabled ( false );
      move_riffle->setEnabled ( false );
   }
}

void App::slotEndOfGame() {
   game_save->setEnabled ( FALSE );
   game_pause->setEnabled ( FALSE );
   move_hint->setEnabled ( FALSE );
   move_riffle->setEnabled ( FALSE );
   move_undo->setEnabled ( FALSE );
   move_redo->setEnabled ( FALSE );

   // create highscore entry
   Record re = b->re;
   if ( re.score == 0 )
      return;

   // check if we made it into Top10
   bool isHighscore = highscore->isHighScore ( re );
   if ( isHighscore ) {
      bool b = getPlayerName();
      if ( b == FALSE )
         return;
      re.name = lastPlayerName;
      int i = highscore->insertRecord ( re );
      showHighscore ( re.difficulty, i );
   } else {
      QString s = tr ( "You final score is %1." ).arg ( re.score );
      QMessageBox::information ( this, s, tr ( "End of Game" ) );
   }
}

bool App::getPlayerName() {
   bool ok;
   QString str = QInputDialog::getText ( tr ( "Hall of Fame" ), tr ( "You've made it into the \"Hall Of Fame\".\nType in your name so mankind will always remember your cool rating." ), QLineEdit::Normal, lastPlayerName, &ok, this, "get_player_name" );

   if ( ok ) {
      if ( !str.isEmpty() ) {
         lastPlayerName = str;
      } else {
         if ( lastPlayerName.isEmpty() || lastPlayerName == "" )
            lastPlayerName = tr ( "Unnamed" );
      }
      config->writeEntry ( "PlayerName", lastPlayerName );
   }

   return ok;
}

void App::insertHighscore ( Record re ) {
   highscore->insertRecord ( re );
}

void App::readHighscore() {
   highscore->readRecord();
}

void App::writeHighscore() {
   highscore->writeRecord();
}

void App::showHighscore ( int difficulty, int rank ) {
   highscore->tabFame->setCurrentPage ( difficulty );

   QListView *lv;
   if ( difficulty == 0 )
      lv = highscore->lvEasy;
   else if ( difficulty == 1 )
      lv = highscore->lvNormal;
   else
      lv = highscore->lvHard;

   if ( rank > 0 && rank <= 10 ) {
      //yal
      lv->setSelected ( lv->findItem ( QString::number ( rank ), 0 ), TRUE );
   } else
      lv->clearSelection();

   highscore->exec();
}

void App::slotHintRifflePointsChanged() {
   if ( b->hintChances <= 0 )
      move_hint->setEnabled ( FALSE );
   else
      move_hint->setEnabled ( TRUE );

   if ( b->riffleChances <= 0 )
      move_riffle->setEnabled ( FALSE );
   else
      move_riffle->setEnabled ( TRUE );
}

void App::slotLoad() {
   b->slotLoad();
   game_save->setEnabled ( TRUE );
   game_pause->setEnabled ( TRUE );
}

void App::slotSave() {
   b->slotSave();
   game_load->setEnabled ( TRUE );
}

void App::showToolbar ( bool as ) {
   if ( !as ) {
      toolBar->hide();
   } else {
      toolBar->show();
   }
   config->setGroup ( QString::null );
   config->writeEntry ( "ShowToolBar", as );
}

void App::slotActionGameRule() {
   if ( !rule ) {
      rule = new gameRule ( this );
      rule->show();
   } else {
      rule->show();
   }
}

void App::slotAboutQt() {
   QMessageBox::aboutQt ( this, "Qt Application Example" );
}

void App::slotAboutLlk() {
   if ( !aboutGui ) {
      aboutGui = new AboutGUI ( this );
      aboutGui->setAboutInfo ( tr ( "llk" ), tr ( "0.1" ) );
      aboutGui->version_lbl->setText ( tr ( "Version %1" ).arg ( LLK_VERSION ) );
      aboutGui->show();
   } else {
      aboutGui->show();
   }
}
