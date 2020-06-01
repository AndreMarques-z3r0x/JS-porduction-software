var n_postos=0
var n_turnos=0
var ctx 
var bigctx
var postochart
var bigchart
var SelType =0   // 0->semana 1->mês 2->dia
$(document).ready(function () {
			$( "#draggable" ).draggable({ handle: ".handler", containment: "#container", scroll: false });
			$( "#draggable2" ).draggable({ containment: "#container", scroll: false });

			$.ajax({
				url: "/getconf",

				success: function (info) {
					n_postos = info.postos
					n_turnos = info.turnos
					loadpage()
					bigctx = document.getElementsByClassName("bigchart")
					ctx = document.querySelectorAll(".infoPostoGraph")
					postochart = new Array(ctx.length)
					drawgraphs()
					for(let i = 0; i<ctx.length; i++){
					get_single_graph_data(i+1,7)
					}
					//get_all_graph_data_week(2)
					test(1)
					selectDataDays(7)
					//get_test_mes(1)
		}
	})
			var slider = document.getElementById("myRange");
			slider.oninput = function(){
				if (SelType==0) {
				test(this.value)
				}
				if (SelType ==1) {
				get_test_mes(this.value)
				}
			}
})

function loadpage(){
	for (let i = 0; i<n_postos; i++){
		$("#container").append(`<div class="itemsmall">
								<div class="handler">Posto `+(i+1)+`: Total de Pe&ccedil;as por turno
								</div>
								<div class="pcharts">
								<canvas class="infoPostoGraph" style="display: inline;"></canvas>
								</div>
								</div>`)
	}
	for (let i=0; i<n_postos;i+=2){
		$('#mytable').append(`<tr>
								<td>Posto `+(i+1)+`<p id="posto`+(i+1)+`"></p></td>
								<td>Posto `+(i+2)+`<p id="posto`+(i+2)+`"></p></td>
							</tr>`)
	}
}

var smallchartinfo = {
            type: 'bar',
            data: {
                labels: ['turno 1', 'turno 2', 'turno 3'],
                datasets: [{
					    data: [510,350,608],
					    label: 'Pecas',
					    backgroundColor: ['rgba(15, 62, 62, .8)',
					    				'rgba(25, 103, 103, .8)',
					                    'rgba(35, 144, 144, .8)',
					                  	],
					    borderWidth: 1
					}],
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
                        	suggestedMax: 900,
                            beginAtZero: true
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
                plugins:{
                	datalabels:{
                		anchor: 'end',
                		align: 'center',
                		borderWidth: 2,
                		borderColor: '#fff',
                		borderRadius: 25,
                		color: '#fff',
                		font:{
                			weight : 'bold',
                			size: '12'                		
                		},
                		backgroundColor: (context) => {
                			return context.dataset.backgroundColor;
                		}
                	}
                }
            }	
}

var bigchartinfo = {
            type: 'bar',
            data: {
                labels: ['semana 1','semana 2'],
                datasets: [{
							data: [0],
							},
						   {
							data: [0],
							},
							{
							data: [0],
							},
							{
							data: [0],							
							},
							{
							data: [0],							
							},
							{
							data: [0],							
							}]
                },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    yAxes: [{
                        ticks: {
                        	fontColor: "white",
                        	fontSize: 18,
                        	suggestedMax: 900,
                            beginAtZero: true
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
                	display: true,
                	labels:{
                		fontColor: '#fff'
                	}
                },
                plugins:{
                	datalabels:{
                		anchor: 'end',
                		align: 'center',
                		borderWidth: 2,
                		borderColor: '#fff',
                		borderRadius: 25,
                		color: '#fff',
                		font:{
                			weight : 'bold',
                			size: '12'                		
                		},
                		backgroundColor: (context) => {
                			return context.dataset.backgroundColor;
                		}
                	}
                }
            }	
}

var colors = ['rgba(15, 62, 62, .8)',
				'rgba(25, 103, 103, .8)',
				'rgba(35, 144, 144, .8)',
                'rgba(45, 185, 185, .8)',
                'rgba(70, 210, 210, .8)',
                'rgba(111, 220, 220, .8)']


function drawgraphs(){
	for (let i=0; i<ctx.length; i++){
		postochart[i] = new Chart (ctx[i], smallchartinfo);
	}
	bigchart= new Chart (bigctx, bigchartinfo);
}

function get_single_graph_data (posto, sdays) {
	let max = 0
	//console.log('data graficos pequenos')
	$.ajax({
		url: '/data',
		data: {
			'days': sdays,
			'dowhat': posto,
		},
		success: function (result) {
			console.log('data graficos pequenos')
			for (let i=0; i<result.results.length; i++){
				if (result.results[i]>max) {
					max = result.results[i]
				}
			}
			postochart[posto-1].data.datasets[0].data = result.results
			postochart[posto-1].options.scales.yAxes[0].ticks.suggestedMax = max+200
			postochart[posto-1].update()
		}
	})
}



function test(weeks){
	var weeks_data = new Array(n_postos)
	for (let i = 0; i<n_postos;i++){
		weeks_data[i] = new Array(weeks+1)
	}
	var weeks_loop = weeks
	let y_labels = new Array(weeks+1)
	let n=0
	let max = 0
	while (weeks_loop >=0) {
		$.ajax({
			url: '/dataweek',
			async: false,
			cache: false,
			data: {
				'week': weeks_loop,
			},
			success: function (result) {
				console.log(result)
				for(let i=0; i<result.results.length; i++){
					weeks_data[i][n] = result.results[i]
					if (result.results[i]>max) {
						max = result.results[i]
					}
				}
				n++
				y_labels[weeks_loop] = result.inicio
			}
		})
		weeks_loop--
	}
	console.log(max)
	//console.log(weeks_data[0].length)
	let newdataset = new Array(n_postos)
	let k = 0
	let data_labels = new Array(n_postos)
	/*for (let i = 0; i<weeks_data[0].length;i++){
		let t = weeks - i
		y_labels[i] = 'semana ' + t
	}*/
	bigchart.data.labels = y_labels.reverse()
	for(let i = 0; i<n_postos; i++){
	k = i+1
	data_labels[i]='Posto '+k 
	newdataset[i] = {
					data: weeks_data[i],
					label: 'Posto '+k,
					backgroundColor: colors[i],
					borderWidth: 1
					}
	bigchart.data.datasets[i] = newdataset[i]
	bigchart.options.scales.yAxes[0].ticks.suggestedMax = (max +200)
	bigchart.update()
	//console.log(newdataset[i])
			}
}

function selectData(event) {
	let dias = event.options[event.selectedIndex].value
	//console.log(dias)
	selectDataDays(dias)
	update_single_graph(dias)
}

function selectDataDays (dias) {
	for (let i=1; i<=n_postos;i++){
		//console.log(i)
	$.ajax({
		url: '/selectdata',
		data: {
			'dias': dias,
			'posto': i
		},
		success: function (result) {
			//console.log(result)
			$('#posto'+result.posto).empty()
			$('#posto'+result.posto).append("<p>"+result.pecas+" Pc's</p>")
		}
	})
	}
}

function update_single_graph(dias){
	for (let i=1; i<=n_postos; i++){
		get_single_graph_data(i, dias)
	}
}

function test_mes (mes, posto, year, newdataset, dados, x_labels) {
	$.ajax({
		url: '/datamonth',
		data: {
			'mes': mes,
			'posto' : posto,
			'year' : year
		},
		success: function (result) {
			console.log(result)
			newdataset[posto-1].data.push(result.pecas)
			bigchart.data.labels = x_labels
			bigchart.data.datasets[posto-1] = newdataset[posto-1]
			bigchart.update() 
		}
	})
}

function get_test_mes (n_mes) {
	const monthNames = ["January", "February", "March", "April", "May", "June",
  							"July", "August", "September", "October", "November", "December"];
	let d = new Date();
	let m = d.getMonth() +1;
	let y = d.getFullYear();
	let dados = new Array()
	let newdataset = new Array(n_postos)
	let x_labels = []
	for (let i=0; i<n_postos;i++){
		newdataset[i] = {
			data: [],
			label: 'Posto '+(i+1),
			backgroundColor: colors[i],
			borderWidth: 1
		}
	}
	let month =0
	for (let k = n_mes; k>=0; k--){

		if ((m-k)>0) {
			month = m-k
			x_labels.push(monthNames[month-1])
			ano = y
		}
		else if ((m-k)==0) {
			month=12
			x_labels.push(monthNames[month-1])
			ano = y-1
		}
		else if ((m-k)<0) {
			month= 12 +(m-k)
			ano = y-1
			x_labels.push(monthNames[month-1])
		}

		for (let i=1; i<=n_postos; i++){
			test_mes(month, i, ano, newdataset, dados, x_labels)	
		}
	}
}

function selectinfo (event) {
	let sel = event.options[event.selectedIndex].value
	if (sel == 's') {
		SelType=0
		test(1)
	}
	else if (sel == 'm') {
		SelType=1
		get_test_mes(1)
	}
}