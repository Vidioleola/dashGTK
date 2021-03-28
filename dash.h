#ifndef dash_h
#define dash_h

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern bool rics_init(void);
extern bool rics_start(int32_t node);
extern bool rics_can_callback(uint32_t id, size_t len, uint8_t *dat);


#endif
