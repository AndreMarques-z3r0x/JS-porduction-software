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

def get_current_turno():
	now = datetime.now().strftime("%H:%M:%S")
	for turno in turnos:
		if (now>= turno['inicio'] and now <= turno['fim']):
			current_turno = turno['turno']
	return current_turno


def get_today_data(posto):
	current_turno = get_current_turno()
	mycursor = mysql.connection.cursor()
	res = mycursor.execute("Select nr_pecas_periodo, tempo_paragem_total from data%s where id_turno=%s and data='%s'" % (posto, current_turno,today))
	if res>0:
		myresult= mycursor.fetchall()
		#for row in myresult:#############


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
	get_today_data(posto)
	return 

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000, debug=True)
