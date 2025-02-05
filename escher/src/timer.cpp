#include <escher/timer.h>

Timer::Timer(uint32_t period) :
  m_period(period),
  m_numberOfTicksBeforeFire(period)
{
}

bool Timer::tick() {
  m_numberOfTicksBeforeFire--;
  if (m_numberOfTicksBeforeFire == 0) {
    bool needRedraw = fire();
    reset();
    return needRedraw;
  }
  return false;
}

void Timer::reset(int time) {
  if (time != 0) {
    m_period = time;
  }
  m_numberOfTicksBeforeFire = m_period;
}
