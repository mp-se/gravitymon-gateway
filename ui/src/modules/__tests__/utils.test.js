/*
 * GravityMon Gateway UI
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

import { describe, expect, it } from 'vitest'
import { httpHeaderOptions, httpPostUrlOptions, httpGetUrlOptions } from '@/modules/utils'

describe('utils - shared options', () => {
  it('exports httpHeaderOptions ref', () => {
    expect(httpHeaderOptions).toBeDefined()
    expect(httpHeaderOptions.value).toBeInstanceOf(Array)
    expect(httpHeaderOptions.value.length).toBeGreaterThan(0)
  })

  it('exports httpPostUrlOptions ref', () => {
    expect(httpPostUrlOptions).toBeDefined()
    expect(httpPostUrlOptions.value).toBeInstanceOf(Array)
    expect(httpPostUrlOptions.value.length).toBeGreaterThan(0)
  })

  it('exports httpGetUrlOptions ref', () => {
    expect(httpGetUrlOptions).toBeDefined()
    expect(httpGetUrlOptions.value).toBeInstanceOf(Array)
  })
})
