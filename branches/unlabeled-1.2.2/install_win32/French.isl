; *** Inno Setup version 4 French messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.htm
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; Transtated by Alain MILANDRE (v3.0.5) email almi@almiservices.com
; Update to v3.0.6 by Alexandre STANCIC email a.stancic@laposte.net
; Update to v4.0.0-pre1 by Romain TARTIERE email romain-tartiere@astase.com

[LangOptions]
LanguageName=French
LanguageID=$0040
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
SetupAppTitle=Installation
SetupWindowTitle=Installation - %1
UninstallAppTitle=D�sinstallation
UninstallAppFullTitle=D�sinstallation de %1

; *** Misc. common
InformationTitle=Information
ConfirmTitle=Confirmation
ErrorTitle=Erreur

; *** SetupLdr messages
SetupLdrStartupMessage=%1 va �tre install�(e). Souhaitez-vous continuer ?
LdrCannotCreateTemp=Impossible de cr�er un fichier temporaire. Installation annul�e
LdrCannotExecTemp=Impossible d'ex�cuter un fichier depuis le r�pertoire temporaire. Installation annul�e

; *** Startup error messages
LastErrorMessage=%1.%n%nErreur %2: %3
SetupFileMissing=Le fichier %1 n'a pas �t� trouv� dans le r�pertoire de l'installation. Corrigez le probl�me ou obtenez une nouvelle copie du programme.
SetupFileCorrupt=Les fichiers de l'installation sont alt�r�s. Obtenez une nouvelle copie du programme.
SetupFileCorruptOrWrongVer=Les fichiers d'installation sont corrompus ou sont incompatibles avec cette version de l'installeur. Corrigez le probl�me ou obtenez une nouvelle copie du programme.
NotOnThisPlatform=Ce programme ne s'ex�cutera pas sur %1.
OnlyOnThisPlatform=Ce programme doit �tre ex�cut� sur  %1.
WinVersionTooLowError=Ce programme requiert  %1 version %2 ou sup�rieure.
WinVersionTooHighError=Ce programme ne peut �tre install� sur %1 version %2 ou sup�rieure.
AdminPrivilegesRequired=Vous devez �tre connect� en tant qu'administrateur pour installer ce programme.
PowerUserPrivilegesRequired=Vous devez �tre authentifi� en tant qu'administrateur ou comme un membre du groupe Administrateurs pour installer ce programme.
SetupAppRunningError=L'installation a d�tect� que %1 est actuellement en cours d'ex�cution.%n%nFermez toutes les instances de cette application  maintenant, puis cliquez OK pour continuer, ou Annulation pour arr�ter l'installation.
UninstallAppRunningError=La proc�dure de d�sinstallation a d�tect� que %1 est actuellement en cours d'ex�cution.%n%nFermez toutes les instances de cette application  maintenant, puis cliquez OK pour continuer, ou Annulation pour arr�ter l'installation.

; *** Misc. errors
ErrorCreatingDir=L'installeur n'a pas pu cr�er le r�pertoire "%1"
ErrorTooManyFilesInDir=L'installeur n'a pas pus cr�er un fichier dans le r�pertoire "%1", il doit contenir trop de fichiers

; *** Setup common messages
ExitSetupTitle=Quitter l'installation
ExitSetupMessage=L'installation n'est pas termin�e. Si vous quittez maintenant, le programme ne sera pas install�.%n%nVous devrez relancer l'installation une autre fois pour la terminer.%n%nQuitter l'installation ?
AboutSetupMenuItem=&A propos...
AboutSetupTitle=A Propos...
AboutSetupMessage=%1 version %2%n%3%n%n%1 Web:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Pr�c�dent
ButtonNext=&Suivant >
ButtonInstall=&Installer
ButtonOK=OK
ButtonCancel=Annuler
ButtonYes=&Oui
ButtonYesToAll=Oui pour &tout
ButtonNo=&Non
ButtonNoToAll=N&on pour tout
ButtonFinish=&Terminer
ButtonBrowse=&Parcourir...

; *** "Select Language" dialog messages
SelectLanguageTitle=Langue de l'installation
SelectLanguageLabel=Choisissez la langue � utiliser durant la proc�dure d'installation :

; *** Common wizard text
ClickNext=Cliquez sur "Suivant" pour continuer ou sur "Annuler" pour quitter l'installation.
BeveledLabel=

; *** "Welcome" wizard page
WelcomeLabel1=Bienvenue dans la proc�dure d'installation de [name]
WelcomeLabel2=Ceci installera [name/ver] sur votre ordinateur.%n%nIl est recommand� de fermer toutes les applications actives avant de continuer.

; *** "Password" wizard page
WizardPassword=Mot de passe
PasswordLabel1=Cette installation est prot�g�e par un mot de passe.
PasswordLabel3=Entrez votre mot de passe.%n%nLes mots de passes tiennent compte des majuscules et des minuscules.
PasswordEditLabel=&Mot de passe :
IncorrectPassword=Le mot de passe entr� n'est pas correct. Essayez � nouveau.

; *** "License Agreement" wizard page
WizardLicense=Accord de licence
LicenseLabel=Veuillez lire l'information importante suivante avant de continuer.
LicenseLabel3=Veuillez lire l'accord de licence qui suit. Utilisez la barre de d�filement ou la touches "Page suivante" pour lire le reste de la licence.
LicenseAccepted=J'&accepte les termes du contrat de licence
LicenseNotAccepted=Je &refuse les termes du contrat de licence

; *** "Information" wizard pages
WizardInfoBefore=Information
InfoBeforeLabel=Veuillez lire ces informations importantes avant de continuer.
InfoBeforeClickLabel=Lorsque vous serez pr�t � continuer, cliquez sur "Suivant".
WizardInfoAfter=Information
InfoAfterLabel=Veuillez lire ces informations importantes avant de continuer.
InfoAfterClickLabel=Lorsque vous serez pr�t � continuer, cliquez sur "Suivant".

; *** "User Information" wizard page
WizardUserInfo=Information utilisateur
UserInfoDesc=Veuillez entrer vos coordonn�es
UserInfoName=&Nom d'utilisateur:
UserInfoOrg=&Organisation:
UserInfoSerial=Numero de &S�rie:
UserInfoNameRequired=Vous devez �crire un nom.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Choisissez le r�pertoire de destination
SelectDirDesc=O� devrait �tre install� [name] ?
SelectDirLabel=Choisissez le dossier ou vous d�sirez installer [name], cliquez ensuite sur Suivant.
DiskSpaceMBLabel=Le programme requiert au moins [mb] Mo d'espace disque.
ToUNCPathname=L'installateur ne sait pas utiliser les chemins UNC. Si vous souhaitez installer [name] sur un r�seau, vous devez "mapper" un disque.
InvalidPath=Vous devez entrer un chemin complet avec un nom de lecteur; par exemple :%nC:\APP
InvalidDrive=Le lecteur que vous avez s�lectionn� n'existe pas. Choisissez en un autre.
DiskSpaceWarningTitle=Vous n'avez pas assez d'espace disque
DiskSpaceWarning=L'installation n�cessite au moins %1 Ko d'espace disque libre. Le lecteur s�lectionn� n'a que %2 Ko de disponible.%n%nSouhaitez-vous tout de m�me continuer ?
BadDirName32=Le nom du r�pertoire ne peut pas contenir les caract�res suivant :%n%n%1
DirExistsTitle=Ce r�pertoire existe
DirExists=Le r�pertoire :%n%n%1%n%nexiste d�j�. Souhaitez-vous l'utiliser quand m�me ?
DirDoesntExistTitle=Ce r�pertoire n'existe pas
DirDoesntExist=Le r�pertoire :%n%n%1%n%nn'existe pas. Souhaitez-vous qu'il soit cr�� ?

; *** "Select Components" wizard page
WizardSelectComponents=S�lection des composants
SelectComponentsDesc=Quels sont les composants que vous d�sirez installer ?
SelectComponentsLabel2=S�lectionnez les composants que vous d�sirez installer; d�sactiver les composants que vous ne d�sirez pas voir install�s. Cliquez sur Suivant pour poursuivre la proc�dure d'installation.
FullInstallation=Installation compl�te
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Installation Minimum
CustomInstallation=Installation Personnalis�e
NoUninstallWarningTitle=Existance d'un composant
NoUninstallWarning=L'Installation � d�tect� que un ou plusieurs composants sont d�j� install�s sur votre syst�me:%n%n%1%n%nD�s�lectionnez ces composants afin de ne pas risquer de les d�sinstaller.%n%nVoulez-vous tout de m�me continuer?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=La s�lection courante n�cessite [mb] MB d'espace disque disponible.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=S�lection des t�ches suppl�mentaires
SelectTasksDesc=Quels sont les t�ches additionnelles que vous d�sirez ex�cuter ?
SelectTasksLabel2=S�lectionnez les t�ches additionnelles que l'assistant d'installation doit ex�cuter pendant l'installation de [name], cliquez sur Suivant.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Selectionnez un groupe de programmes
SelectStartMenuFolderDesc=O� voulez vous placer les racourcis du programme ?
SelectStartMenuFolderLabel=S�lectionnez le groupe de programme dans lequel vous d�sirez que l'assistant d'installation cr�e les raccourcis du programme, cliquez ensuite sur Suivant.
NoIconsCheck=&Ne pas cr�er d'ic�ne
MustEnterGroupName=Vous devez entrer un nom de groupe.
BadGroupName=L'installation va ajouter les icones du programme dans le groupe du %1 suivant.
NoProgramGroupCheck2=Ne pas cr�er le &groupe de programme

; *** "Ready to Install" wizard page
WizardReady=Pr�t � installer
ReadyLabel1=L'installateur vas maintenant installer [name] sur votre ordinateur.
ReadyLabel2a=Cliquez sur "Installer" pour continuer ou sur "Pr�c�dent" pour changer une option.
ReadyLabel2b=Cliquez sur "Installer" pour continuer.
ReadyMemoUserInfo=Information utilisateur:
ReadyMemoDir=Dossier de destination:
ReadyMemoType=Type d'installation:
ReadyMemoComponents=Composants s�lectionn�s:
ReadyMemoGroup=Dossier du menu de d�marrage:
ReadyMemoTasks=T�ches additionnelles:

; *** "Preparing to Install" wizard page
WizardPreparing=Pr�paration de la phase d'installation
PreparingDesc=L'assistant d'installation est pr�t � installer [name] sur votre ordinateur.
PreviousInstallNotCompleted=L'assistant d'Installation/D�sinstallation d'une version pr�c�dente du programme n'est pas totalement achev�. Vueillez red�marrer votre ordinateur pour achever la phase d'installation.%n%nApr�s le red�marrage de votre ordinateur,  ex�cutez cet assistant pour ex�cuter une installation compl�te de [name].
CannotContinue=L'assistant d'installation ne peut continuer. Veulliez cliquer sur Annuler pour quitter l'Assistant d'installation.

; *** "Installing" wizard page
WizardInstalling=Installation en cours
InstallingLabel=Veuillez patienter pendant que l'assistant d'installation copie [name] sur votre ordinateur.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Finalisation de l'assistant d'installation de [name]
FinishedLabelNoIcons=L'installation a termin� d'installer [name] sur votre ordinateur.
FinishedLabel=L'installation a termin� d'installer [name] sur votre ordinateur. L'application peut �tre lanc�e par la s�lection de l'ic�ne install�e.
ClickFinish=Cliquez sur Terminer pour quitter la proc�dure d'installation.
FinishedRestartLabel=Pour finir l'installation de [name], L'installeur doit red�marrer votre ordinateur.%n%nVoulez-vous red�marrer maintenant ?
FinishedRestartMessage=Pour finir l'installation de [name], L'installeur doit red�marrer votre ordinateur.%n%nVoulez-vous red�marrer maintenant ?
ShowReadmeCheck=Oui, Je veux bien lire le fichier LisezMoi
YesRadio=&Oui, Red�marrer l'ordinateur maintenant
NoRadio=&Non, je d�sire red�marrer mon ordinateur plus tard
; used for example as 'Run MyProg.exe'
RunEntryExec=Executer %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Voir %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=L'installation a besoin du disque suivant
SelectDirectory=S�lectionnez un r�pertoire
SelectDiskLabel2=Veuillez ins�rer le disque %1 et cliquer sur OK.%n%nSi les fichiers de ce disque peuvent �tre trouv�s dans un %2 diff�rent de celui affich� ci-dessous, entrez le chemin correct ou cliquez sur "Chercher".
PathLabel=&Chemin:
FileNotInDir2=Le fichier "%1" ne peut pas �tre trouv� dans "%2". Veuillez ins�rer le disque correct ou s�lectionnez un autre %3.
SelectDirectoryLabel=Veuillez indiquer l'emplacement du disque suivant.

; *** Installation phase messages
SetupAborted=L'installation n'a pas �t� termin�e.%n%nVeuillez corriger le probl�me et relancer l'installation.
EntryAbortRetryIgnore=Cliquez sur "Reprendre" pour essayer � nouveau, sur "Ignorer" pour continuer dans tous les cas, ou sur "Abandonner" pour annuler l'installation.

; *** Installation status messages
StatusCreateDirs=Cr�ation des r�pertoires...
StatusExtractFiles=Extraction des fichiers...
StatusCreateIcons=Cr�ation des ic�nes...
StatusCreateIniEntries=Cr�ation des entr�es de profils...
StatusCreateRegistryEntries=Cr�ation des entr�es de registre...
StatusRegisterFiles=Enregistrement des fichiers...
StatusSavingUninstall=Sauvegarde des informations de d�sinstallation...
StatusRunProgram=Finalisation de l'installation...
StatusRollback=Annulation des changements...

; *** Misc. errors
ErrorInternal2=Erreur interne %1
ErrorFunctionFailedNoCode=%1 �chec
ErrorFunctionFailed=%1 �chec; code %2
ErrorFunctionFailedWithMessage=%1 �chec; code %2.%n%3
ErrorExecutingProgram=Impossible d'ex�cuter le fichier :%n%1

; *** Registry errors
ErrorRegOpenKey=Erreur pendant l'ouverture de la clef de registre:%n%1\%2
ErrorRegCreateKey=Erreur pendant la cr�ation de la clef de registre:%n%1\%2
ErrorRegWriteKey=Erreur lors de l'�criture de la clef de registre:%n%1\%2

; *** INI errors
ErrorIniEntry=Erreur de cr�action de l'entr�e"%1" du fichier INI.

; *** File copying errors
FileAbortRetryIgnore=Cliquez sur "Reprendre" pour essayer � nouveau, sur "Ignorer" pour ignorer ce fichier (non recommand�), ou sur "Abandonner" pour annuler l'installation.
FileAbortRetryIgnore2=Cliquez sur "Reprendre" pour essayer � nouveau, sur "Ignorer" continuer dans tous les cas (non recommand�), ou sur "Abandonner" pour annuler l'installation.
SourceIsCorrupted=Le fichier source est alt�r�
SourceDoesntExist=Le fichier source "%1" n'existe pas
ExistingFileReadOnly=Le fichier existe d�j� et est en lecture seule.%n%nCliquez sur "Reprendre" pour supprimer l'attribut lecture seule et r�essayer, sur "Ignorer" pour ignorer ce fichier, ou sur "Abandonner" pour annuler l'installation.
ErrorReadingExistingDest=Le fichier existe d�j� et est en lecture seule.%n%nCliquez sur "Reprendre" pour supprimer l'attribut lecture seule et r�essayer, sur "Ignorer" pour ignorer ce fichier, ou sur "Abandonner" pour annuler l'installation.
FileExists=Le fichier existe d�j�.%n%nSouhaitez-vous que l'installeur l'�crase ?
ExistingFileNewer=Le fichier existant est plus r�cent que celui qui doit �tre install�. Il est recommand� de conserver le fichier existant.%n%nSouhaitez-vous garder le fichier existant ?
ErrorChangingAttr=Une erreur est survenue en essayant de changer les attributs du fichier existant :
ErrorCreatingTemp=Une erreur est survenue en essayant de cr�er un fichier dans le r�pertoire de destination :
ErrorReadingSource=Une erreur est survenue lors de la lecture du fichier source :
ErrorCopying=Une erreur est survenue lors de la copie d'un fichier :
ErrorReplacingExistingFile=Une erreur est survenue lors du remplacement d'un fichier existant :
ErrorRestartReplace=Remplacement au red�marrage �chou� :
ErrorRenamingTemp=Une erreur est survenue en essayant de renommer un fichier dans le r�pertoire de destination :
ErrorRegisterServer=Impossible d'enregistrer la librairie : %1
ErrorRegisterServerMissingExport=DllRegisterServer: export non trouv�
ErrorRegisterTypeLib=Impossible d'enregistrer la librairie de type : %1

; *** Post-installation errors
ErrorOpeningReadme=Une erreur est survenue � l'ouverture du fichier LISEZMOI.
ErrorRestartingComputer=L'installeur a �t� incapable de red�marrer l'ordinateur. Veuillez le faire "manuellement".

; *** Uninstaller messages
UninstallNotFound=Le fichier "%1" n'existe pas. Suppression impossible.
UninstallOpenError=Le fichier "%1" ne peut pas �tre ouvert. Suppression impossible.
UninstallUnsupportedVer=Le fichier journal de d�sinstallation "%1" est dans un format non reconnu par cette version du d�sinstalleur. Impossible de d�sinstaller ce produit.
UninstallUnknownEntry=Une entr�e inconnue (%1) � �t� rencontr�e dans le journal de d�sinstallation
ConfirmUninstall=Souhaitez-vous supprimer d�finitivement %1 ainsi que tous ses composants ?
OnlyAdminCanUninstall=Cette application ne peut �tre supprim�e que par un utilisateur poss�dant les droits d'administration.
UninstallStatusLabel=Patientez pendant la d�sinstallation de %1 de votre ordinateur.
UninstalledAll=%1 a �t� supprim� de votre ordinateur.
UninstalledMost=La d�sinstallation de %1 est termin�e.%n%nCertains �l�ments n'ont pu �tre supprim�s automatiquement. Vous devrez les supprimer "manuellement".
UninstalledAndNeedsRestart=Pour compl�ter la d�sinstallation de %1, il faut red�marrer votre ordinateur.%n%nVoulez-vous red�marrer maintenant ?
UninstallDataCorrupted=Le ficher "%1" est alt�r�. Suppression impossible

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Supprimer les fichiers partag�s ?
ConfirmDeleteSharedFile2=Le syst�me indique que le fichier partag� suivant n'est pas utilis� par d'autres programmes. Souhaitez-vous supprimer celui-ci ?%n%n%1%n%nSi certains programmes utilisent ce fichier et qu'il est supprim�, ces programmes risquent de ne pas fonctionner normallement. Si vous n'�tes pas certain, choisissez Non; laisser ce fichier sur votre syst�me ne pose aucun probl�me.
SharedFileNameLabel=Nom de fichier:
SharedFileLocationLabel=Emplacement:
WizardUninstalling=Etat de la d�sinstallation
StatusUninstalling=D�sintallation de %1...

