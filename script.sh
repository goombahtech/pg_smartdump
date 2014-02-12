Required Tables On Remote:-
===========================
CREATE TABLE LOAD_AVG(AVG REAL);

INSERT INTO LOAD_AVG VALUES(0.0);

Required Shell Script For Every One Minute On Remote:-
========================================================
PGBIN="/opt/PostgreSQL/9.0/bin"

PORT=5432

USER="postgres"

PASSWORD="postgres"

DATABASE="postgres"

LOAD=$(cat /proc/loadavg|awk -F ' ' '{print $1}')

$PGBIN/psql -p $PORT -U $USER -d $DATABASE -c "UPDATE LOAD_AVG SET

AVG='$LOAD'";
