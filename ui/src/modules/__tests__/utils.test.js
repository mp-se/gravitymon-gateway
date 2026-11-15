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
import {
  httpHeaderOptions,
  httpPostUrlOptions,
  httpGetUrlOptions,
  brewgraphEndpointWarning
} from '@/modules/utils'

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

describe('utils - brewgraphEndpointWarning', () => {
  const gravityUrl = 'http://ingest.brewgraph.com/ingest/gravitymon'
  const pressureUrl = 'http://ingest.brewgraph.com/ingest/pressuremon'
  const autoUrl = 'http://ingest.brewgraph.com/ingest/dispatch'

  it('warns when the gravity endpoint also has pressure enabled', () => {
    expect(brewgraphEndpointWarning(gravityUrl, true, true)).toContain('gravity endpoint')
    expect(brewgraphEndpointWarning(gravityUrl, false, true)).toContain('gravity endpoint')
  })

  it('warns when the pressure endpoint also has gravity enabled', () => {
    expect(brewgraphEndpointWarning(pressureUrl, true, true)).toContain('pressure endpoint')
  })

  it('does not warn for matching, auto or other urls', () => {
    expect(brewgraphEndpointWarning(gravityUrl, true, false)).toBe('')
    expect(brewgraphEndpointWarning(pressureUrl, false, true)).toBe('')
    expect(brewgraphEndpointWarning(autoUrl, true, true)).toBe('')
    expect(brewgraphEndpointWarning('http://example.com/x', true, true)).toBe('')
    expect(brewgraphEndpointWarning(undefined, true, true)).toBe('')
  })
})
