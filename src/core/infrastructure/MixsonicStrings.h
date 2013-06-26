#ifndef MixsonicStrings_h
#define MixsonicStrings_h

#include "../../libs/JuceIncludes.h"

//#define FRENCH // define FRENCH here for french version

#ifndef FRENCH 

//-------------------------------------------------------------------------------------------------
// strings for the message boxes:

static const String okStr               = String("OK");
static const String cancelStr           = String("Cancel");
static const String exitStr             = String("Exit");
static const String theFileStr          = String("The file: ");
static const String theAudioFileStr     = String("The audiofile: ");
static const String theDirectoryStr     = String("The directory: ");
static const String thePathStr          = String("The path: ");
static const String theProjectStr       = String("The project: ");
static const String overwriteFileStr    = String("Overwrite file?");
static const String overwriteRequestStr = String(" will be overwritten - proceed?");
static const String creationFailedStr   = String(" could not be created.");
static const String theArrangementStr   = String("The arrangement");
//static const String tempProjectNameStr  = String("TempProject");

static const String globalSettingsHeadlineStr  = String("Global Settings");
static const String projectsParentDirStr       = String("Projects:");
static const String projectsParentDirHelpStr   = String("Directory where the projects will be saved");
static const String sampleContentDirStr        = String("Samples:");
static const String sampleContentDirHelpStr    = String("Directory where the sample content resides");
static const String selectProjectsParentDirStr = String("Select directory for projects");
static const String selectSampleContentDirStr  = String("Select directory with the sample content");

static const String memAllocErrorStr1 = String("Memory Allocation Error");
static const String memAllocErrorStr2 = String("Memory could not be allocated in: ");

static const String invalidAudioFileStr1 = String("Audiofile invalid");
static const String invalidAudioFileStr2 = String(" is not a supported audiofile.");

static const String audioFileTooLargeStr1 = String("Audiofile too large");
static const String audioFileTooLargeStr2 = String("Audiofiles with more than ");
static const String audioFileTooLargeStr3 = String(" samples are not supported.");

static const String audioFileWriteErrorStr1 = String("Audiofile write error");
static const String audioFileWriteErrorStr3 = String(" could not be written.");

static const String audioFormatWriterCreateErrorStr1 = String("Failed to create AudioFormatWriter");
static const String audioFormatWriterCreateErrorStr2 = String("It was not possible to create an AudioFormatWriter object in: ");

static const String dirCreateErrorStr1 = String("Directory creation error");

static const String dirDeleteWarningStr1 = String("Delete directory?");
static const String dirDeleteWarningStr2 = String(" will be deleted from the project directory and all clips using a sample from this directory will be removed - proceed?");

static const String enterProjectNameStr1 = String("Please enter a name");
static const String enterProjectNameStr2 = String("Please enter a name for the new project in the textfield.");

static const String fileCopyErrorStr1 = String("File could not be copied");
static const String fileCopyErrorStr2 = String(" could not be copied into the directory: ");

static const String fileDeleteErrorStr1 = String("File could not be deleted");
static const String fileDeleteErrorStr2 = String(" could not be deleted.");

static const String fileMoveErrorStr1 = String("File could not be moved");
static const String fileMoveErrorStr2 = String(" could not be moved into the directory: ");

static const String fileRenameErrorStr1 = String("File could not be moved or renamed");
static const String fileRenameErrorStr2 = String(" could not be moved or renamed to:");

static const String fileSaveErrorStr1 = String("File could not be saved");
static const String fileSaveErrorStr2 = String(" could not be saved.");

static const String fileIsNoDirStr1 = String("Not a directory");
static const String fileIsNoDirStr2 = String("The chosen file: ");
static const String fileIsNoDirStr3 = String(" is not a directory.");

static const String fileOutputStreamCreateErrorStr1 = String("Failed to create file output stream");
static const String fileOutputStreamCreateErrorStr2 = String("For the file: ");
static const String fileOutputStreamCreateErrorStr3 = String(" could no FileOutputStream object be created.");

static const String audioFileInvalidStr1 = String("File not found or invalid");
static const String audioFileInvalidStr2 = String(" was not found or is not a valid audiofile - clips using this file will be omitted.");

static const String invalidPathNameStr1 = String("Invalid path name");
static const String invalidPathNameStr2 = String("The chosen project directory name: ");
static const String invalidPathNameStr3 = String(" is not a valid name for a directory. Please enter another name.");

static const String selectProjectFileStr1 = String("Please select a project file");
static const String selectProjectFileStr2 = String("Please select a valid Mixsonic project file to load.");

static const String projectCreationErrorStr1 = String("Project could not be created");

static const String projectAlreadyExistsStr1 = String("Project already exists");
static const String projectAlreadyExistsStr2 = String(" already exists. Please enter another name.");

static const String invalidProjectParentPathStr1 = String("Invalid path for projects");
static const String invalidProjectParentPathStr2 = String("The parent directory for the projects: ");
static const String invalidProjectParentPathStr3 = String(" does not exist or is invalid. Please choose a valid directory via the settings dialog.");

static const String audiofileDeleteWarningStr1 = String("Delete audiofile?");
static const String audiofileDeleteWarningStr2 = String(" will be deleted from the project directory and all clips using that sample will be removed - proceed?");

static const String invalidSampleContentPathStr1 = String("Invalid sample content path");
static const String invalidSampleContentPathStr2 = String(" for the sample content does not exist or is invalid. Please choose a valid directory via the settings dialog.");

static const String settingsFileMissingStr1 = String("Settings file missing");
static const String settingsFileMissingStr2 = String("The global application settings file: MixsonicSettings.xml is missing - Mixsonic will use fallback settings, which are likely to be meaningless. Please re-install to resolve this issue.");

static const String settingsFileInvalidStr1 = String("Settings file corrupted");
static const String settingsFileInvalidStr2 = String("The global application settings file: MixsonicSettings.xml is corrupted - Mixsonic will use fallback settings, which are likely to be meaningless. Please re-install to resolve this issue.");

static const String songUnsavedStr1 = String("Song not saved!");
static const String songUnsavedStr2 = String("The current song is not saved and will be discarded - proceed?");

static const String fileNotMovedBecauseAlreadyExistsStr1 = String("File could not be moved");
static const String fileNotMovedBecauseAlreadyExistsStr2 = String(" could not be moved into the directory");
static const String fileNotMovedBecauseAlreadyExistsStr3 = String(" because there already exists a file with the same name.");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicTrackControlComponent:

static const String levelMinusStr  = String("Decreases volume level by one dB");
static const String levelPlusStr   = String("Increases volume level by one dB");
static const String muteStr        = String("Mutes the track");
static const String panCenterStr   = String("Sets panorama position to center");
static const String panLeftStr     = String("Moves panorama position one step left");
static const String panRightStr    = String("Moves panorama position one step right");
static const String soloStr        = String("Switches track into solo mode");
static const String levelSliderStr = String("Adjusts the track's volume level");
static const String panSliderStr   = String("Adjusts the track's panorama position");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicArrangementEditor:

static const String bounceButtonHelpStr            = String("Bounces the arrangement into an audiofile");
static const String copyClipsButtonHelpStr         = String("Copies selected clips into the clipboard");
static const String cutClipsButtonHelpStr          = String("Cuts selected clips out and stores them in the clipboard");
static const String splitClipButtonHelpStr         = String("Splits the clip at the mouse cursor position");
static const String duplicateClipsButtonHelpStr    = String("Duplicates selected clips");
static const String deleteClipsButtonHelpStr       = String("Deletes selected clips from the arrangement");
static const String moveClipsButtonHelpStr         = String("Move selected clips around");
static const String pasteClipsButtonHelpStr        = String("Pastes clips from clipboard into the arrangement");
static const String arrangementPauseButtonHelpStr  = String("Stops playback of the arrangement");
static const String arrangementPlayButtonHelpStr   = String("Starts playback of the arrangement");
static const String arrangementRewindButtonHelpStr = String("Rewinds cursor to the beginning of the arrangement");
static const String zoomInButtonHelpStr            = String("Zooms in");
static const String zoomOutButtonHelpStr           = String("Zooms out");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicProjectComponent and MixsonicSampleBrowser:

static const String browserPlayButtonStr   = String("Starts playback of the selected audiofile");
static const String browserPauseButtonStr  = String("Stops playback of the audio file");
static const String browserRewindButtonStr = String("Rewinds playback position in the audio file");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicProjectComponent (only):

static const String createDirButtonHelpStr     = String("Creates a new directory in the project directory");
static const String deleteSampleButtonHelpStr  = String("Deletes a sample from the project directory");
static const String createProjectButtonHelpStr = String("Creates a new project");
static const String openProjectButtonHelpStr   = String("Opens an existing project");
static const String recordButtonHelpStr1       = String("Records an audio file into the project directory");
static const String saveButtonHelpStr          = String("Saves the current project");

//-------------------------------------------------------------------------------------------------
// strings for the AudioExport and AudioRecord dialogs:

static const String renderHeadlineStr            = String("Render arrangement to file");
static const String recordHeadlineStr            = String("Record from audio input to file");
static const String doneStr                      = String("Done");
static const String doneHelpStr                  = String("Close dialog and return to the main application");
static const String selectTargetFileStr          = String("Select target file");
static const String selectTargetDirectoryStr     = String("Select target directory");
static const String targetFileNameStr            = String("Target file:");
static const String targetFileNameHelpStr        = String("Enter name of the file into which the audio data will be written");
static const String targetDirectoryStr           = String("Target directory:");
static const String targetDirectoryHelpStr       = String("The directory where the file will be saved");
static const String formatStr1                   = String("Format: ");
static const String formatStr2                   = String("Format for the target file");
static const String numChannelsStr1              = String("Channels: ");
static const String numChannelsStr2              = String("Number of channels for the target file");
static const String monoStr                      = String("mono");
static const String stereoStr                    = String("stereo");
static const String sampleRateStr1               = String("Sample rate: ");
static const String sampleRateStr2               = String("Samplerate for the target file");
static const String bitDepthStr1                 = String("Bit depth: ");
static const String bitDepthStr2                 = String("Bit depth for the target file");
static const String browseButtonStr              = String("Browse");
static const String directoryBrowseButtonHelpStr = String("Browse for the directory where the file will be saved");
static const String renderButtonStr              = String("Render");
static const String renderButtonHelpStr          = String("Start the rendering process");
static const String recordButtonStr              = String("Record");
static const String recordButtonHelpStr          = String("Start and stops recording.");
//static const String defaultRecordFileNameStr     = String("RecordedAudio");
static const String renderingFinishedStr         = String("Rendering finished");
static const String wasRenderedStr               = String(" was rendered and saved in the audiofile: ");
static const String renderingAbortedStr          = String("Rendering aborted.");
static const String renderingAbortedStr2         = String("Rendering was aborted by the user.");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicCreateDirectoryDialog:

static const String createDirectoryHeadlineStr = String("Create Directory");
static const String enterDirectoryNameStr      = String("Enter directory name:");
static const String enterDirectoryHelpStr      = String("Enter name of the directory to be created");
static const String createStr                  = String("Create");
static const String createDirectoryHelpStr     = String("Creates the new directory");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicEnterProjectNameDialog:

static const String enterProjectNameHeadlineStr = String("Enter project name");
static const String enterProjectNameStr         = String("Enter project name:");
static const String enterProjectNameHelpStr     = String("Enter name of the project to be created");
static const String createProjectHelpStr        = String("Creates the new project");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicEnterNameDialog:

static const String enterNameHeadlineStr = String("Enter name");
static const String enterNameStr         = String("Enter name:");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicMoveFileDialog:

static const String moveFileHeadlineStr = String("Move file or directory");
static const String moveButtonStr       = String("Move");
static const String renameStr           = String("Rename");
static const String deleteStr           = String("Delete");
static const String moveButtonHelpStr   = String("Moves the file to the chosen directory");

//-------------------------------------------------------------------------------------------------
// misc strings:

static const String infoLineStr           = String("Info:");
static const String levelStr              = String("Level");
static const String panStr                = String("Pan");
static const String clipMuteStr           = String("Mute");
static const String clipMuteHelpStr       = String("Mutes the clip");
static const String navigatorHeadlineStr  = String("Navigator");   // Navigateur
static const String projectHeadlineStr    = String("Project");     // Projet
static const String arrangerHeadlineStr   = String("Arrangement"); // Multipiste


// these are parts of the string that appears in the info line when the user adjust the clip level
// or fade-in/out:
static const String clipSettingsStr1 = String("Level: ");
static const String clipSettingsStr2 = String(" dB, Fade-In: ");
static const String clipSettingsStr3 = String(" sec, Fade-Out: ");
static const String clipSettingsStr4 = String(" sec");

//=================================================================================================
// from here: strings for the french vesrion:
#else

//-------------------------------------------------------------------------------------------------
// strings for the message boxes:

static const String okStr = String("OK");
static const String cancelStr = String("Annuler");
static const String exitStr = String("Quitter");
static const String theFileStr = String("Fichier: ");
static const String theAudioFileStr = String("Fichier Audio: ");
static const String theDirectoryStr = String("R\351pertoire: ");
static const String thePathStr = String("Chemin: ");
static const String theProjectStr = String("Projet: ");
static const String overwriteFileStr = String("Ecraser le fichier ?");
static const String overwriteRequestStr = String(" sera effac\351 - continuer ?");
static const String creationFailedStr = String(" ne peut \352tre cr\351e.");

static const String globalSettingsHeadlineStr  = String("Global Settings");
static const String projectsParentDirStr       = String("Projects:");
static const String projectsParentDirHelpStr   = String("Directory where the projects will be saved");
static const String sampleContentDirStr        = String("Samples:");
static const String sampleContentDirHelpStr    = String("Directory where the sample content resides");
static const String selectProjectsParentDirStr = String("Select directory for projects");
static const String selectSampleContentDirStr  = String("Select directory with the sample content");

static const String theArrangementStr = String("Le mixage");
static const String tempProjectNameStr = String("ProjetTemporaire"); // this is a filename - don't use special characters

static const String memAllocErrorStr1 = String("Erreur d'allocation m\351moire");
static const String memAllocErrorStr2 = String("La m\351moire ne peut \352tre allou\351 dans: ");

static const String invalidAudioFileStr1 = String("Fichier son invalide");
static const String invalidAudioFileStr2 = String(" n'est pas un type de fichier audio support\351.");

static const String audioFileTooLargeStr1 = String("Fichier audio trop long");
static const String audioFileTooLargeStr2 = String("Les fichiers audio avec plus de ");
static const String audioFileTooLargeStr3 = String(" \351chantillons ne sont pas support\351s.");

static const String audioFileWriteErrorStr1 = String("Erreur d'\351criture du fichier audio");
static const String audioFileWriteErrorStr3 = String(" ne peut \352tre \351crit.");

static const String audioFormatWriterCreateErrorStr1 = String("Impossible de cr\351er AudioFormatWriter");
static const String audioFormatWriterCreateErrorStr2 = String("Il n'a pas \351t\351 possible de cr\351er un AudioFormatWriter dans: ");

static const String dirCreateErrorStr1 = String("Erreur de cr\351ation de r\351pertoire");

static const String dirDeleteWarningStr1 = String("Supprimer le r\351pertoire ?");
static const String dirDeleteWarningStr2 = String(" sera supprim\351 du r\351pertoire de projet ainsi que tous les \351chantillons audio utilis\351s - continuer ?");

static const String enterProjectNameStr1 = String("Entrez un nom");
static const String enterProjectNameStr2 = String("Veuillez entrer un nom de projet dans la bo\356te d'entr\351e de texte.");

static const String fileCopyErrorStr1 = String("Le fichier ne peut \352tre copi\351");
static const String fileCopyErrorStr2 = String(" ne peut \352tre copi\351 dans le r\351pertoire: ");

static const String fileDeleteErrorStr1 = String("Le fichier ne peut \352tre supprim\351");
static const String fileDeleteErrorStr2 = String(" ne peut \352tre supprim\351.");

static const String fileMoveErrorStr1 = String("Le fichier ne peut \352tre d\351plac\351");
static const String fileMoveErrorStr2 = String(" ne peut \352tre d\351plac\351 dans le r\351pertoire: ");

static const String fileRenameErrorStr1 = String("Le fichier ne peut \352tre d\351plac\351 ou renomm\351");
static const String fileRenameErrorStr2 = String(" ne peut \352tre d\351plac\351 ou renomm\351 en:");

static const String fileSaveErrorStr1 = String("Le fichier ne peut \352tre sauv\351");
static const String fileSaveErrorStr2 = String(" ne peut \352tre sauv\351.");

static const String fileIsNoDirStr1 = String("Pas un r\351pertoire");
static const String fileIsNoDirStr2 = String("Fichier choisi: ");
static const String fileIsNoDirStr3 = String(" n'est pas un r\351pertoire.");

static const String fileOutputStreamCreateErrorStr1 = String("Impossible de cr\351er un flux de sortie");
static const String fileOutputStreamCreateErrorStr2 = String("Pour le fichier: ");
static const String fileOutputStreamCreateErrorStr3 = String(" could no FileOutputStream object be created.");

static const String audioFileInvalidStr1 = String("Fichier non trouv\351 ou invalide");
static const String audioFileInvalidStr2 = String(" n'a pas \351t\351 trouv\351 ou n'est pas un fichier audio valide - les s\351quences utilisant ce fichier seront ignor\351es.");

static const String invalidPathNameStr1 = String("Chemin invalide");
static const String invalidPathNameStr2 = String("Nom de r\351pertoire de projet choisi: ");
static const String invalidPathNameStr3 = String(" n'est pas un nom valide pour un r\351pertoire. Entrez un autre nom.");

static const String selectProjectFileStr1 = String("S\351lectionner un fichier de projet");
static const String selectProjectFileStr2 = String("Veuillez s\351lectionner un fichier de projet \340 charger.");

static const String projectCreationErrorStr1 = String("Le projet ne peut \352tre cr\351e");

static const String projectAlreadyExistsStr1 = String("Projet existant");
static const String projectAlreadyExistsStr2 = String(" existe d\351ja. Veuillez entrer un autre nom.");

static const String invalidProjectParentPathStr1 = String("Chemin de projet invalide");
static const String invalidProjectParentPathStr2 = String("Le dossier contenant les projets: ");
//static const String invalidProjectParentPathStr3 = String(" n'existe pas ou est invalide.");
static const String invalidProjectParentPathStr3 = String(" does not exist or is invalid. Please choose a valid directory via the settings dialog.");

static const String audiofileDeleteWarningStr1 = String("Supprimer le fichier audio ?");
static const String audiofileDeleteWarningStr2 = String(" sera supprim\351 du dossier de projet et toutes les s\351quences utilisant ce fichier seront supprim\351es - continuer ?");

static const String invalidSampleContentPathStr1 = String("Chemin de base sonore invalide");
//static const String invalidSampleContentPathStr2 = String(" n'existe pas ou n'est pas valide.");
static const String invalidSampleContentPathStr2 = String(" for the sample content does not exist or is invalid. Please choose a valid directory via the settings dialog.");

static const String settingsFileMissingStr1 = String("Fichier de configuration manquant");
static const String settingsFileMissingStr2 = String("Le fichier de pr\351f\351rence globale: MixsonicSettings.xml est manquant - Mixsonic utilisera les pr\351ferences par d\351faut, qui malheureusement ont peu de chance d'\352tre correcte. Veuillez r\351installer pour r\351soudre ce probl\350me.");

static const String settingsFileInvalidStr1 = String("Fichier de configuration corrompu");
static const String settingsFileInvalidStr2 = String("Le fichier de pr\351f\351rence globale: MixsonicSettings.xml est corrompu - Mixsonic utilisera les pr\351f\351rences par d\351faut, qui malheureusement ont peu de chance d'\352tre correctes. Veuillez r\351installer pour r\351soudre ce probl\350me.");

static const String songUnsavedStr1 = String("Mixage non sauvegard\351 !");
static const String songUnsavedStr2 = String("Le mixage courant n'a pas \351t\351 sauv\351 et sera donc effac\351 - continuer ?");

static const String fileNotMovedBecauseAlreadyExistsStr1 = String("Fichier ne peut \352tre d\351plac\351");
static const String fileNotMovedBecauseAlreadyExistsStr2 = String(" ne peut \352tre d\351plac\351 dans le r\351pertoire");
static const String fileNotMovedBecauseAlreadyExistsStr3 = String(" car il existe d\351ja un fichier ayant le m\350me nom.");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicTrackControlComponent:

static const String levelMinusStr = String("diminue le volume de 1 d\351cibel");
static const String levelPlusStr = String("Augmente le volume de 1 d\351cibel");
static const String muteStr = String("Rend la piste muette");
static const String panCenterStr = String("R\351initialise le panoramique \340 z\351ro");
static const String panLeftStr = String("Change le panoramique d'un pas vers la gauche");
static const String panRightStr = String("Change le panoramique d'un pas vers la droite");
static const String soloStr = String("Active la piste en mode SOLO");
static const String levelSliderStr = String("Modifie le volume de la piste");
static const String panSliderStr = String("Modifie le panoramique de la piste");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicArrangementEditor:

static const String bounceButtonHelpStr = String("Sauvegarde le mixage dans un fichier audio");
static const String copyClipsButtonHelpStr  = String("Copie les s\351quences s\351l\351ctionn\351es dans le presse papier");
static const String cutClipsButtonHelpStr = String("Coupe les s\351quences s\351l\351ctionn\351es et les placent dans le presse papier");

static const String splitClipButtonHelpStr = String("S\351pare la s\351quence \340 l'emplacement du curseur de la souris");


static const String duplicateClipsButtonHelpStr = String("Duplique les s\351quences s\351l\351ctionn\351es");
static const String deleteClipsButtonHelpStr = String("Supprime les s\351quences s\351l\351ctionn\351es du mixage");
static const String moveClipsButtonHelpStr = String("Bouge les s\351quences s\351l\351ctionn\351es");
static const String pasteClipsButtonHelpStr = String("Colle les s\351quences s\351l\351ctionn\351es du presse papier vers le mixage");
static const String arrangementPauseButtonHelpStr = String("Arr\352te la lecture du mixage");
static const String arrangementPlayButtonHelpStr = String("D\351marre la lecture du mixage");
static const String arrangementRewindButtonHelpStr = String("R\351initialise le curseur de lecture du mixage");
static const String zoomInButtonHelpStr = String("Zoom avant");
static const String zoomOutButtonHelpStr = String("Zoom arri\350re");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicProjectComponent and MixsonicSampleBrowser:

static const String browserPlayButtonStr = String("D\351marre la lecture du fichier son s\351lectionn\351");
static const String browserPauseButtonStr = String("Arr\352te la lecture du fichier son s\351lectionn\351");
static const String browserRewindButtonStr = String("R\351initialise le curseur de lecture du fichier son s\351lectionn\351");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicProjectComponent (only):

static const String createDirButtonHelpStr = String("Cr\351e un nouveau r\351pertoire dans le r\351pertoire de projet");
static const String deleteSampleButtonHelpStr = String("Supprime un \351chantillon du r\351pertoire de projet");
static const String createProjectButtonHelpStr = String("Cr\351e un nouveau projet");
static const String openProjectButtonHelpStr = String("Ouvre un projet existant");
static const String recordButtonHelpStr1 = String("Enregistre un fichier audio dans le dossier de projet");
static const String saveButtonHelpStr = String("Sauvegarde le projet courant");

//-------------------------------------------------------------------------------------------------
// strings for the AudioExport and AudioRecord dialogs:

static const String renderHeadlineStr = String("Rendu du mixage dans un fichier");
static const String recordHeadlineStr = String("Enregistrer l'entr\351 audio dans un fichier");
static const String doneStr = String("Termin\351");
static const String doneHelpStr = String("Fermer la boite de dialogue et retourner \340 l'application principale");
static const String selectTargetFileStr = String("S\351lectionnez un fichier de destination");
static const String selectTargetDirectoryStr = String("S\351lectionnez un r\351pertoire de destination");
static const String targetFileNameStr = String("Fichier destination:");
static const String targetFileNameHelpStr = String("Entrez le nom du fichier dans lesquels les donn\351es audio vont \352tre sauvegard\351es");
static const String targetDirectoryStr = String("R\351pertoire de destination:");
static const String targetDirectoryHelpStr = String("Le r\351pertoire ou le fichier va \352tre sauvegard\351");
static const String formatStr1 = String("Format: ");
static const String formatStr2 = String("Format du fichier destination");
static const String numChannelsStr1 = String("Canaux: ");
static const String numChannelsStr2 = String("Nombre de canaux du fichier destination");
static const String monoStr = String("mono");
static const String stereoStr = String("st\351r\351o");
static const String sampleRateStr1 = String("Fr\351quence d'\351chantillonage: ");
static const String sampleRateStr2 = String("Fr\351quence d'\351chantillonage du fichier de destination");
static const String bitDepthStr1 = String("Nombre de Bit: ");
static const String bitDepthStr2 = String("Nombre de Bit du fichier de destination");
static const String browseButtonStr = String("Choisissez");
static const String directoryBrowseButtonHelpStr = String("Choisissez le r\351pertoire ou le fichier va \352tre sauvegard\351");
static const String renderButtonStr = String("Enregistrez");
static const String renderButtonHelpStr = String("D\351marre le mixage");
static const String recordButtonStr = String("Enregistre");
static const String recordButtonHelpStr = String("D\351marre et arr\350te l'enregistrement.");

static const String defaultRecordFileNameStr = String("EnregistrementAudio"); // this is a filename - don't use special characters
static const String renderingFinishedStr = String("Mixage  termin\351");
static const String wasRenderedStr = String(" a \351t\351 rendu et sauv\351 dans le fichier audio: ");
static const String renderingAbortedStr = String("op\351ration annul\351.e");
static const String renderingAbortedStr2 = String("Líop\351ration \340 \351t\351 annul\351e par l'utilisateur.");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicCreateDirectoryDialog:

static const String createDirectoryHeadlineStr = String("Cr\351er un r\351pertoire");
static const String enterDirectoryNameStr = String("Entrez un nom de r\351pertoire:");
static const String enterDirectoryHelpStr = String("Entrez un nom de r\351pertoire \340 cr\351er");
static const String createStr = String("Cr\351e");
static const String createDirectoryHelpStr = String("Cr\351e le nouveau r\351pertoire");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicEnterProjectNameDialog:

static const String enterProjectNameHeadlineStr = String("Entrez un nom de projet");
static const String enterProjectNameStr = String("Entrez un nom de projet:");
static const String enterProjectNameHelpStr = String("Entrez un nom de projet \340 cr\351er");
static const String createProjectHelpStr = String("Cr\351e le nouveau projet");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicEnterNameDialog:

static const String enterNameHeadlineStr = String("Entrez un nom");
static const String enterNameStr = String("Entrez un nom:");

//-------------------------------------------------------------------------------------------------
// strings for the MixsonicMoveFileDialog:

static const String moveFileHeadlineStr = String("D\351placer le fichier ou le dossier");
static const String moveButtonStr = String("D\351placer");

// TO BE TRANSLATED:
static const String renameStr = String("Renommer");
static const String deleteStr = String("Supprimer");

static const String moveButtonHelpStr = String("D\351place le fichier ou le dossier");

//-------------------------------------------------------------------------------------------------
// misc strings:

static const String infoLineStr = String("Information:");
static const String levelStr = String("Niveau");
static const String panStr = String("Panoramique");
static const String clipMuteStr = String("Rendre muet");
static const String clipMuteHelpStr = String("Rend muette la s\351quence");
static const String navigatorHeadlineStr = String("Navigateur");
static const String projectHeadlineStr = String("Projet");
static const String arrangerHeadlineStr = String("Multipistes"); 


// these are parts of the string that appears in the info line when the user adjust the clip level
// or fade-in/out:
static const String clipSettingsStr1 = String("Niveau: ");
static const String clipSettingsStr2 = String(" dB, Fondu d'entr\351: ");
static const String clipSettingsStr3 = String(" secondes, Fondu de sortie: ");
static const String clipSettingsStr4 = String(" secondes");

#endif

#endif
