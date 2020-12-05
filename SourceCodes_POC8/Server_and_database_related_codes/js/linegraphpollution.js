$(document).ready(function () {
  $.ajax({// ajax is used to do http requests easily
    url: "http://localhost/code/monitoring_data.php",// this program will call the php file to get the database data
    type: "GET",
    success: function (data) {
      console.log(data);
      var time = [];
      var pollution_value = [];// arrays to store pollution and time values

      for (var i in data) {
        time.push(data[i].time);// the push function is like python's append function, here it adds the next time value to the time array 
        pollution_value.push(data[i].pollution_value);
      }

      var chartdata = {// define a dictionnary to store all informations that will be displayed on the graphs
        labels: time,
        datasets: [
          {
            label: "collected pollution rates", // title of the graph
            fill: false,
            lineTension: 0.1,
            backgroundColor: "rgba(29, 202, 255, 0.75)",
            borderColor: "rgba(29, 202, 255, 1)",
            pointHoverBackgroundColor: "rgba(29, 202, 255, 1)",
            pointHoverBorderColor: "rgba(29, 202, 255, 1)",
            data: pollution_value// use the data from the pollution_value array
          },
        ]
      };
      var ctx = $("#mycanvas");
      var LineGraph = new Chart(ctx, {// use Chart.JS to draw the chart using the data chartdata
        type: 'line',// type of graph, here linegraph
        data: chartdata
      });

    },
    error: function (data) {
    }
  });
});