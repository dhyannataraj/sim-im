; Script generated by the My Inno Setup Extensions Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#include "simfiles.iss"
#include "isxdl.iss"

[Setup]
OutputBaseFilename=sim-0.9.3
MinVersion=4.1,4

[Files]

[Code]
const url1 = 'http://sim-icq.sf.net/qt.exe';
const url2 = 'http://sim-icq.sf.net/ssl.exe';
const url3 = 'http://sim-icq.sf.net/libxml.exe';
const url4 = 'http://sim-icq.sf.net/msvcrt.exe';
const url5 = 'http://sim-icq.sf.net/opengl.exe';

function NextButtonClick(CurPage: Integer): Boolean;
var
  hWnd: Integer;
  sFileName: String;
  nCode: Integer;
  bDownloadQt: Boolean;
  bDownloadSSL: Boolean;
  bDownloadXML: Boolean;
  bDownloadMsvcrt: Boolean;
  bDownloadOpengl: Boolean;
  sParam: String;
begin
  Result := true;
  bDownloadQt := false;
  bDownloadSSL := false;
  bDownloadXML := false;
  bDownloadMsvcrt := false;
  bDownloadOpengl := false;

  if CurPage = wpSelectTasks then begin
    hWnd := StrToInt(ExpandConstant('{wizardhwnd}'));
    isxdl_ClearFiles;
    sFileName := ExpandConstant('{app}\libeay32.dll');
    if not FileExists(sFileName) then begin
      bDownloadSSL := true;
    end;
    sFileName := ExpandConstant('{app}\ssleay32.dll');
    if not FileExists(sFileName) then begin
      bDownloadSSL := true;
    end;
    sFileName := ExpandConstant('{sys}\msvcrt.dll');
    if not FileExists(sFileName) then begin
      bDownloadMsvcrt := true;
    end;
    sFileName := ExpandConstant('{sys}\msvcp60.dll');
    if not FileExists(sFileName) then begin
      bDownloadMsvcrt := true;
    end;
    sFileName := ExpandConstant('{sys}\opengl32.dll');
    if not FileExists(sFileName) then begin
      bDownloadOpengl := true;
    end;
    sFileName := ExpandConstant('{sys}\glu32.dll');
    if not FileExists(sFileName) then begin
      bDownloadOpengl := true;
    end;
    sFileName := ExpandConstant('{app}\libxml2.dll');
    if not FileExists(sFileName) then begin
      bDownloadXML := true;
    end;
    sFileName := ExpandConstant('{app}\libxslt.dll');
    if not FileExists(sFileName) then begin
      bDownloadXML := true;
    end;

    sFileName := ExpandConstant('{app}\qt-mt230nc.dll');
    if not FileExists(sFileName) then begin
      isxdl_AddFileSize(url1, ExpandConstant('{tmp}\qt.exe'), 1263204);
      bDownloadQt := true;
    end;
    if bDownloadSSL then begin
      isxdl_AddFileSize(url2, ExpandConstant('{tmp}\ssl.exe'), 575447);
    end;
    if bDownloadXML then begin
      isxdl_AddFileSize(url3, ExpandConstant('{tmp}\libxml.exe'), 571422);
    end;
    if bDownloadMsvcrt then begin
      isxdl_AddFileSize(url4, ExpandConstant('{tmp}\msvcrt.exe'), 483886);
    end;
    if bDownloadOpengl then begin
      isxdl_AddFileSize(url5, ExpandConstant('{tmp}\opengl.exe'), 579482);
    end;

    if bDownloadQt or bDownloadSSL or bDownloadXML or bDownloadMsvcrt or bDownloadOpengl then begin
      if isxdl_DownloadFiles(hWnd) <> 0 then begin
        sParam := ExpandConstant('/VERYSILENT /DIR="{app}"');
        if (bDownloadQt) then begin
            sFileName := ExpandConstant('{tmp}\qt.exe');
            if FileExists(sFileName) then
              InstExec(sFileName, sParam, '', true, false, 0, nCode)
            else
              Result := false
        end;
        if (Result and bDownloadSSL) then begin
            sFileName := ExpandConstant('{tmp}\ssl.exe');
            if FileExists(sFileName) then
               InstExec(sFileName, sParam, '', true, false, 0, nCode)
            else
              Result := false;
        end
        if (Result and bDownloadOpengl) then begin
            sFileName := ExpandConstant('{tmp}\opengl.exe');
            if FileExists(sFileName) then
               InstExec(sFileName, sParam, '', true, false, 0, nCode)
            else
              Result := false;
        end
        if (Result and bDownloadMsvcrt) then begin
            sFileName := ExpandConstant('{tmp}\msvcrt.exe');
            if FileExists(sFileName) then
               InstExec(sFileName, sParam, '', true, false, 0, nCode)
            else
              Result := false;
        end
        if (Result and bDownloadXML) then begin
            sFileName := ExpandConstant('{tmp}\libxml.exe');
            if FileExists(sFileName) then
               InstExec(sFileName, sParam, '', true, false, 0, nCode)
            else
              Result := false;
            end
        end else begin
          Result := false;
      end;
    end;
 end;
end;

function InitializeSetup: Boolean;
begin
  isxdl_SetOption('title','Downloading lots of files...');
  isxdl_SetOption('noftpsize','false');
  isxdl_SetOption('aborttimeout','15');
  DeleteFile(ExpandConstant('{userstartup}\SIM.lnk'));
  DeleteFile(ExpandConstant('{commonstartup}\SIM.lnk'));
  Result := true;
end;


