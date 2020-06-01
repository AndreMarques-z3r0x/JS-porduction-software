def get_data():
	sdays = 7
	StartDay = today - timedelta(days=sdays)
	mycursor = mysql.connection.cursor()
	data_pecas = np.zeros((7,4), dtype=int)
	data_tempo = np.zeros((7,4), dtype=int)
	total_pecas_dia = 0
	total_tempo_dia = 0
	total_pecas = []
	total_tempo = []
	contdias=0
	tempo =0
	for posto in range(1,7):
		for turno in range(1,4):
			for k in range(sdays):
				Sel_day = today - timedelta(days=k)
				res=mycursor.execute("Select total_pecas_bloco, tempo_paragem_total, data from data%s where id_turno=%s and data='%s'" % (posto, turno, Sel_day) )
				if (res > 0) :
					myresult = mycursor.fetchall()
					for row in myresult:
						pecas = row[0]
						tempo += row[1]
				else:
					pecas=0
					tempo=0
				total_pecas_dia += pecas
				total_tempo_dia += tempo
				tempo=0
			data_pecas[posto][turno] = total_pecas_dia / sdays
			data_tempo[posto][turno] = total_tempo_dia / sdays
			total_pecas_dia =0
			total_tempo_dia =0	
		pass
	pass
	return data_pecas