; Translation made with Translator 1.32 (http://www2.arnes.si/~sopjsimo/translator.html)
; $Translator:NL=%n:TB=%t
;
; *** Inno Setup version 4.0.0 Catalan messages ***
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; Tradu�t a Catal� per Jos� Manuel P�rez (Xose) - Barcelona
; e-mail: xosem@cablecat.com
; Revisat a partir de la versi� 2.08 i posteriors per Gerard Visent Moln�
; amb l'ajuda inestimable d'Albert P. Mart� (Larry)
; e-mail: gerard@zootec.ad
; Actualitzat des de la versi� 3.0.5+ a la 4.0.0 per Vicent LL�cer Gil
; e-mail: llacer@users.sourceforge.net
;

[LangOptions]
LanguageName=Catal�
LanguageID=$0403
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
SetupAppTitle=Instal�laci�
SetupWindowTitle=Instal�laci� - %1
UninstallAppTitle=Desinstal�lar
UninstallAppFullTitle=Desinstal�lar %1

; *** Icons

; *** Misc. common
InformationTitle=Informaci�
ConfirmTitle=Confirma
ErrorTitle=Error

; *** SetupLdr messages
SetupLdrStartupMessage=Aquest programa instal�lar� %1. Voleu continuar?
LdrCannotCreateTemp=No s'ha pogut crear el directori temporal. Instal�laci� cancel�lada
LdrCannotExecTemp=No s'ha pogut executar el fitxer en el directori temporal. Instal�laci� cancel�lada

; *** Startup error messages
LastErrorMessage=%1.%n%nError %2: %3
SetupFileMissing=L'arxiu %1 no es troba al directori d'instal�laci�. Si us plau, solucioneu el problema o obteniu una nova c�pia del programa.
SetupFileCorrupt=Els arxius d'instal�laci� estan corromputs. Si us plau, obteniu una nova c�pia del programa.
SetupFileCorruptOrWrongVer=Els arxius d'instal�laci� estan danyats o s�n incompatibles amb aquesta versi� del programa. Si us plau, solucioneu el problema o obteniu una nova c�pia del programa.
NotOnThisPlatform=Aquest programa no funcionar� sota %1.
OnlyOnThisPlatform=Aquest programa nom�s pot ser executat sota %1.
WinVersionTooLowError=Aquest programa requereix %1 versi� %2 o posterior.
WinVersionTooHighError=Aquest programa no pot ser instal�lat sota %1 versi� %2 o posterior.
AdminPrivilegesRequired=Heu de tenir privilegis d'administrador per poder instal�lar aquest programa
PowerUserPrivilegesRequired=Cal ser un administrador del sistema o b� un membre del grup d'usuaris amb privilegis per insta�lar aquest programa.
SetupAppRunningError=El programa d'instal�laci� ha detectat que %1 s'est� executant actualment.%n%nSi us plau, tanqueu el programa i premeu 'Seg�ent' per continuar o 'Cancel�lar' per sortir.
UninstallAppRunningError=El programa de desinstal�laci� ha detectat que %1 s'est� executant actualment.%n%nSi us plau, tanqueu el programa i premeu 'Seg�ent' per continuar o 'Cancel�lar' per sortir.

; *** Misc. errors
ErrorCreatingDir=El programa d'instal�laci� no ha pogut crear el directori "%1"
ErrorTooManyFilesInDir=No s'ha pogut crear un arxiu al directori "%1" perque aquest cont� massa arxius

; *** Setup common messages
ExitSetupTitle=Sortir
ExitSetupMessage=La instal�laci� no s'ha completat. Si sortiu ara, el programa no ser� instal�lat.%n%nPodeu tornar a executar el programa d'instal�laci� quan vulgueu per completar-la.%n%nVoleu sortir?
AboutSetupMenuItem=&Sobre la instal�laci�...
AboutSetupTitle=Sobre la instal�laci�
AboutSetupMessage=%1 versi� %2%n%3%n%nP�gina web de %1:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Tornar
ButtonNext=&Seg�ent >
ButtonInstall=&Instal�lar
ButtonOK=Seg�ent
ButtonCancel=Cancel�lar
ButtonYes=&S�
ButtonYesToAll=S� a &Tot
ButtonNo=&No
ButtonNoToAll=N&o a tot
ButtonFinish=&Finalitzar
ButtonBrowse=&Explorar...

; *** "Select Language" dialog messages
SelectLanguageTitle=Elegisca l'idioma de l'instal�lador
SelectLanguageLabel=Elegisca l'idioma a utilitzar durant la instal�laci�:

; *** Common wizard text
ClickNext=Premeu 'Seg�ent' per continuar o 'Cancel�lar' per sortir del programa.

; *** "Welcome" wizard page
BeveledLabel=

; *** "Password" wizard page
WelcomeLabel1=Benvingut a l'assistent d'instal�laci� de [name]
WelcomeLabel2=S'instal�lar� [name/ver] al seu ordinador.%n%nEs recomana que es tanqueu totes les aplicacions que s'estan executant abans de continuar. Aix� ajudar� a prevenir conflictes durant el proc�s d'instal�laci�.
WizardPassword=Codi d'acc�s
PasswordLabel1=Aquesta instal�laci� est� protegida amb un codi d'acc�s.
PasswordLabel3=Indiqueu el codi d'acc�s i premeu 'Seg�ent' per continuar. El codi �s sensible a les maj�scules/min�scules.
PasswordEditLabel=&Codi:
IncorrectPassword=El codi introdu�t no �s correcte. Torneu-ho a intentar.

; *** "License Agreement" wizard page
WizardLicense=Acceptaci� de la llicencia d'�s.
LicenseLabel=Si us plau, llegiu aquesta informaci� important abans de continuar.

; *** "Information" wizard pages
LicenseLabel3=Si us plau, llegiu el seg�ent Acord de Llic�ncia. Heu d'acceptar els termes d'aquest acord abans de continuar amb l'instal�laci�.
LicenseAccepted=&Accepto l'acord
LicenseNotAccepted=&No accepto l'acord
WizardInfoBefore=Informaci�
InfoBeforeLabel=Si us plau, llegiu la seg�ent informaci� abans de continuar.
InfoBeforeClickLabel=Quan esteu preparat per continuar, premeu 'Seg�ent >'
WizardInfoAfter=Informaci�
InfoAfterLabel=Si us plau, llegiu la seg�ent informaci� abans de continuar.
InfoAfterClickLabel=Quan esteu preparat per continuar, premeu 'Seg�ent >'

; *** "Select Destination Directory" wizard page
WizardUserInfo=Informaci� sobre l'usuari
UserInfoDesc=Si us plau, entreu la vostra informaci�.
UserInfoName=&Nom de l'usuari:
UserInfoOrg=&Organitzaci�
UserInfoSerial=&N�mero de s�rie:
UserInfoNameRequired=Heu d'entrar un nom.
WizardSelectDir=Triar Directori de Dest�
; the %1 below is changed to either DirectoryOld or DirectoryNew
; depending on whether the user is running Windows 3.x, or 95 or NT 4.0
SelectDirDesc=On s'ha d'instal�lar [name]?
SelectDirLabel=Trieu el directori on voleu instal�lar [name], i despr�s premeu 'Seg�ent'.
DiskSpaceMBLabel=Aquest programa necessita un m�nim de [mb] MB d'espai en disc.
ToUNCPathname=El programa d'instal�laci� no pot instal�lar el programa en un directori UNC. Si esteu  provant d'instal�lar-lo en xarxa, haureu d'assignar una lletra (D:,E:,etc...) al disc de dest�.
InvalidPath=Cal informar un cam� complet amb lletra d'unitat, per exemple:%n%nC:\Aplicaci�%n%no b� un cam� UNC en la forma:%n%n\\servidor\compartit
InvalidDrive=El disc o cam� de xarxa seleccionat no existeix, si us plau trieu-ne un altre.
DiskSpaceWarningTitle=No hi ha prou espai en disc
DiskSpaceWarning=El programa d'instal�laci� necessita com a m�nim %1 KB d'espai lliure, per� el disc seleccionat nom�s t� %2 KB disponibles.%n%nTot i aix�, voleu continuar?
BadDirName32=Un nom de directori no pot contenir cap dels caracters seg�ents:%n%n%1
DirExistsTitle=El directori existeix
DirExists=El directori:%n%n%1%n%nja existeix. Voleu instal�lar el programa en aquest directori?
DirDoesntExistTitle=El directori no existeix
DirDoesntExist=El directori:%n%n%1%n%nno existeix. Voleu crear-lo?

; *** "Select Program Group" wizard page
WizardSelectComponents=Triar Components
SelectComponentsDesc=Quins components cal instal�lar?
SelectComponentsLabel2=Seleccioneu �nicament els components que voleu instal�lar. Premeu 'Seg�ent' per continuar.
FullInstallation=Instal�laci� completa
CompactInstallation=Instal�laci� m�nima
CustomInstallation=Instal�laci� personalitzada
NoUninstallWarningTitle=Els components ja existeixen
NoUninstallWarning=El programa d'instal�laci� ha detectat que els seg�ents components ja es troben al seu ordinador:%n%n%1%n%nEl fet que no estiguin seleccionats no els desinstal�lar�.%n%nVoleu continuar?
ComponentSize1=%1 Kb
ComponentSize2=%1 Mb
ComponentsDiskSpaceMBLabel=La selecci� de components actual requereix un m�nim de [mb] mb d'espai lliure en el disc.
WizardSelectTasks=Triar tasques adicionals
SelectTasksDesc=Quines tasques adicionals cal executar?
SelectTasksLabel2=Treu les tasques adicionals que voleu que s'executin durant la instal�laci� de [name], despr�s premeu 'Seg�ent'.
WizardSelectProgramGroup=Seleccionar un Grup de Programes
; the %1 below is changed to either ProgramManagerOld or ProgramManagerNew
; depending on whether the user is running Windows 3.x, or 95 or NT 4.0
SelectStartMenuFolderDesc=On voleu que el programa d'instal�laci� cre� els enlla�os?
SelectStartMenuFolderLabel=Trieu la carpeta del Men� Inici on voleu que el programa d'instal�laci� cre� els enlla�os, despr�s premeu 'Seg�ent'.
NoIconsCheck=&No crear cap icona
MustEnterGroupName=Heu d'informar un nom pel grup de programes.
BadGroupName=El nom del grup no pot contenir cap dels seg�ents car�cters:%n%n%1

; *** "Ready to Install" wizard page
NoProgramGroupCheck2=&No crear una carpeta al Men� Inici
WizardReady=Preparat per instal�lar
ReadyLabel1=El programa d'instal�laci� comen�ar� ara la instal�laci� de [name] al seu ordinador.
ReadyLabel2a=Premeu 'Instal�lar' per continuar amb la instal�laci�, o 'Tornar' si voleu revisar o modificar les opcions d'instal�laci�.
ReadyLabel2b=Premeu 'Instal�lar' per continuar amb la instal�laci�.

; *** "Setup Completed" wizard page
ReadyMemoUserInfo=Informaci� sobre l'usuari:
ReadyMemoDir=Directori de dest�:
ReadyMemoType=Tipus d'instal�laci�:
ReadyMemoComponents=Components seleccionats:
ReadyMemoGroup=Carpeta del Men� Inici:
ReadyMemoTasks=Tasques adicionals:
WizardPreparing=Preparant l'instal�laci�
PreparingDesc=Preparant l'instal�laci� de [name] al seu ordinador.
PreviousInstallNotCompleted=L'instal�laci� o desinstal�laci� anterior s'ha dut a terme. Cal que reinicicieu el vostre ordinador per  finalitzar aquesta l'instal�laci� o desintal�laci�.%n%nDespr�s de reiniciar el vostre ordinador, executeu aquest programa de nou per completar l'instal�laci� de [name].
CannotContinue=L'instal�laci� no pot continuar. Si us plau, premeu el bot� 'Cancel�lar' per sortir.
WizardInstalling=Instal�lant
InstallingLabel=Si us plau, espereu mentre s'instal�la [name] al seu ordinador.
FinishedHeadingLabel=Finalitzant l'assistent d'instal�laci� de [name]
FinishedLabelNoIcons=El programa ha finalitzat la instal�laci� de [name] al seu ordinador.
FinishedLabel=El programa ha finalitzat la instal�laci� de [name] al seu ordinador. Podeu executar l'aplicaci� seleccionant les icones instal�lades.
ClickFinish=Premeu 'Finalitzar' per sortir de la instal�laci�.
FinishedRestartLabel=Per completar la instal�laci� de [name], el programa ha de reiniciar el seu ordinador. Voleu que ho faci ara?
FinishedRestartMessage=Per completar la instal�laci� de [name] cal reiniciar el seu ordinador.%n%nVoleu  que ho faci ara?
ShowReadmeCheck=S�, vull veure el fitxer LLEGIUME.TXT
YesRadio=&S�, reiniciar ara
NoRadio=&No, reiniciar� l'ordinador m�s tard

; *** "Setup Needs the Next Disk" stuff
RunEntryExec=Executar %1
RunEntryShellExec=Visualitzar %1
ChangeDiskTitle=El programa d'instal�laci� necessita el seg�ent disc
SelectDirectory=Seleccioneu directori
; the %2 below is changed to either SDirectoryOld or SDirectoryNew
; depending on whether the user is running Windows 3.x, or 95 or NT 4.0
SelectDiskLabel2=Si us plau, introduiu el disc %1 i premeu 'Continuar'.%n%nSi els fitxers d'aquest disc es poden trobar en un directori diferent de l'indicat a continuaci�, indiqueu el cam� correcte o b� premeu 'Explorar' per trobar-los.
PathLabel=&Cam�:
; the %3 below is changed to either SDirectoryOld or SDirectoryNew
; depending on whether the user is running Windows 3.x, or 95 or NT 4.0
FileNotInDir2=El fitxer "%1" no s'ha pogut trobar a "%2". Si us plau, introduiu el disc correcte o seleccioneu un altre directori.
SelectDirectoryLabel=Si us plau, indiqueu on es troba el seg�ent disc.

; *** Installation phase messages
SetupAborted=La instal�laci� no s'ha completat.%n%n%Solucioneu el problema abans d'executar de nou el programa d'instal�laci�.
EntryAbortRetryIgnore=Trieu 'Reintentar' per tornar-ho a intentar, 'Ignorar' per continuar, o 'Cancel�lar' per cancel�lar la instal�laci�.

; *** Installation status messages
StatusCreateDirs=Creant directoris...
StatusExtractFiles=Extraient arxius...
StatusCreateIcons=Creant icones de programa...
StatusCreateIniEntries=Creant entrades del fitxer INI...
StatusCreateRegistryEntries=Creant entrades de registre...
StatusRegisterFiles=Registrant arxius...
StatusSavingUninstall=Desant informaci� de desinstal�laci�...

; *** Misc. errors
StatusRunProgram=Finalitzant la instal�laci�...
StatusRollback=Retrocedint els canvis...
ErrorInternal2=Error intern: %1
ErrorFunctionFailedNoCode=%1 ha fallat
ErrorFunctionFailed=%1 ha fallat; codi %2
ErrorFunctionFailedWithMessage=%1 ha fallat; codi %2.%n%3
ErrorExecutingProgram=No �s possible executar el fitxer:%n%1

; *** DDE errors

; *** Registry errors
ErrorRegOpenKey=Error obrint la clau de registre:%n%1\%2
ErrorRegCreateKey=Error creant la clau de registre:%n%1\%2
ErrorRegWriteKey=Error escrivint a la clau de registre:%n%1\%2

; *** INI errors
ErrorIniEntry=Error creant la entrada INI al fitxer "%1".

; *** File copying errors
FileAbortRetryIgnore=Trieu 'Reintentar' per tornar-ho a intentar, 'Ignorar' per continuar sense aquest fitxer (no recomanat), o 'Cancel�lar' per cancel�lar la instal�laci�.
FileAbortRetryIgnore2=Trieu 'Reintentar' per tornar-ho a intentar, 'Ignorar' per continuar (no recomenat), o 'Cancel�lar' per cancel�lar la instal�laci�.
SourceIsCorrupted=El fitxer d'origen est� corrupte
SourceDoesntExist=El fitxer d'origen "%1" no existeix
ExistingFileReadOnly=El fitxer �s nom�s de lectura.%n%nTrieu 'Reintentar' per treure l'atribut de nom�s lectura i tornar-ho a intentar, 'Ignorar' per continuar sense aquest fitxer, o 'Cancel�lar' per cancel�lar la instal�laci�.
ErrorReadingExistingDest=S'ha produit ha hagut un error llegint el fitxer:
FileExists=El fitxer ja existeix.%n%nVoleu sobreescriure'l?
ExistingFileNewer=El fitxer existent �s m�s actual que el que s'intenta instal�lar. Es recomana mantenir el fitxer existent.%n%nVoleu mantenir el fitxer existent?
ErrorChangingAttr=Hi ha hagut un error canviant els atributs del fitxer:
ErrorCreatingTemp=Hi ha hagut un error creant un fitxer en el directori de dest�:
ErrorReadingSource=Hi ha hagut un error llegint el fitxer d'origen:
ErrorCopying=Hi ha hagut un error copiant el fitxer:
ErrorReplacingExistingFile=Hi ha hagut un error reempla�ant el fitxer:
ErrorRestartReplace=Reempla�ar ha fallat:
ErrorRenamingTemp=Hi ha hagut un error renombrant un fitxer en el directori de dest�:
ErrorRegisterServer=No s'ha pogut registrar el DLL/OCX: %1
ErrorRegisterServerMissingExport=No s'ha trobat l'exportador DllRegisterServer
ErrorRegisterTypeLib=No s'ha pogut registrar la biblioteca de tipus: %1

; *** Post-installation errors
ErrorOpeningReadme=Hi ha hagut un error obrint el fitxer LLEGIUME.TXT.
ErrorRestartingComputer=El programa d'instal�laci� no ha pogut reiniciar l'ordinador. Si us plau, feu-ho manualment.

; *** Uninstaller messages
UninstallNotFound=L'arxiu "%1" no existeix. No es pot desinstal�lar.
UninstallUnsupportedVer=El fitxer de desinstal�laci� "%1" est� en un format no reconegut per aquesta versi� del desinstal�lador. No es pot desinstal�lar
UninstallUnknownEntry=S'ha trobat una entrada desconeguda (%1) al fitxer de desinstal�laci�.
ConfirmUninstall=Esteu segur de voler eliminar completament %1 i tots els seus components?
OnlyAdminCanUninstall=Aquest programa nom�s pot ser desinstal�lat per un usuari amb privilegis d'administrador.
UninstallStatusLabel=Si us plau, espereu mentre s'elimina %1 del seu ordinador.
UninstalledAll=%1 ha estat desinstal�lat correctament del seu ordinador.
UninstalledMost=Desintal�laci� de %1 completada.%n%nAlguns elements no s'han pogut eliminar. Poden ser eliminats manualment.
UninstalledAndNeedsRestart=Per completar l'instal�laci� de %1, cal reiniciar el vostre ordinador.%n%nVoleu reiniciar-lo ara?
UninstallDataCorrupted=El fitxer "%1" est� corromput. No es pot desinstal�lar.
UninstallOpenError=El fitxer "%1" no es va poder obrir. No es pot desinstal�lar.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Eliminar Arxiu Compartit?
ConfirmDeleteSharedFile2=El sistema indica que el seg�ent arxiu compartit ja no el fa servir cap altre programa. Voleu eliminar aquest arxiu compartit?%n%nSi algun programa encara fa servir aquest arxiu, podria no funcionar correctament si s'elimina. Si no esteu segur, trieu 'No'. Deixar l'arxiu al sistema no far� cap mal.
SharedFileNameLabel=Nom de l'arxiu:
SharedFileLocationLabel=Localitzaci�:
WizardUninstalling=Estat de la desinstal�laci�
StatusUninstalling=Desinstal�lant %1...


