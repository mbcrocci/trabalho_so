#!/bin/bash
rm server
rm client
rm client_*
gcc -o server object.c monster.c user.c start.c util.c server.c
gcc -o client client.c
