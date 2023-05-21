class myGraph {
    constructor(chartName) {
        this.chartName = chartName;
        this.graphLines = [
            {
            key: "max", //this.lineList[0],
            values: [ [ 0 , 10] , [ 10 , 10] , [ 20 , 20] , [ 30 , 30] , [ 40 , 40] , [ 50 , 50]],
            },
            {
            key: "min", //this.lineList[1],
            values: [ [ 0 , 10] , [ 10 , 60] , [ 20 , 60] , [ 30 , 60] , [ 40 , 60] , [ 50 , 60]],
            },
            {
            key: "measure", // this.lineList[2],
            values: [ [ 0 , 5] , [ 10 , 15] , [ 20 , 25] , [ 30 , 35] , [ 40 , 45] , [ 50 , 55]],
            }
        ];
    }
    // lineList = ["max", "min", "avg"];
  
    setLine(lineName, line){
        for(var i = 0; i < this.graphLines.length; i++){
            if(this.graphLines[i].key == lineName){
                this.graphLines[i].values = line;
                break;
            }
            if(i == this.graphLines.length - 1){
                console.log("Warning in graph: line not found, a new one will be created");
                this.graphLines.push({
                    key: lineName,
                    values: line
                });
            }
        }
    };

    getGraphLines(){
        return this.graphLines;
    };

    updateGraph(){
        var lines = this.graphLines;
        var gramName = this.chartName;
        nv.addGraph(function() {
            var chart = nv.models.cumulativeLineChart()
                .x(function(d) { return d[0] })
                .y(function(d) { return d[1]/100});
            d3.select("#"+gramName+" svg")
                .datum(lines)
                .call(chart);
            return chart;
        });
    };
}
