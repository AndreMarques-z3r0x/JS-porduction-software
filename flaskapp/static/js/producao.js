var n_turnos=3
var n_postos=6
$(document).ready(function(){
	for(let i=1; i<=n_postos; i++){
		loadData(i)
	}
})

function loadData (posto) {
		$.ajax({
		url: '/backproducao',
		async: false,
		cache: false,
		data: {
			'posto': posto
		},
		success: function (result) {
			//console.log('aaaaaaa')
			//console.log(result)
			maketables(posto)
			let tableData= ''
			let TotalDayPecas = [0,0,0,0,0,0,0]
			let TotalDayTempo = [0,0,0,0,0,0,0]

			for(let i=1; i<=n_turnos; i++){
				tableData += `<tr>
								<td>Turno`+i+`</td>`;
				for(let n=0; n<result.pecas2.length; n++){
					tableData += `<td>`+result['pecas'+i][n]+`</td><td>`+result['tempo'+i][n]+`</td>`;
					if (n<result.pecas2.length -1){
						TotalDayPecas[n] += result['pecas'+i][n]
						TotalDayTempo[n] += result['tempo'+i][n]
					}
				}
				tableData += '</tr>'
			}
			tableData += '<tr><td>Total</td>'
			for (let i=0; i<TotalDayPecas.length; i++){
				tableData += '<td>'+TotalDayPecas[i]+'</td><td>'+TotalDayTempo[i]+'</td>'
			}
			tableData+='</tr>'
			//console.log(tableData)
			$('#Tbody'+posto).append(tableData)
		}
	})	
}

function maketables(posto) {
	let dias = ['Segunda','Terça','Quarta','Quinta','Sexta','Sabado','Domingo','TOTAL']
	let teste = ''
	for (let i=0; i<dias.length; i++){
		teste += '<td colspan=2>'+dias[i]+'</td>'
	}
	let data = `<table class="tableWeek" align="center">
				<tr>
				<th colspan=17>Posto `+posto+`</th>
				</tr>
				<tr>
				<td></td>`+teste;
	data += `</tr>
			<tbody id="Tbody`+posto+`"></tbody>
			</table>`;

	if (posto <= n_postos/2) {
		$('#weektableleft').append(data)
	}
	else {
		$('#weektableright').append(data)
	}	
}