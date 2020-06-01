
var labels_tag = ['turno 1', 'turno2', 'turno 3'];
var data_tag = [{% for turno in range(1,4) %}
                {{ pecas[posto][turno] }},
                {% endfor %}];

var ctx = document.getElementById("postos_chart{{posto}}");

var postod_chart = new Chart (ctx, {
type: 'bar',
data: {
labels: labels_tag,
datasets: [{
    label: 'Pecas',
    data: data_tag,
    backgroundColor: 'rgba(255, 159, 64, 0.5)',
    borderWidth: 1
    }]
    },
 options: {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
        yAxes: [{
            ticks: {
                 beginAtZero: true
                    }
            }]
        }
    }
});