pg_SmartDump
============

Using this innovative tool, we can the dump the data from remote machine database to local system.

The main thing here is, pg_SmartDump works in a step by step manner. 
It will take sometime to complete it's dump, but it don't increase the load on the remote production machine.

First deploy the scripts.sh on the remote/production machine to collect the loadAVG of that machine.

If load avg goes beyond 10, this script pause the running pg_dump, and will resume once the load comes under control. 

HOW TO COMPILE 
==============

1.export LD_LIBRARY_PATH=/opt/PostgreSQL/9.0/lib/
2.gcc -o pg_SmartDump pg_SmartDump.c -L/opt/PostgreSQL/9.0/lib -lpq -lssl -lcrypto

HOW TO RUN 
===========
./pg_SmartDump 172.24.35.67 5432 postgres postgres postgres
 [host] [port] [user] [password] [database]
