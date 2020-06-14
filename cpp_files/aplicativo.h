#include <stdio.h>
#include <string.h>
#include <time.h> 
#include <mysql/my_global.h> //--> http://dev.mysql.com/doc/refman/5.0/en/windows-client-compiling.html
#include <mysql/mysql.h>
//#include <conio.h>
#pragma comment( lib, "libmysql" )


#define QUERY_LENGTH 1000
#define TMP_LENGTH 20

typedef unsigned int u_int;
typedef unsigned long u_long;

typedef struct TABELAS{
	int posto_id;
	char controlo[25];
	char data_info[25];
}s_tabelas;

typedef struct CONTROLO{
	char descricao[100];
	char data_info[100];
	unsigned int duracao_periodo;
	unsigned int duracao_bloco;
	unsigned int ref_cont_tempo_paragem;
	unsigned int ref_tempo_paragem;
	unsigned int ref_producao_periodo;
}s_controlo;

typedef struct TURNO{
	unsigned int id_turno;
	struct tm hora_inicio;
	struct tm hora_fim;
}s_turno;

typedef struct DATA{
	unsigned int id_turno;
	unsigned int id_bloco;
	unsigned int nr_pecas_periodo;
	unsigned int total_pecas_bloco;
	unsigned int tempo_paragem_total;
	struct tm    hora_inicio_periodo;
	struct tm    hora_primeira_peca;
	struct tm    hora_ultima_peca;
	struct tm	 data;
}s_data; 

typedef struct LOGIN{
	int ComPort;
	char server[256];
	char user[25];
	char pass[30];
	char base_dados[50];
}s_login;

typedef struct SPTR{
	s_controlo *controlo;
	s_data *data;
	s_turno *turno;
	int nr_controlo;
	int nr_data;
	int nr_turno;
}sptr;

int resto[6] = { 0 };
//HANDLE mysql_mutex;
//HANDLE hThread[6];
//DWORD WINAPI funcao(LPVOID arg);
	 
//DECLARA��O DE VARIAVEIS P/ MYSQL
MYSQL mysql;

void* funcao(void* arg);
int total_pecas(int id_bloco,sptr *ptr,time_t hora_pc);
long duracao_turno(time_t hora_pc);
int ObterIdBloco(sptr *ptr,time_t hora_pc,int nr_bloco);
void verificar(int posto_id);								   // ------------
int ObterIdRegisto(sptr *ptr,time_t hora_pc, int posto_id);
int ObterData(s_data *data_info,time_t hora_pc,int posto_id);  // ------------
int ObterIdTurno(time_t hora_pc);
int ObterDadosTurno(s_turno *def_turno,time_t hora_pc);
void strconvert(int *tmp,char *str,const char *c);
int ObterDadosControlo(s_controlo *def_controlo); // ------------


void connectComPort2();//ver Linux
void connectMySQL(s_login *d_login);
void connectComPort(s_login *d_login);
void login(s_login *d_login);
void ReadConfigData(s_login *d_login);
