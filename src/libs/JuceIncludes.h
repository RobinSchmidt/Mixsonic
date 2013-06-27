#include "juce/AppConfig.h" // move this file to somewhere else and add the path to the search-path
                            // (maybe just a folder up)

#include "juce/modules/juce_core/juce_core.h"
#include "juce/modules/juce_gui_basics/juce_gui_basics.h"
#include "juce/modules/juce_data_structures/juce_data_structures.h"
#include "juce/modules/juce_events/juce_events.h"
#include "juce/modules/juce_graphics/juce_graphics.h"
//#include "juce/modules/juce_video/juce_video.h"
//#include "juce/modules/juce_opengl/juce_opengl.h"
#include "juce/modules/juce_audio_basics/juce_audio_basics.h"
#include "juce/modules/juce_audio_formats/juce_audio_formats.h"
#include "juce/modules/juce_audio_processors/juce_audio_processors.h"
#include "juce/modules/juce_audio_devices/juce_audio_devices.h"
//#include "juce/modules/juce_cryptography/juce_cryptography.h"
//#include "juce/modules/juce_gui_extra/juce_gui_extra.h"
#include "juce/modules/juce_audio_utils/juce_audio_utils.h"
using namespace juce;