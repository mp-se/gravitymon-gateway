/*
 * GravityMon Gateway
 * Copyright (c) 2021-2026 Magnus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef SRC_MAIN_GATEWAY_HPP_
#define SRC_MAIN_GATEWAY_HPP_

#if defined(GATEWAY)

#include <templating.hpp>

// Defines that needs to be there for the common base but not used in the
// gateway
#if defined(LOLIN_D32_PRO)
#define PIN_VOLT T0
#define CFG_FILENAMEBIN "firmware32pro-tft.bin"
#elif defined(LOLIN_S3_PRO)
#define PIN_VOLT A3
#define CFG_FILENAMEBIN "firmware32s3pro-tft-sd.bin"
#elif defined(WAVESHARE_S3_TFT)
#define PIN_VOLT A3
#define CFG_FILENAMEBIN "firmware32s3wave-tft-sd.bin"
#endif

#endif  // GATEWAY

#endif  // SRC_MAIN_GATEWAY_HPP_
