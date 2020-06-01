import numpy as np 

N=7
M=np.zeros((N,7),dtype=int)

print(M)

$( function () {
            $('button#btn-json').bind('click' , function () {
                $.getJSON('/background_process', {
                    proglang: $('input[name="proglang"]').val(),
                } , function (data) {
                    $("#result").text(data.result);
                    var dados = data.result; 
                    console.log(dados)
                });
                return false
            });
        });

        <a href="/R_diario">Registo Diario</a>
        <a href="/padrao">Padroes diarios</a>
        <a href="/alvo">Tempo no alvo</a>