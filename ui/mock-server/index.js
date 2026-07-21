/*
 * Project specific mock server
 *
 * (c) 2023-2024 Magnus Persson
 */
import { createRequire } from 'module'
import { registerEspFwk } from './espfwk.js'
import { formatData } from './data.js'

const require = createRequire(import.meta.url)
const express = require('express')
var cors = require('cors')
const path = require('path')

const app = express()
const port = 3000

app.use(cors())
app.use(express.json())

registerEspFwk(app)

var testRunning = false

app.post('/api/push', (req, res) => {
  console.log('GET: /api/push')
  /* 
   * Description:    Initiate the push test for a defined target
   * Authentication: Required
   * Limitation:     - 
   * Note:           Use /api/test/push/status to check for completion
   * Return:         200 OK, 401 Access Denied, 422 Content Invalid
   * Request body:
     {
       push_format: "http_format|http_format2|http_format3|influxdb2_format|mqtt_format"
     }
   */
  if (!Object.prototype.hasOwnProperty.call(req.body, 'push_format')) {
    res.sendStatus(422)
    return
  }
  testRunning = true
  setTimeout(() => {
    testRunning = false
  }, 5000)
  var data = {
    success: true,
    message: 'Test scheduled.'
  }
  res.type('application/json')
  res.send(data)
})

app.get('/api/push/status', (req, res) => {
  console.log('GET: /api/push/status')
  /*
   * Description:    Return status of the current gyro calibration process.
   * Authentication: Required
   * Limitation:     -
   * Note:           -
   * Return:         200 OK, 401 Access Denied
   */
  var data = {}
  if (testRunning) {
    data = {
      status: testRunning,
      success: false,
      message: 'Push test running...'
    }
  } else {
    data = {
      status: false,
      success: true,
      message: 'Push test completed...',
      push_return_code: 200,
      push_enabled: true
    }
  }
  res.type('application/json')
  res.send(data)
})

app.get('/api/format', (req, res) => {
  console.log('GET: /api/format')
  /*
   * Description:    Return format data as json document.
   * Authentication: Required
   * Limitation:     -
   * Note:           -
   * Return:         200 OK, 401 Access Denied
   */
  res.type('application/json')
  res.send(formatData)
})

app.post('/api/format', (req, res) => {
  console.log('POST: /api/format')
  /*
   * Description:    Update the format data that is in body
   * Authentication: Required
   * Limitation:     -
   * Note:           See format read for options.
   * Return:         200 OK, 401 Access Denied
   */
  console.log(req.body)
  for (var o in req.body) {
    formatData[o] = req.body[o]
  }
  var data = {
    success: true,
    message: 'Format stored'
  }
  res.type('application/json')
  res.send(data)
})

app.get('/sd/data.csv', function (req, res) {
  console.log('GET: /')
  const options = {
    root: path.join('.')
  }
  res.type('text/csv')
  res.sendFile('mock-server/data.csv', options)
})

app.get('/sd/data1.csv', function (req, res) {
  console.log('GET: /')
  const options = {
    root: path.join('.')
  }
  res.type('text/csv')
  res.sendFile('mock-server/data1.csv', options)
})

app.get('/sd/data2.csv', function (req, res) {
  console.log('GET: /')
  const options = {
    root: path.join('.')
  }
  res.type('text/csv')
  res.sendFile('mock-server/data2.csv', options)
})

app.post('/api/sd', (req, res) => {
  console.log('POST: /api/sd')
  /* 
     * Description:    Interact with SD file system on the device
     * Authentication: Required
     * Limitation:     Does not support get, use GET: /sd/<filename> instead
     * Return:         200 OK, 401 Access Denied, 400 Faulty request
     * Request body:
       {
         command: "dir|del",
         file: "name of file for del"
       }
     */
  console.log('Command:', req.body.command)

  if (req.body.command == 'dir') {
    var data = {
      total: 1000,
      used: 900,
      free: 100,
      files: [{ file: '/data.csv', size: 200 },{ file: '/data1.csv', size: 200 },{ file: '/data2.csv', size: 200 }]
    }
    res.type('application/json')
    res.send(data)
    return
  } else if (req.body.command == 'del') {
    console.log(req.body.file)
    setTimeout(() => {
      res.sendStatus(200)
    }, 2000)
    return
  }

  res.sendStatus(400)
})

app.listen(port, () => {
  console.log(`Gravitymon BLE Gateway API simulator port http://localhost:${port}/`)
})
