# Microsoft Developer Studio Project File - Name="jabber" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=jabber - Win32 Debug
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "jabber.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "jabber.mak" CFG="jabber - Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "jabber - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "jabber - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release"
# PROP Intermediate_Dir "..\..\Release_Wrk"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "JABBER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /O1 /I "$(QTDIR)\include" /I "..\..\sim\api" /I "..\..\sim\ui" /I "C:\openssl\include" /I "..\_core" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_THREAD_SUPPORT" /D VERSION=\"0.9\" /D PACKAGE=\"SIM\" /D USE_QT=1 /D "USE_OPENSSL" /FR"Debug/" /Fo"Debug/" /Fd"Debug/" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ..\..\Release\simapi.lib ..\..\Release\simui.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib $(QTDIR)\lib\qt-mt230nc.lib $(QTDIR)\lib\libexpat.lib c:\openssl\lib\libeay32.lib c:\openssl\lib\ssleay32.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Release/plugins/jabber.dll"

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug"
# PROP Intermediate_Dir "..\..\Debug_Wrk"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "JABBER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /ZI /Od /I "$(QTDIR)\include" /I "..\..\sim\api" /I "..\..\sim\ui" /I "C:\openssl\include" /I "..\_core" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_THREAD_SUPPORT" /D VERSION=\"0.9\" /D PACKAGE=\"SIM\" /D USE_QT=1 /D "USE_OPENSSL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\Debug\simapi.lib ..\..\Debug\simui.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib $(QTDIR)\lib\qt-mt230nc.lib $(QTDIR)\lib\libexpat.lib c:\openssl\lib\libeay32.lib c:\openssl\lib\ssleay32.lib /nologo /dll /debug /machine:I386 /out:"../../Debug/plugins/jabber.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "jabber - Win32 Release"
# Name "jabber - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\addresult.cpp
# End Source File
# Begin Source File

SOURCE=.\addresultbase.cpp
# End Source File
# Begin Source File

SOURCE=.\jabber.cpp
# End Source File
# Begin Source File

SOURCE=.\jabber_auth.cpp
# End Source File
# Begin Source File

SOURCE=.\jabber_rosters.cpp
# End Source File
# Begin Source File

SOURCE=.\jabber_ssl.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberaboutinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberaboutinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberadd.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberaddbase.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberclient.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberconfigbase.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberhomeinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberhomeinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\jabbermessage.cpp
# End Source File
# Begin Source File

SOURCE=.\jabbersearch.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberworkinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\jabberworkinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_addresult.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_addresultbase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberaboutinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberaboutinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberadd.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberaddbase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberclient.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberconfigbase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberhomeinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberhomeinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabbersearch.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberworkinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_jabberworkinfobase.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\addresult.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\addresult.h
InputName=addresult

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\addresult.h
InputName=addresult

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\addresultbase.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\addresultbase.h
InputName=addresultbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\addresultbase.h
InputName=addresultbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabber.h
# End Source File
# Begin Source File

SOURCE=.\jabber_ssl.h
# End Source File
# Begin Source File

SOURCE=.\jabberaboutinfo.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberaboutinfo.h
InputName=jabberaboutinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\jabberaboutinfo.h
InputName=jabberaboutinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberaboutinfobase.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\jabberaboutinfobase.h
InputName=jabberaboutinfobase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\jabberaboutinfobase.h
InputName=jabberaboutinfobase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberadd.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberadd.h
InputName=jabberadd

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\jabberadd.h
InputName=jabberadd

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberaddbase.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\jabberaddbase.h
InputName=jabberaddbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\jabberaddbase.h
InputName=jabberaddbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberclient.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberclient.h
InputName=jabberclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\jabberclient.h
InputName=jabberclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberconfig.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberconfig.h
InputName=jabberconfig

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\jabberconfig.h
InputName=jabberconfig

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberconfigbase.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\jabberconfigbase.h
InputName=jabberconfigbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\jabberconfigbase.h
InputName=jabberconfigbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberhomeinfo.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberhomeinfo.h
InputName=jabberhomeinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\jabberhomeinfo.h
InputName=jabberhomeinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberhomeinfobase.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\jabberhomeinfobase.h
InputName=jabberhomeinfobase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\jabberhomeinfobase.h
InputName=jabberhomeinfobase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberinfo.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberinfo.h
InputName=jabberinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\jabberinfo.h
InputName=jabberinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberinfobase.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\jabberinfobase.h
InputName=jabberinfobase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\jabberinfobase.h
InputName=jabberinfobase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabbermessage.h
# End Source File
# Begin Source File

SOURCE=.\jabbersearch.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabbersearch.h
InputName=jabbersearch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\jabbersearch.h
InputName=jabbersearch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberworkinfo.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberworkinfo.h
InputName=jabberworkinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\jabberworkinfo.h
InputName=jabberworkinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberworkinfobase.h

!IF  "$(CFG)" == "jabber - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\jabberworkinfobase.h
InputName=jabberworkinfobase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\jabberworkinfobase.h
InputName=jabberworkinfobase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "xpm"

# PROP Default_Filter "xpm"
# Begin Source File

SOURCE=.\xpm\jabber_away.xpm
# End Source File
# Begin Source File

SOURCE=.\xpm\jabber_dnd.xpm
# End Source File
# Begin Source File

SOURCE=.\xpm\jabber_invisible.xpm
# End Source File
# Begin Source File

SOURCE=.\xpm\jabber_na.xpm
# End Source File
# Begin Source File

SOURCE=.\xpm\jabber_offline.xpm
# End Source File
# Begin Source File

SOURCE=.\xpm\jabber_online.xpm
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=.\addresultbase.ui

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\addresultbase.ui
InputName=addresultbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\addresultbase.ui
InputName=addresultbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberaboutinfobase.ui

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberaboutinfobase.ui
InputName=jabberaboutinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\jabberaboutinfobase.ui
InputName=jabberaboutinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberaddbase.ui

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberaddbase.ui
InputName=jabberaddbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\jabberaddbase.ui
InputName=jabberaddbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberconfigbase.ui

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberconfigbase.ui
InputName=jabberconfigbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\jabberconfigbase.ui
InputName=jabberconfigbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberhomeinfobase.ui

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberhomeinfobase.ui
InputName=jabberhomeinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\jabberhomeinfobase.ui
InputName=jabberhomeinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberinfobase.ui

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberinfobase.ui
InputName=jabberinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\jabberinfobase.ui
InputName=jabberinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jabberworkinfobase.ui

!IF  "$(CFG)" == "jabber - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\jabberworkinfobase.ui
InputName=jabberworkinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "jabber - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\jabberworkinfobase.ui
InputName=jabberworkinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
