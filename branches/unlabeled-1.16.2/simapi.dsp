# Microsoft Developer Studio Project File - Name="simapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=simapi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "simapi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "simapi.mak" CFG="simapi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "simapi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "simapi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "simapi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release_Wrk"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMAPI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I "$(QTDIR)\include" /I "sim" /I "c:\openssl\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMAPI_EXPORTS" /D "QT_DLL" /D "UNICODE" /D "USE_OPENSSL" /D LTDL_SHLIB_EXT=\".dll\" /D LTDL_OBJDIR=\"\" /D "HAVE_STDIO_H" /D "HAVE_STRING_H" /D "HAVE_MALLOC_H" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 $(QTDIR)\lib\qtmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib $(QTDIR)\lib\qt-mt230nc.lib c:\openssl\lib\libeay32.lib c:\openssl\lib\ssleay32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug_Wrk"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMAPI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /WX /Gm /ZI /Od /I "$(QTDIR)\include" /I "sim" /I "c:\openssl\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMAPI_EXPORTS" /D "QT_DLL" /D "UNICODE" /D "USE_OPENSSL" /D LTDL_SHLIB_EXT=\".dll\" /D LTDL_OBJDIR=\"\" /D "HAVE_STDIO_H" /D "HAVE_STRING_H" /D "HAVE_MALLOC_H" /FR /Fp"..\Debug/simapi.pch" /YX /Fo"..\Debug/" /Fd"..\Debug/" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib $(QTDIR)\lib\qt-mt230nc.lib c:\openssl\lib\libeay32.lib c:\openssl\lib\ssleay32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "simapi - Win32 Release"
# Name "simapi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sim\api\aboutdata.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\cfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\cmddef.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\contacts.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\country.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\exec.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\fetch.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\html.cpp
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\sim\api\html.ll

!IF  "$(CFG)" == "simapi - Win32 Release"

# Begin Custom Build - Flex $(InputPath)
InputDir=.\sim\api
InputPath=.\sim\api\html.ll
InputName=html

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -o$(InputDir)\$(InputName).cpp $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

# Begin Custom Build - Flex $(InputPath)
InputDir=.\sim\api
InputPath=.\sim\api\html.ll
InputName=html

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -o$(InputDir)\$(InputName).cpp $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sim\api\log.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\ltdl.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\sim\api\message.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\moc_exec.cpp

!IF  "$(CFG)" == "simapi - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

# ADD CPP /W1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sim\api\moc_socket.cpp

!IF  "$(CFG)" == "simapi - Win32 Release"

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

# ADD CPP /W2 /WX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sim\api\moc_sockfactory.cpp

!IF  "$(CFG)" == "simapi - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

# ADD CPP /W1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sim\api\plugins.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\simapi.cpp

!IF  "$(CFG)" == "simapi - Win32 Release"

# ADD CPP /W3 /WX

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sim\api\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\sockfactory.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\sslclient.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\translit.cpp
# End Source File
# Begin Source File

SOURCE=.\sim\api\unquot.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\sim\api\aboutdata.h
# End Source File
# Begin Source File

SOURCE=.\sim\api\buffer.h
# End Source File
# Begin Source File

SOURCE=.\sim\api\cjk_variants.h
# End Source File
# Begin Source File

SOURCE=.\sim\api\exec.h

!IF  "$(CFG)" == "simapi - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sim\api
InputPath=.\sim\api\exec.h
InputName=exec

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sim\api
InputPath=.\sim\api\exec.h
InputName=exec

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sim\api\fetch.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\sim\api\html.h
# End Source File
# Begin Source File

SOURCE=.\sim\api\johab_hangul.h
# End Source File
# Begin Source File

SOURCE=.\sim\api\ltdl.h
# End Source File
# Begin Source File

SOURCE=.\sim\api\simapi.h
# End Source File
# Begin Source File

SOURCE=.\sim\api\socket.h

!IF  "$(CFG)" == "simapi - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\sim\api
InputPath=.\sim\api\socket.h
InputName=socket

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sim\api
InputPath=.\sim\api\socket.h
InputName=socket

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sim\api\sockfactory.h

!IF  "$(CFG)" == "simapi - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\sim\api
InputPath=.\sim\api\sockfactory.h
InputName=sockfactory

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "simapi - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sim\api
InputPath=.\sim\api\sockfactory.h
InputName=sockfactory

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sim\api\stl.h
# End Source File
# Begin Source File

SOURCE=.\sim\api\translit.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
