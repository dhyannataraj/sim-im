# Microsoft Developer Studio Project File - Name="yahoo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=yahoo - Win32 Debug
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "yahoo.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "yahoo.mak" CFG="yahoo - Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "yahoo - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "yahoo - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "yahoo - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "yahoo_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "$(QTDIR)\include" /I "..\..\sim" /I "..\_core" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_THREAD_SUPPORT" /D VERSION=\"0.9.4\" /D PACKAGE=\"SIM\" /D USE_QT=1 /D "CVS_BUILD" /D "USE_OPENSSL" /FR"Debug/" /Fo"Debug/" /Fd"Debug/" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "..\.." /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ..\..\Release\sim.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib $(QTDIR)\lib\$(QTLIB) /nologo /dll /pdb:none /machine:I386 /out:"../../Release/plugins/yahoo.dll"

!ELSEIF  "$(CFG)" == "yahoo - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "yahoo_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /I "..\..\sim" /I "..\_core" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_THREAD_SUPPORT" /D VERSION=\"0.9.4\" /D PACKAGE=\"SIM\" /D USE_QT=1 /D "DEBUG" /D CVS_BUILD=1 /D "USE_OPENSSL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "..\.." /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\Debug\sim.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib $(QTDIR)\lib\$(QTLIB) /nologo /dll /debug /machine:I386 /out:"../../Debug/plugins/yahoo.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "yahoo - Win32 Release"
# Name "yahoo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\crypt.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.c
# End Source File
# Begin Source File

SOURCE=.\moc_yahoocfg.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_yahoocfgbase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_yahooclient.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_yahooinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_yahooinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_yahoosearch.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_yahoosearchbase.cpp
# End Source File
# Begin Source File

SOURCE=.\sha.c
# End Source File
# Begin Source File

SOURCE=.\yahoo.cpp
# End Source File
# Begin Source File

SOURCE=.\yahooauth.cpp
# End Source File
# Begin Source File

SOURCE=.\yahoocfg.cpp
# End Source File
# Begin Source File

SOURCE=.\yahoocfgbase.cpp
# End Source File
# Begin Source File

SOURCE=.\yahooclient.cpp
# End Source File
# Begin Source File

SOURCE=.\yahoohttp.cpp
# End Source File
# Begin Source File

SOURCE=.\yahooinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\yahooinfobase.cpp
# End Source File
# Begin Source File

SOURCE=.\yahoosearch.cpp
# End Source File
# Begin Source File

SOURCE=.\yahoosearchbase.cpp
# End Source File
# Begin Source File

SOURCE=.\yahootransformtables.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\sha.h
# End Source File
# Begin Source File

SOURCE=.\yahoo.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\yahoo_fn.h
# End Source File
# Begin Source File

SOURCE=.\yahoocfg.h

!IF  "$(CFG)" == "yahoo - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\yahoocfg.h
InputName=yahoocfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "yahoo - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\yahoocfg.h
InputName=yahoocfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\yahoocfgbase.h
# End Source File
# Begin Source File

SOURCE=.\yahooclient.h

!IF  "$(CFG)" == "yahoo - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\yahooclient.h
InputName=yahooclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "yahoo - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\yahooclient.h
InputName=yahooclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\yahooinfo.h

!IF  "$(CFG)" == "yahoo - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\yahooinfo.h
InputName=yahooinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "yahoo - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\yahooinfo.h
InputName=yahooinfo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\yahooinfobase.h
# End Source File
# Begin Source File

SOURCE=.\yahoosearch.h

!IF  "$(CFG)" == "yahoo - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\yahoosearch.h
InputName=yahoosearch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "yahoo - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\yahoosearch.h
InputName=yahoosearch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\yahoosearchbase.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\yahoo.rc
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=.\yahoocfgbase.ui

!IF  "$(CFG)" == "yahoo - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\yahoocfgbase.ui
InputName=yahoocfgbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "yahoo - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\yahoocfgbase.ui
InputName=yahoocfgbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\yahooinfobase.ui

!IF  "$(CFG)" == "yahoo - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\yahooinfobase.ui
InputName=yahooinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "yahoo - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\yahooinfobase.ui
InputName=yahooinfobase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\yahoosearchbase.ui

!IF  "$(CFG)" == "yahoo - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\yahoosearchbase.ui
InputName=yahoosearchbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "yahoo - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\yahoosearchbase.ui
InputName=yahoosearchbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
