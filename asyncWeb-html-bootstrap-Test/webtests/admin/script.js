var ws = new WebSocket('ws://' + location.hostname + '/ws');

// chart **********************
// var xValuesMin = [];
// var xValuesMax = [];
// ****************************

const xArray = [];
const line = [];

ws.onopen = function() {
  window.alert("Connected");
};

ws.onmessage = function(evt) {
  if(evt.data.startsWith("arr:")){
    console.log("arr" + evt.data);
    var arr = evt.data.split(",");
    var x = arr[1];
    var y = Math.round(arr[2]);
    if(xArray.length > y){
      xArray[y] = x;
    }
    else{
      while(!(xArray.length > y)){
        xArray.push(0);
      }
      xArray[y] = x;
    }
    line = [];
    for(var i = 0; i < xArray.length; i++){
      line.push([i, xArray[i]]);
    }
    chartUpdate();    
  }

  // $(".progress-bar").css("width", evt.data + "%").text(evt.data + "%");
  // document.getElementById("display").innerHTML  = "temperature: " + evt.data + " C";
};

 

// var i = 0;
// function makeProgress(){
//   if(i < 100){
//     i = i + 1;
//     $(".progress-bar").css("width", i + "%").text(i + "%");
//   }
  
//   // Wait for sometime before running this script again
//   setTimeout("makeProgress()", 100);
// }


function onClick(element){

  ws.send(element.id);

  // document.cookie = "username=John Doe";

  
  // graph = new myGraph("chart1");
  // graph.setLine("measure", [
  //                             [ 10 , 10], 
  //                             [ 11 , 20], 
  //                             [ 12 , 30], 
  //                             [ 13 , 40],
  //                             [ 14 , 50],
  //                             [ 15 , 60],
  //                             [ 16 , 70],
  //                             [ 17 , 80],
  //                             [ 18 , 90],
  //                             [ 19 , 100],
  //                             [ 20 , 110],
  //                           ]);
  // graph.updateGraph();

  // // Wrapping in nv.addGraph allows for '0 timeout render', stores rendered charts in nv.graphs,
	// 						// and may do more in the future... it's NOT required
	// 						// graphLineUpdate();

              
												

  // // const layout = {
  // //   xaxis: {range: [0, 200], title: "l"},
  // //   yaxis: {range: [0, 200], title: "F"},  
  // //   // title: "House Prices vs. Size"
  // // };
  // // const data = [{
  // //   x: xArray,
  // //   y: yArray,
  // //   mode:"lines+markers"
  // // }];
  // // Plotly.newPlot("myPlot", data, layout);
  // // if(yArray.length == 30){
  // //   yArray[yArray.length-15] = 100;
  // // }
  // // xArray.push(yArray.length*5);
  // // yArray.push(yArray.length);

  // var xhr = new XMLHttpRequest();
  // xhr.open("GET", "/update?type=button&id="+element.id, true);
  // i = 0;
  // // makeProgress();

  // const file = new File(['foo'], 'new-note.txt', {
  //   type: 'text/plain',
  // })
  
  // // // function download() {
  // //   const link = document.createElement('a')
  // //   const url = URL.createObjectURL(file)
  
  // //   link.href = url
  // //   link.download = file.name
  // //   document.body.appendChild(link)
  // //   link.click()
  
  // //   document.body.removeChild(link)
  // //   window.URL.revokeObjectURL(url)
  // // }

  // xhr.send();
}

// const xValues = [];
// function chartUpdate(){
//   new Chart("myChart", {
//     type: "line",
//     data: {
//     labels: xValues,
//     datasets: [{ 
//       data: xValues,
//       borderColor: "red",
//       fill: false
//     }, { 
//       data: xValuesMin,
//       borderColor: "green",
//       fill: false
//     }, { 
//       data: xValuesMax,
//       borderColor: "blue",
//       fill: false
//     }]
//     },
//     options: {
//     legend: {display: false}
//     }
//   });
// }



