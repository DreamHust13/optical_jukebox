target=Mysql.h  mysql.c mysocket.h mysocket_data.h mysocket.c main.c  tcp_server.h tcp_server.c
test:${target}
	gcc ${target}  -o test  -lmysqlclient -I/usr/local/include/mysql -L/usr/local/lib/mysql   -lpthread   -g
clean:
	rm test

