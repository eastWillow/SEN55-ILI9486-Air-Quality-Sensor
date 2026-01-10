// Since SensorReal is fully defined in the header (template-like or inline for simplicity in Arduino),
// we might not strictly need a .cpp unless we move implementation there.
// For now, I'll keep it empty or minimal to satisfy the "split header/impl" if needed,
// but for this wrapper, inline is cleaner for Arduino.
// However, the plan said "SensorReal.cpp". Let's put a dummy include.

#include "SensorReal.h"
