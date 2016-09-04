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

#ifndef _QURLREQUESTER_H_
#define _QURLREQUESTER_H_

#include <qhbox.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <qurl.h>

class QComboBox;
class QFileDialog;
class QLineEdit;
class QString;
class QTimer;
class QURLDragPushButton;

/**
 * This class is a widget showing a lineedit and a button, which invokes a
 * filedialog. Not like KURLRequester, file name completion is not available in the lineedit.
 *
 * The defaults for the filedialog are to ask for one existing local file, i.e.
 * QFileDialog::setMode( QFile::File | QFile::ExistingOnly | QFile::LocalOnly )
 * The default filter is "*", i.e. show all files, and the start directory is
 * the current working directory.
 *
 * You can change this behavior by using setMode() or setFilter().
 *
 */
class QURLRequester : public QHBox {
   Q_OBJECT
   Q_PROPERTY ( QString url READ url WRITE setURL )

public:
   /**
    * Constructs a QURLRequester widget.
    */
   QURLRequester ( QWidget *parent = 0, const char *name = 0 );

   /**
    * Constructs a QURLRequester widget with the initial URL @p url.
    */
   QURLRequester ( const QString& url, QWidget *parent = 0, const char *name = 0 );

   /**
    * Special constructor, which creates a QURLRequester widget with a custom
    * edit-widget. The edit-widget can be either a QComboBox or a QLineEdit
    * (or inherited thereof). Note: for geometry management reasons, the
    * edit-widget is reparented to have the QURLRequester as parent.
    * @param modal specifies whether the filedialog should be opened as modal
    * or not.
    */
   QURLRequester ( QWidget *editWidget, QWidget *parent, const char *name = 0 );
   /**
    * Destructs the QURLRequester.
    */
   ~QURLRequester();

   /**
    * @returns the current url in the lineedit. May be malformed, if the user
    * entered something weird. ~user or environment variables are substituted
    * for local files.
    */
   QString url() const;

   /**
    * Enables/disables showing file:/ in the lineedit, when a local file has
    * been selected in the filedialog or was set via setURL().
    * Default is false, not showing file:/
    * @see showLocalProtocol
    */
   void setShowLocalProtocol ( bool b );

   /**
    * Sets the mode of the file dialog.
    * Note: you can only select one file with the filedialog,
    * so QFile::Files doesn't make much sense.
    * @see QFileDialog::setMode()
    */
   void setMode ( unsigned int m );

   /**
    * Sets the filter for the file dialog.
    * @see QFileDialog::setFilter()
    */
   void setFilter ( const QString& filter );

   /**
    * @returns whether local files will be prefixed with file:/ in the
    * lineedit
    * @see setShowLocalProtocol
    */
   bool showLocalProtocol() const {
      return myShowLocalProt;
   }

   /**
    * @returns a pointer to the filedialog
    * You can use this to customize the dialog, e.g. to specify a filter.
    * Never returns 0L.
    */
   virtual QFileDialog * fileDialog() const;

   /**
    * @returns a pointer to the lineedit, either the default one, or the
    * special one, if you used the special constructor.
    *
    * It is provided so that you can e.g. set an own completion object into it.
    */
   QLineEdit * lineEdit() const;

   /**
    * @returns a pointer to the combobox, in case you have set one using the
    * special constructor. Returns 0L otherwise.
    */
   QComboBox * comboBox() const;

   /**
    * @returns a pointer to the pushbutton. It is provided so that you can
    * specify an own pixmap or a text, if you really need to.
    */
   QPushButton * button() const;

public slots:
   /**
    * Sets the url in the lineedit to @p url. Depending on the state of
    * showLocalProtocol(), file:/ on local files will be shown or not.
    * @since 3.1
    */
   void setURL ( const QString& url );

   /**
    * Sets the caption of the file dialog.
    * @since 3.1
    */
   virtual void setCaption ( const QString& caption );

   /**
    * Clears the lineedit/combobox.
    */
   void clear();

signals:
   // forwards from LineEdit
   /**
    * Emitted when the text in the lineedit changes.
    * The parameter contains the contents of the lineedit.
    * @since 3.1
    */
   void textChanged ( const QString& );

   /**
    * Emitted when return or enter was pressed in the lineedit.
    */
   void returnPressed();

   /**
    * Emitted when return or enter was pressed in the lineedit.
    * The parameter contains the contents of the lineedit.
    */
   void returnPressed ( const QString& );

   /**
    * Emitted before the filedialog is going to open. Connect
    * to this signal to "configure" the filedialog, e.g. set the
    * filefilter, the mode, a preview-widget, etc. It's usually
    * not necessary to set a URL for the filedialog, as it will
    * get set properly from the editfield contents.
    *
    * If you use multiple QURLRequesters, you can connect all of them
    * to the same slot and use the given QURLRequester pointer to know
    * which one is going to open.
    */
   void openFileDialog ( QURLRequester * );

   /**
    * Emitted when the user changed the URL via the file dialog.
    * The parameter contains the contents of the lineedit.
    */
   void urlSelected ( const QString& );

protected:
   void  init();


private:
   QURLDragPushButton * myButton;
   bool   myShowLocalProt;
   mutable QFileDialog * myFileDialog;


protected slots:
   /**
    * Called when the button is pressed to open the filedialog.
    * Also called when KStdAccel::Open (default is Ctrl-O) is pressed.
    */
   void slotOpenDialog();

private slots:
   void slotUpdateURL();

protected:
   virtual void virtual_hook ( int id, void* data );
private:
   class QURLRequesterPrivate;
   QURLRequesterPrivate *d;
};

class QURLComboRequester : public QURLRequester // For use in Qt Designer
{
   Q_OBJECT
public:
   /**
    * Constructs a QURLRequester widget with a combobox.
    */
   QURLComboRequester ( QWidget *parent = 0, const char *name = 0 );
};


#endif // _QURLREQUESTER_H_
