#ifndef setpoint_knob_h
#define setpoint_knob_h

#include "WProgram.h"
#include <SafetyPin.h>

class SetpointKnob {
  private:
    AnalogIn& m_pin;
    int m_minVal;
    int m_maxVal;
  
  public: 
    // Construct a bicolor LED with the given red and green digital output pins.
    SetpointKnob(AnalogIn& pin, int m_minVal, int m_maxVal);
    
    // read the current value
    int read();
};

#endif
