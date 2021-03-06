/***************************************************************************
                          filetransfer.cpp  -  description
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

#include "filetransfer.h"
#include "core.h"

#include <qpixmap.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qcheckbox.h>

#include <time.h>

const unsigned MAX_AVERAGE	= 40;
const unsigned SHOW_AVERAGE	= 5;

class FileTransferDlgNotify : public FileTransferNotify
{
public:
    FileTransferDlgNotify(FileTransferDlg *dlg);
    ~FileTransferDlgNotify();
protected:
    void process();
    void transfer(bool);
    FileTransferDlg *m_dlg;
};

FileTransferDlgNotify::FileTransferDlgNotify(FileTransferDlg *dlg)
{
    m_dlg = dlg;
}

FileTransferDlgNotify::~FileTransferDlgNotify()
{
    m_dlg->notifyDestroyed();
}

void FileTransferDlgNotify::process()
{
    m_dlg->process();
}

void FileTransferDlgNotify::transfer(bool bState)
{
    m_dlg->transfer(bState);
}

FileTransferDlg::FileTransferDlg(FileMessage *msg)
        : FileTransferBase(NULL, "filetransfer", false, WDestructiveClose)
{
    m_msg = msg;
    SET_WNDPROC("filetransfer")
    setIcon(Pict("file"));
    setButtonsPict(this);
    setCaption((msg->getFlags() & MESSAGE_RECEIVED) ? i18n("Receive file") : i18n("Send file"));
    disableWidget(edtTime);
    disableWidget(edtEstimated);
    disableWidget(edtSpeed);
    msg->m_transfer->setNotify(new FileTransferDlgNotify(this));
    sldSpeed->setValue(m_msg->m_transfer->speed());
    connect(sldSpeed, SIGNAL(valueChanged(int)), this, SLOT(speedChanged(int)));
    m_time  = 0;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer->start(1000);
    printTime();
    m_bTransfer = false;
    m_transferTime = 0;
    m_speed     = 0;
    m_nAverage  = 0;
    m_files		= 0;
    m_bytes		= 0;
    m_fileSize	= 0;
    m_totalBytes = 0;
    m_totalSize	= 0;
    m_state = FileTransfer::Unknown;
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
    chkClose->setChecked(CorePlugin::m_plugin->getCloseTransfer());
    connect(chkClose, SIGNAL(toggled(bool)), this, SLOT(closeToggled(bool)));
}

FileTransferDlg::~FileTransferDlg()
{
    if (m_msg == NULL)
        return;
    if (m_msg->m_transfer)
        m_msg->m_transfer->setNotify(NULL);
    Event e(EventMessageCancel, m_msg);
    e.process();
}

void FileTransferDlg::process()
{
    if (m_msg->m_transfer == NULL)
        return;
    if ((m_msg->m_transfer->state() != m_state) || (m_msg->m_transfer->file() != m_file)){
        m_state = m_msg->m_transfer->state();
        m_file  = m_msg->m_transfer->file();
        QString status;
        bool bName = false;
        switch (m_state){
        case FileTransfer::Listen:
            status = i18n("Listen");
            break;
        case FileTransfer::Connect:
            status = i18n("Connect");
            break;
        case FileTransfer::Negotiation:
            status = i18n("Negotiation");
            break;
        case FileTransfer::Read:
            status = i18n("Receive");
            bName = true;
            break;
        case FileTransfer::Write:
            status = i18n("Send");
            bName = true;
            break;
        case FileTransfer::Done:
            status = i18n("Transfer done");
            break;
        case FileTransfer::Error:
            if (m_msg->getError())
                status = i18n(m_msg->getError());
            break;
        }
        if (bName && (m_files > 1)){
            FileMessage::Iterator it(*m_msg);
            status += " ";
            const char *n = it[m_file];
            if (n){
#ifdef WIN32
                const char *p = strrchr(n, '\\');
#else
                const char *p = strrchr(n, '/');
#endif
                if (p)
                    n = p + 1;
                status += QString::fromLocal8Bit(n);
                status += QString(" %1/%2")
                          .arg(m_file + 1)
                          .arg(m_msg->m_transfer->files());
            }
        }
        lblState->setText(status);
        setBars();
    }
    calcSpeed();
    if (m_msg->m_transfer->speed() != sldSpeed->value())
        sldSpeed->setValue(m_msg->m_transfer->speed());
    if (m_msg->m_transfer->files() != m_files){
        m_files = m_msg->m_transfer->files();
        if (m_files > 1){
            if (!barFile->isVisible())
                barFile->show();
        }else{
            if (barFile->isVisible())
                barFile->hide();
        }
    }
}

void FileTransferDlg::transfer(bool bState)
{
    m_bTransfer = bState;
    if (bState && m_msg->m_transfer){
        time_t now;
        time(&now);
        m_transferBytes = m_msg->m_transfer->transferBytes();
        m_transferTime  = now;
    }
}

void FileTransferDlg::notifyDestroyed()
{
    sldSpeed->hide();
    m_timer->stop();
    btnCancel->setText(i18n("&Close"));
    if (m_state == FileTransfer::Done){
        if (chkClose->isChecked())
            close();
        return;
    }
    if (m_msg->getError()){
        lblState->setText(i18n(m_msg->getError()));
    }else{
        lblState->setText(i18n("Transfer failed"));
    }
}

void FileTransferDlg::speedChanged(int value)
{
    if (m_msg->m_transfer)
        m_msg->m_transfer->setSpeed(value);
}

void FileTransferDlg::timeout()
{
    m_time++;
    printTime();
    calcSpeed();
    setBars();
}

void FileTransferDlg::setBars()
{
    if (m_msg->m_transfer == NULL)
        return;
    if (m_totalSize != m_msg->m_transfer->totalSize()){
        m_totalSize = m_msg->m_transfer->totalSize();
        barTotal->setTotalSteps(m_totalSize);
    }
    if (m_totalBytes != m_msg->m_transfer->totalBytes()){
        m_totalBytes = m_msg->m_transfer->totalBytes();
        barTotal->setProgress(m_totalBytes);
    }
    if (m_files > 1){
        if (m_fileSize != m_msg->m_transfer->fileSize()){
            m_fileSize = m_msg->m_transfer->fileSize();
            barFile->setTotalSteps(m_fileSize);
        }
        if (m_bytes != m_msg->m_transfer->bytes()){
            m_bytes = m_msg->m_transfer->bytes();
            barFile->setProgress(m_bytes);
        }
    }
}

void FileTransferDlg::calcSpeed()
{
    if (!m_bTransfer)
        return;
    time_t now;
    time(&now);
    if (now == m_transferTime)
        return;
    if (m_nAverage < MAX_AVERAGE)
        m_nAverage++;
    m_speed = (m_speed * (m_nAverage - 1) + m_msg->m_transfer->transferBytes() - m_transferBytes) / m_nAverage;
    m_transferBytes = m_msg->m_transfer->transferBytes();
    m_transferTime  = now;
    unsigned n_speed = 0;
    double speed = m_speed;
    if (speed >= 1024){
        speed = speed / 1024;
        n_speed++;
    }
    if (speed >= 1024){
        speed = speed / 1024;
        m_speed++;
    }
    if (m_nAverage < SHOW_AVERAGE)
        return;
    if (speed == 0){
        edtEstimated->setText("");
        edtSpeed->setText(i18n("Stalled"));
        return;
    }
    QString speedText;
    if (speed >= 100){
        speedText = QString::number((unsigned)speed);
    }else{
        speedText = QString::number(speed, 'f', 3);
    }
    speedText += " ";
    switch (n_speed){
    case 2:
        speedText += i18n("Mb/s");
        break;
    case 1:
        speedText += i18n("kb/s");
        break;
    default:
        speedText += i18n("b/s");
    }
    if (edtSpeed->text() != speedText)
        edtSpeed->setText(speedText);
    unsigned estimate = (m_msg->m_transfer->totalSize() - m_msg->m_transfer->totalBytes()) / m_speed;
    unsigned m = estimate / 60;
    unsigned h = m / 60;
    m = m % 60;
    char b[64];
    sprintf(b, "%u:%02u:%02u", h, m, estimate % 60);
    edtEstimated->setText(b);
}

void FileTransferDlg::printTime()
{
    unsigned m = m_time / 60;
    unsigned h = m / 60;
    m = m % 60;
    char b[64];
    sprintf(b, "%u:%02u:%02u", h, m, m_time % 60);
    edtTime->setText(b);
}

void FileTransferDlg::closeToggled(bool bState)
{
    CorePlugin::m_plugin->setCloseTransfer(bState);
}

#ifndef WIN32
#include "filetransfer.moc"
#endif

