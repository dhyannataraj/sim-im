#ifndef _ENABLE_H

#include "defs.h"
#include "country.h"

#include <string>
using namespace std;

class QWidget;
class QComboBox;

void disableWidget(QWidget *);

void initCombo(QComboBox *cmb, unsigned short code, const ext_info *tbl);
void initTZCombo(QComboBox *cmb, char tz);

unsigned short getComboValue(QComboBox *cmb, const ext_info *tbl);
char getTZComboValue(QComboBox *cmb);

void set(string &s, const QString &str);
void set(QString &s, const string &str);

#ifdef WIN32

void setWndProc(QWidget*);

#define SET_WNDPROC	setWndProc(this);

#else

#define SET_WNDPROC

#endif


#endif
