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

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qstrlist.h>
#include <qstringlist.h>
#include <qcolor.h>
#include <qsize.h>
#include <qrect.h>
#include <qsize.h>
#include <qfont.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <cstdlib>

#include "qconfig.h"

QConfig::QConfig ( const QString &configFileName ) {
   fileName = configFileName;
   mGroup = "<default>";
   bConfigFileExists = FALSE;
   bReadOnly = FALSE;

   groupDict.setAutoDelete ( TRUE );

   QFileInfo fi ( fileName );
   if ( fi.exists() == FALSE ) {
      bConfigFileExists = FALSE;

      QString dir;
      int pos;
      if ( ( pos = fileName.findRev ( '/' ) ) < 0 )
         dir = ".";
      else
         dir = fileName.left ( pos );

      QFileInfo fiDir ( dir );
      if ( fiDir.isWritable() == TRUE ) {
         bReadOnly = FALSE;
      } else {
         bReadOnly = TRUE;
         qDebug ( "no write privilege, can't create config file" );
      }
   } else {
      if ( fi.isDir() == TRUE ) {
         qFatal ( "%s exists but it is a directory", fileName.local8Bit().data() );
         exit ( 1 );
      }

      if ( fi.isReadable() == FALSE ) {
         qFatal ( "can't read config file %s!", fileName.local8Bit().data() );
         exit ( 1 );
      } else {
         bConfigFileExists = TRUE;
         bReadOnly = ! ( fi.isWritable() );
      }
   }

   parseConfigFile();
}

QConfig::~QConfig() {
   sync();
}

void QConfig::sync() {
   if ( bReadOnly == FALSE )
      writeConfigFile();
}

QStringList QConfig::groupList() const {
   QStringList sl;

   QDictIterator<EntryMap> it ( groupDict );
   for ( ;it.current();++it ) {
      //if(it.currentKey()!="<default>")
      sl.append ( it.currentKey() );
   }

   return sl;
}

QMap<QString, QString> QConfig::entryMap ( const QString &pGroup ) const {
   QMap<QString, QString> tmpMap;
   EntryMap *map;
   if ( pGroup == QString::null )
      map = groupDict.find ( "<default>" );
   else
      map = groupDict.find ( pGroup );

   if ( map != 0 )
      tmpMap = *map;

   return tmpMap;
}

void QConfig::reparseConfiguration() {
   sync();
   groupDict.clear();
   mGroup = "<default>";
   parseConfigFile();
}

bool QConfig::isReadOnly() const {
   return bReadOnly;
}

bool QConfig::hasKey ( const QString &key ) const {
   EntryMapIterator it = currentGroup->find ( key );
   if ( it != currentGroup->end() )
      return TRUE;

   return FALSE;
}

bool QConfig::hasGroup ( const QString &group ) const {
   if ( groupDict.find ( group ) != 0 )
      return TRUE;

   return FALSE;
}

void QConfig::setGroup ( const QString &group ) {
   if ( group == mGroup )
      return;
   if ( group.isEmpty() == TRUE && mGroup == "<default>" )
      return;

   if ( group.isEmpty() == TRUE )
      mGroup = "<default>";
   else
      mGroup = group;

   currentGroup = groupDict.find ( mGroup );
   if ( currentGroup == 0 ) {
      currentGroup = new EntryMap;
      groupDict.insert ( mGroup, currentGroup );
   }
}

QString QConfig::group() const {
   if ( mGroup == "<default>" )
      return QString::null;

   return mGroup;
}

QString QConfig::readEntry ( const QString &pKey, const QString &aDefault ) const {
   EntryMapIterator it = currentGroup->find ( pKey );
   if ( it != currentGroup->end() ) {
      QString str = *it;
      if ( str.isEmpty() == TRUE )
         return QString::null;
      else
         return str;
   } else
      return aDefault;
}

QVariant QConfig::readPropertyEntry ( const QString &pKey, const QVariant &aDefault ) const {
   if ( hasKey ( pKey ) == FALSE )
      return aDefault;

   QVariant tmp = aDefault;

   switch ( aDefault.type() ) {
   case QVariant::Invalid:
      return QVariant();
   case QVariant::String:
      return QVariant ( readEntry ( pKey, aDefault.toString() ) );
   case QVariant::StringList:
      return QVariant ( readListEntry ( pKey ) );
   case QVariant::List: {
         QStringList strList = readListEntry ( pKey );
         QStringList::ConstIterator it = strList.begin();
         QStringList::ConstIterator end = strList.end();
         QValueList<QVariant> list;

         for ( ;it != end;++it ) {
            tmp = *it;
            list.append ( tmp );
         }
         return QVariant ( list );
      }
   case QVariant::Font:
      return QVariant ( readFontEntry ( pKey, &tmp.asFont() ) );
   case QVariant::Point:
      return QVariant ( readPointEntry ( pKey, &tmp.asPoint() ) );
   case QVariant::Rect:
      return QVariant ( readRectEntry ( pKey, &tmp.asRect() ) );
   case QVariant::Size:
      return QVariant ( readSizeEntry ( pKey, &tmp.asSize() ) );
   case QVariant::Color:
      return QVariant ( readColorEntry ( pKey, &tmp.asColor() ) );
   case QVariant::Int:
      return QVariant ( readNumEntry ( pKey, aDefault.toInt() ) );
   case QVariant::UInt:
      return QVariant ( readUnsignedNumEntry ( pKey, aDefault.toUInt() ) );
   case QVariant::LongLong:
      return QVariant ( readNum64Entry ( pKey, aDefault.toLongLong() ) );
   case QVariant::ULongLong:
      return QVariant ( readUnsignedNum64Entry ( pKey, aDefault.toULongLong() ) );
   case QVariant::Bool:
      return QVariant ( readBoolEntry ( pKey, aDefault.toBool() ), 0 );
   case QVariant::Double:
      return QVariant ( readDoubleNumEntry ( pKey, aDefault.toDouble() ) );
   case QVariant::DateTime:
      return QVariant ( readDateTimeEntry ( pKey, &tmp.asDateTime() ) );
   case QVariant::Date:
      return QVariant ( readDateTimeEntry ( pKey, &tmp.asDateTime() ).date() );

   case QVariant::Pixmap:
   case QVariant::Image:
   case QVariant::Brush:
   case QVariant::Palette:
   case QVariant::ColorGroup:
   case QVariant::Map:
   case QVariant::IconSet:
   case QVariant::CString:
   case QVariant::PointArray:
   case QVariant::Region:
   case QVariant::Bitmap:
   case QVariant::Cursor:
   case QVariant::SizePolicy:
   case QVariant::Time:
   case QVariant::ByteArray:
   case QVariant::BitArray:
   case QVariant::KeySequence:
   case QVariant::Pen:
      break;
   }

   Q_ASSERT ( 0 );
   return QVariant();
}

QStringList QConfig::readListEntry ( const QString &pKey, char sep ) const {
   QStringList sl;
   QString str = readEntry ( pKey );
   if ( str.isEmpty() == FALSE )
      sl = QStringList::split ( sep, str );

   return sl;
}

QValueList<int> QConfig::readIntListEntry ( const QString &pKey ) const {
   QStringList sl = readListEntry ( pKey );
   QValueList<int> vl;
   QStringList::iterator it = sl.begin();
   for ( ;it != sl.end();++it )
      vl << ( *it ).toInt();

   return vl;
}

QString QConfig::readPathEntry ( const QString &pKey, const QString &aDefault ) const {
   return readEntry ( pKey, aDefault );
}

QStringList QConfig::readPathListEntry ( const QString &pKey, char sep ) const {
   return readListEntry ( pKey, sep );
}

int QConfig::readNumEntry ( const QString &pKey, int nDefault ) const {
   QString str = readEntry ( pKey );

   bool ok;
   int i = str.toInt ( &ok );
   if ( ok == TRUE )
      return i;

   return nDefault;
}

unsigned int QConfig::readUnsignedNumEntry ( const QString &pKey, unsigned int nDefault ) const {
   QString str = readEntry ( pKey );

   bool ok;
   unsigned int i = str.toUInt ( &ok );
   if ( ok == TRUE )
      return i;

   return nDefault;
}

long QConfig::readLongNumEntr ( const QString &pKey, long nDefault ) const {
   QString str = readEntry ( pKey );

   bool ok;
   long i = str.toLong ( &ok );
   if ( ok == TRUE )
      return i;

   return nDefault;
}

unsigned long QConfig::readUnsignedLongNumEntry ( const QString &pKey, unsigned long nDefault ) const {
   QString str = readEntry ( pKey );

   bool ok;
   unsigned long i = str.toULong ( &ok );
   if ( ok == TRUE )
      return i;

   return nDefault;
}

Q_INT64 QConfig::readNum64Entry ( const QString &pKey, Q_INT64 nDefault ) const {
   QString str = readEntry ( pKey );

   bool ok;
   Q_INT64 i = str.toLongLong ( &ok );
   if ( ok == TRUE )
      return i;

   return nDefault;
}

Q_UINT64 QConfig::readUnsignedNum64Entry ( const QString &pKey, Q_UINT64 nDefault ) const {
   QString str = readEntry ( pKey );

   bool ok;
   Q_UINT64 i = str.toULongLong ( &ok );
   if ( ok == TRUE )
      return i;

   return nDefault;
}

double QConfig::readDoubleNumEntry ( const QString &pKey, double nDefault ) const {
   QString str = readEntry ( pKey );

   bool ok;
   double i = str.toDouble ( &ok );
   if ( ok == TRUE )
      return i;

   return nDefault;
}

QFont QConfig::readFontEntry ( const QString &pKey, const QFont *pDefault ) const {
   // TODO use QRegExp to reimplement this function
   QFont font;

   QString str = readEntry ( pKey );
   if ( str.isEmpty() == TRUE ) {
      if ( pDefault )
         font = *pDefault;
      else
         font = QFont();
   } else {
      if ( str.contains ( ',' ) > 5 ) {
         // KDE3 and upwards entry
         if ( !font.fromString ( str ) && pDefault )
            font = *pDefault;
      } else {
         // backward compatibility with older font formats
         // ### remove KDE 3.1 ?
         // find first part (font family)
         int nIndex = str.find ( ',' );
         if ( nIndex == -1 ) {
            if ( pDefault )
               font = *pDefault;
            return font;
         }
         font.setFamily ( str.left ( nIndex ) );

         // find second part (point size)
         int nOldIndex = nIndex;
         nIndex = str.find ( ',', nOldIndex + 1 );
         if ( nIndex == -1 ) {
            if ( pDefault )
               font = *pDefault;
            return font;
         }

         font.setPointSize ( str.mid ( nOldIndex + 1, nIndex - nOldIndex - 1 ).toInt() );

         // find third part (style hint)
         nOldIndex = nIndex;
         nIndex = str.find ( ',', nOldIndex + 1 );

         if ( nIndex == -1 ) {
            if ( pDefault )
               font = *pDefault;
            return font;
         }

         font.setStyleHint ( ( QFont::StyleHint ) str.mid ( nOldIndex + 1, nIndex - nOldIndex - 1 ).toUInt() );

         // find fourth part (char set)
         nOldIndex = nIndex;
         nIndex = str.find ( ',', nOldIndex + 1 );

         if ( nIndex == -1 ) {
            if ( pDefault )
               font = *pDefault;
            return font;
         }

         QString chStr = str.mid ( nOldIndex + 1, nIndex - nOldIndex - 1 );
         // find fifth part (weight)
         nOldIndex = nIndex;
         nIndex = str.find ( ',', nOldIndex + 1 );

         if ( nIndex == -1 ) {
            if ( pDefault )
               font = *pDefault;
            return font;
         }

         font.setWeight ( str.mid ( nOldIndex + 1, nIndex - nOldIndex - 1 ).toUInt() );

         // find sixth part (font bits)
         uint nFontBits = str.right ( str.length() - nIndex - 1 ).toUInt();

         font.setItalic ( nFontBits & 0x01 );
         font.setUnderline ( nFontBits & 0x02 );
         font.setStrikeOut ( nFontBits & 0x04 );
         font.setFixedPitch ( nFontBits & 0x08 );
         font.setRawMode ( nFontBits & 0x20 );
      }
   }

   return font;
}

bool QConfig::readBoolEntry ( const QString &pKey, const bool bDefault ) const {
   QString str = readEntry ( pKey );
   str = str.lower();

   if ( str.isEmpty() == TRUE )
      return bDefault;
   else if ( str == "true" || str == "yes" || str == "on" || str == "1" )
      return TRUE;
   else {
      bool ok;
      int i = str.toInt ( &ok );
      if ( ok && i != 0 )
         return TRUE;
   }

   return false;
}

QRect QConfig::readRectEntry ( const QString &pKey, const QRect *pDefault ) const {
   QString str = readEntry ( pKey );
   if ( str.isEmpty() == FALSE ) {
      int left, top, width, height;
      if ( sscanf ( str.local8Bit().data(), "%d,%d,%d,%d", &left, &top, &width, &height ) == 4 ) {
         return QRect ( left, top, width, height );
      }
   }

   if ( pDefault )
      return * pDefault;

   return QRect();
}

QPoint QConfig::readPointEntry ( const QString &pKey, const QPoint *pDefault ) const {
   QString str = readEntry ( pKey );
   if ( str.isEmpty() == FALSE ) {
      int x, y;
      if ( sscanf ( str.local8Bit().data(), "%d,%d", &x, &y ) == 2 ) {
         return QPoint ( x, y );
      }
   }

   if ( pDefault )
      return * pDefault;

   return QPoint();
}

QSize QConfig::readSizeEntry ( const QString &pKey, const QSize *pDefault ) const {
   QString str = readEntry ( pKey );
   if ( str.isEmpty() == FALSE ) {
      int width, height;
      if ( sscanf ( str.local8Bit().data(), "%d,%d", &width, &height ) == 2 ) {
         return QSize ( width, height );
      }
   }

   if ( pDefault )
      return * pDefault;

   return QSize();
}

QColor QConfig::readColorEntry ( const QString &pKey, const QColor *pDefault ) const {
   QColor color;
   int nRed, nGreen, nBlue;

   QString str = readEntry ( pKey );
   if ( str.isEmpty() == FALSE ) {
      if ( str.startsWith ( "#" ) ) {
         color.setNamedColor ( str );
      } else {
         if ( sscanf ( str.local8Bit().data(), "%d,%d,%d", &nRed, &nGreen, &nBlue ) == 3 )
            color.setRgb ( nRed, nGreen, nBlue );
         else if ( pDefault )
            return * pDefault;
      }
   } else if ( pDefault )
      return * pDefault;

   return color;
}

QDateTime QConfig::readDateTimeEntry ( const QString &pKey, const QDateTime *pDefault ) const {
   if ( hasKey ( pKey ) == FALSE ) {
      if ( pDefault )
         return * pDefault;
      else
         return QDateTime::currentDateTime();
   }

   QValueList<int> vl = readIntListEntry ( pKey );
   if ( vl.count() == 6 ) {
      QDate date ( vl[0], vl[1], vl[2] );
      QTime time ( vl[3], vl[4], vl[5] );
      return QDateTime ( date, time );
   }

   return QDateTime::currentDateTime();
}

void QConfig::writeEntry ( const QString &pKey, const QString &pValue ) {
   ( *currentGroup ) [pKey] = pValue;
}

void QConfig::writeEntry ( const QString &pKey, const QVariant &rValue ) {
   switch ( rValue.type() ) {
   case QVariant::Invalid:
      writeEntry ( pKey, "" );
      return;
   case QVariant::String:
      writeEntry ( pKey, rValue.toString() );
      return;
   case QVariant::StringList:
      writeEntry ( pKey, rValue.toStringList(), ',' );
      return;
   case QVariant::List: {
         QValueList<QVariant> list = rValue.toList();
         QValueList<QVariant>::ConstIterator it = list.begin();
         QValueList<QVariant>::ConstIterator end = list.end();
         QStringList strList;
         for ( ;it != end;++it )
            strList.append ( ( *it ).toString() );
         writeEntry ( pKey, strList, ',' );
         return;
      }
   case QVariant::Font:
      writeEntry ( pKey, rValue.toFont() );
      return;
   case QVariant::Point:
      writeEntry ( pKey, rValue.toPoint() );
      return;
   case QVariant::Rect:
      writeEntry ( pKey, rValue.toRect() );
      return;
   case QVariant::Size:
      writeEntry ( pKey, rValue.toSize() );
      return;
   case QVariant::Color:
      writeEntry ( pKey, rValue.toColor() );
      return;
   case QVariant::Int:
      writeEntry ( pKey, rValue.toInt() );
      return;
   case QVariant::UInt:
      writeEntry ( pKey, rValue.toUInt() );
      return;
   case QVariant::LongLong:
      writeEntry ( pKey, rValue.toLongLong() );
      return;
   case QVariant::ULongLong:
      writeEntry ( pKey, rValue.toULongLong() );
      return;
   case QVariant::Bool:
      writeEntry ( pKey, rValue.toBool() );
      return;
   case QVariant::Double:
      writeEntry ( pKey, rValue.toDouble(), 'g', 6 );
      return;
   case QVariant::DateTime:
      writeEntry ( pKey, rValue.toDateTime() );
      return;
   case QVariant::Date:
      writeEntry ( pKey, QDateTime ( rValue.toDate() ) );
      return;

   case QVariant::Pixmap:
   case QVariant::Image:
   case QVariant::Brush:
   case QVariant::Palette:
   case QVariant::ColorGroup:
   case QVariant::Map:
   case QVariant::IconSet:
   case QVariant::CString:
   case QVariant::PointArray:
   case QVariant::Region:
   case QVariant::Bitmap:
   case QVariant::Cursor:
   case QVariant::SizePolicy:
   case QVariant::Time:
   case QVariant::ByteArray:
   case QVariant::BitArray:
   case QVariant::KeySequence:
   case QVariant::Pen:
      break;
   }

   Q_ASSERT ( 0 );
}

void QConfig::writeEntry ( const QString &pKey, const QStrList &rValue, char sep ) {
   if ( rValue.isEmpty() ) {
      writeEntry ( pKey, QString ( "" ) );
      return;
   }

   QString str;
   QStrListIterator it ( rValue );
   for ( ;it.current();++it ) {
      uint i;
      QString value;
      //TODO A QStrList may contain values in 8bit locale specified
      //encoding or in UTF8 encoding.
      //need fix this??
      value = QString::fromLocal8Bit ( it.current() );
      for ( i = 0;i < value.length();i++ ) {
         if ( value[i] == sep || value[i] == '\\' )
            str += '\\';
         str += value[i];
      }
      str += sep;
   }

   if ( str.at ( str.length() - 1 ) == sep )
      str.truncate ( str.length() - 1 );

   writeEntry ( pKey, str );
}

void QConfig::writeEntry ( const QString &pKey, const QStringList &rValue, char sep ) {
   if ( rValue.isEmpty() ) {
      writeEntry ( pKey, QString ( "" ) );
      return;
   }

   QString str;
   QStringList::ConstIterator it = rValue.begin();
   for ( ;it != rValue.end();++it ) {
      uint i;
      QString value = ( *it );
      for ( i = 0;i < value.length();i++ ) {
         if ( value[i] == sep || value[i] == '\\' )
            str += '\\';
         str += value[i];
      }
      str += sep;
   }

   if ( str.at ( str.length() - 1 ) == sep )
      str.truncate ( str.length() - 1 );

   writeEntry ( pKey, str );
}

void QConfig::writeEntry ( const QString &pKey, const QValueList<int> &rValue ) {
   QStringList strlist;
   QValueList<int>::ConstIterator it = rValue.begin();
   for ( ;it != rValue.end();++it )
      strlist << QString::number ( *it );
   writeEntry ( pKey, strlist, ',' );
}

void QConfig::writeEntry ( const QString &pKey, const char *pValue ) {
   writeEntry ( pKey, QString::fromLatin1 ( pValue ) );
}

void QConfig::writeEntry ( const QString &pKey, int nValue ) {
   writeEntry ( pKey, QString::number ( nValue ) );
}

void QConfig::writeEntry ( const QString &pKey, unsigned int nValue ) {
   writeEntry ( pKey, QString::number ( nValue ) );
}

void QConfig::writeEntry ( const QString &pKey, long nValue ) {
   writeEntry ( pKey, QString::number ( nValue ) );
}

void QConfig::writeEntry ( const QString &pKey, unsigned long nValue ) {
   writeEntry ( pKey, QString::number ( nValue ) );
}

void QConfig::writeEntry ( const QString &pKey, Q_INT64 nValue ) {
   writeEntry ( pKey, QString::number ( nValue ) );
}

void QConfig::writeEntry ( const QString &pKey, Q_UINT64 nValue ) {
   writeEntry ( pKey, QString::number ( nValue ) );
}

void QConfig::writeEntry ( const QString &pKey, double nValue, char format, int precision ) {
   writeEntry ( pKey, QString::number ( nValue, format, precision ) );
}

void QConfig::writeEntry ( const QString &pKey, bool bValue ) {
   if ( bValue == TRUE )
      writeEntry ( pKey, "true" );
   else
      writeEntry ( pKey, "false" );
}

void QConfig::writeEntry ( const QString &pKey, QFont &rFont ) {
   writeEntry ( pKey, rFont.toString() );
}

void QConfig::writeEntry ( const QString &pKey, QColor &rColor ) {
   QString aValue;
   if ( rColor.isValid() )
      aValue.sprintf ( "%d,%d,%d", rColor.red(), rColor.green(), rColor.blue() );
   else
      aValue = "invalid";

   writeEntry ( pKey, aValue );
}

void QConfig::writeEntry ( const QString &pKey, QDateTime &rDateTime ) {
   QString str;
   QDate date = rDateTime.date();
   QTime time = rDateTime.time();
   str.sprintf ( "%d,%d,%d,%d,%d,%d", date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second() );
   writeEntry ( pKey, str );
}

void QConfig::writeEntry ( const QString &pKey, QRect &rRect ) {
   QString str;
   str.sprintf ( "%d,%d,%d,%d", rRect.left(), rRect.top(), rRect.width(), rRect.height() );
   writeEntry ( pKey, str );
}

void QConfig::writeEntry ( const QString &pKey, QPoint &rPoint ) {
   QString str;
   str.sprintf ( "%d,%d", rPoint.x(), rPoint.y() );
   writeEntry ( pKey, str );
}

void QConfig::writeEntry ( const QString &pKey, QSize &rSize ) {
   QString str;
   str.sprintf ( "%d,%d", rSize.width(), rSize.height() );
   writeEntry ( pKey, str );
}

void QConfig::writePathEntry ( const QString &pKey, const QString &path ) {
   // TODO implement this later if needed
   writeEntry ( pKey, path );
}

void QConfig::writePathEntry ( const QString &pKey, const QStringList &pathList, char sep ) {
   // TODO implement this later if needed
   writeEntry ( pKey, pathList, sep );
}

void QConfig::deleteEntry ( const QString &pKey ) {
   currentGroup->remove ( pKey );
}

void QConfig::deleteGroup ( const QString &group ) {
   if ( group.isEmpty() == TRUE ) {
      EntryMap * map = groupDict.find ( "<default>" );
      map->clear();
   } else {
      if ( mGroup == group )
         setGroup ( QString::null );

      groupDict.remove ( group );
   }
}

void QConfig::parseConfigFile() {
   currentGroup = new EntryMap;
   groupDict.insert ( "<default>", currentGroup );

   if ( bConfigFileExists == FALSE )
      return;

   QFile file ( fileName );
   file.open ( IO_ReadOnly );

   QTextStream ts ( &file );
   ts.setEncoding ( QTextStream::Locale );
   QString str;
   QRegExp re1 ( "\\[(.*)\\]" );
   QRegExp re2 ( "(.*)(?:\\[.*\\]){0,1}=(.*)" );

   while ( ts.atEnd() == FALSE ) {
      str = ts.readLine();
      str = str.simplifyWhiteSpace();

      if ( str.startsWith ( "#" ) )
         continue;
      else if ( str.isEmpty() && mGroup != "<default>" )
         setGroup ( QString::null );
      else if ( re1.exactMatch ( str ) )
         setGroup ( re1.cap ( 1 ) );
      else if ( re2.exactMatch ( str ) )
         currentGroup->insert ( re2.cap ( 1 ), re2.cap ( 2 ) );
   }

   file.close();
}

void QConfig::writeConfigFile() {
   if ( bReadOnly ) {
      qWarning ( "config file %s is read only", fileName.local8Bit().data() );
      return;
   }

   QFile file ( fileName );
   file.open ( IO_WriteOnly );
   QTextStream ts ( &file );
   ts.setEncoding ( QTextStream::Locale );

   // write default first
   EntryMap *map = groupDict.find ( "<default>" );
   EntryMapIterator it2 = map->begin();
   for ( ;it2 != map->end();++it2 )
      ts << it2.key() << "=" << it2.data() << endl;
   ts << endl;

   QDictIterator<EntryMap> it ( groupDict );
   for ( ; it.current(); ++it ) {
      if ( it.currentKey() == "<default>" )
         continue;

      EntryMap *map = it.current();
      ts << "[" << it.currentKey() << "]" << endl;

      EntryMapIterator it1 = map->begin();
      for ( ;it1 != map->end(); ++it1 ) {
         ts << it1.key() << "=" << it1.data() << endl;
      }

      ts << endl;
   }
}
