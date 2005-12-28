# Microsoft Developer Studio Project File - Name="sound" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sound - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sound.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sound.mak" CFG="sound - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sound - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sound - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sound - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "sound_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /WX /O1 /I "$(QTDIR)\include" /I "..\..\sim" /I "..\_core" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_THREAD_SUPPORT" /D VERSION=\"0.9.4\" /D PACKAGE=\"SIM\" /D USE_QT=1 /FR"Debug/" /Fo"Debug/" /Fd"Debug/" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "..\.." /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib $(QTDIR)\lib\qt-mt230nc.lib ..\..\Release\sim.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Release/plugins/sound.dll"

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "sound_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /WX /Gm /ZI /Od /I "$(QTDIR)\include" /I "..\..\sim" /I "..\_core" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_THREAD_SUPPORT" /D VERSION=\"0.9.4\" /D PACKAGE=\"SIM\" /D USE_QT=1 /D "DEBUG" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "..\.." /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib $(QTDIR)\lib\qt-mt230nc.lib ..\..\Debug\sim.lib /nologo /dll /debug /machine:I386 /out:"../../Debug/plugins/sound.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "sound - Win32 Release"
# Name "sound - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\moc_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_soundconfig.cpp
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\moc_soundconfigbase.cpp
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\moc_sounduser.cpp
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\moc_sounduserbase.cpp
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\soundconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\soundconfigbase.cpp
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\sounduser.cpp
# End Source File
# Begin Source File

SOURCE=.\sounduserbase.cpp
# ADD CPP /W3
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\sound.h

!IF  "$(CFG)" == "sound - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\sound.h
InputName=sound

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\sound.h
InputName=sound

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soundconfig.h

!IF  "$(CFG)" == "sound - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\soundconfig.h
InputName=soundconfig

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\soundconfig.h
InputName=soundconfig

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soundconfigbase.h

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\soundconfigbase.h
InputName=soundconfigbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\soundconfigbase.h
InputName=soundconfigbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounduser.h

!IF  "$(CFG)" == "sound - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\sounduser.h
InputName=sounduser

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\sounduser.h
InputName=sounduser

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounduserbase.h

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\sounduserbase.h
InputName=sounduserbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\sounduserbase.h
InputName=sounduserbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\sound.rc
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=.\soundconfigbase.ui

!IF  "$(CFG)" == "sound - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\soundconfigbase.ui
InputName=soundconfigbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\soundconfigbase.ui
InputName=soundconfigbase

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

SOURCE=.\sounduserbase.ui

!IF  "$(CFG)" == "sound - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\sounduserbase.ui
InputName=sounduserbase

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\sounduserbase.ui
InputName=sounduserbase

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
# Begin Group "Sound"

# PROP Default_Filter "wav"
# Begin Source File

SOURCE=.\sounds\alert.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\alert.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\alert.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\auth.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\auth.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\auth.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\contacts.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\contacts.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\contacts.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\file.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\file.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\file.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\filedone.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\filedone.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\filedone.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\message.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\message.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\message.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\message2.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\message2.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\message2.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\msgsent.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\msgsent.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\msgsent.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\sms.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\sms.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\sms.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\startup.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\startup.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\startup.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\system.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\system.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\system.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounds\url.wav

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=.\sounds\url.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=.\sounds\url.wav

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\sounds\vip-online.wav"

!IF  "$(CFG)" == "sound - Win32 Release"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Release
InputPath=".\sounds\vip-online.wav"

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "sound - Win32 Debug"

# Begin Custom Build - Copy $(InputPath)
OutDir=.\..\..\Debug
InputPath=".\sounds\vip-online.wav"

"$(OutDir)\$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y $(InputPath) $(OutDir)\$(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
