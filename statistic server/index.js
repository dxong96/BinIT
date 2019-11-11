const express = require('express')
const bodyParser = require('body-parser')
const app = express()
const port = 3000

app.use(bodyParser.urlencoded({ extended: false }));

const fs = require('fs');
let dataFd = null
let data = []
let fileSize = 0

try {
  dataFd = fs.openSync('data.json', 'r+')
} catch (err) {
  if (err && err.code === 'ENOENT') {
    console.error('data.json does not exists initializing');
    fs.writeFileSync('data.json', '[]');
    dataFd = fs.openSync('data.json', 'r+')
  } else {
    throw err; 
  }
}

const dataStr = fs.readFileSync(dataFd, {
  encoding: 'utf8'
})
data = JSON.parse(dataStr)
fileSize = dataStr.length

const appendTemperatureAndFullness = (temperature, fullness) => {
  const currentTimeInSeconds = Math.floor(Date.now() / 1000)
  const item = [temperature, fullness, currentTimeInSeconds]
  data.push(item)
  const itemStr = JSON.stringify(item)
  if (fileSize < 3) {
    fs.writeSync(dataFd, itemStr + ']', fileSize - 1)
  } else {
    fs.writeSync(dataFd, ',' + itemStr + ']', fileSize - 1)
    fileSize += 1
  }
  fileSize += itemStr.length
}

app.get('/temp_fullness', (req, res) => {
  const {temperature, fullness} = req.query
  const temperatureF = parseFloat(temperature)
  const fullnessF = parseFloat(fullness)
  if (isNaN(temperatureF) || isNaN(fullnessF)) {
    res.status(422).send('invalid fullness or temperature')
    return
  }
  appendTemperatureAndFullness(temperatureF, fullnessF)
  res.send('success')
})

app.listen(port)
