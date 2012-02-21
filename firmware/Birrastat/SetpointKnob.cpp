#include "SetpointKnob.h"

SetpointKnob::SetpointKnob(AnalogIn& pin, int minVal, int maxVal)
  : m_pin(pin), m_minVal(minVal), m_maxVal(maxVal) {
}

int SetpointKnob::read() {
  return map(m_pin.read(), 0, 1023, m_minVal, m_maxVal);
}
