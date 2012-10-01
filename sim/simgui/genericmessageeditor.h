/*
 * genericmessageeditor.h
 *
 *  Created on: Aug 17, 2011
 */

#ifndef GENERICMESSAGEEDITOR_H_
#define GENERICMESSAGEEDITOR_H_

#include "messageeditor.h"

#include <QTextEdit>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>
#include <QToolBar>
#include <QComboBox>

#include "contacts/imcontact.h"
#include "simapi.h"

namespace SIM
{
class EXPORT GenericMessageEditor: public SIM::MessageEditor
{
    Q_OBJECT
public:
    GenericMessageEditor(const IMContactPtr& from, const IMContactPtr& to, QWidget* parent);
    virtual ~GenericMessageEditor();

    virtual QString messageTypeId() const;

public slots:
    void cursorPositionChanged();

private slots:
    QColor colorFromDialog(QString oldColorName);
    void chooseBackgroundColor();
    void chooseForegroundColor();
    QString getBGStyleSheet(QString bgColorName);
    void chooseFont();
    void setBold(bool b);
    void setItalic(bool b);
    void setUnderline(bool b);
    void insertSmile();
    void setTranslit(bool on);
    void setTranslateIncomming(bool on);
    void setTranslateOutgoing(bool on);
    void setCloseOnSend(bool b);
    void send();

    void textChanged();

private:
    QToolBar* createToolBar();
    QString m_bgColorName;
    QString m_txtColorName;
    QToolBar* m_bar;

    QTextEdit *m_edit;
    QTextEdit *m_editTrans;
    QTextEdit *m_editActive;
    bool m_bTranslationService;

    QVBoxLayout *m_layout;

    QComboBox * m_cmbLanguageIncomming;
    QComboBox * m_cmbLanguageOutgoing;

    QAction* m_sendAction;
    QAction* m_sendMultiple;

    QToolButton * m_cmdSend;

    IMContactPtr m_from;
    IMContactPtr m_to;

};
}

#endif /* GENERICMESSAGEEDITOR_H_ */
