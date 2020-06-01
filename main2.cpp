#include <iostream>
#include <mysql/mysql.h>
#include "aplicativo.h"
#include <mysql/my_global.h>
#include <stdlib.h>
#include <cstdio>
#include <fcntl.h> 
#include <errno.h>
#include <termios.h> 
#include <unistd.h>
#include <thread>
#include <mutex>

#define Max_char 24
#define BaudRate B9600 
using namespace std; 

mutex mysql_mutex;
thread hThread [6];
char read_buffer [Max_char];

class serial{
    public:
         string Sport;
    serial( string aport){
        Sport = aport;
    }
    int com_conection (){
        
        int serial_port = open(Sport.c_str(), O_RDWR);
        if (serial_port<0) 
            printf("Error %i from open: %s\n",errno, strerror(errno)); //check for errors
            else 
                cout << "open serial port complete, file descritor is: " << serial_port << endl; 

    return serial_port;
    }
    int read_data(int serial_port){
        int n_bytes = read(serial_port, &read_buffer, sizeof(read_buffer));
        if (n_bytes < 0) {   
           printf("Error reading: %s", strerror(errno));
           exit(1);
        }
        return n_bytes;   
    }
        int conf_conection(int serial_port){

                struct termios tty;
                memset(&tty, 0, sizeof tty); //reserve enough memory

                if ( tcgetattr(serial_port, &tty) !=0 ){
                printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));  //check for existing errors in current settings
                }
                //_________________________________________Configurations________________________________________________________________________________
                tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
                tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
                tty.c_cflag |= CS8; // 8 bits per byte (most common)
                tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
                tty.c_lflag &= ~ICANON;
                tty.c_lflag &= ~ECHO; // Disable echo
                tty.c_lflag &= ~ECHOE; // Disable erasure
                tty.c_lflag &= ~ECHONL; // Disable new-line echo
                tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
                tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
                tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
                tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
                tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
                tty.c_cc[VMIN] = 18;
                tty.c_cc[VTIME] =10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
                // Set in/out baud rate to be 9600
                cfsetispeed(&tty, BaudRate);
                cfsetospeed(&tty, BaudRate);

                if (tcsetattr(serial_port,TCSANOW,&tty) !=0) {
                printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
                }

                tcflush(serial_port, TCIFLUSH);   // Discards old data in the rx buffer

         }
};


int main()
{
	int i = 0, posto[6] = { 0 };
	s_login d_login = { 0 };

	printf("\nIndmeibyCRC v1.1/Jan2011\n\n");
	//mysql_mutex = CreateMutex( 0, FALSE, 0);
	
	ReadConfigData(&d_login);
	connectMySQL(&d_login);
	
	for(i = 0; i < 6; i++)
	{
		*(posto+i) = i;
		//*(hThread+i) = CreateThread( NULL, 0, funcao, &posto[i], 0, NULL);	
        hThread[i] = thread(funcao, &posto[i]);
    } 
	connectComPort2(/*&d_login*/); 
	mysql_close(&mysql);

    for (auto& th : hThread) th.join();

	return 0;
}

void* funcao(void* arg)
{
	int i = *((int*)arg);
	verificar(i);
	return 0;
}

void preencher(int posto_id)
{
	char inicio_periodo[TMP_LENGTH] = { 0 }; 
	char data[TMP_LENGTH] = { 0 };
	char hora_actual[TMP_LENGTH] = { 0 };
	char query[QUERY_LENGTH]= { 0 };

	int f = 0, tempo_paragem_total = 0, H = 0, M = 0, S = 0;
	int nr_tabelas = 0, nr_periodos = 0, nr_pecas_periodo = 0, total_pecas_bloco = 0;
	int id_turno = 0, id_registo = 0;
	long duracao_t = 0, resto_t = 0, diferenca = 0;
	time_t hora_pc = 0;
	time_t hora_inicio_turno = 0, hora_fim_turno = 0;
	time_t limite_inferior = 0, limite_superior = 0, hora_tmp = 0;
	struct tm *ptrtm = NULL;

	s_controlo d_controlo[6] = { 0 };
	s_turno d_turno[24] = { 0 };
	s_data data_info[100] = { 0 };
	sptr ptr = { 0 };

	time(&hora_pc);
	
	ptr.nr_controlo = ObterDadosControlo(d_controlo);
	ptr.nr_turno    = ObterDadosTurno(d_turno,hora_pc);
	ptr.nr_data     = ObterData(data_info,hora_pc,posto_id);
	ptr.controlo    = d_controlo;
	ptr.data        = data_info;
	ptr.turno       = d_turno;
	
	id_turno          = ObterIdTurno(hora_pc);								//RETORNA O ID DO TURNO EM QUE PERTENCE A HORA ACTUAL
	hora_inicio_turno = mktime(&d_turno[id_turno-1].hora_inicio); //CONVERTE A HORA DO INICIO DO TURNO EM QUE ESTAMOS EM SEGUNDOS E COPIA PARA VAR.
	hora_fim_turno    = mktime(&d_turno[id_turno-1].hora_fim);	
	duracao_t         = duracao_turno(hora_pc);							//RETORNA A DURA��O DO TURNO EM QUE ESTAMOS
	nr_periodos       = duracao_t/(d_controlo[posto_id].duracao_periodo*60);		//CALCULA O NR DE PERIODOS
	resto_t           = duracao_t%(d_controlo[posto_id].duracao_periodo*60);			//CALCULA O RESTO

	for(f = 0; f < nr_periodos; f++)
	{									//VERIFICA EM QUE PERIODO ESTA SITUADO
		limite_inferior = hora_inicio_turno+f*d_controlo[posto_id].duracao_periodo*60;
		limite_superior = hora_inicio_turno+(f+1)*d_controlo[posto_id].duracao_periodo*60;

		( (limite_inferior <= hora_pc) && (hora_pc<limite_superior) ) ? hora_tmp = limite_inferior : 0;						//GUARDA INICIO DO PERIODO NA VAR. HORA_TMP
	}

	( (resto_t != 0) && ((hora_fim_turno-resto_t) <= hora_pc) && (hora_pc < hora_fim_turno) ) ? hora_tmp = hora_fim_turno-resto_t: 0;

	id_registo = ObterIdRegisto(&ptr,hora_pc, posto_id);	//RETORNA O INDICE DO VECTOR ONDE CONTEM A HORA INICIO DO PERIODO

	ptrtm = localtime(&hora_tmp);					//CONVERTE PARA STRUCT TM A HORA DO INICIO DO PERIODO
	sprintf( inicio_periodo, "%i:%i:%i", ptrtm->tm_hour, ptrtm->tm_min, ptrtm->tm_sec );	//CONVERTE PARA STRING A HORA DO INICIO DO PERIODO
	sprintf( data, "%i-%i-%i", ptrtm->tm_year+1900, ptrtm->tm_mon+1, ptrtm->tm_mday );	//CONVERTE PARA STRING A DATA DO PC 
	ptrtm = localtime(&hora_pc);					//CONVERTE A HORA DO PC EM STRUCT TM
	
	sprintf( hora_actual, "%i:%i:%i", ptrtm->tm_hour, ptrtm->tm_min, ptrtm->tm_sec);		//CONVERTE PARA STRING A DATA ACTUAL 
	
	nr_pecas_periodo = ptr.data[id_registo].nr_pecas_periodo + 1;	//SE A ESTA FUN��O FOI CHAMADA QUER DIZER QUE HOUVE PRODU��O DE UMA PE�A
	total_pecas_bloco = ptr.data[id_registo].total_pecas_bloco + 1;	//LOGO ACRESCENTA UMA PE�A NO TOTAL DO PERIODO E NO TOTAL DO BLOCO
	
	diferenca = difftime( hora_pc, mktime( &ptr.data[id_registo].hora_ultima_peca ) );
	( diferenca > 86400 ) ? diferenca -= 86400 : 0;
	( diferenca > d_controlo[posto_id].ref_cont_tempo_paragem )
		? tempo_paragem_total = ptr.data[id_registo].tempo_paragem_total + diferenca - d_controlo[posto_id].ref_cont_tempo_paragem 
		: tempo_paragem_total = ptr.data[id_registo].tempo_paragem_total;
			
	H = ptr.data[id_registo].hora_primeira_peca.tm_hour; 
	M = ptr.data[id_registo].hora_primeira_peca.tm_min;
	S = ptr.data[id_registo].hora_primeira_peca.tm_sec;
	
	( (H == 0) && (M == 0) && (S == 0) ) // SE HORA DA PRIMEIRA PE�A FOR 00:00:00 ACTUALIZA ESSE CAMPO
		? sprintf(query,
		"UPDATE %s SET nr_pecas_periodo=%i, total_pecas_bloco=%i, tempo_paragem_total=%i, hora_primeira_peca='%s',hora_ultima_peca='%s' WHERE hora_inicio_periodo='%s' AND data='%s'",
		d_controlo[posto_id].data_info, nr_pecas_periodo, total_pecas_bloco, tempo_paragem_total, hora_actual,hora_actual, inicio_periodo, data)
		
		: sprintf(query,
		"UPDATE %s SET nr_pecas_periodo=%i,total_pecas_bloco=%i,tempo_paragem_total=%i,hora_ultima_peca='%s' WHERE hora_inicio_periodo='%s' AND data='%s'",
		d_controlo[posto_id].data_info, nr_pecas_periodo, total_pecas_bloco, tempo_paragem_total, hora_actual, inicio_periodo, data);
	
	//WaitForSingleObject( mysql_mutex, INFINITE ); 
	mysql_mutex.lock();
    mysql_query( &mysql, query );		
	mysql_mutex.unlock();
    //ReleaseMutex( mysql_mutex );					
}

void verificar(int posto_id)
{
	char tmp[TMP_LENGTH] = { 0 };
	char query[QUERY_LENGTH] = { 0 };
	char data[TMP_LENGTH] = { 0 };
	char h_u_peca[TMP_LENGTH] = { 0 };
	char data_periodo[TMP_LENGTH] = { 0 };

	int id_turno = 0, id_registo = 0, id_bloco = 0, nr_bloco = 0, nr_periodos = 0, nr_tabelas = 0;
	int f = 0, total = 0, i = 5000;
	double diferenca = 0;
	long duracao_t = 0;
	time_t hora_inicio_turno = 0, hora_fim_turno = 0;
	time_t limite_inferior = 0, limite_superior = 0, hora_tmp = 0;
	struct tm *ptrtm = NULL;

	s_controlo d_controlo[6] = { 0 };
	s_data data_info[100] = { 0 };
	s_turno d_turno[24] = { 0 };
	time_t hora_pc = 0;
	sptr ptr = { 0 };

	while(1){	 			
		time(&hora_pc);

		ptr.nr_controlo = ObterDadosControlo(d_controlo);
		ptr.nr_turno    = ObterDadosTurno(d_turno, hora_pc);
		ptr.nr_data     = ObterData(data_info, hora_pc, posto_id);
		ptr.controlo    = d_controlo;
		ptr.data        = data_info;
		ptr.turno       = d_turno;
		id_turno        = ObterIdTurno(hora_pc); 

		id_registo = ObterIdRegisto(&ptr, hora_pc, posto_id); //RETORNA O INDICE DO REGISTO
		
		hora_inicio_turno = mktime(&ptr.turno[id_turno-1].hora_inicio);
		if(id_turno != 0){
			
			if(id_registo >= 0)
			{
				hora_fim_turno = mktime( &ptr.turno[id_turno-1].hora_fim );
				if( (hora_fim_turno - resto[posto_id] <= hora_pc) && (hora_pc<hora_fim_turno) )
				{
					limite_inferior = hora_fim_turno-1;
					limite_superior = hora_fim_turno;
					(limite_inferior - hora_pc > 0) ? i = (limite_inferior-hora_pc) * 1000 : i = 500;
				}
				else
				{
					limite_inferior = mktime( &ptr.data[id_registo].hora_inicio_periodo ) + ( 60 * d_controlo[posto_id].duracao_periodo ) - 1;
					limite_superior = mktime( &ptr.data[id_registo].hora_inicio_periodo ) + ( 60 * d_controlo[posto_id].duracao_periodo );
					(limite_inferior - hora_pc > 0) ? i = (limite_inferior-hora_pc) * 1000 : i = 500;
				}
				if( (limite_inferior <= hora_pc) && (hora_pc < limite_superior) )
				{
					diferenca = difftime( hora_pc, mktime( &ptr.data[id_registo].hora_ultima_peca) );
					(diferenca > 86400) ? diferenca -= 86400 : 0;
					
					if(diferenca>d_controlo[posto_id].ref_cont_tempo_paragem)
					{
						total = ptr.data[id_registo].tempo_paragem_total + (diferenca - d_controlo[posto_id].ref_cont_tempo_paragem);
						ptrtm = localtime(&hora_pc);
						sprintf( h_u_peca, "%i:%i:%i", ptrtm->tm_hour, ptrtm->tm_min, ptrtm->tm_sec);
						sprintf( data_periodo, "%i-%i-%i", ptr.data[id_registo].data.tm_year+1900, ptr.data[id_registo].data.tm_mon+1, ptr.data[id_registo].data.tm_mday);						
						sprintf( tmp, "%i:%i:%i", ptr.data[id_registo].hora_inicio_periodo.tm_hour, ptr.data[id_registo].hora_inicio_periodo.tm_min, ptr.data[id_registo].hora_inicio_periodo.tm_sec); //CONVERTE PARA STRING A HORA DO INICIO DO PERIODO 
						sprintf( query, "UPDATE %s SET tempo_paragem_total=%i, hora_ultima_peca='%s' WHERE hora_inicio_periodo='%s' AND data='%s'", d_controlo[posto_id].data_info, total, h_u_peca, tmp, data_periodo);
						//WaitForSingleObject( mysql_mutex, INFINITE ); 
						mysql_mutex.lock();
                        mysql_query( &mysql, query );		
						mysql_mutex.unlock();
                        //ReleaseMutex( mysql_mutex );													
					}
				}	
			}
			else
			{
				nr_bloco        = d_controlo[posto_id].duracao_bloco;
				id_bloco        = ObterIdBloco( &ptr, hora_pc, nr_bloco);
				duracao_t       = duracao_turno(hora_pc);
				nr_periodos     = duracao_t / (d_controlo[posto_id].duracao_periodo * 60);
				resto[posto_id] = duracao_t % (d_controlo[posto_id].duracao_periodo * 60);
				hora_fim_turno  = mktime( &d_turno[id_turno-1].hora_fim);
				for(f = 0; f < nr_periodos; f++)
				{
					limite_inferior = hora_inicio_turno + f * d_controlo[posto_id].duracao_periodo * 60;
					limite_superior = hora_inicio_turno + (f + 1) * d_controlo[posto_id].duracao_periodo * 60;
					( (limite_inferior <= hora_pc) && (hora_pc < limite_superior) ) ? hora_tmp = limite_inferior : 0;				
				}

				( (resto[posto_id] != 0) && ((hora_fim_turno - resto[posto_id]) <= hora_pc) && (hora_pc < hora_fim_turno) ) ? hora_tmp = hora_fim_turno - resto[posto_id] : 0;

				total = total_pecas( id_bloco, &ptr, hora_pc );		
				ptrtm = localtime( &hora_tmp );
				sprintf( tmp, "%i:%i:%i", ptrtm->tm_hour, ptrtm->tm_min, ptrtm->tm_sec);
				sprintf( data, "%i-%i-%i", ptrtm->tm_year+1900, ptrtm->tm_mon+1, ptrtm->tm_mday);
				sprintf( query, "INSERT INTO %s VALUES('%i','%i','0','%i','0','%s','00:00:00','%s','%s')", d_controlo[posto_id].data_info, id_turno, id_bloco, total, tmp, tmp, data);
				
				//WaitForSingleObject( mysql_mutex, INFINITE );
				mysql_mutex.lock();
                mysql_query( &mysql, query );		
				mysql_mutex.unlock();
                //ReleaseMutex(mysql_mutex);					
			}	
		}
		else
		{
			i=5000;
		}
		//Slepp(i);
        usleep(i*1000);

	}
}

int total_pecas(int id_bloco,sptr *ptr,time_t hora_pc)
{
	int i = 0, id_turno = 0, total = 0;
	long duracao_t = 0;
	time_t inicio_periodo = 0;

	id_turno  = ObterIdTurno(hora_pc);
	duracao_t = duracao_turno(hora_pc);
	for( i = 0; i < ptr->nr_data; i++ )
	{
		inicio_periodo = mktime( &ptr->data[i].hora_inicio_periodo );
		( (id_bloco == ptr->data[i].id_bloco) && (id_turno == ptr->data[i].id_turno) && ((hora_pc - inicio_periodo) < duracao_t) ) ? total += ptr->data[i].nr_pecas_periodo : 0;
	}
	return(total);
}

long duracao_turno(time_t hora_pc)
{
	int id_turno = 0, nr_turno = 0, i = 0;
	s_turno d_turno[24] = { 0 };
	time_t inicio_turno = 0, fim_turno = 0;
	long duracao_turno = 0;

	nr_turno = ObterDadosTurno( d_turno, hora_pc);
	id_turno = ObterIdTurno( hora_pc);
	for( i = 0; i < nr_turno; i++)
	{
		if( id_turno == d_turno[i].id_turno)
		{
			inicio_turno  = mktime(&d_turno[i].hora_inicio);
			fim_turno     = mktime(&d_turno[i].hora_fim);
			duracao_turno = fim_turno - inicio_turno;
		}
	}
	return(duracao_turno);
}

int ObterIdBloco(sptr *ptr, time_t hora_pc, int nr_bloco)
{
	int i=0,id_bloco=0, id_turno = 0, j = 0;
	long duracao_turno = 0;
	time_t inicio_periodo=0, inicio_turno = 0, fim_turno = 0;
	
	(ptr->nr_data > 0) ? id_bloco = ptr->data[ptr->nr_data-1].id_bloco : id_bloco = 1;
	
	id_turno = ObterIdTurno(hora_pc);
	for( i = 0; i < ptr->nr_turno; i++)
	{
		if(id_turno == ptr->turno[i].id_turno){
			inicio_turno  = mktime(&ptr->turno[i].hora_inicio);
			fim_turno     = mktime(&ptr->turno[i].hora_fim);
			duracao_turno = fim_turno - inicio_turno;
		}
	}	
	for( i = 0; i < ptr->nr_data; i++)
	{
		inicio_periodo = mktime(&ptr->data[i].hora_inicio_periodo);
		( (id_bloco == ptr->data[i].id_bloco) && ((hora_pc-inicio_periodo) < duracao_turno) )? j++: 0;
		( (hora_pc - inicio_periodo) > duracao_turno)? id_bloco = 1 : 0;
	}
	if( j == nr_bloco)
	{
		return(id_bloco+1);
	}
	else
	{
		return(id_bloco);
	}
}

int ObterIdRegisto(sptr *ptr, time_t hora_pc, int posto_id)
{
	int i = 0, id_turno = 0;
	time_t hora_inicio_periodo = 0, hora_fim_periodo = 0, hora_fim_turno = 0, tmp =0;
	
	id_turno = ObterIdTurno(hora_pc);
	while(id_turno != ptr->turno[i].id_turno)
	{  
		i++; //obter o indice do turno
	}

	hora_fim_turno = mktime(&ptr->turno[i].hora_fim);
	for( i = 0; i < ptr->nr_data; i++)
	{	
		hora_inicio_periodo = mktime(&ptr->data[i].hora_inicio_periodo);
		tmp = hora_inicio_periodo + ptr->controlo[posto_id].duracao_periodo * 60;
		if( (hora_inicio_periodo <= hora_pc) && (hora_pc < tmp) && (tmp <= hora_fim_turno) )
		{
			return (i);
		}
		if( (hora_inicio_periodo <= hora_pc) && (hora_pc < hora_fim_turno) && (tmp>hora_fim_turno) )
		{
			return(i);
		}
	}
	return (-1);
}

int ObterData(s_data *data_info, time_t hora_pc, int posto_id)
{
	char tmp[TMP_LENGTH] = { 0 };
	char tmp1[TMP_LENGTH]= { 0 };
	char query[QUERY_LENGTH]= { 0 };

	int id_turno = 0, nr_registos_controlo=0, data_hora_tmp[] = {0,0,0};
	u_int coluna = 0, linha = 0;
	u_long *lengths = NULL;
	my_ulonglong nr_registos = 0;
	time_t rawtime = 0;
	struct tm *ptr = NULL;
	MYSQL_RES *result_set = NULL;
	MYSQL_ROW row = { 0 };
	s_controlo d_controlo[6] = { 0 };

	nr_registos_controlo = ObterDadosControlo(d_controlo);
	id_turno = ObterIdTurno(hora_pc);
	ptr = localtime(&hora_pc);
	sprintf( tmp, "%i-%i-%i", ptr->tm_year+1900, ptr->tm_mon+1, ptr->tm_mday);
	sprintf( tmp1, "%i-%i-%i", ptr->tm_year+1900, ptr->tm_mon+1, ptr->tm_mday-1);
	sprintf( query, "SELECT * FROM %s WHERE id_turno=%i AND (data='%s' OR data='%s')", d_controlo[posto_id].data_info, id_turno,tmp,tmp1);

	//WaitForSingleObject(mysql_mutex, INFINITE); 
	mysql_mutex.lock();
    if(mysql_query(&mysql,query))
	{												
		printf("Erro na query da %s.\n",d_controlo[posto_id].data_info);
	}
	else if( result_set = mysql_store_result(&mysql) )
	{ 				
			nr_registos = mysql_num_rows(result_set);							
	}
	mysql_mutex.unlock();
    //ReleaseMutex(mysql_mutex);	

	while ( (row = mysql_fetch_row(result_set)) )
	{
   		lengths = mysql_fetch_lengths(result_set);
		for(coluna = 0; coluna < 9; coluna++)
		{
			if( coluna == 0 )
			{  //id_turno
				sprintf(tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				data_info[linha].id_turno = atoi(tmp);
				memset(tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 1)
			{	//id_bloco
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				data_info[linha].id_bloco = atoi(tmp);
				memset( tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 2)
			{	//nr_peca_periodo
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				data_info[linha].nr_pecas_periodo = atoi(tmp);
				memset( tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 3)
			{	//total_pecas_bloco
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				data_info[linha].total_pecas_bloco = atoi(tmp);	
				memset( tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 4)
			{	//tempo_paragem_total
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				data_info[linha].tempo_paragem_total = atoi(tmp);
				memset( tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 5)
			{	//hora_inicio_periodo
				memcpy( &data_info[linha].hora_inicio_periodo, ptr, sizeof(struct tm)); 
				//preenche a hora na estrutura
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				strconvert(data_hora_tmp, tmp, ":");
				data_info[linha].hora_inicio_periodo.tm_hour = data_hora_tmp[0];
				data_info[linha].hora_inicio_periodo.tm_min = data_hora_tmp[1]; 
				data_info[linha].hora_inicio_periodo.tm_sec = data_hora_tmp[2];	
				//preenche a data na estrutura
				sprintf( tmp, "%.*s",(int) lengths[8], row[8] ? row[8] : "NULL");
				strconvert(data_hora_tmp, tmp, "-");
				data_info[linha].hora_inicio_periodo.tm_year = data_hora_tmp[0]-1900;
				data_info[linha].hora_inicio_periodo.tm_mon = data_hora_tmp[1]-1;
				data_info[linha].hora_inicio_periodo.tm_mday = data_hora_tmp[2];
				memset( tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 6)
			{	//hora_primeira_peca
				memcpy( &data_info[linha].hora_primeira_peca, ptr, sizeof(struct tm) );
				//preenche a hora na estrutura
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				strconvert(data_hora_tmp, tmp, ":");				
				data_info[linha].hora_primeira_peca.tm_hour = data_hora_tmp[0];
				data_info[linha].hora_primeira_peca.tm_min = data_hora_tmp[1];
				data_info[linha].hora_primeira_peca.tm_sec = data_hora_tmp[2];	
				//preenche a data na estrutura
				sprintf( tmp, "%.*s",(int) lengths[8], row[8] ? row[8] : "NULL");
				strconvert( data_hora_tmp, tmp, "-");
				data_info[linha].hora_primeira_peca.tm_year = data_hora_tmp[0]-1900;
				data_info[linha].hora_primeira_peca.tm_mon = data_hora_tmp[1]-1;
				data_info[linha].hora_primeira_peca.tm_mday = data_hora_tmp[2];
				memset( tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 7)
			{	//hora_ultima_peca
				memcpy( &data_info[linha].hora_ultima_peca, ptr, sizeof(struct tm));
				//preenche a hora na estrutura
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				strconvert(data_hora_tmp, tmp, ":");				
				data_info[linha].hora_ultima_peca.tm_hour = data_hora_tmp[0];
				data_info[linha].hora_ultima_peca.tm_min = data_hora_tmp[1];
				data_info[linha].hora_ultima_peca.tm_sec = data_hora_tmp[2];
				
				if(ptr->tm_hour < data_hora_tmp[0])
				{
					sprintf( tmp, "%.*s",(int) lengths[8], row[8] ? row[8] : "NULL");
					strconvert(data_hora_tmp, tmp, "-");
					data_info[linha].hora_ultima_peca.tm_year = data_hora_tmp[0]-1900;
					data_info[linha].hora_ultima_peca.tm_mon = data_hora_tmp[1]-1;
					data_info[linha].hora_ultima_peca.tm_mday = data_hora_tmp[2];
				}
				memset( tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 8)
			{	//data
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				strconvert(data_hora_tmp, tmp, "-");
				data_info[linha].data.tm_year = data_hora_tmp[0]-1900;
				data_info[linha].data.tm_mon = data_hora_tmp[1]-1;
				data_info[linha].data.tm_mday = data_hora_tmp[2];
				memset( tmp, '\0', TMP_LENGTH);
			}
		}
		linha++;
	}
	mysql_free_result(result_set);
	return ((int)nr_registos);
}

int ObterIdTurno(time_t hora_pc)
{
	s_turno turnos[24] = { 0 };	
	int i = 0, nr_registos = 0;
	time_t hora_inicio=0, hora_fim=0;

	nr_registos = ObterDadosTurno(turnos,hora_pc);
	for( i = 0; i < nr_registos; i++)
	{
		hora_inicio = mktime(&turnos[i].hora_inicio);
		hora_fim = mktime(&turnos[i].hora_fim);
		if( hora_inicio <= hora_pc && hora_pc < hora_fim)
		{
			return(turnos[i].id_turno);
		}
	}
	return(0);
}

int ObterDadosTurno(s_turno *def_turno, time_t hora_pc)
{
	time_t time_tmp = 0, hora_inicio = 0, hora_fim = 0;
	struct tm *ptr = NULL;
	u_int coluna = 0, linha = 0; 
	int hora_tmp[] = {0,0,0};
	u_long *lengths = NULL;
	u_long diferenca = 0;
	my_ulonglong nr_registos = 0;
	char tmp[TMP_LENGTH] = { 0 };
	char query[QUERY_LENGTH] = { 0 };
	MYSQL_RES *result_set = NULL;
	MYSQL_ROW row = { 0 };

	
	//WaitForSingleObject(mysql_mutex, INFINITE); 
	mysql_mutex.lock();
	sprintf( query, "SELECT * FROM def_turno");
	if(mysql_query(&mysql,query)){												
			printf("Erro na query do def_turno.\n");
	}
	else{																									
		if(result_set = mysql_store_result(&mysql)){
			nr_registos = mysql_num_rows(result_set);
		}
	}
	mysql_mutex.unlock();
	//ReleaseMutex(mysql_mutex);	

	while ( (row = mysql_fetch_row(result_set)) )
	{
   		lengths = mysql_fetch_lengths(result_set);
		for(coluna = 0; coluna < 3; coluna++){
			if(coluna == 0)
			{
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");
				def_turno[linha].id_turno = atoi(tmp);
				memset(tmp,'\0',TMP_LENGTH);
			}
			if(coluna == 1)
			{
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");	
				strconvert(hora_tmp, tmp, ":");  //CONVERTE A HORA DO INICIO LIDO BD EM STRING PARA INT
				ptr = localtime(&hora_pc);	   //CONVERTE A HORA E DATA EM SEGUNDOS PARA STRUCT TM					
				memcpy( &def_turno[linha].hora_inicio, ptr, sizeof(struct tm)); //COPIA A DATA E HORA DO PTR PARA HORA_INICIO DO TURNO
				def_turno[linha].hora_inicio.tm_hour = *(hora_tmp+0);		//SUBSTITUI A HORA DO HORA_INICIO DO TURNO PELA HORA LIDO DA BD
				def_turno[linha].hora_inicio.tm_min = *(hora_tmp+1);	    //OU SEJA, HORA_INICIO DO TURNO FICA COM A
				def_turno[linha].hora_inicio.tm_sec = *(hora_tmp+2);		//HORA LIDO NA BASE DE DADOS E DATA DO PROPRIO DIA
				hora_inicio = mktime(&def_turno[linha].hora_inicio);		//CONVERTE HORA_INICIO EM SEGUNDOS E GUARDA NA VAR. hora_inicio	
				memset( tmp, '\0', TMP_LENGTH);
			}
			if( coluna == 2)
			{
				sprintf( tmp, "%.*s",(int) lengths[coluna], row[coluna] ? row[coluna] : "NULL");	
				strconvert(hora_tmp, tmp, ":");	//CONVERTE A HORA DO FIM LIDO NA BD EM STRING PARA INT															
				ptr->tm_hour = *(hora_tmp+0);		//COPIA PARA STRUCT TM A HORA LIDO NA BD
				ptr->tm_min = *(hora_tmp+1);		//MANTENDO A DATA DO PROPRIO DIA
				ptr->tm_sec = *(hora_tmp+2);				
				hora_fim = mktime(ptr);			//CONVERTE A STRUCT TM PARA SEGUNDOS E GUARDA NA VAR. hora_fim
				if(hora_inicio > hora_fim)
				{
					if( hora_pc > hora_inicio ) 
					{
						diferenca = hora_fim - (hora_inicio - 86400);
						hora_inicio += diferenca;
						ptr = localtime(&hora_inicio);
					}
					if(hora_pc<hora_fim)
					{
						hora_inicio -= 86400;
						ptr = localtime(&hora_inicio);
						memcpy( &def_turno[linha].hora_inicio, ptr, sizeof(struct tm));
						ptr = localtime(&hora_fim);
					}
					
				}	
				memcpy( &def_turno[linha].hora_fim, ptr, sizeof(struct tm));	
			}
		}
		linha++;
	}
	mysql_free_result(result_set);
	return((int)nr_registos);
}

void strconvert(int *tmp, char *str, const char *c)
{
	int i = 0;
	char *split = NULL;
	split = strtok( str, c);
	while( split != NULL)
	{
		tmp[i] = atoi(split);
		split = strtok(NULL,c);
		i++;
	}          
}

int ObterDadosControlo(s_controlo *def_controlo)
{
	u_int coluna = 0, linha = 0;
	u_long *lengths = NULL;
	char tmp[TMP_LENGTH] = { 0 };
	char query[QUERY_LENGTH] = { 0 };
	MYSQL_RES *result_set = NULL;
	MYSQL_ROW row = { 0 };
	my_ulonglong nr_registos = 0;
	
	
	//WaitForSingleObject(mysql_mutex, INFINITE); 
	mysql_mutex.lock();
	sprintf( query, "SELECT * FROM controlo");
	if( mysql_query(&mysql,query) )
	{												
		printf("Obter dados controlo - Erro na query: controlo\n");
	}
	else
	{ 
		if( result_set=mysql_store_result(&mysql) )
		{
			nr_registos=mysql_num_rows(result_set);
		}
	}
	mysql_mutex.unlock();
	//ReleaseMutex(mysql_mutex);	

	while ( (row = mysql_fetch_row(result_set)) )
	{
		lengths = mysql_fetch_lengths(result_set);
		for(coluna = 0; coluna < 6; coluna++){
			if( coluna == 0)
			{
				sprintf(def_controlo[linha].descricao,"%.*s",(int) lengths[coluna], row[coluna] ? row[coluna]:"NULL");			
			}
			if( coluna == 1)
			{
				sprintf(def_controlo[linha].data_info,"%.*s",(int) lengths[coluna],row[coluna] ? row[coluna]:"NULL");			
			}
			if( coluna == 2)
			{
				sprintf(tmp,"%.*s",(int) lengths[coluna], row[coluna] ? row[coluna]:"NULL");	
				def_controlo[linha].duracao_periodo = atoi(tmp);
				memset(tmp,'\0',TMP_LENGTH);
			}
			if( coluna == 3)
			{
				sprintf(tmp,"%.*s",(int) lengths[coluna], row[coluna] ? row[coluna]:"NULL");	
				def_controlo[linha].duracao_bloco = atoi(tmp);	
				memset(tmp,'\0',TMP_LENGTH);
			}
			if( coluna == 4)
			{
				sprintf(tmp,"%.*s",(int) lengths[coluna], row[coluna] ? row[coluna]:"NULL");	
				def_controlo[linha].ref_cont_tempo_paragem = atoi(tmp);
				memset(tmp,'\0',TMP_LENGTH);
			}
			if( coluna == 5)
			{
				sprintf(tmp,"%.*s",(int) lengths[coluna], row[coluna] ? row[coluna]:"NULL");	
				def_controlo[linha].ref_tempo_paragem = atoi(tmp);
				memset(tmp,'\0',TMP_LENGTH);
			}
			if( coluna == 6)
			{
				sprintf(tmp,"%.*s",(int) lengths[coluna], row[coluna] ? row[coluna]:"NULL");	
				def_controlo[linha].ref_producao_periodo = atoi(tmp);
				memset(tmp,'\0',TMP_LENGTH);
			}
		}
		linha++;
	}
	mysql_free_result(result_set);
	return ((int)nr_registos);
}

void connectMySQL(s_login *d_login)
{
	my_bool reconnect = 1;
	
	mysql_init(&mysql);									  //INICIALIZAR A ESTRUTURA QUE SERVE PARA CONEX�O A BASE DE DADOS
	mysql_options(&mysql,MYSQL_OPT_RECONNECT,&reconnect); //FUN��O PARA ACTIVAR AS OP��ES DE CONEX�O,NESSE CASO SERVE RECONECTAR CASO CAIA A LIGA��O

	if(!mysql_real_connect(&mysql,d_login->server,d_login->user,d_login->pass,d_login->base_dados,0,NULL,CLIENT_REMEMBER_OPTIONS))
	{
		fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql)); 
	}
}

void ReadConfigData(s_login *d_login)
{
	FILE *fp;
	char array[255];
	char *pt;
	
	fp=fopen("IndmeibyCRC.conf","rt");
	if(fp==NULL)
		{
			printf("Erro ao abrir o ficheiro: 'IndmeibyCRC.conf' !\n\nPress Enter to continue ...");
		getchar();
		exit(0);
		}
	
	do
	{
	if(fgets(array,255,fp)!=NULL)
	{
		
	if( NULL != (pt=strstr(array,"ComPort: ")))
		{
		pt+=strlen("ComPort: ");
		sscanf(pt, "%d" ,&d_login->ComPort );
		printf("ComPort: %d\n",d_login->ComPort);
		}
	
	if( NULL != (pt=strstr(array,"Server: ")))
		{
		pt+=strlen("Server: ");
		sscanf(pt, "%s" ,&d_login->server );
		printf("Server: %s\n",d_login->server);
		}

	if( NULL != (pt=strstr(array,"User: ")))
		{
		pt+=strlen("User: ");
		sscanf(pt, "%s" ,&d_login->user );
		printf("user: %s\n",d_login->user);
		}
	
	if( NULL != (pt=strstr(array,"Passwd: ")))
		{
		pt+=strlen("Passwd: ");
		sscanf(pt, "%s" , &d_login->pass);
		printf("Passwd: %s\n",d_login->pass);
		}
	
	if( NULL != (pt=strstr(array,"DataBase: ")))
		{
		pt+=strlen("DataBase: ");
		sscanf(pt, "%s" , &d_login->base_dados);
		printf("DataBase: %s\n\n",d_login->base_dados);
		}
	}
	
	}while(!feof(fp));


}

void login(s_login *d_login)
{
	int i=0;
	d_login->ComPort=2;
	strcpy(d_login->server,"localhost");
	strcpy(d_login->user,"root");
	strcpy(d_login->pass,"root");	
	strcpy(d_login->base_dados,"indmei");
}

void connectComPort2()
{
	 memset(&read_buffer,'\0',sizeof(read_buffer));
    serial com1("/dev/ttyUSB0");
    int serial_port = com1.com_conection();
    com1.conf_conection(serial_port);
    while (true)
    {     
        memset(&read_buffer,'\0',sizeof(read_buffer));  //alocate memory to read buffer
        int n_bytes = com1.read_data(serial_port);
        //cout <<"Read "<<n_bytes<<" bytes: \n"<< read_buffer;
        //cout << sizeof(read_buffer) <<endl;
		//cout << "experiment: \n";
		if(n_bytes<0)
		{
			cout << "erro na leituta dos dados";
		}
			else 
			{			
				for (int k = 0; k<(n_bytes); k++)
				{	
					if (read_buffer[k]==49)
						preencher(0);
							
					if (read_buffer[k]==50)
						preencher(1);
							
					if (read_buffer[k]==51)
						preencher(2);
							
					if (read_buffer[k]==52)
						preencher(3);
							
					if (read_buffer[k]==53)
						preencher(4);
							
					if (read_buffer[k]==54)
						preencher(5);
					
					read_buffer[k]=0;
				}
			}
    }
    
}
