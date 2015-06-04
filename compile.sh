#!/bin/bash
rm server
rm client
rm client_*
gcc -o server object.c user.c util.c monster.c start.c server.c
gcc -o client client.c
