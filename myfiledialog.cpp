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

#include <qpixmap.h>
#include <qlayout.h>

#include "myfiledialog.h"

Preview::Preview ( QWidget *parent )
      : QLabel ( parent ) {
   setEraseColor ( white );
   setFrameStyle ( QFrame::Panel | QFrame::Sunken );
   setMinimumSize ( 320, 240 );
   setAlignment ( Qt::AlignCenter );
}

void Preview::previewUrl ( const QUrl &u ) {
   QString path = u.path();
   m_image.load ( path );
   dealWithPixmap();
}

void Preview::resizeEvent ( QResizeEvent *e ) {
   int w1 = e->oldSize().width(), h1 = e->oldSize().height(), w2 = e->size().width(), h2 = e->size().height();
   if ( ( m_resizedImage.size() == m_image.size() && m_image.width() <= w2 && m_image.height() <= h2 )
         || ( w2 > w1 && h2 == m_resizedImage.height() )
         || ( h2 > h1 && w2 == m_resizedImage.width() ) )
      return;
   dealWithPixmap();
}

void Preview::dealWithPixmap() {
   if ( m_image.isNull() )
      setText ( tr ( "This is not a pixmap" ) );
   else {
      if ( m_image.width() > width() || m_image.height() > height() )
         m_resizedImage = m_image.smoothScale ( rect().size(), QImage::ScaleMin );
      else
         m_resizedImage = m_image;

      setPixmap ( m_resizedImage );
   }
}



MyFileDialog::MyFileDialog ( const QString &dirName, const QString &filter, QWidget *parent, const char *name, bool modal )
      : QFileDialog ( dirName, filter, parent, name, modal ) {
   init();
}

MyFileDialog::MyFileDialog ( QWidget *parent, const char *name, bool modal )
      : QFileDialog ( parent, name, modal ) {
   init();
}

MyFileDialog::~MyFileDialog() {
   /* if(preview)
     delete preview;
    preview=0;
    
    if(frame)
     delete frame;
    frame=0;*/
}

void MyFileDialog::init() {
   setMode ( QFileDialog::ExistingFile );

   QFrame *frame = new QFrame ( this );
   QHBoxLayout *hbox = new QHBoxLayout ( frame );
   Preview *preview = new Preview ( frame );
   hbox->addWidget ( preview );

   setContentsPreviewEnabled ( true );
   setContentsPreview ( frame, preview );
   setPreviewMode ( QFileDialog::Contents );
}

