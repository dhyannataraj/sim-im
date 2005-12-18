/***************************************************************************
                          filetransfer.h  -  description
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

#ifndef _FILETRANSFER_H
#define _FILETRANSFER_H

#include "simapi.h"
#include "filetransferbase.h"

class QTimer;
class Q3ProgressBar;
class BalloonMsg;

class FileTransferDlg : public QDialog, public Ui::FileTransferBase
{
    Q_OBJECT
public:
    FileTransferDlg(FileMessage*);
    ~FileTransferDlg();
protected slots:
    void speedChanged(int);
    void closeToggled(bool);
    void timeout();
    void action(int, void*);
    void goDir();
protected:
    void setProgress(Q3ProgressBar *bar, unsigned bytes, unsigned size);
    void process();
    void notifyDestroyed();
    void printTime();
    void transfer(bool);
    void calcSpeed(bool);
    void setBars();
    FileMessage	*m_msg;
    QTimer	*m_timer;
    unsigned m_time;
    unsigned m_file;
    bool     m_bTransfer;
    unsigned m_displayTime;
    unsigned m_transferTime;
    unsigned m_transferBytes;
    unsigned m_speed;
    unsigned m_nAverage;
    unsigned m_files;
    unsigned m_bytes;
    unsigned m_fileSize;
    unsigned m_totalBytes;
    unsigned m_totalSize;
    QString  m_dir;
    BalloonMsg *m_ask;
    FileTransfer::State m_state;
    friend class FileTransferDlgNotify;
};

#endif
