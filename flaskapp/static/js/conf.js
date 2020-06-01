var n_postos = 0

$(document).ready(function () {
	$.ajax({
		url: "/getconf",

		success: function (result) {
			console.log(result.postos)
			n_postos = result.postos
			n_turnos = result.turnos
		}
	})
});

function load_it(){
	console.log(n_postos)
}