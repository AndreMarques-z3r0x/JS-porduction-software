from datetime import date, timedelta, datetime
import numpy as np
from flask import Flask, render_template, redirect, url_for, request, jsonify
from flask_fontawesome import FontAwesome
from flask_mysqldb import MySQL
from random import sample
import re
from calendar import monthrange


today = date.today()
turnos = 0

def infogeral():
	info = []
	mycursor = mysql.connection.cursor()
	res=mycursor.execute("Select * from def_turno")
	if res>0:
		myresult = mycursor.fetchall()
		for row in myresult:
			info.append({'turno':row[0], 'inicio':row[1], 'fim':row[2]})
	else:
		return 'error'
	return info

def get_c_turno(turnos):
	now = str(datetime.now().strftime("%H:%M:%S"))
	time= now.split(':')
	h=int(time[0])
	for turno in turnos:
		h_inicio = int(str(turno['inicio']).split(':')[0])
		h_fim = int(str(turno['fim']).split(':')[0])
		
		if h_fim>h_inicio:
			if h>=h_inicio and h<h_fim:
				return turno, h_inicio, h_fim
		else:
			h_pointer = turno['inicio']
			while True:
				if (int(h_pointer.seconds/3600)==h):
					return turno, h_inicio, h_fim
				elif h_pointer!=turno['fim']:
					h_pointer+=timedelta(hours=1)
				else:
					break
	return 'error'

def get_today_turno(turnos):
	current_turno, h_inicio, h_fim = get_c_turno(turnos)
	return current_turno


def get_today_data(turno,posto):
	hora = turno['inicio']
	t_pecas=[]
	t_tempo = []
	horas_vec = []
	while True:
		h_sql = str(hora).split(', ')[-1]
		if h_sql == str(turno['fim']):
			break;
		mycursor = mysql.connection.cursor()
		res=mycursor.execute("Select nr_pecas_periodo,tempo_paragem_total from data%s where id_turno=%s and data='%s' and hora_inicio_periodo='%s'" % (posto, turno['turno'], today, h_sql ))
		if res>0:
			myresult = mycursor.fetchall()
			for row in myresult:
				pecas = row[0]
				tempo = row[1]
		else:
			pecas=0
			tempo=0
		t_pecas.append(int(pecas))
		t_tempo.append(int(tempo)) 
		horas_vec.append(str(h_sql))
		hora += timedelta(hours=1)
	return t_pecas, t_tempo, horas_vec


app = Flask(__name__)

app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'andre'
app.config['MYSQL_PASSWORD'] = 'andre199921'
app.config['MYSQL_DB'] = 'indmei'

mysql = MySQL(app) 

@app.route("/")
@app.route("/home")
def home():
	mycursor = mysql.connection.cursor()
	turnos = infogeral()
	return render_template('interface.html', data = turnos)

@app.route("/getData")
def getData():
	posto = request.args.get('posto')
	mycursor = mysql.connection.cursor()
	turnos = infogeral()
	c_turno = get_today_turno(turnos)
	pecas, tempo, horas = get_today_data(c_turno, posto)
	return jsonify({'turno':c_turno['turno'], 'pecas':pecas, 'tempo':tempo, 'hora':str(c_turno['inicio']), 'h':horas})

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000, debug=True)
