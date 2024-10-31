#include "MScrollBar.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MScrollBar::MScrollBar(bool vertical, bool buttonsVisible, Skin *skinToUse) 
: ScrollBar(vertical, buttonsVisible)
, MWidget(skinToUse)
{

}

//-------------------------------------------------------------------------------------------------
// others:

void MScrollBar::paint(Graphics& g)
{
  ScrollBar::paint(g);
  //g.fillAll(Colours::red); // preliminary
}