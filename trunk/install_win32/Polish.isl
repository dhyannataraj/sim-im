; *** Inno Setup version 4.1.8+ Polish messages ***
; S�awomir Adamski <sadam@wa.onet.pl>
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.53 2004/02/25 01:55:24 jr Exp $


[LangOptions]
LanguageName=Polski
LanguageID=$0415
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=MS Shell Dlg
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Instalator
SetupWindowTitle=Instalacja - %1
UninstallAppTitle=Odinstalowanie
UninstallAppFullTitle=Odinstaluj %1

; *** Misc. common
InformationTitle=Informacja
ConfirmTitle=Potwierdzenie
ErrorTitle=B��d

; *** SetupLdr messages
SetupLdrStartupMessage=Ten program zainstaluje aplikacj� %1. Czy chcesz kontynuowa�?
LdrCannotCreateTemp=Nie mo�na utworzy� pliku tymczasowego. Instalacja przerwana
LdrCannotExecTemp=Nie mo�na uruchomi� pliku w tymczasowym folderze. Instalacja przerwana

; *** Startup error messages
LastErrorMessage=%1.%n%nB��d %2: %3
SetupFileMissing=W folderze Instalatora brakuje pliku %1. Prosz� rozwi�za� ten problem lub uzyska� now� kopi� Instalatora tego programu od producenta.
SetupFileCorrupt=Pliki sk�adowe Instalatora s� uszkodzone. Prosz� uzyska� now� kopi� Instalatora od producenta.
SetupFileCorruptOrWrongVer=Pliki sk�adowe instalatora s� uszkodzone lub niezgodne z t� wersj� Instalatora. Prosz� rozwi�za� ten problem lub uzyska� now� kopi� Instalatora od producenta.
NotOnThisPlatform=Ten program nie dzia�a pod %1.
OnlyOnThisPlatform=Ten program musi by� uruchomiony z %1.
WinVersionTooLowError=Ten program wymaga %1 w wersji %2 lub p�niejszej.
WinVersionTooHighError=Ten program nie mo�e by� zainstalowany w wersji %2 lub p�niejszej systemu %1.
AdminPrivilegesRequired=Musisz mie� uprawnienia administratora aby instalowa� ten program.
PowerUserPrivilegesRequired=Musisz mie� uprawnienia administratora lub uzytkownika zaawansowanego aby instalowa� ten program.
SetupAppRunningError=Instalator wykry�, �e jest uruchomiony %1.%n%nZamknij teraz wszystkie okienka tej aplikacji, a potem wybierz przycisk OK, aby kontynuowa�, lub Anuluj, aby przerwa� instalacj�.
UninstallAppRunningError=Program odinstalowuj�cy wykry�, �e jest uruchomiony %1.%n%nZamknij teraz wszystkie okna tej aplikacji, a potem wybierz przycisk OK, aby kontynuowa�, lub Anuluj, aby przerwa� odinstalowywanie.

; *** Misc. errors
ErrorCreatingDir=Instalator nie m�g� utworzy� folderu "%1"
ErrorTooManyFilesInDir=Nie mo�na utworzy� pliku w folderze %1, poniewa� zawiera on za du�o plik�w

; *** Setup common messages
ExitSetupTitle=Zako�cz instalacj�
ExitSetupMessage=Instalacja nie zosta�a zako�czona. Je�li zako�czysz j� teraz, aplikacja nie b�dzie zainstalowana.%n%nJe�li chcesz doko�czy� instalacj� innym razem, uruchom ponownie Instalatora.%n%nZako�czyc instalacj�?
AboutSetupMenuItem=&O Instalatorze...
AboutSetupTitle=O Instalatorze
AboutSetupMessage=%1 wersja %2%n%3%n%n Strona domowa %1:%n%4
AboutSetupNote=Polska wersja: Rafa� P�atek, S�awomir Adamski

; *** Buttons
ButtonBack=< &Wstecz
ButtonNext=&Dalej >
ButtonInstall=&Instaluj
ButtonOK=OK
ButtonCancel=Anuluj
ButtonYes=&Tak
ButtonYesToAll=Tak na &wszystkie
ButtonNo=&Nie
ButtonNoToAll=N&ie na wszystkie
ButtonFinish=&Zako�cz
ButtonBrowse=&Przegl�daj...
ButtonWizardBrowse=P&rzegl�daj...
ButtonNewFolder=&Stw�rz nowy folder

; *** "Select Language" dialog messages
SelectLanguageTitle=Wybierz j�zyk instalacji
SelectLanguageLabel=Wybierz j�zyk u�ywany w czasie instalacji:

; *** Common wizard text
ClickNext=Wybierz przycisk Dalej, aby kontynuowa�, lub Anuluj, aby zako�czy� instalacj�.
BeveledLabel=
BrowseDialogTitle=Wska� folder
BrowseDialogLabel=Wybierz folder z poni�szej listy, a potem wybierz przycisk OK.
NewFolderName=Nowy folder

; *** "Welcome" wizard page
WelcomeLabel1=Witamy w Kreatorze instalacji programu [name].
WelcomeLabel2=Instalator zainstaluje teraz program [name/ver] na Twoim komputerze.%n%nZalecamy zako�czenie pracy z wszystkimi innymi przez Ciebie uruchomionymi aplikacjami przed rozpocz�ciem instalacji.

; *** "Password" wizard page
WizardPassword=Has�o
PasswordLabel1=Ta instalacja jest zabezpieczona has�em.
PasswordLabel3=Podaj has�o, potem wybierz przycisk Dalej, aby kontynuowa�. W has�ach s� rozr�niane du�e i ma�e litery.
PasswordEditLabel=&Has�o:
IncorrectPassword=Wprowadzone has�o nie jest poprawne. Spr�buj ponownie.

; *** "License Agreement" wizard page
WizardLicense=Umowa Licencyjna
LicenseLabel=Przed kontynuacj� prosz� uwa�nie przeczyta� t� informacj�.
LicenseLabel3=Prosz� przeczyta� tekst Umowy Licencyjnej. Musisz zgodzi� si� na warunki tej umowy przed kontynuacj� instalacji.
LicenseAccepted=&Akceptuj� warunki umowy
LicenseNotAccepted=&Nie akceptuj� warunk�w umowy

; *** "Information" wizard pages
WizardInfoBefore=Informacja
InfoBeforeLabel=Przed przej�ciem do dalszego etapu instalacji, prosz� przeczyta� t� wa�n� informacj�.
InfoBeforeClickLabel=Kiedy b�dziesz gotowy do instalacji, wybierz przycisk Dalej.
WizardInfoAfter=Informacja
InfoAfterLabel=Przed przej�ciem do dalszego etapu instalacji, prosz� przeczyta� t� wa�n� informacj�.
InfoAfterClickLabel=Gdy b�dziesz gotowy do zako�czenia instalacji, wybierz przycisk Dalej.

; *** "User Information" wizard page
WizardUserInfo=Dane U�ytkownika
UserInfoDesc=Prosz� poda� swoje dane.
UserInfoName=&Nazwisko:
UserInfoOrg=&Organizacja:
UserInfoSerial=Numer &seryjny:
UserInfoNameRequired=Musisz poda� nazwisko.

; *** "Select Destination Location" wizard page
WizardSelectDir=Wybierz docelow� lokalizacj�
SelectDirDesc=Gdzie ma by� zainstalowany program [name]?
SelectDirLabel3=Instalator zainstaluje  [name] do poni�szego folderu.
SelectDirBrowseLabel=Wybierz przycisk Nast�pny, aby kontynuowa�. Je�li chcesz okre�li� iny folder, wybierz przycisk Przegl�daj.
DiskSpaceMBLabel=Potrzeba przynajmniej [mb] MB wolnego miejsca na dysku.
ToUNCPathname=Instalator nie mo�e instalowa� do �cie�ki UNC. Je�li pr�bujesz instalowa� program na dysku sieciowym, najpierw zmapuj ten dysk.
InvalidPath=Musisz wprowadzi� pe�n� �cie�k� wraz z liter� dysku, np.:%nC:\PROGRAM
InvalidDrive=Wybrany dysk nie istnieje. Prosz� wybra� inny.
DiskSpaceWarningTitle=Niewystarczaj�ca ilo�� wolnego miejsca na dysku
DiskSpaceWarning=Instalator wymaga co najmniej %1 KB wolnego miejsca na dysku. Wybrany dysk posiada tylko %2 KB dost�pnego miejsca.%n%nCzy mimo to chcesz kontynuowa�?
DirNameTooLong=Nazwa folderu lub �cie�ki jest za d�uga.
InvalidDirName=Niepoprawna nazwa folderu.
BadDirName32=Nazwa folderu nie mo�e zawiera� �adnego z nast�puj�cych znak�w po dwukropku:%n%n%1
DirExistsTitle=Ten folder ju� istnieje
DirExists=Folder%n%n%1%n%nju� istnieje. Czy chcesz zainstalowa� program w tym folderze?
DirDoesntExistTitle=Nie ma takiego folderu
DirDoesntExist=Folder:%n%n%1%n%nnie istnieje. Czy chcesz, aby zosta� utworzony?

; *** "Select Components" wizard page
WizardSelectComponents=Zaznacz sk�adniki
SelectComponentsDesc=Kt�re komponenty maj� by� zainstalowane?
SelectComponentsLabel2=Zaznacz sk�adniki, kt�re chcesz zainstalowa�, odznacz te, kt�rych nie chcesz zainstalowa�. Wybierz przycisk Dalej, aby kontynuowa�.
FullInstallation=Instalacja pe�na
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalacja podstawowa
CustomInstallation=Dopasowanie instalacji
NoUninstallWarningTitle=Zainstalowane sk�adniki
NoUninstallWarning=Instalator wykry�, �e w twoim komputerze s� ju� zainstalowane nast�puj�ce sk�adniki:%n%n%1%n%nOdznaczenie ich nie spowoduje odinstalowania.%n%nCzy mimo tego chcesz kontynuowa�?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Wybrane sk�adniki wymagaj� co najmniej [mb] MB na dysku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zaznacz dodatkowe zadania
SelectTasksDesc=Kt�re dodatkowe zadania maj� by� wykonane?
SelectTasksLabel2=Zaznacz dodatkowe zadania, kt�re Instalator ma wykona� podczas instalacji programu [name], potem wybierz przycisk Dalej, aby kontynuowa�.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Wybierz folder Menu Start
SelectStartMenuFolderDesc=Gdzie maj� by� umieszczone skr�ty do programu?
SelectStartMenuFolderLabel3=Instalator stworzy skr�ty do programu w poni�szym folderze Menu Start.
SelectStartMenuFolderBrowseLabel=Wybierz przycisk Nast�pny, aby kontynuowa�. Je�li chcesz okre�li� iny folder, wybierz przycisk Przegl�daj.
NoIconsCheck=&Nie tw�rz �adnych skr�t�w
MustEnterGroupName=Musisz wprowadzi� nazw� folderu.
GroupNameTooLong=Nazwa folderu lub �cie�ki jest za d�uga.
InvalidGroupName=Niepoprawna nazwa folderu.
BadGroupName=Nazwa folderu nie mo�e zawiera� �adnego z nast�puj�cych znak�w:%n%n%1
NoProgramGroupCheck2=&Nie tw�rz folderu w Menu Start

; *** "Ready to Install" wizard page
WizardReady=Gotowy do rozpocz�cia instalacji
ReadyLabel1=Instalator jest ju� gotowy do rozpocz�cia instalacji programu [name] na twoim komputerze.
ReadyLabel2a=Wybierz przycisk Instaluj, aby rozpocz�� instalacj� lub Wstecz, je�li chcesz przejrze� lub zmieni� ustawienia.
ReadyLabel2b=Wybierz przycisk Instaluj, aby kontynuowa� instalacj�.
ReadyMemoUserInfo=Informacje u�ytkownika:
ReadyMemoDir=Lokalizacja docelowa:
ReadyMemoType=Rodzaj instalacji:
ReadyMemoComponents=Wybrane sk�adniki:
ReadyMemoGroup=Folder w Menu Start:
ReadyMemoTasks=Dodatkowe zadania:

; *** "Preparing to Install" wizard page
WizardPreparing=Przygotowanie do instalacji
PreparingDesc=Instalator przygotowuje si� do instalacji [name] na Twoim komputerze.
PreviousInstallNotCompleted=Instalacja (usuni�cie) poprzedniej wersji programu nie zosta�a zako�czona. B�dziesz musia� ponownie uruchomi� komputer, aby zako�czy� instalacj�. %n%nPo ponownym uruchomieniu komputera uruchom ponownie instalatora, aby zako�czy� instalacj� [name].
CannotContinue=Instalator nie mo�e kontynuowa�. Wybierz przycisk Anuluj, aby przerwa� instalacj�.


; *** "Installing" wizard page
WizardInstalling=Instalowanie
InstallingLabel=Poczekaj, a� instalator zainstaluje [name] na Twoim komputerze.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Zako�czono instalacj� [name]
FinishedLabelNoIcons=Instalator zako�czy� instalacj� programu [name] na Twoim komputerze.
FinishedLabel=Instalator zako�czy� instalacj� programu [name] na Twoim komputerze. Aplikacja mo�e by� wywo�ana poprzez u�ycie zainstalowanych skr�t�w.
ClickFinish=Wybierz przycisk Zako�cz, aby zako�czy� instalacj�.
FinishedRestartLabel=Aby zako�czy� instalacj� programu [name], Instalator musi ponownie uruchomi� Tw�j komputer. Czy chcesz teraz ponownie uruchomi� sw�j komputer?
FinishedRestartMessage=Aby zako�czy� instalacj� programu [name], Instalator musi ponownie uruchomi� Tw�j komputer.%n%nCzy chcesz teraz ponownie uruchomi� sw�j komputer?
ShowReadmeCheck=Tak, chc� przeczyta� dodatkowe informacje
YesRadio=&Tak, teraz uruchom ponownie
NoRadio=&Nie, sam zrestartuj� p�niej
; used for example as 'Run MyProg.exe'
RunEntryExec=Uruchom %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Poka� %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Instalator potrzebuje nast�pnej dyskietki
SelectDiskLabel2=Prosz� w�o�y� dyskietk� %1 i wybra� przycisk OK.%n%nJe�li pokazany poni�ej folder nie okre�la po�o�enia plik�w z tej dyskietki, wprowad� poprawn� �cie�k� lub wybierz przycisk Przegl�daj.
PathLabel=�&cie�ka:
FileNotInDir2=Plik "%1" nie zosta� znaleziony na dyskietce "%2". Prosz� w�o�y� w�a�ciw� dyskietk� lub wybra� inny folder.
SelectDirectoryLabel=Prosz� okre�li� lokalizacj� nast�pnej dyskietki.

; *** Installation phase messages
SetupAborted=Instalacja nie zosta�a zako�czona.%n%nProsz� rozwi�za� problem i ponownie rozpocz�� instalacj�.
EntryAbortRetryIgnore=Mo�esz ponowi� nieudan� czynno��, zignorowa� j� (nie zalecane) lub przerwa� instalacj�.

; *** Installation status messages
StatusCreateDirs=Tworzenie folder�w...
StatusExtractFiles=Dekompresja plik�w...
StatusCreateIcons=Tworzenie ikon aplikacji...
StatusCreateIniEntries=Tworzenie zapis�w w plikach INI...
StatusCreateRegistryEntries=Tworzenie zapis�w w rejestrze...
StatusRegisterFiles=Rejestrowanie plik�w...
StatusSavingUninstall=Zachowanie informacji o odinstalowywaniu...
StatusRunProgram=Ko�czenie instalacji...
StatusRollback=Wycofywanie zmian...

; *** Misc. errors
ErrorInternal2=Wewn�trzny b��d: %1
ErrorFunctionFailedNoCode=B��d podczas wykonywania %1
ErrorFunctionFailed=B��d podczas wykonywania %1; kod %2
ErrorFunctionFailedWithMessage=B��d podczas wykonywania %1; code %2.%n%3
ErrorExecutingProgram=Nie mog� uruchomi�:%n%1

; *** Registry errors
ErrorRegOpenKey=B��d podczas otwierania klucza rejestru:%n%1\%2
ErrorRegCreateKey=B��d podczas tworzenia klucza rejestru:%n%1\%2
ErrorRegWriteKey=B��d podczas zapisu do klucza rejestru:%n%1\%2

; *** INI errors
ErrorIniEntry=B��d podczas tworzenia pozycji w pliku INI: "%1".

; *** File copying errors
FileAbortRetryIgnore=Mo�esz ponowi� nieudan� czynno��, zignorowa� j�, aby omin�� ten plik (nie zalecane), lub przerwa� instalacj�.
FileAbortRetryIgnore2=Mo�esz ponowi� nieudan� czynno��, zignorowa� j� (nie zalecane) lub przerwa� instalacj�.
SourceIsCorrupted=Plik �r�d�owy jest uszkodzony
SourceDoesntExist=Plik �r�d�owy "%1" nie istnieje
ExistingFileReadOnly=Istniej�cy plik jest oznaczony jako tylko-do-odczytu.%n%nMo�esz ponowi� (aby usun�� oznaczenie) zignorowa� (aby omin�� ten plik) lub przerwa� instalacj�.
ErrorReadingExistingDest=Wyst�pi� b��d podczas pr�by odczytu istniej�cego pliku:
FileExists=Plik ju� istnieje.%n%nCzy chcesz, aby Instalator zamieni� go na nowy?
ExistingFileNewer=Istniej�cy plik jest nowszy ni� ten, kt�ry Instalator pr�buje skopiowa�. Zalecanym jest zachowanie istniej�cego pliku.%n%nCzy chcesz zachowa� istniej�cy plik?
ErrorChangingAttr=Podczas pr�by zmiany atrybut�w istniej�cego pliku wyst�pi� b��d:
ErrorCreatingTemp=Podczas pr�by utworzenia pliku w folderze docelowym wyst�pi� b��d:
ErrorReadingSource=Podczas pr�by odczytu pliku �r�d�owego wyst�pi� b��d:
ErrorCopying=Podczas pr�by kopiowania pliku wyst�pi� b��d:
ErrorReplacingExistingFile=Podczas pr�by zamiany istniej�cego pliku wyst�pi� b��d:
ErrorRestartReplace=B��d RestartReplace:
ErrorRenamingTemp=Podczas pr�by zmiany nazwy pliku w folderze docelowym wyst�pi� b��d:
ErrorRegisterServer=Nie mo�na zarejestrowa� DLL/OCX: %1
ErrorRegisterServerMissingExport=Eksportowana funkcja DllRegisterServer nie zosta�a znaleziony
ErrorRegisterTypeLib=Nie mog� zarejestrowa� biblioteki typ�w: %1

; *** Post-installation errors
ErrorOpeningReadme=Wyst�pi� b��d podczas pr�by otwarcia pliku README.
ErrorRestartingComputer=Instalator nie m�g� zrestartowa� tego komputera. Prosz� zrobi� to samodzielnie.

; *** Uninstaller messages
UninstallNotFound=Plik "%1" nie istnieje. Nie mo�na go odinstalowa�.
UninstallOpenError=Plik "%1" nie m�g� by� otwarty. Nie mo�na odinstalowa�
UninstallUnsupportedVer=Ta wersja programu odinstalowuj�cego nie rozpoznaje formatu logu deinstalacji. Nie mo�na odinstalowa�
UninstallUnknownEntry=W logu deinstalacji wyst�pi�a nie znana pozycja (%1)
ConfirmUninstall=Czy na pewno chcesz usun�� program %1 i wszystkie jego sk�adniki?
OnlyAdminCanUninstall=Ta instalacja mo�e by� odinstalowana tylko przez u�ytkownika z prawami administratora.
UninstallStatusLabel=Poczekaj a� %1 zostanie usuni�ty z Twojego komputera.
UninstalledAll=%1 zosta� usuni�ty z Twojego komputera.
UninstalledMost=Odinstalowywanie programu %1 zako�czone.%n%nNiekt�re elementy nie mog�y by� usuni�te. Mo�esz je usun�� r�cznie.
UninstalledAndNeedsRestart=Tw�j komputer musi by� ponownie uruchomiony, aby zako�czy� odinstalowywanie %1.%n%nCzy chcesz teraz ponownie uruchomi� komputer?
UninstallDataCorrupted=Plik "%1" jest uszkodzony. Nie mo�na odinstalowa�

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Usun�� plik wsp�dzielony?
ConfirmDeleteSharedFile2=System wykry�, �e nast�puj�cy plik nie jest ju� u�ywany przez �aden program. Czy chcesz odinstalowa� ten wsp�dzielony plik?%n%nJe�li inne programy nadal u�ywaj� tego pliku, a zostanie on usuni�ty, mog� one przesta� dzia�a� prawid�owo. Je�li nie jeste� pewny, wybierz przycisk Nie. Pozostawienie tego pliku w Twoim systemie nie spowoduje �adnych szk�d.
SharedFileNameLabel=Nazwa pliku:
SharedFileLocationLabel=Po�o�enie:
WizardUninstalling=Stan odinstalowywania
StatusUninstalling=Odinstalowywanie %1...

