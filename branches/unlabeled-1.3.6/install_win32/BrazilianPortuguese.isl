; *** Inno Setup version 4.0.5+ English messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $Id: BrazilianPortuguese.isl,v 1.3.6.1 2004-01-09 17:16:34 shutoff Exp $

[LangOptions]
LanguageName=Portuguese (Brazil)
LanguageID=$0416
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
SetupAppTitle=Programa de Instala��o
SetupWindowTitle=Programa de Instala��o - %1
UninstallAppTitle=Desinstala��o
UninstallAppFullTitle=Desinstala��o - %1

; *** Misc. common
InformationTitle=Informa��es
ConfirmTitle=Confirma��o
ErrorTitle=Erro

; *** SetupLdr messages
SetupLdrStartupMessage=Este programa ir� instalar o %1. Voc� gostaria de continuar?
LdrCannotCreateTemp=N�o foi poss�vel criar um arquivo tempor�rio. Instala��o abortada
LdrCannotExecTemp=N�o foi poss�vel executar um arquivo na pasta de arquivos tempor�rios. Instala��o abortada

; *** Startup error messages
LastErrorMessage=%1.%n%nErro %2: %3
SetupFileMissing=O arquivo %1 est� faltando na pasta de instala��o. Corrija o problema ou obtenha uma nova c�pia do programa.
SetupFileCorrupt=Os arquivos de instala��o est�o corrompidos. Obtenha uma c�pia nova do programa.
SetupFileCorruptOrWrongVer=Os arquivos de instala��o est�o corrompidos ou s�o incompat�veis com esta vers�o do Programa de Instala��o. Corrija o problema ou obtenha uma c�pia nova.
NotOnThisPlatform=Este programa n�o ir� executar em %1.
OnlyOnThisPlatform=Este programa dever� executar em %1.
WinVersionTooLowError=Este programa exige o %1 vers�o %2 ou mais nova.
WinVersionTooHighError=Este programa n�o pode ser instalado no %1 vers�o %2 ou mais nova.
AdminPrivilegesRequired=Voc� dever� estar logado como administrador para instalar este programa.
PowerUserPrivilegesRequired=Voc� deve ser logado como um administrador ou como um membro do grupo "Power Users" quando instalar este programa.
SetupAppRunningError=O programa de instala��o detectou que %1 est� em executando.%n%nPor favor, feche todas as suas inst�ncias agora, e ent�o clique em OK para continuar, ou    Cancelar para sair.
UninstallAppRunningError=O programa de desinstala��o detectou que %1 est� executando.%n%nPor favor, feche todas as suas inst�ncias agora, e ent�o clique em OK para continuar, ou Cancelar para sair.

; *** Misc. errors
ErrorCreatingDir=O programa de instala��o n�o pode criar a pasta "%1"
ErrorTooManyFilesInDir=N�o foi poss�vel criar um arquivo no pasta "%1" - muitos arquivos

; *** Setup common messages
ExitSetupTitle=Terminar a Instala��o
ExitSetupMessage=A instala��o n�o est� completa. Se voc� terminar agora, o programa n�o ser� instalado.%n%nVoc� poder� tentar novamente mais tarde para completar a instala��o.%n%nTerminar a instala��o?
AboutSetupMenuItem=&Sobre o Programa de Instala��o...
AboutSetupTitle=Sobre o Programa de Instala��o
AboutSetupMessage=%1 vers�o %2%n%3%n%n%1 home page:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Voltar
ButtonNext=&Avan�ar >
ButtonInstall=&Instalar
ButtonOK=OK
ButtonCancel=Cancelar
ButtonYes=&Sim
ButtonYesToAll=Sim para &Todos
ButtonNo=&N�o
ButtonNoToAll=N�&o para Todos
ButtonFinish=&Concluir
ButtonBrowse=&Procurar...

; *** "Select Language" dialog messages
SelectLanguageTitle=Selecione o idioma da instala��o
SelectLanguageLabel=Selecione o idioma a ser utilizado durante a instala��o:

; *** Common wizard text
ClickNext=Clique Avan�ar para continuar, Cancelar para encerrar.
BeveledLabel=

; *** "Welcome" wizard page
WelcomeLabel1=Bem-vindo ao Assistente de Instala��o do [name].
WelcomeLabel2=Este programa ir� instalar o [name/ver] no seu computador.%n%n� recomendado que voc� feche as aplica��es abertas antes de continuar. Isto evitar� conflitos durante a instala��o.

; *** "Password" wizard page
WizardPassword=Senha
PasswordLabel1=Esta instala��o est� protegida por senha.
PasswordLabel3=Por favor digite a senha e ent�o clique em Avan�ar para continuar. Mai�sculas e min�sculas s�o diferentes.
PasswordEditLabel=&Senha:
IncorrectPassword=A senha que voc� digitou n�o est� correta. Tente novamente.

; *** "License Agreement" wizard page
WizardLicense=Contrato de Licen�a de Uso
LicenseLabel=Leia as seguintes informa��es importantes antes de continuar.
LicenseLabel3=Leia a licen�a seguinte. Voc� precisa aceitar os termos desta licen�a antes de continuar com a instala��o
LicenseAccepted=Eu &aceito a licen�a
LicenseNotAccepted=Eu &n�o aceito a licen�a

; *** "Information" wizard pages
WizardInfoBefore=Informa��o
InfoBeforeLabel=Leia as seguintes informa��es importantes antes de continuar.
InfoBeforeClickLabel=Quando voc� estiver pronto para continuar clique em Avan�ar.
WizardInfoAfter=Informa��o
InfoAfterLabel=Leia as seguintes informa��es importantes antes de continuar.
InfoAfterClickLabel=Quando voc� estiver pronto para continuar clique em Avan�ar.

; *** "User Information" wizard page
WizardUserInfo=Informa��es do Usu�rio
UserInfoDesc=Informe seus dados
UserInfoName=&Nome de Usu�rio:
UserInfoOrg=&Organiza��o:
UserInfoSerial=&N�mero Serial:
UserInfoNameRequired=Voc� precisa informar um nome

; *** "Select Destination Directory" wizard page
WizardSelectDir=Escolha a pasta destino
SelectDirDesc=Onde [name] ser� instalado?
SelectDirLabel=Escolha a pasta onde voc� quer instalar [name] e ent�o clique em Avan�ar.
DiskSpaceMBLabel=Este programa exige [mb] MB de espa�o.
ToUNCPathname=O Programa de Instala��o n�o pode instalar em um caminho UNC. Se voc� est� tentando instalar em uma rede, voc� precisa mapear uma unidade da rede.
InvalidPath=Voc� deve entrar um caminho completo com a letra da unidade; por exemplo:%nC:\APP
InvalidDrive=A unidade n�o existe. Escolha outra.
DiskSpaceWarningTitle=N�o h� espa�o suficiente.
DiskSpaceWarning=O Programa de Instala��o exige %1 KB de espa�o livre para instalar, mas a unidade selecionada tem somente %2 KB dispon�veis.%n%voc� quer continuar?
BadDirName32=O nome da pasta n�o pode conter os seguintes caracteres:%n%n%1
DirExistsTitle=A pasta j� existe
DirExists=A pasta%n%n%1%n%nj� existe. Voc� gostaria de instalar nesta pasta?
DirDoesntExistTitle=A pasta n�o existe.
DirDoesntExist=A pasta :%n%n%1%n%nn�o existe. Voc� gostaria de criar a pasta?

; *** "Select Components" wizard page
WizardSelectComponents=Selecione Componentes
SelectComponentsDesc=Que componentes ser�o instalados?
SelectComponentsLabel2=Selecione os componentes que voc� quer instalar, desmarque os componentes que voc� n�o quer instalar. Clique em Avan�ar quando voc� estiver pronto para continuar.
FullInstallation=Instala��o Completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instala��o Compacta
CustomInstallation=Instala��o Personalizada
NoUninstallWarningTitle=Componentes Encontrados
NoUninstallWarning=O Programa de Instala��o detectou que os seguintes componentes est�o instalados em seu computador:%n%n%1%n%nDesmarcando estes componentes eles n�o ser�o desinstalados.%n%nVoc� gostaria de continuar assim mesmo?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Sele��o atual requer [mb] MB de espa�o.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Selecione Tarefas Adicionais
SelectTasksDesc=Que tarefas adicionais ser�o executadas?
SelectTasksLabel2=Selecione as tarefas adicionais que voc� gostaria que o Programa de Instala��o execute enquanto instala o [name] e ent�o clique em Avan�ar.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Escolha a pasta do Menu Iniciar
SelectStartMenuFolderDesc=Onde o Programa de Instala��o ir� criar os �cones do programa?
SelectStartMenuFolderLabel=Escolha a pasta do Menu Iniciar onde o Setup Programa de Instala��o ir� criar os �cones do programa e ent�o clique em Avan�ar
NoIconsCheck=N�o criar �cones
MustEnterGroupName=Voc� deve digitar um nome de uma pasta do Menu Iniciar
BadGroupName=O nome do pasta n�o pode incluir os seguintes caracteres:%n%n%1
NoProgramGroupCheck2=&N�o criar a pasta do Menu Iniciar

; *** "Ready to Install" wizard page
WizardReady=Pronto para Instalar
ReadyLabel1=O Programa de Instala��o est� pronto para iniciar a instalar [name] no seu computador
ReadyLabel2a=Clique em Instalar para iniciar a instala��o, ou clique Voltar se voc� quer rever ou verificar suas op��es
ReadyLabel2b=Clique em Instalar para iniciar a instala��o
ReadyMemoUserInfo=Informa��es do Usu�rio:
ReadyMemoDir=Diret�rio Destino:
ReadyMemoType=Tipo de Instala��o:
ReadyMemoComponents=Componentes Selecionados:
ReadyMemoGroup=Grupo de Programas:
ReadyMemoTasks=Tarefas Adicionais:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparando para Instalar
PreparingDesc=Instala��o est� preparando para instalar [name] em seu computador
PreviousInstallNotCompleted=A instala��o/remo��o do programa anterior n�o foi completada. Voc� precisa reiniciar o computadorpara completar esta instala��o. %n%nAp�s reiniciar seu computador, rode a Instala��o novamente para completar a instala��o do [name].
CannotContinue=A Instala��o n�o pode continuar. Clique em Cancelar para sair.

; *** "Installing" wizard page
WizardInstalling=Instalando
InstallingLabel=Aguarde enquanto o Programa de Instala��o instala [name] em seu computador

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Instala��o Terminada
FinishedLabelNoIcons=O Programa de Instala��o terminou de instalar [name] no seu computador.
FinishedLabel=O Programa de Instala��o terminou de instalar [name] no seu computador. A aplica��o pode ser iniciada escolhendo os �cones instalados.
ClickFinish=Clique em Concluir para finalizar o Programa de Instala��o.
FinishedRestartLabel=Para completar a instala��o do [name], o Programa de Instala��o dever� reiniciar o seu computador. Voc� gostaria de reiniciar agora?
FinishedRestartMessage=Para completar a instala��o do [name], o Programa de Instala��o dever� reiniciar o seu computador. Voc� gostaria de reiniciar agora?
ShowReadmeCheck=Sim, eu quero ver o arquivo LEIAME
YesRadio=&Sim, reiniciar o computador agora
NoRadio=&N�o, eu reiniciarei o computador mais tarde
; used for example as 'Run MyProg.exe'
RunEntryExec=Executar %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Visualizar %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=O Programa de Instala��o precisa do pr�ximo disco
SelectDirectory=Escolha a Pasta
SelectDiskLabel2=Insira o disco %1 e clique OK.%n%nSe os arquivos deste disco estiverem em uma outra pasta, digite o caminho correto ou clique em Procurar.
PathLabel=&Caminho:
FileNotInDir2=O arquivo "%1" n�o p�de ser encontrado em "%2". Insira o disco correto ou escolha outra pasta.
SelectDirectoryLabel=Indique a localiza��o do pr�ximo disco.

; *** Installation phase messages
SetupAborted=A instala��o n�o foi completada.%n%nCorrija o problema e execute o Programa de Instala��o novamente.
EntryAbortRetryIgnore=Clique Repetir para tentar novamente, Ignorar para continuar, Anular para cancelar.

; *** Installation status messages
StatusCreateDirs=Criando pastas...
StatusExtractFiles=Extraindo arquivos...
StatusCreateIcons=Criando �cones...
StatusCreateIniEntries=Criando entradas INI...
StatusCreateRegistryEntries=Criando entradas no registro...
StatusRegisterFiles=Registrando arquivos...
StatusSavingUninstall=Salvando informa��o para desinstala��o...
StatusRunProgram=Terminando a instala��o...
StatusRollback=Rolando de volta as mudan�as...

; *** Misc. errors
ErrorInternal2=Erro interno: %1
ErrorFunctionFailedNoCode=%1 falhou
ErrorFunctionFailed=%1 falhou; c�digo %2
ErrorFunctionFailedWithMessage=%1 falhou; c�digo %2.%n%3
ErrorExecutingProgram=N�o foi poss�vel executar:%n%1

; *** Registry errors
ErrorRegOpenKey=Erro ao abrir a chave de registro:%n%1\%2
ErrorRegCreateKey=Erro ao criar a chave de registro:%n%1\%2
ErrorRegWriteKey=Erro ao escrever na chave de registro:%n%1\%2

; *** INI errors
ErrorIniEntry=Erro ao criar entrada INI no arquivo %1.

; *** File copying errors
FileAbortRetryIgnore=Clique Repetir para tentar novamente, Ignorar para ignorar este arquivo (n�o recomendado), ou Anular para cancelar a instala��o.
FileAbortRetryIgnore2=Clique Repetir para tentar novamente, Ignorar para continuar assim mesmo (n�o recomendado), ou Anular para cancelar a instala��o.
SourceIsCorrupted=O arquivo de origem est� corrompido
SourceDoesntExist=O arquivo de origem "%1" n�o existe
ExistingFileReadOnly=O arquivo existente no seu computador est� marcado como somente para leitura.%n%nClique em Repetir para remover o atributo de somente leitura e tentar novamente, Ignorar para continuar, ou Anular para cancelar a instala��o.
ErrorReadingExistingDest=Um erro ocorreu ao tentar ler o arquivo existente no seu computador.
FileExists=O arquivo j� existe.%n%nVoc� gostaria de sobrescrev�-lo?
ExistingFileNewer=O arquivo existente no seu computador � mais novo que aquele que o Programa de Instala��o est� tentando instalar. � recomendado que voc� mantenha o arquivo existente.%n%nVoc� quer manter o arquivo existente?
ErrorChangingAttr=Um erro ocorreu ao tentar mudar os atributos do arquivo existente no seu computador.
ErrorCreatingTemp=Um erro ocorreu ao tentar criar um arquivo na pasta destino.
ErrorReadingSource=Um erro ocorreu ao tentar ler o arquivo fonte:
ErrorCopying=Um erro ocorreu ao tentar compiar um arquivo.
ErrorReplacingExistingFile=Um erro ocorreu ao tentar substituir um arquivo existente:
ErrorRestartReplace=RestartReplace falhou:
ErrorRenamingTemp=Um erro ocorreu ao tentar renomear um arquivo na pasta destino:
ErrorRegisterServer=N�o foi poss�vel registrar DLL/OCX: %1
ErrorRegisterServerMissingExport=DllRegisterServer n�o encontrado
ErrorRegisterTypeLib=N�o foi poss�vel registrar a biblioteca de tipos: %1

; *** Post-installation errors
ErrorOpeningReadme=Um erro ocorreu ao tentar abrir o arquivo LEIAME.
ErrorRestartingComputer=O Programa de Instala��o n�o conseguiu reiniciar o computador. Por favor fa�a isso manualmente.

; *** Uninstaller messages
UninstallNotFound=O arquivo "%1" n�o existe. N�o � poss�vel desinstalar.
UninstallOpenError=O arquivo "%1" n�o pode ser aberto. N�o � poss�vel desinstalar
UninstallUnsupportedVer=O arquivo de log de desinsta��o "%1" est� em um formato que n�o � reconhecido por esta vers�o do desinstalador. N�o � poss�vel desinstalar
UninstallUnknownEntry=Uma entrada desconhecida (%1) foi encontrada no log de desinstala��o
ConfirmUninstall=Voc� tem certeza que quer remover completamente %1 e todos os seus componentes?
OnlyAdminCanUninstall=Est� instala��o s� pode ser desinstalada por um usu�rio com privil�gios administrativos.
UninstallStatusLabel=Por favor, aguarde enquanto o %1 � removido do seu computador.
UninstalledAll=O %1 foi removido com sucesso do seu computador.
UninstalledMost=A desinstala��o do %1 terminou.%n%nAlguns elementos n�o podem ser removidos. Estes elementos podem ser removidos manualmente.
UninstalledAndNeedsRestart=Para completar a desintala��o do %1, voc� precisa reiniciar seu computador. %n%nVoc� gostaria de reiniciar seu computador agora?
UninstallDataCorrupted=O arquivo "%1" est� corrompido. N�o pode desinstalar

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Remover arquivo compartilhado?
ConfirmDeleteSharedFile2=O sistema indicou que o seguinte arquivo compartilhado n�o est� mais sendo usando por nenhum outro programa. Voc� gostaria de remover este arquivo compartilhado?%n%n%Se qualquer programa ainda estiver usando este arquivo e ele for removido, este programa pode n�o funcionar corretamente. Se voc� n�o tiver certeza, escolha N�o. Manter o arquivo no computador n�o causar� nenhum problema.
SharedFileNameLabel=Nome do Arquivo:
SharedFileLocationLabel=Localiza��o:
WizardUninstalling=Progresso da Desinstala��o
StatusUninstalling=Desinstalando %1...
