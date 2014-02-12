#include <stdlib.h>

#include <sys/types.h>

#include <unistd.h>

#include <sys/wait.h>

#include <stdio.h>

#include <string.h>

#include "/opt/PostgreSQL/9.0/include/libpq-fe.h"

int main(int argc,char *argv[])

{

FILE *fp,*remote_machine,*log;

char pid[10],cmd[30],con_string[110],host[40],port[4],dbname[20],user[20],password[20],pg_dump_cmd[1000];

int status,remote_avg,dump_initiated=0,dump_stopped=0;

PGconn *conn;

PGresult *res;

log=fopen("/tmp/remote_dump.log","w");

if(fp==NULL)

{

fprintf(log,"\n%s","remote_credentials file is not available");

return 1;

}

else

if(argc<5)

{

printf("\nProvide the following Arguments ");

printf("\nhostaddress port dbname user password");

return 1;

}

sprintf(con_string,"hostaddr=%s port=%s dbname=%s user=%s password=%s",argv[1],argv[2],argv[3],argv[4],argv[5]);

conn = PQconnectdb(con_string);

if (PQstatus(conn) == CONNECTION_BAD)

        {

                 fprintf(log,"\n Not able to connect to the database %s",PQerrorMessage(conn));

                 return 1;

        }

 

while(1)

{

res=PQexec(conn,"BEGIN");

PQclear(res);

res=PQexec(conn,"DECLARE find_cpu_load CURSOR FOR select round(avg) from public.load_avg");

if (PQresultStatus(res) != PGRES_COMMAND_OK)

        {

                fprintf(stderr, "Erorr %s", PQerrorMessage(conn));

                PQclear(res);

        }

PQclear(res);

res = PQexec(conn, "FETCH ALL in find_cpu_load");

remote_avg=atoi(PQgetvalue(res,0,0));

PQclear(res);

res=PQexec(conn,"END");

PQclear(res);

 

if(!dump_initiated)

{

if(remote_avg<10)

{

fp=popen("/sbin/pidof -s pg_dump","r");

fgets(pid,10,fp);

pid[strlen(pid)-1]='\0';

pclose(fp);

if(strlen(pid)<2)

{

sprintf(pg_dump_cmd,"/opt/PostgreSQL/9.0/bin/pg_dump -f /opt/PostgreSQL/9.0/data/dump.sql -v -h %s -p %s -U %s %s >/tmp/remote_dump1.log 2>/tmp/remote_dump1.log &",argv[1],argv[2],argv[3],argv[4],argv[5]);

system(pg_dump_cmd);

fp=popen("/sbin/pidof -s pg_dump","r");

fgets(pid,10,fp);

pid[strlen(pid)-1]='\0';

pclose(fp);

dump_initiated=1;

}

while (waitpid(-1, &status, WNOHANG) > 0);

}

else

{

fprintf(log,"\nHigh Load Average .. So, Con't start pg_dump ... Load is %d",remote_avg);

PQfinish(conn);

fclose(log);

return 1;

exit(0);

}

}

else

{

fp=popen("/sbin/pidof -s pg_dump","r");

fgets(pid,10,fp);

pid[strlen(pid)-1]='\0';

pclose(fp);

if(strlen(pid)<2)

{

fprintf(log,"\nDump has completed .. hence closing the connections ... ");

PQfinish(conn);

return 1;

}

 

if(remote_avg<10&&!dump_stopped)

{

fprintf(log,"\nSleeping 10 seconds pid load is normal ==> %d",remote_avg);

sleep(10);

}

else if(remote_avg>10&&!dump_stopped)

{

fprintf(log,"\nSleeping 10 Seconds ****Load is high ****.. so stopping dump and Load is %d Processes is %d",remote_avg,atoi(pid));

sleep(10);

dump_stopped=1;

sprintf(cmd,"kill -s STOP %d",atoi(pid));

system(cmd);

}

else if(remote_avg>10&&dump_stopped)

{

fprintf(log,"\nSleeping 10 More Seconds Load is high ... Load is %d",remote_avg);

sleep(10);

}

else

{

fprintf(log,"\nLoad is Nomal Again so resumig the dump processes ... load is %d",remote_avg);

dump_stopped=0;

sprintf(cmd,"\nkill -s CONT %d",atoi(pid));

system(cmd);

}

}

while (waitpid(-1, &status, WNOHANG) > 0 );

}

fclose(fp);
}


