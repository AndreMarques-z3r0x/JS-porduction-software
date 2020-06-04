$(document).ready(function (){

	$.ajax({
		url: "/getData",
		success: function (result) {
			console.log(result)
		}
	})

})