#ifndef ION_TIMING_H
#define ION_TIMING_H

#ifdef __cplusplus
#define EXTERNC extern "C"
namespace Ion {
#else
#define EXTERNC
#endif

EXTERNC uint32_t millis();
EXTERNC uint32_t micros();

EXTERNC void mssleep(long ms);
EXTERNC void ussleep(long us);

#ifdef __cplusplus
}
#endif

#endif
