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

#include <qpainter.h>
#include <qdrawutil.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qcolordialog.h>
#include <qdragobject.h>

#include "qcolorbutton.h"

class QColorButton::QColorButtonPrivate {
public:
   bool m_bdefaultColor;
   QColor m_defaultColor;
};

QColorButton::QColorButton ( QWidget *parent, const char *name )
      : QPushButton ( parent, name ) {
   d = new QColorButtonPrivate;
   d->m_bdefaultColor = false;
   d->m_defaultColor = QColor();
   setAcceptDrops ( true );

   // 2000-10-15 (putzer): fixes broken keyboard usage
   connect ( this, SIGNAL ( clicked() ), this, SLOT ( chooseColor() ) );
}

QColorButton::QColorButton ( const QColor &c, QWidget *parent, const char *name )
      : QPushButton ( parent, name ), col ( c ) {
   d = new QColorButtonPrivate;
   d->m_bdefaultColor = false;
   d->m_defaultColor = QColor();
   setAcceptDrops ( true );

   // 2000-10-15 (putzer): fixes broken keyboard usage
   connect ( this, SIGNAL ( clicked() ), this, SLOT ( chooseColor() ) );
}

QColorButton::QColorButton ( const QColor &c, const QColor &defaultColor, QWidget *parent, const char *name )
      : QPushButton ( parent, name ), col ( c ) {
   d = new QColorButtonPrivate;
   d->m_bdefaultColor = true;
   d->m_defaultColor = defaultColor;
   setAcceptDrops ( true );

   // 2000-10-15 (putzer): fixes broken keyboard usage
   connect ( this, SIGNAL ( clicked() ), this, SLOT ( chooseColor() ) );
}

QColorButton::~QColorButton() {
   delete d;
}

void QColorButton::setColor ( const QColor &c ) {
   if ( col != c ) {
      col = c;
      repaint ( false );
      emit changed ( col );
   }
}

void QColorButton::drawButtonLabel ( QPainter *painter ) {
   int x, y, w, h;
   QRect r = style().subRect ( QStyle::SR_PushButtonContents, this );
   r.rect ( &x, &y, &w, &h );

   int margin = style().pixelMetric ( QStyle::PM_ButtonMargin, this );
   x += margin;
   y += margin;
   w -= 2 * margin;
   h -= 2 * margin;

   if ( isOn() || isDown() ) {
      x += style().pixelMetric ( QStyle::PM_ButtonShiftHorizontal, this );
      y += style().pixelMetric ( QStyle::PM_ButtonShiftVertical, this );
   }

   QColor fillCol = isEnabled() ? col : backgroundColor();
   qDrawShadePanel ( painter, x, y, w, h, colorGroup(), true, 1, NULL );
   if ( fillCol.isValid() )
      painter->fillRect ( x + 1, y + 1, w - 2, h - 2, fillCol );

   if ( hasFocus() ) {
      QRect focusRect = style().subRect ( QStyle::SR_PushButtonFocusRect, this );
      style().drawPrimitive ( QStyle::PE_FocusRect, painter, focusRect, colorGroup() );
   }
}

QSize QColorButton::sizeHint() const {
   return style().sizeFromContents ( QStyle::CT_PushButton, this, QSize ( 40, 15 ) ).expandedTo ( QApplication::globalStrut() );
}

void QColorButton::dragEnterEvent ( QDragEnterEvent *event ) {
   event->accept ( QColorDrag::canDecode ( event ) && isEnabled() );
}

void QColorButton::dropEvent ( QDropEvent *event ) {
   QColor c;
   if ( QColorDrag::decode ( event, c ) ) {
      setColor ( c );
   }
}

void QColorButton::mousePressEvent ( QMouseEvent *e ) {
   mPos = e->pos();
   QPushButton::mousePressEvent ( e );
}

void QColorButton::mouseMoveEvent ( QMouseEvent *e ) {
   if ( ( e->state() & LeftButton ) &&
         ( e->pos() - mPos ).manhattanLength() > QApplication::startDragDistance() ) {
      // Drag color object
      QColorDrag * dg = new QColorDrag ( color(), this );
      dg->dragCopy();
      setDown ( false );
   }
}

void QColorButton::chooseColor() {
   QColor c = QColorDialog::getColor ( color(), this );
   if ( c.isValid() ) {
      setColor ( c );
   }
}

void QColorButton::virtual_hook ( int, void* ) {
}



