#ifndef bicolor_h
#define bicolor_h

#include "WProgram.h"
#include <SafetyPin.h>

class Bicolor {
  private:
    DigitalOut& m_redPin;
    DigitalOut& m_greenPin;
  
  public: 
    // Construct a bicolor LED with the given red and green digital output pins.
    Bicolor(DigitalOut& greenPin, DigitalOut& redPin);
    
    // Turn the LED off
    void off();
    
    // Glow red
    void red();
    
    // Glow green
    void green();
};

#endif
