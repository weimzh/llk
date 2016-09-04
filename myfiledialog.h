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

#ifndef _MYFILEDIALOG_H_
#define _MYFILEDIALOG_H_

#include <qlabel.h>
#include <qfiledialog.h>
#include <qimage.h>

class Preview: public QLabel, public QFilePreview {
   Q_OBJECT
public:
   Preview ( QWidget *parent = 0 );

   void previewUrl ( const QUrl &u );

protected:
   void resizeEvent ( QResizeEvent *e );

private:
   void dealWithPixmap();

private:
   QImage m_image;
   QImage m_resizedImage;
};

class MyFileDialog: public QFileDialog {
   Q_OBJECT
public:
   MyFileDialog ( const QString &dirName, const QString &filter = QString::null, QWidget *parent = 0, const char *name = 0, bool modal = false );

   MyFileDialog ( QWidget *parent = 0, const char *name = 0, bool modal = false );

   ~MyFileDialog();

private:
   void init();

private:
   //QFrame *frame;
   //Preview *preview;
};

#endif //_MYFILEDIALOG_H_
