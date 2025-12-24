#pragma once

#define MSTATUS_MPIE   (1 << 7)
#define MSTATUS_MPP    (3 << 11)

void prepare_return();

void userret();