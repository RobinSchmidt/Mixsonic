#include "Parameter.h"

//=================================================================================================
// class ParameterObserver:

bool ParameterObserver::globalAutomationSwitch = true;
bool ParameterObserver::guiAutomationSwitch    = true;

ParameterObserver::ParameterObserver()
{
  localAutomationSwitch  = false;
  isGuiElement           = false;
}

ParameterObserver::~ParameterObserver()
{

}

bool ParameterObserver::wantsAutomationNotification()
{
  bool result = localAutomationSwitch && globalAutomationSwitch;
  if( this->isGuiElement == true )
    return result && guiAutomationSwitch;
  else
    return result;
}


//=================================================================================================
// class Parameter:

// construction/destruction:

Parameter::Parameter(CriticalSection *criticalSectionToUse, const String& newName, 
  double newMinValue, double newMaxValue, double newInterval, double newDefaultValue, 
  int newScaling) 
{
  jassert( !(newMinValue >= newMaxValue) )                      // that would result in a zero or negative range
  jassert( !(newMinValue <= 0.0 && newScaling == EXPONENTIAL) ) // exponential scaling requires strictly positive minimum value

  mutex         = criticalSectionToUse;
  name          = newName;
  minValue      = newMinValue;
  maxValue      = newMaxValue;
  interval      = newInterval;
  scaling       = newScaling;  // set this before restrictValueToParameterRange is called
  defaultValue  = restrictValueToParameterRange(newDefaultValue);;
  value         = defaultValue;
  saveAndRecall = true;
}

Parameter::~Parameter()
{
  ScopedPointerLock spl(mutex);
  while( parameterObservers.size() > 0 )
  {
    ParameterObserver *o = parameterObservers[0];
    o->parameterIsGoingToBeDeleted(this);
    deRegisterParameterObserver(o);
  }
  // remark: we use a while-loop to account for the possibility that the observer de-registers 
  // itself in the callback to parameterIsGoingToBeDeleted in which case the array-size shrinks 
  // inside the iteration which would make a for-loop ...mmm...a bug
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void Parameter::setValue(double newValue, bool sendNotification)
{
  ScopedPointerLock spl(mutex);
  value = restrictValueToParameterRange(newValue);
  if( sendNotification == true )
    notifyObservers();
}

void Parameter::resetToDefaultValue(bool sendNotification)
{
  ScopedPointerLock spl(mutex);
  setValue(defaultValue, sendNotification);
}

void Parameter::setStringValue(const juce::String &newString, bool sendNotification)
{
  ScopedPointerLock spl(mutex);
  for(int i=0; i<stringValues.size(); i++)
  {
    if( stringValues[i] == newString )
    {
      setValue((double) i, sendNotification);
      return;
    }
  }
  setValue(defaultValue, sendNotification);  // use default option when string wasn't found
}

void Parameter::setRange(double newMinValue, double newMaxValue)
{
  ScopedPointerLock spl(mutex);
  minValue = newMinValue;
  maxValue = newMaxValue;
  valueSanityCheck();
  for(int i=0; i < (int) parameterObservers.size(); i++)
  {
    if( parameterObservers[i]->wantsAutomationNotification() )
      parameterObservers[i]->parameterRangeChanged(this);
  }
}

void Parameter::setMinValue(double newMinValue)
{
  if( newMinValue <= maxValue )
    setRange(newMinValue, maxValue);
  else
    setRange(maxValue, maxValue);
}

void Parameter::setMaxValue(double newMaxValue)
{
  if( newMaxValue >= minValue )
    setRange(minValue, newMaxValue);
  else
    setRange(minValue, minValue);
}

void Parameter::setDefaultValue(double newDefaultValue, bool setToDefault)
{
  ScopedPointerLock spl(mutex);
  defaultValue = restrictValueToParameterRange(newDefaultValue);
  if( setToDefault == true )
    setValue(defaultValue, true);
}

void Parameter::setScaling(int newScaling)
{
  ScopedPointerLock spl(mutex);
  if( newScaling < BOOLEAN || newScaling > EXPONENTIAL )
  {
    jassertfalse;
    return; // invalid scaling index
  }
  scaling = newScaling;
}

void Parameter::setScalingFromString(String newScalingString)
{
  ScopedPointerLock spl(mutex);
  if( newScalingString == String("Boolean") )
    setScaling(BOOLEAN);
  else if( newScalingString == String("Integer") )
    setScaling(INTEGER);
  else if( newScalingString == String("Linear") )
    setScaling(LINEAR);
  else if( newScalingString == String("Exponential") )
    setScaling(EXPONENTIAL);
  else if( newScalingString == String("LinearBipolar") )
    setScaling(LINEAR_BIPOLAR);
  else
    setScaling(LINEAR);
}

void Parameter::addStringValue(const String& valueToAdd)
{
  ScopedPointerLock spl(mutex);
  stringValues.addIfNotAlreadyThere(valueToAdd);
  minValue = 0.0;
  maxValue = (double) (stringValues.size()-1);
}

void Parameter::registerParameterObserver(ParameterObserver *observerToAdd)
{
  ScopedPointerLock spl(mutex);
  parameterObservers.addIfNotAlreadyThere(observerToAdd);
}

void Parameter::deRegisterParameterObserver(ParameterObserver *observerToRemove)
{
  ScopedPointerLock spl(mutex);
  parameterObservers.removeValue(observerToRemove);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

String Parameter::getStringValue() const
{
  ScopedPointerLock spl(mutex);
  if( this->isStringParameter() )
  {
    int index = (int) getValue();
    if( index >=0 && index < stringValues.size() )
      return stringValues[index];
    else
    {
      jassertfalse; // value does represent a valid index in the array of string values
      return String::empty;
    }
  }
  else
    return String::empty;
}

String Parameter::getOptionStringAtIndex(int index) const
{
  ScopedPointerLock spl(mutex);
  if( !isStringParameter() )
  {
    jassertfalse; // tyring to retrieve an option-string from a non-string parameter
    return String::empty;
  }

  if( index >= 0 && index < stringValues.size() )
    return stringValues[index];
  else
  {
    jassertfalse; // tyring to retrieve an option-string with invalid index
    return String::empty;
  }
}

String Parameter::getDefaultStringValue() const
{
  ScopedPointerLock spl(mutex);
  if( !isStringParameter() )
    return String::empty;
  else
  {
    int index = (int) getDefaultValue();
    if( index >= 0 && index < stringValues.size() )
      return stringValues[index];
    else
      return String::empty;
  }
}

String Parameter::getScalingString() const
{
  ScopedPointerLock spl(mutex);
  switch( scaling )
  {
  case BOOLEAN:        return String("Boolean");
  case INTEGER:        return String("Integer");
  case LINEAR:         return String("Linear");
  case EXPONENTIAL:    return String("Exponential");
  case LINEAR_BIPOLAR: return String("LinearBipolar");
  default:             return String("Linear");
  }
}
//-------------------------------------------------------------------------------------------------
// others:

void Parameter::notifyObservers()
{
  ScopedPointerLock spl(mutex);
  for(int i=0; i < (int) parameterObservers.size(); i++)
  {
    ParameterObserver *observer = parameterObservers[i];
    if( parameterObservers[i]->wantsAutomationNotification() )
      parameterObservers[i]->parameterChanged(this);
  }
}

double Parameter::restrictValueToParameterRange(double valueToRestrict)
{
  ScopedPointerLock spl(mutex);
  if( scaling == BOOLEAN )
    valueToRestrict = (double) (valueToRestrict >= 0.5);

  if( valueToRestrict > maxValue )
    return maxValue;
  if( valueToRestrict < minValue )
    return minValue;
  return valueToRestrict;
}

void Parameter::valueSanityCheck()
{
  ScopedPointerLock spl(mutex);
  jassert( !( minValue <= 0.0 && scaling == EXPONENTIAL ) );
  if( ( minValue <= 0.0 && scaling == EXPONENTIAL ) )
    minValue = 0.1;
  jassert( maxValue > minValue );
  if( maxValue <= minValue )
    maxValue = minValue+1.0;
  value                = restrictValueToParameterRange(value);
  defaultValue         = restrictValueToParameterRange(defaultValue);
}



//=================================================================================================
// class ParameterSetHolder:

void ParameterSetHolder::registerParameterSetObserver(ParameterSetObserver *observerToRegister)
{
  parameterSetObservers.addIfNotAlreadyThere(observerToRegister);
}

void ParameterSetHolder::deRegisterParameterSetObserver(ParameterSetObserver *observerToDeRegister)
{
  parameterSetObservers.removeValue(observerToDeRegister);
}

void ParameterSetHolder::sendParameterSetChangeNotification(
  ParameterSetHolder* parameterSetHolderThatHasChanged)
{
  for(int i=0; i<parameterSetObservers.size(); i++)
    parameterSetObservers[i]->parameterSetChanged(parameterSetHolderThatHasChanged);
}
