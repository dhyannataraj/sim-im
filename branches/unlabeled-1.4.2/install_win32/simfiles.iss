; Script generated by the My Inno Setup Extensions Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=Simple Instant Messenger
AppVerName=SIM 0.9
AppPublisher=shutoff@mail.ru
AppPublisherURL=http://sim-icq.sourceforge.net/
AppSupportURL=http://sim-icq.sourceforge.net/
AppUpdatesURL=http://sim-icq.sourceforge.net/
DefaultDirName={pf}\SIM
DisableProgramGroupPage=yes
DisableReadyPage=yes
LicenseFile=..\COPYING
Compression=bzip/9
AppId=SIM
AppMutex=SIM_Mutex
AppCopyright=Copyright � 2002-2003, Vladimir Shutoff

[Tasks]
Name: "startup"; Description: "Launch SIM on &startup"
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"

[Dirs]
Name: "{app}\po"
Name: "{app}\sounds"
Name: "{app}\pict"
Name: "{app}\icons"
Name: "{app}\plugins"
Name: "{app}\plugins\styles"

[Files]
Source: "..\Release\sim.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Release\simapi.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Release\simui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Release\qjpegio.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Release\COPYING"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Release\plugins\__homedir.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\__migrate.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\_core.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\about.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\autoaway.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\background.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\dock.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\filter.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\floaty.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\forward.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\icons.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\icq.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\jabber.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\loger.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\msn.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\navigate.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\netmonitor.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\ontop.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\osd.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\proxy.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\shortcuts.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\splash.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\sound.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\transparent.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\windock.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion
Source: "..\Release\plugins\styles\xpstyle.dll"; DestDir: "{app}\plugins\styles"; Flags: ignoreversion
Source: "..\Release\sounds\startup.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\filedone.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\message.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\file.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\sms.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\auth.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\alert.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\url.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\contacts.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\web.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\sounds\mailpager.wav"; DestDir: "{app}\sounds"; Flags: ignoreversion
Source: "..\Release\pict\splash.png"; DestDir: "{app}\pict"; Flags: ignoreversion
Source: "..\Release\pict\connect.gif"; DestDir: "{app}\pict"; Flags: ignoreversion

[Icons]
Name: "{commonprograms}\SIM"; Filename: "{app}\sim.exe"
Name: "{userdesktop}\Simple Instant Messenger"; Filename: "{app}\sim.exe"; Tasks: desktopicon
Name: "{userstartup}\SIM"; Filename: "{app}\sim.exe"; Tasks: startup

[Run]
Filename: "{app}\sim.exe"; Description: "Launch Simple Instant Messenger"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: {userappdata}\sim



