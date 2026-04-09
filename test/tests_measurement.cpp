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
#include <AUnit.h>

#include <measurement.hpp>

MeasurementList myMeasurementList;

test(measure_list) {
  MeasurementList list;

  assertEqual(list.size(), 0);
  assertEqual(list.findMeasurementById("test"), -1);  

  std::unique_ptr<MeasurementBaseData> td;
  
  td.reset(new TiltData(MeasurementSource::NoSource, TiltColor::Red, 11.5, 2.2, 10, -10, false)); 
  list.updateData(td);
  // const TiltData* ptr = list.getMeasurementEntry(0)->getTiltData();

  assertEqual(list.findMeasurementById("Red"), 0);  
  assertEqual(list.size(), 1);  
  assertEqual(list.getMeasurementEntry(0)->getTiltData()->getRssi(), -10);
  assertEqual(list.getMeasurementType(0), MeasurementType::Tilt);  
  assertEqual(list.getMeasurementType(-1), MeasurementType::NoType);  
  assertEqual(list.getMeasurementType(1), MeasurementType::NoType);  
  td.reset(new TiltData(MeasurementSource::NoSource, TiltColor::Blue, 11.5, 2.2, 10, -12, false)); 

  list.updateData(td);
  assertEqual(list.findMeasurementById("Blue"), 1);  
  assertEqual(list.size(), 2);  
  assertEqual(list.getMeasurementType(1), MeasurementType::Tilt);  
  assertEqual(list.getMeasurementEntry(1)->getTiltData()->getRssi(), -12);

  td.reset(new TiltData(MeasurementSource::NoSource, TiltColor::Blue, 12.5, 3.2, 12, -18, false)); 
  list.updateData(td);
  assertEqual(list.findMeasurementById("Blue"), 1);  
  assertEqual(list.size(), 2);  
  Serial.printf("%d ", list.getMeasurementType(0));
  Serial.printf("%d ", list.getMeasurementType(1));
  assertEqual(list.getMeasurementType(1), MeasurementType::Tilt);  
  assertEqual(list.getMeasurementEntry(1)->getTiltData()->getRssi(), -18);
}

test(measure_container) {
  MeasurementEntry d("test");

  assertEqual(d.getId(), "test");
  assertEqual(d.isUpdated(), false);
  assertEqual(d.getType(), MeasurementType::NoType);
  assertEqual(d.getData(), nullptr);

  d.setUpdated();
  assertEqual(d.isUpdated(), true);

  d.setPushed();
  assertEqual(d.isUpdated(), false);
}

test(measure_base) {
  MeasurementBaseData d("test", MeasurementType::NoType, MeasurementSource::NoSource);

  assertEqual(d.getId(), "test");
  assertEqual(d.getType(), MeasurementType::NoType);
  assertEqual(d.getSource(), MeasurementSource::NoSource);
}

test(measure_tilt) {
  TiltData d(MeasurementSource::NoSource, TiltColor::Red, 11.5, 2.2, 10, -10, false);

  assertEqual(d.getType(), MeasurementType::Tilt);
  assertEqual(d.getId(), "Red");
  float t = 11.5;
  assertEqual(d.getTempF(), t);
  t = 2.2;
  assertEqual(d.getGravity(), t);
  assertEqual(d.getTxPower(), 10);
  assertEqual(d.getRssi(), -10);
  assertEqual(d.getTiltColor(), TiltColor::Red);
}

test(measure_gravity) {
  GravityData d(MeasurementSource::NoSource, "id", "name", "token", 11.5, 2.2, 3.3, 4.4, 10, -10, 5);

  assertEqual(d.getType(), MeasurementType::Gravitymon);
  assertEqual(d.getId(), "id");
  assertEqual(d.getName(), "name");
  assertEqual(d.getToken(), "token");
  float t = 11.5;
  assertEqual(d.getTempC(), t);
  t = 2.2;
  assertEqual(d.getGravity(), t);
  t = 3.3;
  assertEqual(d.getAngle(), t);
  t = 4.4;
  assertEqual(d.getBattery(), t);
  assertEqual(d.getTxPower(), 10);
  assertEqual(d.getRssi(), -10);
  assertEqual(d.getInterval(), 5);
}

test(measure_pressure) {
  PressureData d(MeasurementSource::NoSource, "id", "name", "token", 11.5, 2.2, 3.3, 4.4, 10, -10, 5);

  assertEqual(d.getType(), MeasurementType::Pressuremon);
  assertEqual(d.getId(), "id");
  assertEqual(d.getName(), "name");
  assertEqual(d.getToken(), "token");
  float t = 11.5;
  assertEqual(d.getTempC(), t);
  t = 2.2;
  assertEqual(d.getPressure(), t);
  t = 3.3;
  assertEqual(d.getPressure1(), t);
  t = 4.4;
  assertEqual(d.getBattery(), t);
  assertEqual(d.getTxPower(), 10);
  assertEqual(d.getRssi(), -10);
  assertEqual(d.getInterval(), 5);
}

test(measure_chamber) {
  ChamberData d(MeasurementSource::NoSource, "id", 11.5, 2.2, -10);

  assertEqual(d.getType(), MeasurementType::Chamber);
  assertEqual(d.getId(), "id");
  float t = 11.5;
  assertEqual(d.getChamberTempC(), t);
  t = 2.2;
  assertEqual(d.getBeerTempC(), t);
  t = 3.3;
  assertEqual(d.getRssi(), -10);
}

// EOF
