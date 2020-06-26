var pointer = 0;
var n_postos = 0
var n_turnos = 0

$(document).ready(function () {

	$('#arrow-left').click(function () {
		pointer++
		console.log(pointer)
		ReadyToUpdate()
	});

	$('#arrow-right').click(function () {
		pointer--
		console.log(pointer)
		ReadyToUpdate()
	});

	$.ajax({
		url: "/getconf",

		success: function (info) {
			n_postos = info.postos
			n_turnos = info.turnos
			fillpage();
			ReadyToUpdate();
		}
	})

});

function updateData (i) {
	$.ajax({
			url: "/live",
			data: {
				'day' : pointer,
				'posto': i,
			},
			success: function (result) {
					console.log(result)
					var rowDados = ''
					let t_pecas = 0
					let t_tempo = 0
					for (var k=0; k<n_turnos; k++){
						rowDados += '<tr id="dados"><td id="pecas">'+result.pecas[k]+'</td><td id="tempo">'+result.tempo[k]+'</td><tr>'
						t_pecas += result.pecas[k]
						t_tempo += result.tempo[k]
					}
					$('#Tbody'+i).empty();
					$('#Tbody'+i).append(rowDados)
					$('#date').empty();
					$('#date').append('Data: '+result.dia)
			}
		})
}

function draw_tables (i) {
	$('#large').append(
		`<div id="box1">
		<div id="posto">
		<table  align="center">
			<tr>
			<th colspan="2">Posto`+i +`</th>
			</tr>
			<tr>
				<td id="tit">Pe&ccedil;as</td>
				<td id="tit">Tempo</td>
			</tr>
			<tbody id="Tbody`+i+`"></tbody>
			<tr>
				<td id="pecasTotal" class="tit2"></td>
				<td id="tempoTotal" class="tit2"></td>
			</tr>
		</table>
		</div>
		</div>`
		);
}

function fillpage () {
	$('#large').empty()
	for(var i = 1; i<(n_postos+1); i++){
		draw_tables(i)
	}
}

function ReadyToUpdate () {
	console.log('estou a atualizar os dados')
	for(var i = 1; i<(n_postos+1); i++){
		updateData(i);
	}	
}

setInterval(function(){
	ReadyToUpdate();
}, 3000);