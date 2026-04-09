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
#include <Arduino.h>
#include <main.hpp>
#include <helper.hpp>
#include <AUnit.h>
#include <battery.hpp>
#include <config_gateway.hpp>

using aunit::Printer;
using aunit::TestRunner;
using aunit::Verbosity;

extern GravmonGatewayConfig myConfig;

void setup() {
  Serial.begin(115200);
  Serial.println("Gravitymon Gateway - Unit Test Build");

  delay(2000);
  Printer::setPrinter(&Serial);
  // TestRunner::setVerbosity(Verbosity::kAll);

  // TestRunner::exclude("measure_*");
}

void loop() {
  TestRunner::run();
  delay(10);
}

// EOF
