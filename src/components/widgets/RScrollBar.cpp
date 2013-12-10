#include "RScrollBar.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

RScrollBar::RScrollBar(bool vertical, bool buttonsVisible, Skin *skinToUse) 
: ScrollBar(vertical, buttonsVisible)
, RWidget(skinToUse)
{

}

//-------------------------------------------------------------------------------------------------
// others:

void RScrollBar::paint(Graphics& g)
{
  ScrollBar::paint(g);
  //g.fillAll(Colours::red); // preliminary
}