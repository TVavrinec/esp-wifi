var jsonWiFiData = '[{"ssid": "WiFi1", "signal": 80, "state": "connected"}, {"ssid": "WiFi2", "signal": 90, "state": "avalable"}, {"ssid": "WiFi3", "signal": 70, "state": "avalable"}, {"ssid": "WiFi4", "signal": 60, "state": "avalable"}, {"ssid": "WiFi5", "signal": 50, "state": "avalable"}, {"ssid": "WiFi6", "signal": 40, "state": "avalable"}, {"ssid": "WiFi7", "signal": 30, "state": "avalable"}, {"ssid": "WiFi8", "signal": 20, "state": "avalable"}, {"ssid": "WiFi9", "signal": 10, "state": "avalable"}]';

// WebSockets client
const ws = new WebSocket('ws://' + location.hostname + '/ws');

function showWifiList(jsonWiFiData){
  // var wifiData = JSON.parse(jsonWiFiData);
  var wifiList = document.getElementById('wifiList');
  jsonWiFiData.forEach(function(item) {
    var listItem = document.createElement('li');
    if (item.state === 'connected') {
      listItem.className = 'list-group-item list-group-item-action active';
    }
    else{
      listItem.className = 'list-group-item list-group-item-action';
    }
    // var wifiIcon = document.createElement('i');
    // wifiIcon.className = 'fas fa-wifi';
    // listItem.appendChild(wifiIcon);
    listItem.innerHTML = '<img src="wifi-high-svgrepo-com.svg" class="ikonka"></img>' + item.ssid;
    listItem.id = item.ssid;
    listItem.onclick = function() {
      wifiAction(this);
    };
    wifiList.appendChild(listItem);
  });
}

function wifiPasswordButton(){
  const password = document.getElementById('password').value;
  const ssid = document.getElementById('wifiName').innerText;
  // Send login data via WebSocket
  ws.send(JSON.stringify({ 
    type: 'wifi', 
    connect: {
      ssid, 
      password
    }}));
  $('#wifiPasswordWin').modal('hide');
}

function wifiAction(element){
  console.log("I wont connect to: " + element.id);
  const ssid = document.getElementById('wifiName').innerText;
  ws.send(JSON.stringify({ 
    type: 'wifi', 
    connect: {
      ssid
    }
  }));
}

function wifiCancelButton(){
  $('#wifiPasswordWin').modal('hide');
}

function madeWifiPaswordWindow(){
  $('#wifiPasswordWin').modal('show');
  $('#wifiPasswordWin').on('shown.bs.modal', function () {
    var modalText = $(this).find('.wifiName p');
    modalText.text(element.id);
  });
}

function apSetButton(){
  const ssid = document.getElementById('apSsid').value;
  const password = document.getElementById('apPassword').value;
  console.log("SSID: " + ssid + " Password: " + password);

  ws.send(JSON.stringify({ 
    type: 'wifi', 
    ap: {
      ssid, 
      password
    }
  }));
}

function apOnButton(){
  ws.send(JSON.stringify({ 
    type: 'wifi', 
    ap: {
      on: true
    }
  }));
}

function apOffButton(){  
  ws.send(JSON.stringify({ 
    type: 'wifi', 
    ap: {
      on: false
    }
  }));
}

function wifiLitRegest(){
  ws.send(JSON.stringify({ 
    type: 'wifi', 
    station: {
      get: 'wifi_list'
    }
  }));
}

// incoming wifi message from server **********************************************************************************************************************

function setWifiList(wifiList){
  let index = wifiList.findIndex(wifiList => wifiList.state === 'state');

  if (index !== -1) {
    let part = wifiList.splice(index, 1)[0];
    wifiList.unshift(part);
  }

  wifiData = JSON.parse(wifiList);
}

function stationMessage(message){
  if('wifiList' in message){
    console.log(message.wifiList);
    showWifiList(message.wifiList);
  }
  else if ('on' in message){
    
  }
  else if ('connect' in message){
    if(message.connect === 'unknown wifi'){
      madeWifiPaswordWindow();
    }
    else if(message.connect === 'not available'){
      console.log('WiFi not available');
      wifiListRegest();
    }
    else if(message.connect === 'connected'){
      wifiListRegest();
    }
    else{
      console.log('Unknown "connect" message');
    }
  }
  else{
    console.log('Unknown message');
  }
}

function apMessage(message){
  if ('on' in message){
    if (message.on){
      document.getElementById('apOnButton').style.backgroundColor = '#0000ff';
      document.getElementById('apOffButton').style.backgroundColor = '#8888ff';
    }
    else{
      document.getElementById('apOnButton').style.backgroundColor = '#8888ff';
      document.getElementById('apOffButton').style.backgroundColor = '#0000ff';
    }
  }
  else if ('set' in message){
    if (message.set){
      document.getElementById('apSetButton').style.backgroundColor = '#0000ff';
    }
    else{
      document.getElementById('apSetButton').style.backgroundColor = '#ff0000';
    }
  }
  else{
    console.log('Unknown message');
  }
}

function solverWifiMessage(message){
  if ('station' in message){
    stationMessage(message.station);
  }
  else if ('ap' in message){
    apMessage(message.ap);
    console.log('Unknown message');
  }
  else{
    console.log('Unknown message');
  }
}


// WebSocket event handlers *********************************************************************************************************************

ws.addEventListener('open', () => {
  console.log('WebSocket connection established');
  ws.send(JSON.stringify({ 
    type: 'general', 
    get: 'admin' 
  }));
});

ws.addEventListener('message', (event) => {
  const data = JSON.parse(event.data);

  switch (data.type) {
    case 'wifi':
      console.log('WiFi connection status: ' + data.status);
      solverWifiMessage(data);
      break;
    case 'apOn':
      console.log('AP status: ' + data.status);
      break;
    case 'unknown_wifi':
      madeWifiPaswordWindow();
    default:
      console.log('Unknown message type');
  }
});

// Form event handlers *********************************************************************************************************************

document.querySelector('form').addEventListener('submit', (event) => {
// event.preventDefault();
});

document.getElementById('wifiForm').addEventListener('click', (event) => {
  event.preventDefault();
  console.log(event.target.id);
  switch (event.target.id) {
    case 'wifiPasswordButton':
      wifiPasswordButton();
      break;
    case 'wifiCancelButton':
      wifiCancelButton();
      break;
    default:
      console.log('Unknown button');
  }
});

document.getElementById('apForm').addEventListener('click', (event) => {
  event.preventDefault();
  console.log(event.target.id);
  switch (event.target.id) {
    case 'apOffButton':
      apOffButton();
      break;
    case 'apOnButton':
      apOnButton();
      break;
    case 'apSetButton':
      apSetButton();
      break;    
    default:
      console.log('Unknown button');
  }
});
