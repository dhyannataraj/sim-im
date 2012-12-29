/***************************************************************************
                          cfg.h  -  description
                             -------------------
    begin                : Sun Mar 10 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan@shutoff.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CFG_H
#define _CFG_H

#include <QByteArray>
#include <QString>
#include <QSharedPointer>

#include "propertyhub.h"
#include "simapi.h"
#include <buffer.h>

namespace SIM {

class EXPORT Config
{
public:
    Config(const QString& filename);
    virtual ~Config();

    PropertyHubPtr rootHub();

    bool mergeOldConfig(const QString& filename);

    bool writeToFile();
    bool readFromFile();
private:
    QByteArray serialize();
    bool deserialize(const QByteArray& arr);

    QString m_filename;
    PropertyHubPtr m_roothub;
    bool m_changed;
};

typedef QSharedPointer<Config> ConfigPtr;

EXPORT QString getToken(QString &from, char c, bool bUnEsacpe=true);
EXPORT QByteArray getToken(QByteArray &from, char c, bool bUnEsacpe=true);

} // namespace SIM

#endif
