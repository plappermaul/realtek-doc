/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ETHERNET_EX_API_H
#define ETHERNET_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef void (*ethernet_callback)(uint32_t event, uint32_t data);

void ethernet_irq_hook(ethernet_callback callback);
/* Set the numbers of Tx/Rx descriptor */
void ethernet_set_descnum(uint8_t txdescCnt, uint8_t rxdescCnt);



#ifdef __cplusplus
}
#endif

#endif  // #ifndef ETHERNET_EX_API_H

