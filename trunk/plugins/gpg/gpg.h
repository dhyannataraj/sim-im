/***************************************************************************
                          gpg.h  -  description
                             -------------------
    begin                : Sun Mar 17 2002
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

#ifndef _GPG_H
#define _GPG_H

#include "simapi.h"
#include "stl.h"

const unsigned MessageGPGKey	= 0x5000;
const unsigned MessageGPGUse	= 0x5001;

typedef struct GpgData
{
    char	*GPG;
    char	*Home;
    char	*GenKey;
    char	*PublicList;
    char	*SecretList;
    char	*Import;
    char	*Export;
    char	*Encrypt;
    char	*Decrypt;
    char	*Key;
} GpgData;

typedef struct GpgUserData
{
    char		*Key;
    unsigned	Use;
} GpgUserData;

class Exec;

typedef struct DecryptMsg
{
    Message		*msg;
    Exec		*exec;
    QString		infile;
    QString		outfile;
    unsigned	contact;
} DecryptMsg;

typedef struct KeyMsg
{
    string	key;
    Message	*msg;
} KeyMsg;

class GpgPlugin : public QObject, public Plugin, public EventReceiver
{
    Q_OBJECT
public:
    GpgPlugin(unsigned, const char*);
    virtual ~GpgPlugin();
    PROP_STR(GPG);
    PROP_STR(Home);
    PROP_STR(GenKey);
    PROP_STR(PublicList);
    PROP_STR(SecretList);
    PROP_STR(Import);
    PROP_STR(Export);
    PROP_STR(Encrypt);
    PROP_STR(Decrypt);
    PROP_STR(Key);
    const char *GPG();
    void reset();
    static GpgPlugin *plugin;
    list<KeyMsg>	 m_sendKeys;
    unsigned user_data_id;
protected slots:
    void decryptReady(Exec*,int,const char*);
    void importReady(Exec*,int,const char*);
    void publicReady(Exec*,int,const char*);
    void clear();
protected:
    virtual QWidget *createConfigWindow(QWidget *parent);
    virtual string getConfig();
    void *processEvent(Event*);
    void registerMessage();
    void unregisterMessage();
    bool m_bMessage;
    list<DecryptMsg> m_decrypt;
    list<DecryptMsg> m_import;
    list<DecryptMsg> m_public;
    GpgData data;
};

class MsgEdit;

class MsgGPGKey : public QObject, public EventReceiver
{
    Q_OBJECT
public:
    MsgGPGKey(MsgEdit *parent, Message *msg);
    ~MsgGPGKey();
protected slots:
    void init();
    void exportReady(Exec*,int,const char*);
    void clearExec();
protected:
    void *processEvent(Event*);
    string  m_client;
    string	m_key;
    MsgEdit	*m_edit;
    Exec	*m_exec;
};

#endif

