var jsonWiFiData = '[{"ssid": "WiFi1", "signal": 80, "state": "connected"}, {"ssid": "WiFi2", "signal": 90, "state": "avalable"}, {"ssid": "WiFi3", "signal": 70, "state": "avalable"}, {"ssid": "WiFi4", "signal": 60, "state": "avalable"}, {"ssid": "WiFi5", "signal": 50, "state": "avalable"}, {"ssid": "WiFi6", "signal": 40, "state": "avalable"}, {"ssid": "WiFi7", "signal": 30, "state": "avalable"}, {"ssid": "WiFi8", "signal": 20, "state": "avalable"}, {"ssid": "WiFi9", "signal": 10, "state": "avalable"}]';

// WebSockets client
const ws = new WebSocket('ws://' + location.hostname + '/ws');

// Parsování JSON dat
var wifiData = JSON.parse(jsonWiFiData);

// Vytvoření seznamu WiFi připojení
var wifiList = document.getElementById('wifiList');
wifiData.forEach(function(item) {
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

function wifiAction(element){
  console.log("I wont connect to: " + element.id); 
  $('#wifiPasswordWin').modal('show');
  $('#wifiPasswordWin').on('shown.bs.modal', function () {
    var modalText = $(this).find('.wifiName p');
    modalText.text(element.id);
  });
}


function wifiPasswordButton(){
  const password = document.getElementById('password').value;
  const ssid = document.getElementById('wifiName').innerText;
  // Send login data via WebSocket
  ws.send(JSON.stringify({ type: 'wifi', ssid, password }));
  $('#wifiPasswordWin').modal('hide');
}

function wifiCancelButton(){
  $('#wifiPasswordWin').modal('hide');
}

function apSetButton(){
  const ssid = document.getElementById('apSsid').value;
  const password = document.getElementById('apPassword').value;
  console.log("SSID: " + ssid + " Password: " + password);
}

function apOnButton(){
  document.getElementById('apForm').style.display = 'block';
  document.getElementById('apOffButton').style.display = 'block'; 

  ws.send(JSON.stringify({ type: 'apOn' }));
}

ws.addEventListener('open', () => {
  console.log('WebSocket connection established');
});

ws.addEventListener('message', (event) => {
  const data = JSON.parse(event.data);

  switch (data.type) {
    case 'wifi':
      console.log('WiFi connection status: ' + data.status);
      break;
    case 'apOn':
      console.log('AP status: ' + data.status);
      break;
    default:
      console.log('Unknown message type');
  }
});

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
      console.log("AP set button");
      apSetButton();
      break;    
    default:
      console.log('Unknown button');
  }
});
