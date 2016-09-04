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

#ifndef _QCONFIG_H_
#define _QCONFIG_H_

#include <qobject.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qdict.h>

class QString;
class QStrList;
class QStringList;
class QSize;
class QColor;
class QDateTime;

class QConfig: public QObject {
   Q_OBJECT
public:
   QConfig ( const QString &configFileName );
   ~QConfig();

   virtual void sync();
   virtual QStringList groupList() const;
   virtual QMap<QString, QString> entryMap ( const QString &pGroup ) const;
   virtual void reparseConfiguration();
   bool isReadOnly() const;

   bool hasKey ( const QString &key ) const;
   bool hasGroup ( const QString &group ) const;
   void setGroup ( const QString &group );
   QString group() const;

   QString readEntry ( const QString &pKey, const QString &aDefault = QString::null ) const;
   QVariant readPropertyEntry ( const QString &pKey, const QVariant &aDefault ) const;
   QStringList readListEntry ( const QString &pKey, char sep = ',' ) const;
   QValueList<int> readIntListEntry ( const QString &pKey ) const;
   QString readPathEntry ( const QString &pKey, const QString &aDefault = QString::null ) const;
   QStringList readPathListEntry ( const QString &pKey, char sep = ',' ) const;
   int readNumEntry ( const QString &pKey, int nDefault = 0 ) const;
   unsigned int readUnsignedNumEntry ( const QString &pKey, unsigned int nDefault = 0 ) const;
   long readLongNumEntr ( const QString &pKey, long nDefault = 0 ) const;
   unsigned long readUnsignedLongNumEntry ( const QString &pKey, unsigned long nDefault = 0 ) const;
   Q_INT64 readNum64Entry ( const QString &pKey, Q_INT64 nDefault = 0 ) const;
   Q_UINT64 readUnsignedNum64Entry ( const QString &pKey, Q_UINT64 nDefault = 0 ) const;
   double readDoubleNumEntry ( const QString &pKey, double nDefault = 0.0 ) const;
   QFont readFontEntry ( const QString &pKey, const QFont *pDefault = 0L ) const;
   bool readBoolEntry ( const QString &pKey, const bool bDefault = FALSE ) const;
   QRect readRectEntry ( const QString &pKey, const QRect *pDefault = 0L ) const;
   QPoint readPointEntry ( const QString &pKey, const QPoint *pDefault = 0L ) const;
   QSize readSizeEntry ( const QString &pKey, const QSize *pDefault = 0L ) const;
   QColor readColorEntry ( const QString &pKey, const QColor *pDefault = 0L ) const;
   QDateTime readDateTimeEntry ( const QString &pKey, const QDateTime *pDefault = 0L ) const;

   void writeEntry ( const QString &pKey, const QString &pValue );
   void writeEntry ( const QString &pKey, const QVariant &rValue );
   void writeEntry ( const QString &pKey, const QStrList &rValue, char sep = ',' );
   void writeEntry ( const QString &pKey, const QStringList &rValue, char sep = ',' );
   void writeEntry ( const QString &pKey, const QValueList<int> &rValue );
   void writeEntry ( const QString &pKey, const char *pValue );
   void writeEntry ( const QString &pKey, int nValue );
   void writeEntry ( const QString &pKey, unsigned int nValue );
   void writeEntry ( const QString &pKey, long nValue );
   void writeEntry ( const QString &pKey, unsigned long nValue );
   void writeEntry ( const QString &pKey, Q_LLONG nValue );
   void writeEntry ( const QString &pKey, Q_ULLONG nValue );
   void writeEntry ( const QString &pKey, double nValue, char format = 'g', int precision = 6 );
   void writeEntry ( const QString &pKey, bool bValue );
   void writeEntry ( const QString &pKey, QFont &rFont );
   void writeEntry ( const QString &pKey, QColor &rColor );
   void writeEntry ( const QString &pKey, QDateTime &rDateTime );
   void writeEntry ( const QString &pKey, QRect &rRect );
   void writeEntry ( const QString &pKey, QPoint &rPoint );
   void writeEntry ( const QString &pKey, QSize &rSize );
   void writePathEntry ( const QString &pKey, const QString &path );
   void writePathEntry ( const QString &pKey, const QStringList &pathList, char sep = ',' );

   void deleteEntry ( const QString &pKey );
   void deleteGroup ( const QString &group );

protected:
   virtual void parseConfigFile();
   virtual void writeConfigFile();

   QString fileName;
   QString mGroup;
   bool bReadOnly;
   bool bConfigFileExists;

   typedef QMap<QString, QString> EntryMap;
   typedef QMap<QString, QString>::iterator EntryMapIterator;
   EntryMap *currentGroup;
   QDict<EntryMap> groupDict;
};

#endif //_QCONFIG_H_
