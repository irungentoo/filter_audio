
#include "signal_processing_library.h"


static int16_t FloatS16ToS16_C(float v) {
  static const float kMaxRound = (float)INT16_MAX - 0.5f;
  static const float kMinRound = (float)INT16_MIN + 0.5f;
  if (v > 0) {
    return v >= kMaxRound ? INT16_MAX : (int16_t)(v + 0.5f);
  }

  return v <= kMinRound ? INT16_MIN :(int16_t)(v - 0.5f);
}

void FloatS16ToS16(const float* src, size_t size, int16_t* dest) {
  size_t i;
  for (i = 0; i < size; ++i)
    dest[i] = FloatS16ToS16_C(src[i]);
}


void S16ToFloatS16(const int16_t* src, size_t size, float* dest) {
  size_t i;
  for (i = 0; i < size; ++i)
    dest[i] = src[i];
}
/*
void FloatToS16(const float* src, size_t size, int16_t* dest) {
  for (size_t i = 0; i < size; ++i)
    dest[i] = FloatToS16(src[i]);
}

void S16ToFloat(const int16_t* src, size_t size, float* dest) {
  for (size_t i = 0; i < size; ++i)
    dest[i] = S16ToFloat(src[i]);
}

void FloatToFloatS16(const float* src, size_t size, float* dest) {
  for (size_t i = 0; i < size; ++i)
    dest[i] = FloatToFloatS16(src[i]);
}

void FloatS16ToFloat(const float* src, size_t size, float* dest) {
  for (size_t i = 0; i < size; ++i)
    dest[i] = FloatS16ToFloat(src[i]);
}
*/