#include "MixsonicPluginScannerBox.h"

MixsonicPluginScannerBox::MixsonicPluginScannerBox(SectionSkin *skinToUse)
: MixsonicModalDialog(skinToUse)
{
  currentFileLabel = new MLabel(&skinToUse->labelSkin);
  addAndMakeVisible(currentFileLabel);

  setHeadline("Scanning Plugins");

  setSize(640, 74);
}

MixsonicPluginScannerBox::~MixsonicPluginScannerBox()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicPluginScannerBox::resized()
{
  MixsonicModalDialog::resized();

  int x = 0;
  int y = headlineLabel->getBottom();
  int w = getWidth();

  currentFileLabel->setBounds(x+4,  y+4, 176, 20);
}

//-------------------------------------------------------------------------------------------------
// misc:

void MixsonicPluginScannerBox::scanPluginsAndShowProgress(PluginDirectoryScanner& scanner)
{
  String nextFile;
  do
  {
    nextFile = scanner.getNextPluginFileThatWillBeScanned();
    // \todo show the name of the file currently being scanned, show the progress bar


    headlineLabel->setText(nextFile, false);


    // dummy loop to slow down the scanning for watching:
    int max = 100000000;
    for(int i = 0; i < max; i++)
    {
      int j = 2*i;
    }



    int dummy = 0;
  }
  while( scanner.scanNextFile(true) );

  int dummy = 0;
}
