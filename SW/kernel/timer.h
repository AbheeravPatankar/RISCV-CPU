#pragma once

#include "utils.h"

#define CLINT_MTIMECMP (volatile uint32*)0x02004000
#define CLINT_MTIME    (volatile uint32*)0x0200BFF8

void handle_mtimer();

void timerinit();

void timervec();

void timer_interrupt();