var n_turnos=3
$(document).ready(function(){
	$.ajax({
		url: '/backproducao',
		success: function (result) {
			console.log('aaaaaaa')
			console.log(result)
			loadPage()
			let tableData= ''
			for(let i=1; i<=n_turnos; i++){
				tableData += `<tr>
								<td>Turno`+i+`</td>`;
				for(let n=0; n<result.pecas2.length; n++){
					tableData += `<td>`+result['pecas'+i][n]+`</td><td>`+result['tempo'+i][n]+`</td>`;
				}
				tableData += '</tr>'
			}
			console.log(tableData)
			$('#Tbody').append(tableData)
		}
	})
})

function loadPage() {
	$('#weektable').append(`<table class="tableWeek">
							<tr>
							<th>Posto 1</th>
							</tr><tbody id="Tbody"></tbody>
							</table>`)
}