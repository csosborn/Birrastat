#include "WProgram.h"
#include "Bicolor.h"

Bicolor::Bicolor(DigitalOut& greenPin, DigitalOut& redPin): m_redPin(redPin), m_greenPin(greenPin) {
  off();
}

void Bicolor::off() {
  m_redPin.low();
  m_greenPin.low();
}

void Bicolor::red() {
  m_redPin.high();
  m_greenPin.low();
}

void Bicolor::green() {
  m_redPin.low();
  m_greenPin.high();
}

