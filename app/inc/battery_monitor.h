#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <stdbool.h>
typedef enum {
    BATTERY_EMPTY,
    BATTERY_VERY_LOW,
    BATTERY_LOW,
    BATTERY_MEDIUM,
    BATTERY_FULL,
    CHARGING
} battery_level_t;

extern bool sleep_mode;
void battery_monitor_init(void);
battery_level_t battery_monitor_update(void);

#endif
