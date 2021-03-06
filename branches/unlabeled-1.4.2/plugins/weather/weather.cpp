/***************************************************************************
                          weather.cpp  -  description
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

#include "weather.h"
#include "weathercfg.h"
#include "buffer.h"
#include "fetch.h"
#include "toolbtn.h"

#include "xpm/0.xpm"
#include "xpm/1.xpm"
#include "xpm/2.xpm"
#include "xpm/3.xpm"
#include "xpm/4.xpm"
#include "xpm/5.xpm"
#include "xpm/6.xpm"
#include "xpm/7.xpm"
#include "xpm/8.xpm"
#include "xpm/9.xpm"
#include "xpm/10.xpm"
#include "xpm/11.xpm"
#include "xpm/12.xpm"
#include "xpm/13.xpm"
#include "xpm/14.xpm"
#include "xpm/15.xpm"
#include "xpm/16.xpm"
#include "xpm/17.xpm"
#include "xpm/18.xpm"
#include "xpm/19.xpm"
#include "xpm/20.xpm"
#include "xpm/21.xpm"
#include "xpm/22.xpm"
#include "xpm/23.xpm"
#include "xpm/24.xpm"
#include "xpm/25.xpm"
#include "xpm/26.xpm"
#include "xpm/27.xpm"
#include "xpm/28.xpm"
#include "xpm/29.xpm"
#include "xpm/30.xpm"
#include "xpm/31.xpm"
#include "xpm/32.xpm"
#include "xpm/33.xpm"
#include "xpm/34.xpm"
#include "xpm/35.xpm"
#include "xpm/36.xpm"
#include "xpm/37.xpm"
#include "xpm/38.xpm"
#include "xpm/39.xpm"
#include "xpm/40.xpm"
#include "xpm/41.xpm"
#include "xpm/42.xpm"
#include "xpm/43.xpm"
#include "xpm/44.xpm"
#include "xpm/45.xpm"
#include "xpm/46.xpm"
#include "xpm/47.xpm"
#include "xpm/na.xpm"

#include <time.h>

#include <qapplication.h>
#include <qwidgetlist.h>
#include <qtoolbar.h>
#include <qmainwindow.h>
#include <qtimer.h>
#include <qfile.h>
#include <qregexp.h>
#include <qtooltip.h>

const unsigned CHECK_INTERVAL = 30 * 60;

Plugin *createWeatherPlugin(unsigned base, bool bInit, const char *config)
{
    Plugin *plugin = new WeatherPlugin(base, bInit, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Weather"),
        I18N_NOOP("Plugin provides show weather"),
        VERSION,
        createWeatherPlugin,
        PLUGIN_NOLOAD_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

static DataDef weatherData[] =
    {
        { "ID", DATA_STRING, 1, 0 },
		{ "Location", DATA_STRING, 1, 0 },
        { "Time", DATA_LONG, 1, 0 },
        { "Text", DATA_UTF, 1, 0 },
        { "Tip", DATA_UTF, 1, 0 },
		{ "Units", DATA_BOOL, 1, 0 },
        { "Bar", DATA_LONG, 7, DATA(QMainWindow::Bottom) },
        { "Updated", DATA_STRING, 1, 0 },
        { "Temperature", DATA_LONG, 1, 0 },
        { "Humidity", DATA_LONG, 1, 0 },
        { "Pressure", DATA_LONG, 1, 0 },
        { "Conditions", DATA_STRING, 1, 0 },
        { "Wind", DATA_STRING, 1, 0 },
        { "Wind_speed", DATA_LONG, 1, 0 },
        { "Sun_raise", DATA_STRING, 1, 0 },
        { "Sun_set", DATA_STRING, 1, 0 },
		{ "Icon", DATA_LONG, 1, 0 },
		{ "UT", DATA_STRING, 1, 0 },
		{ "US", DATA_STRING, 1, 0 },
		{ "UP", DATA_STRING, 1, 0 },
        { NULL, 0, 0, 0 }
    };

WeatherPlugin::WeatherPlugin(unsigned base, bool bInit, const char *config)
        : Plugin(base)
{
    load_data(weatherData, &data, config);
    memset(&m_handler, 0, sizeof(m_handler));
    m_handler.startElement = p_element_start;
    m_handler.endElement   = p_element_end;
    m_handler.characters   = p_char_data;
    BarWeather = registerType();
    CmdWeather = registerType();
    EventWeather = registerType();
    Event eBar(EventToolbarCreate, (void*)BarWeather);
    eBar.process();
    IconDef icon;
    icon.name = "weather";
    icon.xpm  = na;
    Event eIcon(EventAddIcon, &icon);
    eIcon.process();
    Command cmd;
    cmd->id = CmdWeather;
    cmd->text = I18N_NOOP("Not connected");
    cmd->icon = "weather";
    cmd->bar_id = BarWeather;
    cmd->bar_grp = 0x1000;
    cmd->flags = BTN_PICT;
    Event eCmd(EventCommandCreate, cmd);
    eCmd.process();
    m_bar = NULL;
    m_fetch_id = 0;
    if (!bInit){
        showBar();
        if (m_bar)
            m_bar->show();
    }
}

WeatherPlugin::~WeatherPlugin()
{
    if (m_bar)
        delete m_bar;
    Event eCmd(EventCommandRemove, (void*)CmdWeather);
    Event eBar(EventToolbarRemove, (void*)BarWeather);
    free_data(weatherData, &data);
}

string WeatherPlugin::getConfig()
{
    if (m_bar)
        saveToolbar(m_bar, data.bar);
    return save_data(weatherData, &data);
}

void WeatherPlugin::timeout()
{
    if (!getSocketFactory()->isActive() || m_fetch_id || (*getID() == 0))
        return;
    time_t now;
    time(&now);
    if ((unsigned)now < getTime() + CHECK_INTERVAL)
        return;
	string url = "http://xoap.weather.com/weather/local/";
	url += getID();
	url += "?cc=*&prod=xoap&par=1004517364&key=a29796f587f206b2&unit=";
	url += getUnits() ? "s" : "m";
    m_fetch_id = fetch(url.c_str());
}

void *WeatherPlugin::processEvent(Event *e)
{
    if (e->type() == EventLanguageChanged)
        updateButton();
    if (e->type() == EventInit)
        showBar();
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if ((cmd->id == CmdWeather) && *getID()){
			string url = "http://www.weather.com/outlook/travel/pastweather/";
			url += getID();
            Event eGo(EventGoURL, (void*)url.c_str());
            eGo.process();
            return e->param();
        }
    }
    if (e->type() == EventFetchDone){
        fetchData *d = (fetchData*)(e->param());
        if (d->req_id != m_fetch_id)
            return NULL;
        m_fetch_id = 0;
        if (d->result != 200)
            return NULL;
		m_data  = "";
		m_bBar  = false;
		m_bWind = false;
		m_bUv	= false;
	    m_context = xmlCreatePushParserCtxt(&m_handler, this, "", 0, "");
		if (xmlParseChunk(m_context, d->data->data(), d->data->size(), 0)){
		    log(L_WARN, "XML parse error");
		    xmlFreeParserCtxt(m_context);
			return NULL;
		}
	    xmlFreeParserCtxt(m_context);
        time_t now;
        time(&now);
        setTime(now);
        updateButton();
        Event eUpdate(EventWeather);
        eUpdate.process();
    }
    return NULL;
}

void WeatherPlugin::barDestroyed()
{
    m_bar = NULL;
}

void WeatherPlugin::hideBar()
{
    if (m_bar){
        delete m_bar;
        m_bar = NULL;
    }
}

bool WeatherPlugin::parseTime(const char *str, int &h, int &m)
{
    string s = str;
    h = atol(getToken(s, ':').c_str());
    m = atol(getToken(s, ' ').c_str());
    if (getToken(s, ' ') == "PM")
        h += 12;
    return true;
}

bool WeatherPlugin::isDay()
{
    int raise_h, raise_m;
    int set_h, set_m;
    if (!parseTime(getSun_raise(), raise_h, raise_m) || !parseTime(getSun_set(), set_h, set_m))
        return false;
    time_t now;
    time(&now);
    struct tm *tm = localtime(&now);
    if ((tm->tm_hour > raise_h) && (tm->tm_hour < set_h))
        return true;
    if ((tm->tm_hour == raise_h) && (tm->tm_min >= raise_m))
        return true;
    if ((tm->tm_hour == set_h) && (tm->tm_min <= set_m))
        return true;
    return false;
}

void WeatherPlugin::showBar()
{
    if (m_bar || (*getID() == 0))
        return;
    QWidgetList  *list = QApplication::topLevelWidgets();
    QWidgetListIt it( *list );
    QWidget *w;
    while ((w=it.current()) != 0) {
        ++it;
        if (w->inherits("MainWindow"))
            break;
    }
    delete list;
    if (w == NULL)
        return;
    BarShow b;
    b.bar_id = BarWeather;
    b.parent = (QMainWindow*)w;
    Event e(EventShowBar, &b);
    m_bar = (QToolBar*)e.process();
    restoreToolbar(m_bar, data.bar);
    connect(m_bar, SIGNAL(destroyed()), this, SLOT(barDestroyed()));
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    QTimer::singleShot(0, this, SLOT(timeout()));
    timer->start(120000);
    updateButton();
}

static const char **xpms[] = 
{
	xpm_0,
	xpm_1,
	xpm_2,
	xpm_3,
	xpm_4,
	xpm_5,
	xpm_6,
	xpm_7,
	xpm_8,
	xpm_9,
	xpm_10,
	xpm_11,
	xpm_12,
	xpm_13,
	xpm_14,
	xpm_15,
	xpm_16,
	xpm_17,
	xpm_18,
	xpm_19,
	xpm_20,
	xpm_21,
	xpm_22,
	xpm_23,
	xpm_24,
	xpm_25,
	xpm_26,
	xpm_27,
	xpm_28,
	xpm_29,
	xpm_30,
	xpm_31,
	xpm_32,
	xpm_33,
	xpm_34,
	xpm_35,
	xpm_36,
	xpm_37,
	xpm_38,
	xpm_39,
	xpm_40,
	xpm_41,
	xpm_42,
	xpm_43,
	xpm_44,
	xpm_45,
	xpm_46,
	xpm_47
};

void WeatherPlugin::updateButton()
{
    if ((getTime() == 0) || (m_bar == NULL))
        return;
    const char **xpm = xpms[getIcon()];
    if (xpm){
        IconDef icon;
        icon.name = "weather";
        icon.xpm  = xpm;
        Event eIcon(EventAddIcon, &icon);
        eIcon.process();
    }
    QString text = unquoteText(getButtonText());
    QString tip = getTipText();
    text = replace(text);
    tip  = replace(tip);
    Command cmd;
    cmd->id		= CmdWeather;
    cmd->param	= m_bar;
    Event e(EventCommandWidget, cmd);
    CToolButton *btn = (CToolButton*)e.process();
    if (btn == NULL)
        return;
    btn->setTextLabel(text);
    btn->repaint();
    QToolTip::add(btn, tip);
}

static QString number(unsigned long n)
{
    char b[32];
    sprintf(b, "%i", n);
    return b;
}

#if 0
i18n("weather", "Overcast")
i18n("weather", "Fog")
i18n("weather", "Storm")
i18n("weather", "Rain")
i18n("weather", "Light Rain")
i18n("weather", "Few Snow Showers")
i18n("weather", "Scattered Snow Showers")
i18n("weather", "Snow Shower")
i18n("weather", "Clear")
i18n("weather", "Showers")
i18n("weather", "Mostly Clear")
i18n("weather", "Sunny")
i18n("weather", "Fair")
i18n("weather", "Cloudy")
i18n("weather", "Mostly Cloudy")
i18n("weather", "Partly Cloudy")
#endif

QString WeatherPlugin::replace(const QString &text)
{
    QString res = text;
    res = res.replace(QRegExp("\\%t"), number(getTemperature()) + QChar((unsigned short)176) + getUT());
    res = res.replace(QRegExp("\\%h"), number(getHumidity()) + "%");
    res = res.replace(QRegExp("\\%w"), number(getWind_speed()) + " " + getUS());
    res = res.replace(QRegExp("\\%i"), number(getPressure()) + " " + getUP());
    res = res.replace(QRegExp("\\%l"), getLocation());
    res = res.replace(QRegExp("\\%r"), getWind());
    res = res.replace(QRegExp("\\%u"), getUpdated());
    res = res.replace(QRegExp("\\%p"), getSun_raise());
    res = res.replace(QRegExp("\\%q"), getSun_set());
    res = res.replace(QRegExp("\\%c"), i18n("weather", getConditions()));
    return res;
}

QString WeatherPlugin::getButtonText()
{
    QString str = getText();
    if (str.isEmpty())
        str = i18n("%t %c");
    return str;
}


QString WeatherPlugin::getTipText()
{
    QString str = getTip();
    if (str.isEmpty())
        str = i18n("%l<br><br>\n"
                   "<img src=\"icon:weather\"> %c<br>\n"
                   "Temperature: <b>%t</b><br>\n"
                   "Humidity: <b>%h</b><br>\n"
                   "Pressure: <b>%i</b><br>\n"
                   "Wind: <b>%r</b> <b>%w</b><br>\n"
                   "Sunrise: %p<br>\n"
                   "Sunset: %q<br>\n"
                   "<br>\n"
                   "Updated: %u<br>\n"
				   "Weather data provided by weather.com") + QChar((unsigned short)176);
    return str;
}

QWidget *WeatherPlugin::createConfigWindow(QWidget *parent)
{
    return new WeatherCfg(parent, this);
}

static const char *tags[] = 
{
	"obst",
	"lsup",
	"sunr",
	"suns",
	"tmp",
	"hmid",
	"t",
	"icon",
	"r",
	"s",
	"ut",
	"us",
	"up",
	NULL,
};

void WeatherPlugin::element_start(const char *el, const char**)
{
	m_bData = false;
	if (!strcmp(el, "bar")){
		m_bBar = true;
		return;
	}
	if (!strcmp(el, "wind")){
		m_bWind = true;
		return;
	}
	if (!strcmp(el, "uv")){
		m_bUv = true;
		return;
	}
	for (const char **p = tags; *p; p++){
		if (!strcmp(*p, el)){
			m_bData = true;
			m_data  = "";
			return;
		}
	}
}

void WeatherPlugin::element_end(const char *el)
{
	if (!strcmp(el, "obst")){
		setLocation(m_data.c_str());
		m_data = "";
		return;
	}
	if (!strcmp(el, "lsup")){
		setUpdated(m_data.c_str());
		m_data = "";
		return;
	}
	if (!strcmp(el, "sunr")){
		setSun_raise(m_data.c_str());
		m_data = "";
		return;
	}
	if (!strcmp(el, "suns")){
		setSun_set(m_data.c_str());
		m_data = "";
		return;
	}
	if (!strcmp(el, "tmp")){
		setTemperature(atol(m_data.c_str()));
		m_data = "";
		return;
	}
	if (!strcmp(el, "hmid")){
		setHumidity(atol(m_data.c_str()));
		m_data = "";
		return;
	}
	if (!strcmp(el, "t")){
		if (!m_bBar && !m_bWind && !m_bUv)
			setConditions(m_data.c_str());
		if (m_bWind)
			setWind(m_data.c_str());
		m_data = "";
		return;
	}
	if (!strcmp(el, "icon")){
		setIcon(atol(m_data.c_str()));
		m_data = "";
		return;
	}
	if (!strcmp(el, "ut")){
		setUT(m_data.c_str());
		m_data = "";
		return;
	}
	if (!strcmp(el, "up")){
		setUP(m_data.c_str());
		m_data = "";
		return;
	}
	if (!strcmp(el, "us")){
		setUS(m_data.c_str());
		m_data = "";
		return;
	}
	if (!strcmp(el, "bar")){
		m_bBar = false;
		return;
	}
	if (!strcmp(el, "r") && m_bBar){
		unsigned long v = 0;
		for (const char *p = m_data.c_str(); *p; p++){
			if (*p == '.')
				break;
			if (*p == ',')
				continue;
			v = (v * 10) + (*p - '0');
		}
		setPressure(v);
		return;
	}
	if (!strcmp(el, "wind")){
		m_bWind = false;
		return;
	}
	if (!strcmp(el, "s") && m_bWind){
		setWind_speed(atol(m_data.c_str()));
		return;
	}
	if (!strcmp(el, "uv")){
		m_bUv = false;
		return;
	}
}

void WeatherPlugin::char_data(const char *str, int len)
{
	if (m_bData)
		m_data.append(str, len);
}

void WeatherPlugin::p_element_start(void *data, const xmlChar *el, const xmlChar **attr)
{
    ((WeatherPlugin*)data)->element_start((char*)el, (const char**)attr);
}

void WeatherPlugin::p_element_end(void *data, const xmlChar *el)
{
    ((WeatherPlugin*)data)->element_end((char*)el);
}

void WeatherPlugin::p_char_data(void *data, const xmlChar *str, int len)
{
    ((WeatherPlugin*)data)->char_data((char*)str, len);
}

#ifdef WIN32
#include <windows.h>

/**
 * DLL's entry point
 **/
int WINAPI DllMain(HINSTANCE, DWORD, LPVOID)
{
    return TRUE;
}

/**
 * This is to prevent the CRT from loading, thus making this a smaller
 * and faster dll.
 **/
extern "C" BOOL __stdcall _DllMainCRTStartup( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return DllMain( hinstDLL, fdwReason, lpvReserved );
}

#endif

#ifndef WIN32
#include "weather.moc"
#endif


