; Script generated by the My Inno Setup Extensions Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#include "isxdl.iss"

[Setup]
AppName=Simple Instant Messenger
AppVerName=SIM 0.8.1
AppPublisher=shutoff@mail.ru
AppPublisherURL=http://sim-icq.sourceforge.net/
AppSupportURL=http://sim-icq.sourceforge.net/
AppUpdatesURL=http://sim-icq.sourceforge.net/
DefaultDirName={pf}\SIM
DisableProgramGroupPage=yes
DisableReadyPage=yes
LicenseFile=C:\sim\COPYING
Compression=bzip/9
AppId=SIM
AppMutex=SIM_Mutex
AppCopyright=Copyright � 2002, Vladimir Shutoff

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"

[Dirs]
Name: "{app}\po"
Name: "{app}\sounds"
Name: "{app}\pict"
Name: "{app}\icons"

[Files]
Source: "C:\sim\Release\sim.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\xpstyle.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\zh_TW.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\cs.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\de.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\es.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\it.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\nl.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\pl.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\ru.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\tr.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\Release\po\uk.qm"; DestDir: "{app}\po"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\url.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\alert.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\auth.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\chat.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\file.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\filedone.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\message.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\sms.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\sounds\startup.wav"; DestDir: "{app}\sounds"; CopyMode: alwaysoverwrite
Source: "C:\sim\pict\splash.png"; DestDir: "{app}\pict"; CopyMode: alwaysoverwrite

[Icons]
Name: "{commonprograms}\SIM"; Filename: "{app}\sim.exe"
Name: "{userdesktop}\Simple Instant Messenger"; Filename: "{app}\sim.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\sim.exe"; Description: "Launch Simple Instant Messenger"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: {userappdata}\sim

[Code]
const url1 = 'http://osdn.dl.sourceforge.net/sourceforge/sim-icq/qt.exe';
const url2 = 'http://osdn.dl.sourceforge.net/sourceforge/sim-icq/ssl.exe';

function NextButtonClick(CurPage: Integer): Boolean;
var
  hWnd: Integer;
  sFileName: String;
  nCode: Integer;
  bDownload: Boolean;
  sParam: String;
begin
  Result := true;
  
  if CurPage = wpSelectTasks then begin
    hWnd := StrToInt(ExpandConstant('{wizardhwnd}'));
    isxdl_ClearFiles;
    sFileName := ExpandConstant('{app}\libeay32.dll');
    if not FileExists(sFileName) then begin
      bDownload := true;
    end;
    sFileName := ExpandConstant('{app}\ssleay32.dll');
    if not FileExists(sFileName) then begin
      bDownload := true;
    end;
    sFileName := ExpandConstant('{sys}\msvcrt.dll');
    if not FileExists(sFileName) then begin
      bDownload := true;
    end;
    if bDownload then begin
      isxdl_AddFileSize(url2, ExpandConstant('{tmp}\ssl.exe'), 910557);
    end;

    sFileName := ExpandConstant('{app}\qt-mt230nc.dll');
    if not FileExists(sFileName) then begin
      isxdl_AddFileSize(url1, ExpandConstant('{tmp}\qt.exe'), 1658577);
      bDownload := true;
    end;

    if bDownload then begin
      ShowWindow(hWnd,SW_HIDE);

      if isxdl_DownloadFiles(hWnd) <> 0 then begin
        ShowWindow(hWnd,SW_SHOWNORMAL);
        sParam := ExpandConstant('/VERYSILENT /DIR="{app}"');
        sFileName := ExpandConstant('{tmp}\qt.exe');
        if FileExists(sFileName) then InstExec(sFileName, sParam, '', true, false, 0, nCode)
        sFileName := ExpandConstant('{tmp}\ssl.exe');
        if FileExists(sFileName) then InstExec(sFileName, sParam, '', true, false, 0, nCode)
      end else begin
        Result := false;
        ShowWindow(hWnd,SW_SHOWNORMAL);
      end;
    end;
  end;
end;

function InitializeSetup: Boolean;
begin
  isxdl_SetOption('title','Downloading lots of files...');
  isxdl_SetOption('noftpsize','false');
  isxdl_SetOption('aborttimeout','15');

  Result := true;
end;


