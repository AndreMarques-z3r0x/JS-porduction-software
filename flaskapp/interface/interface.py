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

def get_today_data(posto, turnos):
	current_turno, h_inicio, h_fim = get_c_turno(turnos)
	return current_turno['turno']


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
	mycursor = mysql.connection.cursor()
	turnos = infogeral()
	pecas = get_today_data(2, turnos)
	return jsonify({'turno':pecas})

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000, debug=True)
