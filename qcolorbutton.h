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

#ifndef __QCOLORBUTTON_H__
#define __QCOLORBUTTON_H__

#include <qpushbutton.h>

class QColorButtonPrivate;
/**
* This widget can be used to display or allow user selection of a color.
* Just like KColorButton
*/
class QColorButton : public QPushButton {
   Q_OBJECT
   Q_PROPERTY ( QColor color READ color WRITE setColor )

public:
   /**
   * Creates a color button.
   */
   QColorButton ( QWidget *parent, const char *name = 0L );
   /**
   * Creates a color button with an initial color @p c.
   */
   QColorButton ( const QColor &c, QWidget *parent, const char *name = 0L );
   QColorButton ( const QColor &c, const QColor &defaultColor, QWidget *parent, const char *name = 0L );

   /**
   * Destructor.
   */
   virtual ~QColorButton();

   /**
   * Returns the currently chosen color.
   */
   QColor color() const {
      return col;
   }
   /**
   * Sets the current color to @p c.
   */
   void setColor ( const QColor &c );

   QSize sizeHint() const;

signals:
   /**
   * Emitted when the color of the widget
   * is changed, either with setColor() or via user selection.
   */
   void changed ( const QColor &newColor );

protected slots:
   void chooseColor();

protected:
   virtual void drawButtonLabel ( QPainter *p );
   virtual void dragEnterEvent ( QDragEnterEvent * );
   virtual void dropEvent ( QDropEvent * );
   virtual void mousePressEvent ( QMouseEvent *e );
   virtual void mouseMoveEvent ( QMouseEvent *e );
private:
   QColor col;
   QPoint mPos;
   bool dragFlag;

protected:
   virtual void virtual_hook ( int id, void* data );
private:
   class QColorButtonPrivate;
   QColorButtonPrivate *d;
};


#endif //__QCOLORBUTTON_H__

