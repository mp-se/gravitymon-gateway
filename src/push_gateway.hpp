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
#ifndef SRC_PUSH_GATEWAY_HPP_
#define SRC_PUSH_GATEWAY_HPP_

#if defined(GATEWAY)

#include <config_gateway.hpp>
#include <templating.hpp>

void setupTemplateEngineGravityGateway(GravmonGatewayConfig* config,
                                       TemplatingEngine& engine, float angle,
                                       float gravitySG, float tempC,
                                       float voltage, int interval,
                                       const char* id, const char* token,
                                       const char* name);

void setupTemplateEnginePressureGateway(GravmonGatewayConfig* config,
                                        TemplatingEngine& engine,
                                        float pressurePsi, float pressurePsi1,
                                        float tempC, float voltage,
                                        int interval, const char* id,
                                        const char* token, const char* name);

#endif  // GATEWAY

#endif  // SRC_PUSH_GATEWAY_HPP_
