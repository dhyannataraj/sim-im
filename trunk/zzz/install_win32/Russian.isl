; *** Inno Setup version 4.1.4+ English messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.51

[LangOptions]
LanguageName=Russian
LanguageID=$0419
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=���������
SetupWindowTitle=��������� - %1
UninstallAppTitle=�������������
UninstallAppFullTitle=%1 �������������

; *** Misc. common
InformationTitle=����������
ConfirmTitle=�����������
ErrorTitle=������

; *** SetupLdr messages
SetupLdrStartupMessage=���������� ������� ��������� %1. ����������?
LdrCannotCreateTemp=��������� ���� �� ������. ����������� ���������
LdrCannotExecTemp=������ �������� ����� �� ���������� ��������. ����������� ���������

; *** Startup error messages
LastErrorMessage=%1.%n%nError %2: %3
SetupFileMissing=� ����� ��������� ��� ����� %1 . ��������� ������ ��� ����������� ����� ����� ���������.
SetupFileCorrupt=������������ ����� ����������. ����������� ����� ����� ���������.
SetupFileCorruptOrWrongVer=������������ ����� ���������� ���� ������������ � ���� ������� ���������. ��������� �������� ���� ����������� ����� ����� ���������.
NotOnThisPlatform=��� ��������� �� ����� �������� ��� %1.
OnlyOnThisPlatform=��� ��������� ���� ��������� ��� %1.
WinVersionTooLowError=��������� ������� %1 ������ %2 ��� ����� �������.
WinVersionTooHighError=��� ��������� ������ ����������������� �� %1 ������ %2 ��� ����� �������.
AdminPrivilegesRequired=�� ������ ����� ��� ������������� ��� ����������� ���� ���������.
PowerUserPrivilegesRequired=�� ������ ����� ��� ������������� ��� ����������� ������������ ��� ����������� ���� ���������.
SetupAppRunningError=��������� ��������� ����������, ��� �������� ��������� %1 .%n%n�������� ��� ����������, ����� ������� �� ��� ����������� ���� ������, ����� ����� �� ��������� ���������.
UninstallAppRunningError=��������� ������������� ����������, ��� �������� ��������� %1 .%n%n�������� ��� ����������, ����� ������� �� ��� ����������� ���� ������, ����� ����� �� ��������� ���������.

; *** Misc. errors
ErrorCreatingDir=������ ��� �������� ����� "%1"
ErrorTooManyFilesInDir=���� � ����� "%1" �� ������, ��� ��� ��� ����� �������� ������� ����� ������

; *** Setup common messages
ExitSetupTitle=�����
ExitSetupMessage=��������� �� ���������. ���� �� ������ �������, ��������� �� ����� ��������� �����������������.%n%n������ ��������� ��������� ��������� � ������ ���.%n%n�����?
AboutSetupMenuItem=&�� ���������...
AboutSetupTitle=�� ���������
AboutSetupMessage=%1 ������ %2%n%3%n%n%1 ��������:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &�����
ButtonNext=&������ >
ButtonInstall=&����������
ButtonOK=��
ButtonCancel=������
ButtonYes=&��
ButtonYesToAll=�� &����
ButtonNo=&���
ButtonNoToAll=�&�� ����
ButtonFinish=&�����
ButtonBrowse=&�����...
ButtonWizardBrowse=&�����...
ButtonNewFolder=&����� �����

; *** "Select Language" dialog messages
SelectLanguageTitle=�������� ���� ���������
SelectLanguageLabel=�������� ���� ���������:

; *** Common wizard text
ClickNext=������ - ����������, ������ - �����.
BeveledLabel=


BrowseDialogTitle=����� �����
BrowseDialogLabel=�������� ����� � ������� ��.
NewFolderName=����� �����

; *** "Welcome" wizard page
WelcomeLabel1=����� ���������� � [name] ������ ���������
WelcomeLabel2=���������� ������� ��������� [name/ver] �� ��� ���������.%n%n������������� ������� ��� ���������� ���������� � ��������� ���������� ������, ��� ���������� ���������.

; *** "Password" wizard page
WizardPassword=������
PasswordLabel1=����������� ��� �������.
PasswordLabel3=������� ������ � �������� ������ ��� �����������. ��� �������� ������ ���������� �������.
PasswordEditLabel=&������:
IncorrectPassword=�������� ������. ��������� �������.

; *** "License Agreement" wizard page
WizardLicense=������������ ����������
LicenseLabel=�������� ��������� ���������� ������, ��� ���������� ���������.
LicenseLabel3=�������� ������������ ����������. �� ������ ������� ������� ����� ����������, ����� ���������� ���������.
LicenseAccepted=� &�������� �������
LicenseNotAccepted=� &�� �������� �������

; *** "Information" wizard pages
WizardInfoBefore=����������
InfoBeforeLabel=�������� ��������� ������ ����������.
InfoBeforeClickLabel=����� ������ ������ ���������� � ���������, �������� ������.
WizardInfoAfter=����������
InfoAfterLabel=�������� ��������� ������ ����������.
InfoAfterClickLabel=����� ������ ������ ���������� � ���������, �������� ������.

; *** "User Information" wizard page
WizardUserInfo=���������� � ������������
UserInfoDesc=������� ���� ������.
UserInfoName=&���:
UserInfoOrg=&�����������:
UserInfoSerial=&�������� �����:
UserInfoNameRequired=�� ������ ������ ���.

; *** "Select Destination Directory" wizard page
WizardSelectDir=��������� ����� ���������
SelectDirDesc=���� ���������� [name] ?
SelectDirBrowseLabel=��������� [name] ����� ����������� � ��������� �����.%n%n��� �����������, ����� ������. ��� ������ ������ ����� ����� �����.
DiskSpaceMBLabel=��� ��������� ��������� ��������� �� ����� [mb] �� ���������� �����.
ToUNCPathname=��������� �� ����� ��������� ��������� �� ���� UNC. ���� �� ����������� �����, ����� �� ������ ������� ���� � �������� ��������� ����������.
InvalidPath=�� ������ ������ ������ ����; ��������:%n%nC:\APP%n%n��� ���� UNC � �����:%n%n\\server\share
InvalidDrive=��������� ���� ���� ��� ������� �� ���������� ��� ����������. �������� ������ ����.
DiskSpaceWarningTitle=���� ���������� ����� �� �����
DiskSpaceWarning=��������� ��������� ������� �� ����� %1 �� ���������� �����, � �� ��������� ���� ����� �������� ����� %2 ��.%n%n��� ����� ����������?
DirNameTooLong=��� ��� ���� ����� ������� �������.
InvalidDirName=������������ ��� �����.
BadDirName32=����� ����� �� ������ ��������� ��������� ��������:%n%n%1
DirExistsTitle=����� ����� ��� ����������
DirExists=�����:%n%n%1%n%n��� ����������. ��� ����� �������� ��������� � ��� �����?
DirDoesntExistTitle=����� ����� �� ����������
DirDoesntExist=�����:%n%n%1%n%n�� ����������. ������ ������� ��� �����?

; *** "Select Components" wizard page
WizardSelectComponents=�������� ����������
SelectComponentsDesc=����� ���������� ����������?
SelectComponentsLabel2=�������� ���������� ���������. �������� ������ ��� �����������.
FullInstallation=������ ���������
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=����������� ���������
CustomInstallation=����������������
NoUninstallWarningTitle=����� ���������� ��� ����
NoUninstallWarning=��������� ��������� ����������, ��� ��������� ���������� ��� ����������������� �� ����� ����������:%n%n%1%n%n. %n%n����������?
ComponentSize1=%1 ��
ComponentSize2=%1 ��
ComponentsDiskSpaceMBLabel=����� �� ����� [mb] �� ���������� ����� �� �����.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=�������� �������������� ������
SelectTasksDesc=����� �������������� ������ ���������?
SelectTasksLabel2=�������� �������������� ������, ������� ������ ��������� ��������� ��� ��������� [name], ����� �������� ������.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=�������� ������� ���������� ����
SelectStartMenuFolderDesc=���� ��������� ������ ���������?
NoIconsCheck=&�� ��������� �������
MustEnterGroupName=������� ��� ��������.
GroupNameTooLong=��� ��� ���� ����� ������� �������.
InvalidGroupName=������������ ��� ��������.
BadGroupName=��� ����� �������� �� ������ ��������� �������� ��������:%n%n%1
NoProgramGroupCheck2=&�� ��������� ������� � ��������� ����

; *** "Ready to Install" wizard page
WizardReady=��� ������ � ���������
ReadyLabel1=������ ��������� ������ ��������� [name] �� ��� ���������.
ReadyLabel2a=�������� ����������, ����� ���������� ���������, ���� �������� �����, ���� ������ ������� ���������.
ReadyLabel2b=�������� ����������, ����� ����������.
ReadyMemoUserInfo=���������� � ������������:
ReadyMemoDir=����� ���������:
ReadyMemoType=��� ���������:
ReadyMemoComponents=��������� ����������:
ReadyMemoGroup=������� ���������� ����:
ReadyMemoTasks=�������������� ������:

; *** "Preparing to Install" wizard page
WizardPreparing=���������� � ���������
PreparingDesc=���������� � ��������� [name] �� ��� ���������.
PreviousInstallNotCompleted=���������/�������� ���������� ��������� �� ���� ���������. �� ������ ������������� ��� ���������.%n%n����� ����� ����� ��������� ��������� ���������, ����� ��������� ����������� [name].
CannotContinue=��������� �� ����� ���������� ���������. �������� ������, ����� �����.

; *** "Installing" wizard page
WizardInstalling=���������
InstallingLabel=���������, ���� ��������� ��������� ��������������� [name] �� ��� ���������.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=���������� ��������� [name] 
FinishedLabelNoIcons=��������� [name] ���������.
FinishedLabel=��������� [name] ���������. 
ClickFinish=�������� �����, ����� �����.
FinishedRestartLabel=����� ��������� ��������� [name], ����� ������������� ��� ���������. ������� ��� ������?
FinishedRestartMessage=����� ��������� ��������� [name], ����� ������������� ��� ���������.%n%n������� ��� ������?
ShowReadmeCheck=��, � ���� ����������� ���� README 
YesRadio=&��, ������������� ���������
NoRadio=&���, � ���������� ��������� �����
; used for example as 'Run MyProg.exe'
RunEntryExec=��������� %1
; used for example as 'View Readme.txt'
RunEntryShellExec=����� %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=�������� ��������� ����
SelectDiskLabel2=�������� ���� %1 � �������� ��.%n%n���� ����� �� ����� ����� ����� � ������ ��������, � �� � ���, ��� ������ ����, ����� ������� ���������� ���� ��� �������� �����.
PathLabel=&����:
FileNotInDir2=���� "%1" �� ����� ���������� � "%2". ������� ���� ��� �������� ������ �������.
SelectDirectoryLabel=������� ��������������� ���������� �����.

; *** Installation phase messages
SetupAborted=��������� �� ���������.%n%n��������� �������� � ��������� ��������� ��������� ��� ���.
EntryAbortRetryIgnore=�������� ���������, ����� ��������� �������, ������������ - ����� ��� ����� ����������, ��� ������ - ����� �������� ���������.

; *** Installation status messages
StatusCreateDirs=�������� �����...
StatusExtractFiles=���������� ������...
StatusCreateIcons=�������� �������...
StatusCreateIniEntries=�������� ������ INI...
StatusCreateRegistryEntries=�������� ���������� ������...
StatusRegisterFiles=����������� ������...
StatusSavingUninstall=���������� ����������...
StatusRunProgram=���������� ���������...
StatusRollback=�������� ���������...

; *** Misc. errors
ErrorInternal2=������: %1
ErrorFunctionFailedNoCode=%1 �������
ErrorFunctionFailed=%1 �������; ��� %2
ErrorFunctionFailedWithMessage=%1 �������; ��� %2.%n%3
ErrorExecutingProgram=�� ������� ������� ����:%n%1

; *** Registry errors
ErrorRegOpenKey=������ ��� �������� �����:%n%1\%2
ErrorRegCreateKey=������ ��� �������� �����:%n%1\%2
ErrorRegWriteKey=������ ��� ������ �����:%n%1\%2

; *** INI errors
ErrorIniEntry=������ ��� �������� INI ������ � ����� "%1".

; *** File copying errors
FileAbortRetryIgnore=�������� ���������, ����� ��������� �������, ������������ - ����� ���������� ���� ���� (�� �������������), ��� ������ - ����� �������� ���������.
FileAbortRetryIgnore2=�������� ���������, ����� ��������� �������, ������������ - ����� ��� ����� ���������� (�� �������������), ��� ������ - ����� �������� ���������.
SourceIsCorrupted=�������� ���� ���������
SourceDoesntExist=���� "%1" �� ����������
ExistingFileReadOnly=�������� ���� ������ ��� ������.%n%n������� ������ ��� �������� ����� �������� � ���������� ��� ���, ������������ ��� �������� �����, ������ ��� ������ �� �����������.
ErrorReadingExistingDest=������ ������ ������������� �����:
FileExists=���� ��� ����������.%n%n������ ������������?
ExistingFileNewer=������������ ���� ����� ���������������. �������������� ��������� ���.%n%n��������� ������������ ����?
ErrorChangingAttr=��������� ������ ��� ������� ��������� ������� ������������� �����:
ErrorCreatingTemp=������ ��� �������� ����� � ����� ���������:
ErrorReadingSource=������ ���������� ��������� �����:
ErrorCopying=������ ��� ����������� �����:
ErrorReplacingExistingFile=������ ��� ������ ������������� �����:
ErrorRestartReplace=RestartReplace �������:
ErrorRenamingTemp=������ ��� �������������� ����� � ����� ���������:
ErrorRegisterServer= ����������� DLL/OCX �� �������: %1
ErrorRegisterServerMissingExport=DllRegisterServer �� ������
ErrorRegisterTypeLib=���������� �� ����������������: %1

; *** Post-installation errors
ErrorOpeningReadme=������ ��� �������� ����� README.
ErrorRestartingComputer=��������� ��������� �� ������ ������������� ���������. �������� ��� ����.

; *** Uninstaller messages
UninstallNotFound=���� "%1" �� ����������. ������������� �� ����� ���� �����������.
UninstallOpenError=�� ������� ������� ���� "%1" . ������������� �� ����� ���� �����������
UninstallUnsupportedVer=������ ����� "%1" �� �������������� ���� ������� ��������������. ������������� �� ����� ���� �����������
UninstallUnknownEntry=� ������� ������������� ���������� ����������� ������ (%1) 
ConfirmUninstall=�� ����� ������ ������� %1 � ��� ����������?
OnlyAdminCanUninstall=��� ��������� ����� ���������������� ������ �������������.
UninstallStatusLabel=���������, ���� ����� ��������� �������� %1 .
UninstalledAll=%1 ������� ������.
UninstalledMost=������������� %1 ���������.%n%n��������� �������� �� �������. �� �� ������ ������� �������.
UninstalledAndNeedsRestart=��� ���������� �������� %1, ��������� ����� �������������.%n%n������� ��� ������?
UninstallDataCorrupted=���� "%1" ���������. ������������� �� ����� ���� �����������

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=������� ����?
ConfirmDeleteSharedFile2=������� ����������, ��� ��������� ���� �� ������������ �� ����� �� ��������.  ������� ���� ����?%n%n�� ���� �����-�� ��������� ���������� ���� ����, ����� ����� ��� �������� ��� ����� �������� � ��������. ���� �� �� �������, �������� ���. �������� ����� �� ����� ����� �� �����.
SharedFileNameLabel=��� �����:
SharedFileLocationLabel=�����:
WizardUninstalling=������ ��������
StatusUninstalling=�������� %1...
