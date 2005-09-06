; *** Inno Setup - version 4.0.5+ SLOVAK messages ***
;
;  TRANSLATION TO SLOVAK:
;  (c) Arpad Toth, ekosoft@signalsoft.sk - 3.0.2
;  (c) Juraj Petrik, jpetrik@i-servis.net - 3.0.6 - 2003-03-14
;  (c) Branislav Kopun, kopun@centrum.sk - 4.0.5 - 2003-07-29
;
;  Pokia� by niekto upravoval tuto verziu, po�lite pros�m, 
;  emailom upraven� verziu na vyssie uvedene adresy, dakujeme.
;  
;

[LangOptions]
LanguageName=Slovak
LanguageID=$041B
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=MS Shell Dlg
;DialogFontSize=8
;DialogFontStandardHeight=13
;TitleFontName=Arial
;TitleFontSize=29
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=In�tal�cia
SetupWindowTitle=In�tal�cia aplik�cie "%1"
UninstallAppTitle=Odin�talovanie
UninstallAppFullTitle=Odin�talovanie aplik�cie "%1"

; *** Misc. common
InformationTitle=Inform�cie
ConfirmTitle=Potvrdenie
ErrorTitle=Chyba

; *** SetupLdr messages
SetupLdrStartupMessage=In�tal�cia aplik�cie "%1". Prajete si pokra�ova�?
LdrCannotCreateTemp=Nemo�no vytvori� do�asn� s�bor. In�tal�cia bude ukon�en�.
LdrCannotExecTemp=Nemo�no spusti� s�bor v do�asnom adres�ri. In�tal�cia bude ukon�en�.

; *** Startup error messages
LastErrorMessage=%1.%n%nChyba %2: %3
SetupFileMissing=V adres�ri in�tal�cie ch�ba s�bor %1. Pros�m, odstr��te probl�m alebo si zaobstarajte nov� k�piu aplik�cie.
SetupFileCorrupt=In�tala�n� s�bory s� poru�en�. Pros�m, zaobstarajte si nov� k�piu aplik�cie.
SetupFileCorruptOrWrongVer=Instala�n� s�bory s� poru�en� alebo sa nezlu�uj� s touto verziou in�tal�cie. Pros�m, odstr��te probl�m, alebo si zaobstarajte nov� k�piu aplik�cie.
NotOnThisPlatform=T�to aplik�ciu nemo�no spusti� na %1.
OnlyOnThisPlatform=T�to aplik�cia vy�aduje pre spustenie syst�m %1.
WinVersionTooLowError=T�to aplick�cia vy�aduje %1 verziu %2 alebo vy��iu.
WinVersionTooHighError=T�to aplik�cia nem��e by� nain�talovan� na %1 verzii %2 alebo vy��ej.
AdminPrivilegesRequired=Pre in�tal�ciu tejto aplik�cie mus�te by� prihl�sen� ako administr�tor.
PowerUserPrivilegesRequired=Pre in�tal�ciu tejto aplik�cie mus� by� prihl�sen� ako administr�tor alebo ako u��vate� skupiny "Power Users".
SetupAppRunningError=In�tal�tor rozpoznal, �e aplik�cia %1 je u� spusten�.%n%nPros�m ukon�ite v�etky jej s��asti a potom pokra�ujte "OK" inak kliknite na "Storno" pre ukon�enie.
UninstallAppRunningError=Odin�tal�tor rozpoznal, �e aplik�cia %1 je spusten�.%n%nPros�m ukon�ite v�etky jej s��asti a potom pokra�ujte "OK" inak kliknite "Storno" pre ukon�enie.

; *** Misc. errors
ErrorCreatingDir=In�tal�tor nemohol vytvori� adres�r "%1"
ErrorTooManyFilesInDir=Nemo�no vytvori� s�bor v adres�ri "%1" preto�e obsahuje pr�li� ve�a s�borov

; *** Setup common messages
ExitSetupTitle=Ukon�enie in�tal�cie
ExitSetupMessage=In�tal�cia nebola dokon�en�. Pokia� teraz skon��te, aplik�cia nebude nain�talovan�.%n%nK dokon�eniu in�tal�cie m��ete in�tala�n� program spusti� inokedy.%n%nUkon�i� in�tal�ciu?
AboutSetupMenuItem=O progr&ame...
AboutSetupTitle=O in�tal�cii
AboutSetupMessage=%1 verzia %2%n%3%n%n%1 domovsk� str�nka:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< N�vra&t
ButtonNext=Pokr&a�uj >
ButtonInstall=&In�talova�
ButtonOK=&OK
ButtonCancel=&Storno
ButtonYes=&Ano
ButtonYesToAll=�no p&re V�etky
ButtonNo=&Nie
ButtonNoToAll=Nie pr&e V�etky
ButtonFinish=&Dokon�i�
ButtonBrowse=&Nalistova�...

; *** "Select Language" dialog messages
SelectLanguageTitle=V�ber jazyka in�tal�cie
SelectLanguageLabel=V�ber sprievodn�ho jazyka po�as in�tal�cie:

; *** Common wizard text
ClickNext="Pokra�uj" pre pokra�ovanie, "Storno" ukon�� in�tal�ciu.
BeveledLabel=

; *** "Welcome" wizard page
WelcomeLabel1=Vitajte v in�tala�nom programe aplik�cie "[name]".
WelcomeLabel2=Tento program nain�taluje aplik�ciu "[name/ver]" na V� po��ta�.%n%nPredt�m, ako budete pokra�ova�, doporu�ujeme uzavrie� v�etky spusten� aplik�cie. Pred�dete t�m mo�n�m konfliktom behom in�tal�cie.

; *** "Password" wizard page
WizardPassword=Heslo
PasswordLabel1=T�to in�tal�cia je chr�nen� heslom.
PasswordLabel3=Pros�m zadajte heslo a pokra�ujte.%n%nHeslo rozli�uje ve�k� a mal� znaky.
PasswordEditLabel=&Heslo:
IncorrectPassword=Zadan� heslo nie je spr�vne. Pros�m sk�ste to znovu.

; *** "License Agreement" wizard page
WizardLicense=Licen�n� podmienky
LicenseLabel=Pros�m ��tajte pozorne nasleduj�ce licen�n� podmienky a� potom pokra�ujte.
LicenseLabel3=Pros�m ��tajte pozorne nasleduj�ce licen�n� podmienky. Pre pokra�ovanie in�tal�cie mus�te s�hlasi� s licen�n�mi podmienkami.
LicenseAccepted=S�hl&as�m s licen�n�mi podmienkami
LicenseNotAccepted=&Nes�hlas�m s licen�n�mi podmienkami

; *** "Information" wizard pages
WizardInfoBefore=Inform�cia
InfoBeforeLabel=Predt�m ako budete pokra�ova�, pre��tajte si pros�m najprv nasleduj�cu d�le�it� inform�ciu.
InfoBeforeClickLabel=Kliknut�m na "Pokra�uj" pokra�ujte v in�tal�ci.
WizardInfoAfter=Inform�cia
InfoAfterLabel=Predt�m ako budete pokra�ova�, pre��tajte si pros�m najprv nasleduj�cu d�le�it� inform�ciu.
InfoAfterClickLabel=Kliknut�m na "Pokra�uj" pokra�ujte v in�tal�ci.

; *** "User Information" wizard page
WizardUserInfo=Inform�cie o u��vate�ovy
UserInfoDesc=Vypl�te pros�m inform�cie o V�s.
UserInfoName=Meno &u��vate�a:
UserInfoOrg=&Organiz�cia:
UserInfoSerial=&S�riov� ��slo:
UserInfoNameRequired=Mus�te zada� meno u��vate�a.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Zvo�te cie�ov� adres�r
SelectDirDesc=Kde m� by� aplik�cia "[name]" nain�talovan�?
DiskSpaceMBLabel=Aplik�cia vy�aduje najmenej [mb] MB miesta na disku.
ToUNCPathname=Nie je mo�n� in�talova� do cesty UNC. Pokia� sa pok��ate in�talova� do siete, mus�te si najk�r namapova� sie�ov� disk.
InvalidPath=Mus�te zada� cel� cestu aj s p�smenom disku, napr�klad:%nC:\APP
InvalidDrive=Vybran� disk neexistuje. Pros�m, vyberte in�.
DiskSpaceWarningTitle=Na disku nie je dostatok miesta.
DiskSpaceWarning=In�tal�cia vy�aduje najmenej %1 KB vo�n�ho miesta, ale na vybranom disku je dostupn� len %2 KB.%n%nChcete napriek tomu pokra�ova�?
BadDirName32=N�zov adres�ra nem��e obsahova� �iaden z nasleduj�cich znakov:%n%n%1
DirExistsTitle=Adres�r u� existuje
DirExists=Adres�r menom:%n%n%1%n%nu� existuje. Chcete napriek tomu in�talova� do tohoto adres�ra?
DirDoesntExistTitle=Adres�r neexistuje
DirDoesntExist=Adres�r menom:%n%n%1%n%nneexistuje. Chcete tento adres�r vytvori�?

; *** "Select Components" wizard page
WizardSelectComponents=Vyberte komponenty ktor� sa bud� in�talova�
SelectComponentsDesc=Ktor� komponenty maj� by� nain�talovan�?
SelectComponentsLabel2=Ozna�te si komponenty, ktor� chcete in�talova� resp. odzna�te tie, ktor� in�talova� nechcete.
FullInstallation=�pln� in�tal�cia
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktn� in�tal�cia
CustomInstallation=Volite�n� in�tal�cia
NoUninstallWarningTitle=Existuj�ce komponenty
NoUninstallWarning=Instala�n� program zistil tieto nain�talovan� komponenty:%n%n%1%n%nZru�en�m v�beru nebud� odin�talovan�.%n%nNaozaj chcete pokra�ova�?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Aktu�lny v�ber si vy�aduje [mb] pr�zdneho miesta na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Vybra� pr�davn� �lohy
SelectTasksDesc=Ktor� pr�davn� �lohy sa maj� vykona�?
SelectTasksLabel2=Zvo�te, ktor� pr�davn� �lohy sa maj� uskuto�ni� pri in�tal�cii "[name]", predt�m ako budete pokra�ova�.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Zvo�te si programov� skupinu
SelectStartMenuFolderDesc=Kde m� in�tal�tor umiestni� � skupinu?
NoIconsCheck=&Nevytv�ra� �iadne ikony
MustEnterGroupName=Mus�te zada� n�zov programovej skupiny.
BadGroupName=N�zov skupiny nem��e obsahova� �iaden z nasleduj�cich znakov:%n%n%1
NoProgramGroupCheck2=N&evytv�ra� programovov� skupinu

; *** "Ready to Install" wizard page
WizardReady=In�tal�cia je pripraven�
ReadyLabel1=Teraz sa bude in�talova� aplik�cia "[name]" do V�ho po��ta�a.
ReadyLabel2a=Pokra�ujte kliknut�m na "In�talova�" alebo kliknite na "N�vrat" pre op�tovn� zmenu nastavenia.
ReadyLabel2b=V in�tal�cii pokra�ujte kliknut�m na "In�talova�".
ReadyMemoUserInfo=Inform�cie o u��vate�ovy:
ReadyMemoDir=Cie�ov� adres�r:
ReadyMemoType=Typ in�tal�cie:
ReadyMemoComponents=Vybran� komponenty:
ReadyMemoGroup=Programov� skupina:
ReadyMemoTasks=�al�ie �lohy:

; *** "Preparing to Install" wizard page
WizardPreparing=Pr�prava na in�tal�ciu
PreparingDesc=In�tala�n� program sa pripravuje na in�tal�ciu aplik�cie "[name]" na V� po��ta�.
PreviousInstallNotCompleted=In�tal�cia/odin�tal�cia predch�dzaj�cej verzie nebola kompletne dokon�en�. Je potrebn� re�tart po��ta�a.%n%nPo re�tarte po��ta�a spustite znovu in�tala�n� program pre dokon�enie in�tal�cie aplik�cie "[name]".
CannotContinue=In�tala�n� program nem��e pokra�ova�. Pros�m, kliknite na "Storno" pre ukon�enie in�tal�cie.

; *** "Installing" wizard page
WizardInstalling=Stav in�tal�cie
InstallingLabel=Pros�m po�kajte, pokia� sa nedokon�� in�tal�cia aplik�cie "[name]" na V� po��ta�.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Dokon�ujem in�tal�ciu [name]
FinishedLabelNoIcons=In�tal�cia aplik�cie "[name]" do V�ho po��ta�a bola dokon�en�.
FinishedLabel=In�tal�cia aplik�cie "[name]" do V�ho po��ta�a bola dokon�en�. Aplik�cia m��e by� spusten� pomocou pripraven�ch ikon.
ClickFinish=Kliknite na "Dokon�i�" pre ukon�enie in�tal�tora.
FinishedRestartLabel=K dokon�eniu instal�cie aplik�cie "[name]" je nutn� re�tartova� V� po��ta�. Chcete re�tartova� po��ta� teraz?
FinishedRestartMessage=K dokon�eniu instal�cie aplik�cie "[name]" sa mus� re�tartova� V� po��ta�.%n%nChcete re�tartova� po��ta� teraz?
ShowReadmeCheck=�no, chcem vidie� s�bor README.
YesRadio=�no, &re�tartova� po��ta� hne�
NoRadio=&Nie, budem po��ta� re�tartova� nesk�r
; used for example as 'Run MyProg.exe'
RunEntryExec=Spusti� %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Zobrazi� %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=In�tal�cia vy�aduje �al�iu disketu
SelectDiskLabel2=Pros�m, vlo�te Disk %1 a kliknite OK.%n%nPokia� m��u by� s�bory na tomto disku ale v inom %2 ako ni��ie uvedenom, zadajte spr�vnu cestu alebo kliknite na Nalistova�.
PathLabel=&Cesta:
FileNotInDir2=S�bor "%1" sa nenach�dza v "%2". Pros�m vlo�te spr�vny disk alebo si vyberte in� %3.
SelectDirectoryLabel=Pros�m, zadajte umiestnenie �al�ieho disku.

; *** Installation phase messages
SetupAborted=In�tal�cia nebola dokon�en�.%n%nPros�m, odstr��te probl�m a spustite in�tal�ciu znovu.
EntryAbortRetryIgnore=Kliknite "Znovu" pre opakovanie, "Ignoruj" pre pokra�ovanie alebo "Storno" k ukon�eniu in�tal�cie.

; *** Installation status messages
StatusCreateDirs=Vytv�ram adres�re...
StatusExtractFiles=Extrahujem s�bory...
StatusCreateIcons=Vytv�ram ikony programov...
StatusCreateIniEntries=Vytv�ram z�znamy v INI...
StatusCreateRegistryEntries=Vytv�ram z�znamy v registroch...
StatusRegisterFiles=Registrujem s�bory...
StatusSavingUninstall=Uklad�m inform�cie k odin�talovaniu...
StatusRunProgram=Ukon�ujem in�tal�ciu...
StatusRollback=Vraciam zmeny do p�vodn�ho stavu...

; *** Misc. errors
ErrorInternal2=Vn�torn� chyba %1
ErrorFunctionFailedNoCode=%1 zlyhal
ErrorFunctionFailed=%1 zlyhal; k�d %2
ErrorFunctionFailedWithMessage=%1 zlyhal; k�d %2.%n%3
ErrorExecutingProgram=Nie je mo�n� spusti� s�bor:%n%1

; *** Registry errors
ErrorRegOpenKey=Chyba pri otv�ran� registrov:%n%1\%2
ErrorRegCreateKey=Chyba pri vytv�ran� registrov:%n%1\%2
ErrorRegWriteKey=Chyba pri z�pise do registrov:%n%1\%2

; *** INI errors
ErrorIniEntry=Chyba pri vytv�ran� INI z�znamu v s�bore %1.

; *** File copying errors
FileAbortRetryIgnore=Kliknite Znovu pre opakovanie, Ignoruj pre vynechanie tohoto s�boru (nedoporu�uje sa) alebo Storno k ukon�eniu in�tal�cie.
FileAbortRetryIgnore2=Kliknite Znovu pre opakovanie, Ignoruj k pokra�ovaniu (nedoporu�uje sa) alebo Storno k ukon�eniu in�tal�cie.
SourceIsCorrupted=Zdrojov� s�bor je poru�en�
SourceDoesntExist=Zdrojov� s�bor "%1" neexistuje
ExistingFileReadOnly=Existuj�ci s�bor je ozna�en� ako read-only.%n%nKliknite Znovu pre odstr�nenie atrib�tu read-only a nov�mu opakovaniu, Ignoruj pre vynechanie tohoto s�boru, alebo Storno k ukon�eniu in�tal�cie.
ErrorReadingExistingDest=Do�lo k chybe pri pokuse pre��ta� u� existuj�ci s�bor:
FileExists=S�bor u� existuje.%n%nChcete aby ho in�tal�cia prep�sala?
ExistingFileNewer=P�vodn� s�bor je nov�� ako ten, ktor� sa bude in�talova�. Doporu�uje sa zachova� p�vodn� s�bor.%n%nChcete zachova� p�vodn� s�bor?
ErrorChangingAttr=Do�lo ku chybe pri pokuse zmeni� atrib�ty existuj�ceho s�boru:
ErrorCreatingTemp=Do�lo ku chybe pri pokuse vytvori� s�bor v cie�ovom adres�ri:
ErrorReadingSource=Do�lo ku chybe pri pokuse pre��ta� zdrojov� s�bor:
ErrorCopying=Do�lo ku chybe pri pokuse kop�rova� s�bor:
ErrorReplacingExistingFile=Do�lo k chybe pri pokuse nahradi� existuj�ci s�bor:
ErrorRestartReplace=RestartReplace zlyhal:
ErrorRenamingTemp=Do�lo ku chybe pri pokuse premenova� s�bor v cie�ovom adres�ri:
ErrorRegisterServer=Nem��em zaregistrova� kni�nicu DLL/OCX: %1
ErrorRegisterServerMissingExport=DllRegisterServer export nebol n�jden�
ErrorRegisterTypeLib=Nem��em zaregistrova� typ kniznice: %1

; *** Post-installation errors
ErrorOpeningReadme=Vyskytla sa chyba pri pokuse otvori� README s�bor.
ErrorRestartingComputer=In�tal�tor nemohol re�tartova� po��ta�. Pros�m, re�tartujte ho manu�lne.

; *** Uninstaller messages
UninstallNotFound=S�bor "%1" neexistuje. Nem��em ho odin�talova�.
UninstallUnsupportedVer=T�to verzia odin�tal�tora nevie rozpozna� odin�tala�n� log s�bor "%1". Nem��em odin�talova�.
UninstallUnknownEntry=Nezn�my vstup (%1) odin�tala�n�ho log s�boru - je neo��slovan� alebo chybn�.
ConfirmUninstall=Ste si ist�, �e chcete odstr�ni� aplik�ciu "%1" vr�tane v�etk�ch nain�talovan�ch s��ast�?
OnlyAdminCanUninstall=T�to in�tal�cia aplik�cie m��e by� odin�talovan� len spr�vcom - administr�torom.
UninstallStatusLabel=�akajte pros�m k�m sa "%1" odstr�ni z V�ho po��ta�a.
UninstalledAll=Aplik�cia "%1" bola �spe�ne odstr�nen� z V�ho po��ta�a.
UninstalledMost=Odin�talovanie aplik�cie "%1" je dokon�en�.%n%nNiektor� �asti nebolo mo�n� odstr�ni�. M��ete ich odstr�ni� manu�lne.
UninstalledAndNeedsRestart=Pre dokon�enie odin�tal�cie aplik�cie "%1" je potrebn� re�tart po��ta�a.%n%nPrajete si vykona� re��tart teraz?
UninstallDataCorrupted=S�bor "%1" je poru�en�. Odin�talovanie nie je mo�n� uskuto�ni�.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Odstr�ni� zdie�an� s�bor?
ConfirmDeleteSharedFile2=Syst�m ukazuje, �e nasleduj�ci zdie�an� s�bor u� nie je �alej pou��van� �iadn�m programom. Chcete odin�talova� tento zdie�an� s�bor?%n%n%1%n%nPokia� niektor� aplik�cie tento s�bor pou��vaj�, po jeho odstr�nen� nemusia pracova� spr�vne. Pokia� nie ste si ist�, vyberte "Nie". Ponechanie s�boru v syst�me nevyvol� �iadnu �kodu.
SharedFileNameLabel=Meno s�boru:
SharedFileLocationLabel=Umiestnenie:
WizardUninstalling=Stav odin�talovania
StatusUninstalling=Odin�talovanie %1...
