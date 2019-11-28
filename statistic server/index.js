const express = require('express')
const bodyParser = require('body-parser')
const app = express()
const port = 3000

app.set('view engine', 'ejs');
app.use(express.static('views'));
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

const fs = require('fs');
let dataFd = null
let data = []
let fileSize = 0
require('dotenv').config()

const client = require('twilio')(process.env.accountSID, process.env.authToken);

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
console.log(data);
fileSize = dataStr.length


app.get('/', (req,res, next) => {

  const temperature_data = data.map((temperature) => {
    return {
      y: temperature[0],
      x: temperature[2]
    }
  })

  res.render('graph', {data: temperature_data})
})


app.get('/content', (req, res) => {
  const content_data = data.map((fullness) => {
    return {
      x: fullness[2],
      y: fullness[1]
    }
  })
  res.render('content', {data: content_data})
})



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

const sendSMS = (temperature, fullness) => {

  if (temperature >= 40 || fullness >= 90){
    const body = temperature >=40 ? `temperature ${temperature} above threshold` : `The bin is at ${fullness} full`
    client.messages.create({body: body, from: '+18052420289', to: '+6596317380'}).then(message => console.log(message));
  }
}




app.get('/temp_fullness', (req, res) => {
  console.log(req.query);
  const {temperature, fullness} = req.query
  const temperatureF = parseFloat(temperature)
  const fullnessF = parseFloat(fullness)
  if (isNaN(temperatureF) || isNaN(fullnessF)) {
    res.status(422).send('invalid fullness or temperature')
    return
  }
  sendSMS(temperature, fullness)
  appendTemperatureAndFullness(temperatureF, fullnessF)
  res.send('success')
})

app.listen(port)
