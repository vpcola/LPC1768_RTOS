/*
 * HEAP.c
 *
 *  Created on: Jun 12, 2016
 *      Author: Vergil
 */

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "cr_section_macros.h"

// For FreeRTOS to use the heap at ahb Ram
//
__DATA(RAM2) uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
