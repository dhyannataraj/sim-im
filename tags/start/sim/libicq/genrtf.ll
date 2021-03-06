%{
    /***************************************************************************
                              clearhtml.ll  -  description
                                 -------------------
        begin                : Sun Mar 10 2002
        copyright            : (C) 2002 by Vladimir Shutoff
        email                : shutoff@mail.ru
     ***************************************************************************/

    /***************************************************************************
     *                                                                         *
     *   This program is free software; you can redistribute it and/or modify  *
     *   it under the terms of the GNU General Public License as published by  *
     *   the Free Software Foundation; either version 2 of the License, or     *
     *   (at your option) any later version.                                   *
     *                                                                         *
     ***************************************************************************/

#include <stdio.h>

#if WIN32
#define vsnprintf _vsnprintf
#endif
#include "icqclient.h"
#include "log.h"

#include <stack>

#if _MSC_VER > 1020
using namespace std;
#pragma warning(disable:4786)
#endif

#define TXT		1
#define TAG_START	2
#define TAG_CLOSE	3
#define ATTR		4
#define VALUE		5
#define TAG_END		6
#define SYMBOL		7
#define BR		8

%}

%option prefix="genrtf"

%x s_tag
%x s_attr
%x s_value
%x s_string
%x s_symbol
%%
"<br"\/?">"					{ return BR; }
"</"[A-Za-z]+">"				{ return TAG_END; }
"<"[A-Za-z]+					{ BEGIN(s_tag); return TAG_START; }
<s_tag>">"					{ BEGIN(INITIAL); return TAG_CLOSE; }
<s_tag>[A-Za-z]+				{ BEGIN(s_attr); return ATTR; }
<s_attr>"="					{ BEGIN(s_value); }
<s_attr>">"					{ BEGIN(s_tag); REJECT; }
<s_attr>[A-Za-z]				{ BEGIN(s_tag); unput(yytext[0]); }
<s_value>"\""					{ BEGIN(s_string); }
<s_value>[A-Za-z0-9]+				{ BEGIN(s_tag); return VALUE; }
               <s_string>"\""					{ BEGIN(s_tag); }
<s_string>[^\"]+				{ return VALUE; }
"&gt";?						{ return SYMBOL; }
"&lt";?						{ return SYMBOL; }
"&amp";?					{ return SYMBOL; }
"&quot";?					{ return SYMBOL; }
"&nbsp";?					{ return SYMBOL; }
.						{ return TXT; }
%%

class attr
{
public:
    attr(const char *_name)  { name = _name; }
    string name;
    string value;
};

class font
{
public:
    unsigned color;
    unsigned size;
    unsigned face;
};

int yywrap() { return 1; }

typedef struct tagDef
{
    const char *name;
    char ch;
} tagDef;

static tagDef tags[] = {
    { "lt", '<' },
    { "gt", '>' },
    { "amp", '&' },
    { "quot", ';' },
    { "nbsp;", ' ' },
    { "", 0 }
};

static bool eq(const char *s1, const char *s2)
{
    for (; *s1 && *s2; s1++, s2++){
        if (tolower(*s1) != tolower(*s2)) return false;
    }
    return (*s1 == 0) && (*s2 == 0);
}

string ICQClient::createRTF(const char *text, unsigned long foreColor)
{
    yy_current_buffer = yy_scan_string(text);
    string tag;
    stack<font> fonts;
    list<attr> attrs;
    list<unsigned long> colors;
    list<string> faces;
    const char *ptr;
    font f;
    f.color = 1;
    f.size  = 1;
    f.face  = 0;
    fonts.push(f);
    string face = "MS Sans Serif";
    faces.push_back(face);
    colors.push_back(foreColor);
    string res;
    const char *p;
    ptr = text;
    for (;;){
        int r = yylex();
        if (!r) break;
        switch (r){
        case TXT:
            for (p = yytext; *p; p++){
                if ((*p & 0x80) || (*p == '\\') || (*p == '{') || (*p == '}')){
                    char b[5];
                    snprintf(b, sizeof(b), "\\\'%02x", *p & 0xFF);
                    res += b;
                    continue;
                }
                res += *p;
            }
            break;
        case BR:
            res += "\n";
            break;
        case SYMBOL:{
                string s = yytext;
                if (s.length() && (s[s.length() - 1] == ';')){
                    s = s.substr(1, s.length() - 1);
                }else{
                    s = s.substr(1);
                }
                for (tagDef *t = tags; t->ch; t++){
                    if (s == t->name){
                        res += t->ch;
                        break;
                    }
                }
            }
        case TAG_CLOSE:{
                log(L_DEBUG, "Tag %s", tag.c_str());
                for (list<attr>::iterator it = attrs.begin(); it != attrs.end(); it++){
                    log(L_DEBUG, ">> %s=%s", (*it).name.c_str(), (*it).value.c_str());
                }
                if (eq(tag.c_str(), "b")){
                    res += " \\b ";
                }else if (eq(tag.c_str(), "i")){
                    res += " \\i ";
                }else if (eq(tag.c_str(), "u")){
                    res += " \\ul ";
                }else if (eq(tag.c_str(), "p")){
                    res += "\\pard ";
                }else if (eq(tag.c_str(), "font")){
                    bool bChange = false;
                    font f = fonts.top();
                    unsigned size = f.size;
                    for (list<attr>::iterator it = attrs.begin(); it != attrs.end(); it++){
                        if (eq((*it).name.c_str(), "color")){
                            unsigned long color = 0;
                            string val = (*it).value;
                            if (val[0] == '#'){
                                for (unsigned i = 0; i < 6; i++){
                                    char c = val[i+1];
                                    if ((c >= '0') && (c <= '9')){
                                        color = (color << 4) + (c - '0');
                                    }else if ((c >= 'a') && (c <= 'f')){
                                        color = (color << 4) + (c - 'a' + 10);
                                    }else if ((c >= 'A') && (c <= 'F')){
                                        color = (color << 4) + (c - 'A' + 10);
                                    }
                                }
                            }
                            unsigned n = 1;
			    list<unsigned long>::iterator it_color;
                            for (it_color = colors.begin(); it_color != colors.end(); it_color++, n++)
                                if ((*it_color) == color) break;
                            if (it_color == colors.end()) colors.push_back(color);
                            if (n != fonts.top().color){
                                f.color = n;
                                char b[16];
                                snprintf(b, sizeof(b), "\\cf%u", n);
                                res += b;
                                bChange = true;
                            }
                        }
                        if (eq((*it).name.c_str(), "face")){
                            unsigned n = 0;
			    list<string>::iterator it_face;
                            for (it_face = faces.begin(); it_face != faces.end(); it_face++, n++)
                                if ((*it_face) == (*it).value) break;
                            if (it_face == faces.end()) faces.push_back((*it).value);
                            if (n != f.face){
                                f.face = n;
                                char b[16];
                                snprintf(b, sizeof(b), "\\f%u", n);
                                res += b;
                                bChange = true;
                            }
                        }
                        if (eq((*it).name.c_str(), "style")){
                            char FONT_SIZE[] = "font-size:";
                            if (((*it).value.length() > strlen(FONT_SIZE)) && !memcmp((*it).value.c_str(), FONT_SIZE, strlen(FONT_SIZE))){
                                size = atol((*it).value.c_str() + strlen(FONT_SIZE)) * 2;
                                if (size == 0) size = f.size;
                            }
                        }
                        if (eq((*it).name.c_str(), "size")){
                            if (size == f.size){
                                size = atol((*it).value.c_str()) * 6;
                                if (size == 0) size = f.size;
                            }
                        }
                    }
                    if (size != f.size){
                        bChange = true;
                        char b[16];
                        snprintf(b, sizeof(b), "\\fs%u", size);
                        res += b;
                        f.size = size;
                    }
                    if (bChange) res += "\\highlight0 ";
                    fonts.push(f);
                }
                attrs.clear();
                break;
            }
        case TAG_START:
            tag = yytext + 1;
            break;
        case TAG_END:
            if (eq(yytext, "</b>")){
                res += " \\b0 ";
            }else if (eq(yytext, "</i>")){
                res += " \\i0 ";
            }else if (eq(yytext, "</u>")){
                res += " \\ul0 ";
            }else if (eq(yytext, "</p>")){
                res += " \\par\r\n";
            }else if (eq(yytext, "</font>")){
                if (fonts.size() > 1){
                    font f = fonts.top();
                    fonts.pop();
                    bool bChange = false;
                    if (fonts.top().color != f.color){
                        char b[16];
                        snprintf(b, sizeof(b), "\\cf%u", fonts.top().color);
                        bChange = true;
                        res += b;
                    }
                    if (fonts.top().size != f.size){
                        char b[16];
                        snprintf(b, sizeof(b), "\\fs%u", fonts.top().size);
                        bChange = true;
                    }
                    if (bChange) res += "\\highlight0 ";
                }
            }
            break;
        case ATTR:
            attrs.push_back(attr(yytext));
            break;
        case VALUE:
            attrs.back().value += yytext;
            break;
        }
    }
    string s;
    s = "{\\rtf1\\ansi\\def0\r\n";
    s += "{\\fonttbl";
    unsigned n = 0;
    for (list<string>::iterator it_face = faces.begin(); it_face != faces.end(); it_face++, n++){
        char b[16];
        snprintf(b, sizeof(b), "{\\f%u", n);
        s += b;
        if (strstr((*it_face).c_str(), "Times")){
            s += "\\froman";
        }else if (strstr((*it_face).c_str(), "Courier")){
            s += "\\fmodern";
        }else{
            s += "\\fswiss";
        }
        s += " ";
        char *p = (char*)((*it_face).c_str());
        char *r = strchr(p, '[');
        if (r){
            *r = 0;
            for (int n = strlen(p) - 1; n >= 0; n--){
                if (p[n] != ' ') break;
                p[n] = 0;
            }
        }
        s += p;
        s += ";}";
    }
    s += "}\r\n";
    s += "{\\colortbl ;";
    for (list<unsigned long>::iterator it_colors = colors.begin(); it_colors != colors.end(); it_colors++){
        char b[64];
        unsigned long c = *it_colors;
        snprintf(b, sizeof(b), "\\red%u\\green%u\\blue%u;", 
		(unsigned)((c >> 16) & 0xFF), 
		(unsigned)((c >> 8) & 0xFF), 
		(unsigned)(c & 0xFF));
        s += b;
    }
    s += "}\r\n";
    s += "\\viewkind4\\pard\\cf1\\f0 ";
    s += res;
    s += "}\r\n";
    yy_delete_buffer(yy_current_buffer);
    yy_current_buffer = NULL;
    return s;
}

