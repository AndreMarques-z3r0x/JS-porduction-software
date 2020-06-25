var n_postos = 0
var n_turnos = 0

$(document).ready(function () {
	$.ajax({
		url: "/getconf",

		success: function (result) {
			console.log(result.postos)
			n_postos = result.postos
			n_turnos = result.turnos
			$('#n_postos').append(n_postos)
			$('#n_turnos').append(n_turnos)
		}
	})

	get_turno_info()
});

function load_it(){
	console.log(n_postos)
}

function get_turno_info () {
	$.ajax({
		url: "/getconfturno",

		success: function (info) {
			console.log(info)
			for(let i=1; i<n_turnos+1; i++){
				$('#inicio'+i).append(info['inicio_turno'+i])
				$('#fim'+i).append(info['fim_turno'+i])
			}
		}
	})
}

function addTurno(){
	el = document.getElementById('add')
	el.style.display = ''
}