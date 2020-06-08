var n_postos=6

$(document).ready(function (){

	$.ajax({
		url: "/getData",
		data: {
			'posto' : 2,
		},
		success: function (result) {
			console.log(result)
		}
	})

	loadPage()
})

function loadPage () {
	for (let i=1; i<=n_postos;i++){
		$('#t_container').append(`
			<div class="posto" id=posto`+i+`></div>
			`)
		drawTables(i)
	}
}

function drawTables (posto) {
	$('#posto'+posto).append(`
		<table align='center'>
		<tr>
		<th colspan=3 id="titulo`+posto+`"></th>
		</tr>
		<tr>
			<th>Hora</th><th>Nr Peças</th><th>Tempo(min)</th>
		</tr>
		<tbody id="Tbody`+posto+`"></tbody>
		</table>
		`) 
	updateData(posto)
}

function updateData(posto){
	$.ajax({
		url: "/getData",
		data: {
			'posto': posto,
		},
		success: function (result) {
			let rowDados =''
			let hour = new Date()
			let after = false
			for(let hora=0; hora<result.h.length; hora++){
				let h_now = hour.getHours() + `:00:00`
				if (h_now == result.h[hora]) {
					rowDados += '<tr id="dados" class="active"><td id="horas">'+result.h[hora]+'</td><td>'+result.pecas[hora]+'</td><td>'+ Math.floor(result.tempo[hora]/60)+'</td></tr>'
					after = true
				}
				else if (after==false) {
					rowDados += '<tr id="dados"><td id="horas">'+result.h[hora]+'</td><td>'+result.pecas[hora]+'</td><td>'+ Math.floor(result.tempo[hora]/60)+'</td></tr>'
				}
				else{
					rowDados += '<tr id="dados"><td id="horas">'+result.h[hora]+'</td><td>---</td><td>---</td></tr>'
				}
			}
			after = false
			$('#Tbody'+posto).empty();
			$('#Tbody'+posto).append(rowDados)
			$('#titulo'+posto).empty()
			$('#titulo'+posto).append(`Posto `+posto+` (turno `+result.turno+`)`)
		}
	})
}

setInterval(function(){
	for (let i=1; i<n_postos; i++){
		updateData(i)
	}
}, 3000);