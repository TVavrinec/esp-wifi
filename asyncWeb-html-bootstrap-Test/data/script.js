var ws = new WebSocket('ws://' + location.hostname + '/ws');

ws.onopen = function() {
  window.alert("Connected");
};

ws.onmessage = function(evt) {
  alert(evt.data);
  if(evt.data.startsWith("arr:")){
    var arr = evt.data.split(",");
    var x = arr[1];
    var y = arr[2];
    alert(x + " " + y);
    document.getElementById("x").innerHTML  = "x: " + x;
    document.getElementById("y").innerHTML  = "y: " + y;
  }

  // $(".progress-bar").css("width", evt.data + "%").text(evt.data + "%");
  // document.getElementById("display").innerHTML  = "temperature: " + evt.data + " C";
};

 

var i = 0;
function makeProgress(){
  if(i < 100){
    i = i + 1;
    $(".progress-bar").css("width", i + "%").text(i + "%");
  }
  
  // Wait for sometime before running this script again
  setTimeout("makeProgress()", 100);
}

const xValues = [100,200,300,400,500,600,700,800,900,1000];
function onClick(element){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/update?output="+element.id+"&state=1", true);
  alert(location.hostname);
  i = 0;
  makeProgress();
  
  
  new Chart("myChart", {
    type: "line",
    data: {
    labels: xValues,
    datasets: [{ 
      data: [860,1140,1060,1060,1070,1110,1330,2210,7830,2478],
      borderColor: "red",
      fill: false
    }, { 
      data: [1600,1700,1700,1900,2000,2700,4000,5000,6000,7000],
      borderColor: "green",
      fill: false
    }, { 
      data: [300,700,2000,5000,6000,4000,2000,1000,200,100],
      borderColor: "blue",
      fill: false
    }]
    },
    options: {
    legend: {display: false}
    }
  });

  xhr.send();
}