rm client.o
rm server.o
rm server
rm client
gcc -pthread -c client.c
gcc -pthread -c server.c
gcc aes.o client.o -pthread -o client 
gcc aes.o server.o -pthread -o server  
