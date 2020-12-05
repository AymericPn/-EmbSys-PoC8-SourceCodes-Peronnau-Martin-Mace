$(document).ready(function () {// basically the same code as the linegraphpollution.js code
  $.ajax({
    url: "http://localhost/code/monitoring_data.php",
    type: "GET",
    success: function (data) {
      console.log(data);
      var time = [];
      var value = [];


      for (var i in data) {
        time.push("time " + data[i].time);
        value.push(data[i].value);
      }

      var chartdata = {
        labels: time,
        datasets: [
          {
            label: "recorded sound levels",
            fill: false,
            lineTension: 0.1,
            backgroundColor: "rgba(59, 89, 152, 0.75)",
            borderColor: "rgba(59, 89, 152, 1)",
            pointHoverBackgroundColor: "rgba(59, 89, 152, 1)",
            pointHoverBorderColor: "rgba(59, 89, 152, 1)",
            data: value
          },
        ]
      };
      var ctx = $("#mycanvas2");
      var LineGraph = new Chart(ctx, {
        type: 'line',
        data: chartdata
      });
    },
    error: function (data) {
    }
  });
});