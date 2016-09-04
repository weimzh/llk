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

#include <qstring.h>
#include <qtooltip.h>
#include <qaccel.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qiconset.h>
#include <qlineedit.h>
#include <qdragobject.h>
#include <qapplication.h>

#include "qurlrequester.h"
#include "general.h"

const int mSpacingSize = 6;

class QURLDragPushButton : public QPushButton {
public:
   QURLDragPushButton ( QWidget *parent, const char *name = 0 )
         : QPushButton ( parent, name ) {
      setDragEnabled ( true );
   }
   ~QURLDragPushButton() {}

   void setDragEnabled ( bool enable ) {
      m_dragEnabled = enable;
   }

   void setURL ( const QUrl& url ) {
      m_urls.clear();
      m_urls.append ( url );
   }

protected:
   void mousePressEvent ( QMouseEvent *e ) {
      if ( m_dragEnabled )
         startPos = e->pos();
      QPushButton::mousePressEvent ( e );
   }

   void mouseMoveEvent ( QMouseEvent *e ) {
      if ( !m_dragEnabled ) {
         QPushButton::mouseMoveEvent ( e );
         return;
      }

      if ( ( e->state() & LeftButton ) &&
            ( e->pos() - startPos ).manhattanLength() > QApplication::startDragDistance() ) {
         QDragObject * d = dragObject();
         if ( d )
            d->dragCopy();
         setDown ( false );
      }
   }

   virtual QDragObject *dragObject() {
      if ( m_urls.isEmpty() )
         return 0L;

      QUriDrag *ud = new QUriDrag ( 0, this, "url drag" );
      ud->setFileNames ( m_urls );
      QDragObject *drag = ud;
      return drag;
   }

private:
   bool m_dragEnabled;
   QPoint startPos;
   QStringList m_urls;
};

class QURLRequester::QURLRequesterPrivate {
public:
   QURLRequesterPrivate() {
      edit = 0L;
      combo = 0L;
      fileDialogMode = QFileDialog::ExistingFile;
   }

   void setText ( const QString& text ) {
      if ( combo ) {
         if ( combo->editable() ) {
            combo->setEditText ( text );
         } else {
            combo->insertItem ( text );
            combo->setCurrentItem ( combo->count() - 1 );
         }
      } else {
         edit->setText ( text );
      }
   }

   void connectSignals ( QObject *receiver ) {
      QObject * sender;
      if ( combo )
         sender = combo;
      else
         sender = edit;

      connect ( sender, SIGNAL ( textChanged ( const QString& ) ),
                receiver, SIGNAL ( textChanged ( const QString& ) ) );
      connect ( sender, SIGNAL ( returnPressed() ),
                receiver, SIGNAL ( returnPressed() ) );
   }

   /**
    * replaces ~user or $FOO, if necessary
    */
   QString url() {
      QString txt = combo ? combo->currentText() : edit->text();
      return txt;
   }

   QLineEdit *edit;
   QComboBox *combo;
   int fileDialogMode;
   QString fileDialogFilter;
};

QURLRequester::QURLRequester ( QWidget *editWidget, QWidget *parent, const char *name )
      : QHBox ( parent, name ) {
   d = new QURLRequesterPrivate;

   // must have this as parent
   editWidget->reparent ( this, 0, QPoint ( 0, 0 ) );
   d->edit = dynamic_cast<QLineEdit*> ( editWidget );
   d->combo = dynamic_cast<QComboBox*> ( editWidget );

   init();
}

QURLRequester::QURLRequester ( QWidget *parent, const char *name )
      : QHBox ( parent, name ) {
   d = new QURLRequesterPrivate;
   init();
}

QURLRequester::QURLRequester ( const QString& url, QWidget *parent, const char *name )
      : QHBox ( parent, name ) {
   d = new QURLRequesterPrivate;
   init();
   setURL ( url );
}

QURLRequester::~QURLRequester() {
   delete myFileDialog;
   delete d;
}

void QURLRequester::init() {
   myFileDialog    = 0L;
   myShowLocalProt = false;

   if ( !d->combo && !d->edit )
      d->edit = new QLineEdit ( this, "line edit" );

   myButton = new QURLDragPushButton ( this, "qfile button" );

   QPixmap tmppix ( QString( LLK_INSTALL_DIR ) + "/icons/16x16/fileopen.png" );

   if ( !tmppix.isNull() ) {
      QIconSet iconSet ( tmppix );
      QPixmap pixMap = iconSet.pixmap ( QIconSet::Small, QIconSet::Normal );
      myButton->setIconSet ( iconSet );
      myButton->setFixedSize ( pixMap.width() + 8, pixMap.height() + 8 );
   } else {
      myButton->setText ( tr ( "select file" ) );
      myButton->setFixedSize ( 24, 24 );
   }

   QToolTip::add ( myButton, tr ( "Open file dialog" ) );

   connect ( myButton, SIGNAL ( pressed() ), SLOT ( slotUpdateURL() ) );

   setSpacing ( mSpacingSize );

   QWidget *widget = d->combo ? ( QWidget* ) d->combo : ( QWidget* ) d->edit;
   setFocusProxy ( widget );

   d->connectSignals ( this );
   connect ( myButton, SIGNAL ( clicked() ), this, SLOT ( slotOpenDialog() ) );

   QAccel *accel = new QAccel ( this );
   accel->connectItem ( accel->insertItem ( Key_O + CTRL ), this, SLOT ( slotOpenDialog() ) );
}


void QURLRequester::setURL ( const QString& url ) {
   bool hasLocalPrefix = ( url.startsWith ( "file:" ) );

   if ( !myShowLocalProt && hasLocalPrefix )
      d->setText ( url.mid ( 5, url.length() - 5 ) );
   else
      d->setText ( url );
}

void QURLRequester::setCaption ( const QString& caption ) {
   QWidget::setCaption ( caption );
   if ( myFileDialog )
      myFileDialog->setCaption ( caption );
}

QString QURLRequester::url() const {
   return d->url();
}

void QURLRequester::slotOpenDialog() {
   emit openFileDialog ( this );

   QFileDialog *dlg = fileDialog();
   if ( !d->url().isEmpty() ) {
      // If we won't be able to list it (e.g. http), then don't try :)
      if ( QFile::exists ( d->url() ) )
         dlg->setSelection ( d->url() );
   }

   if ( dlg->exec() == QDialog::Accepted ) {
      // Only support locale file
      setURL ( dlg->selectedFile() );
      emit urlSelected ( d->url() );
   }
}

void QURLRequester::setMode ( unsigned int mode ) {
   Q_ASSERT ( ( mode & QFileDialog::ExistingFiles ) == 0 );
   d->fileDialogMode = mode;

   if ( myFileDialog )
      myFileDialog->setMode ( static_cast<QFileDialog::Mode> ( d->fileDialogMode ) );
}

void QURLRequester::setFilter ( const QString &filter ) {
   d->fileDialogFilter = filter;
   if ( myFileDialog )
      myFileDialog->setFilter ( d->fileDialogFilter );
}

QFileDialog *QURLRequester::fileDialog() const {
   if ( !myFileDialog ) {
      QWidget * p = parentWidget();
      myFileDialog = new QFileDialog ( QString::null, QString::null, p, "file dialog", true );

      myFileDialog->setMode ( static_cast<QFileDialog::Mode> ( d->fileDialogMode ) );
      myFileDialog->setFilter ( d->fileDialogFilter );
      myFileDialog->setCaption ( caption() );
   }

   return myFileDialog;
}

void QURLRequester::setShowLocalProtocol ( bool b ) {
   if ( myShowLocalProt == b )
      return;

   myShowLocalProt = b;
   setURL ( url() );
}

void QURLRequester::clear() {
   d->setText ( QString::null );
}

QLineEdit *QURLRequester::lineEdit() const {
   return d->edit;
}

QComboBox *QURLRequester::comboBox() const {
   return d->combo;
}

void QURLRequester::slotUpdateURL() {
   // bin compat, myButton is declared as QPushButton
   QUrl u ( QDir::currentDirPath() + '/', url() );
   ( static_cast<QURLDragPushButton *> ( myButton ) ) ->setURL ( u );
}

QPushButton *QURLRequester::button() const {
   return myButton;
}

void QURLRequester::virtual_hook ( int, void * ) {
}

QURLComboRequester::QURLComboRequester ( QWidget *parent,
      const char *name )
      : QURLRequester ( new QComboBox ( false ), parent, name ) {
}
