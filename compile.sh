#!/bin/bash
gcc -o server object.c user.c util.c monster.c start.c server.c
gcc -o client client.c