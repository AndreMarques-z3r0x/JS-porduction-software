var n_postos
var ctx
var postochart = new Array(n_postos)
$(document).ready(function () {
	$.ajax({
		url: '/getconf',
		success: function (result) {
			n_postos = result.postos
			loadpage()
            for (let i=0; i<n_postos;i++){
            get_hour_data (i+1)
            }
		}
	})

	$('#container').empty();

})

function loadpage () {
	for(let i=1; i<=n_postos; i++){
		console.log(i)
		$('#container').append(`
            <div style="width: 5%;float: left;text-align: center;color: white;">
            <p>Posto `+i+`</p>
            </div>
			<div class="posto">
			<canvas class="infoGraph" style="display: inline;"></canvas>
			</div>
            <div class="infoposto">
            <table class="mytable" id="mytable" align="center" cellspacing="10px" >
                <tr>
                <td>Total<p id="total`+i+`"></p></td>
                <td>Media Pc's/h<p id="media`+i+`"></p></td>
                <td>Objetivo<p id="objetivo`+i+`"></p></td>
                </tr>
            </table>
            </div>
		`)
	}
    ctx = document.querySelectorAll(".infoGraph")
	drawgraphs()
}

var linechart ={    
	type: 'line',
    data: {
        labels: ['08','09','10','11','12','13','14','15','16','17','18','19','20','21','22','23','00','01','02','03','04','05','06','07'],
        datasets: [{
            data: [],
            backgroundColor: 'rgba(97, 151, 126, 1)',
            borderWidth: 3,
            borderColor: 'rgba(97, 151, 126, .8)',
            fill: false
        }]
    },
    options: {
        tooltips: {enabled: false},
        hover: {mode: null},
    	responsive: true,
        maintainAspectRatio: false,
        scales: {
            yAxes: [{
                ticks: {
                    fontColor: "white",
                    fontSize: 18,
                    beginAtZero: false
                    }
            }],
            xAxes: [{
                ticks: {
                    fontColor: "white",
                    fontSize: 18,
                }
            }]
        },
        legend: {
            display: false,
            labels:{
                fontColor: '#fff'
            }
        },
        elements: {
        	line: {
            	tension: 0
        	}
    	},
        plugins:{
            datalabels:{
                anchor: 'end',
                align: 'center',
                color: '#fff',
                font:{
                    weight : 'bold',
                    size: '12'                      
                },
            }
        }
    }
}

function drawgraphs(){
	for(let i=0; i<n_postos; i++){
	postochart[i] = new Chart(ctx[i], linechart)
	}
}

function get_hour_data (posto) {
    $.ajax({
        url: '/hourdata',
        data: {
            'posto': posto
        },
        success: function(result) {
            console.log(result)
            postochart[posto-1].data.datasets[0].data = result.pecas
            postochart[posto-1].update()
            $('#total'+posto).empty()
            $('#total'+posto).append(result.tot+" PC's")
            $('#media'+posto).empty()
            $('#media'+posto).append(result.med+" PC's")
            $('#objetivo'+posto).append('<button class="mybutton" type="button"><i class="fas fa-plus-circle fa-2x"></i></button')     
        }
    })
}