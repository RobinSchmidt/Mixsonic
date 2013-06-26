#ifndef FileFilters_h
#define FileFilters_h

#include "../../libs/JuceIncludes.h"

/**

This file defines a couple of subclasses of the juce::FileFilter class.

\todo check, if AudioFileFilter, DirectoriesOnlyFilter are still used - if not, delete them, get rid
      of MixsonicProjectFileFilter, move FileFilterForBrowser into the header file for the browser

*/

class AudioFileFilter : public WildcardFileFilter
{  

public:

  /** Constructor. */
  AudioFileFilter() 
    : WildcardFileFilter(String("*.wav, *.aif, *.aiff, *.flac"), 
                         String::empty,
                         String("Audio Files")) {};
};

class MixsonicProjectFileFilter : public WildcardFileFilter
{  

public:

  /** Constructor. */
  MixsonicProjectFileFilter() 
    : WildcardFileFilter(String("*.xml"), 
                         String::empty,
                         String("Mixsonic Project Files")) {};
};

class DirectoriesOnlyFilter : public FileFilter
{  

public:

  /** Constructor. */
  DirectoriesOnlyFilter() : FileFilter(String("Directories")) {};

  /** Returns true if the passed file is a directory, false otherwise. */
  virtual bool isFileSuitable(const File &file) const 
  {
    if( file.isDirectory() )
      return true;
    else
      return false;
  }

  /** Returns true if the passed file is a directory, false otherwise. */
  virtual bool isDirectorySuitable(const File &file) const 
  {
    if( file.isDirectory() )
      return true;
    else
      return false;
  }

};

class FileFilterForBrowser : public FileFilter
{  

public:

  /** Constructor. */
  FileFilterForBrowser() : FileFilter(String("FileFilterForBrowser")) {};

  /** Returns true if the passed file is a directory or has any of the supported file-formats (as
  indicated by the extension), false otherwise. */
  virtual bool isFileSuitable(const File &file) const 
  {
    return file.isDirectory() || isAudioFile(file);
  }

  /** Returns true if the passed file is a directory, false otherwise. */
  virtual bool isDirectorySuitable(const File &file) const 
  {
    return file.isDirectory();
  }

  /** Returns true if the passed file has any of the supported audio file-formats (as indicated by 
  the extension), false otherwise. */  
  virtual bool isAudioFile(const File &file) const 
  {
    return file.hasFileExtension("wav;aif;aiff;flac;");
  }


};

#endif  