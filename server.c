#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include "aes.h"
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

struct data
{
	int* new_socket;
	char* buffer;
    	struct AES_ctx* ctx;
	uint8_t* key;
	uint8_t* iv;
};



struct data1
{
	char* input_mess;
	uint8_t *in;	
	size_t *mess_size;
	int* new_socket;
    	struct AES_ctx* ctx;
	uint8_t* key;
	uint8_t* iv;
};

 
void replace_end(char* str);
void* contact(void* data);
void* contact1(void* data1);


int main(int argc, char const* argv[])
{
    uint8_t key[] = "secret key 123";
    uint8_t *in;
    uint8_t iv[16] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };


    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {'\0'};
    size_t mess_size = 32;

    char input_mess[32*40] = {0};

    in = input_mess;

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CTR_xcrypt_buffer(&ctx, in, strlen((char*)in));

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

 
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
	perror("listen");
	exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
	perror("accept");
	exit(EXIT_FAILURE);
    }
    struct data* feed = (struct data*)malloc(sizeof(struct data));
    feed->new_socket = &new_socket;
    feed->buffer = buffer;
    feed->ctx = &ctx;
    feed->key = key;
    feed->iv = iv;


    struct data1* feed1 = (struct data1*)malloc(sizeof(struct data1));
    feed1->new_socket = &new_socket;
    feed1->in = in;
    feed1->ctx = &ctx;
    feed1->key = key;
    feed1->iv = iv;
    feed1->mess_size = &mess_size;
    feed1->input_mess = input_mess;



    pthread_t threads[3];
    int thing1 = pthread_create(&threads[0], NULL, contact,(void*) feed);	
    int thing2 = pthread_create(&threads[0], NULL, contact1,(void*) feed1);	

    pthread_exit(NULL);


    free(feed1);
    free(feed);
    free(input_mess);
     
    return 0;
}

void* contact1(void* feed1)
{
	while (1)
	{
		printf("\n");
		getline(&(((struct data1*)feed1)->input_mess), (((struct data1*)feed1)->mess_size), stdin);

		AES_init_ctx_iv(((struct data1*)feed1)->ctx, (((struct data1*)feed1)->key), (((struct data1*)feed1)->iv));
		AES_CTR_xcrypt_buffer((((struct data1*)feed1)->ctx), (((struct data1*)feed1)->in), strlen((char*)(((struct data1*)feed1)->in)));
		send(*(((struct data1*)feed1)->new_socket), (char*) (((struct data1*)feed1)->in), strlen((char*)(((struct data1*)feed1)->in)), 0);

	}

		    
		    
}

void* contact(void* feed)
{
	int valread;
	while (1)
	    {
		    
		    valread = read(*(((struct data*)feed)->new_socket), ((struct data*)feed)->buffer, 1024);
		    AES_init_ctx_iv(((struct data*)feed)->ctx, ((struct data*)feed)->key, ((struct data*)feed)->iv);
		    AES_CTR_xcrypt_buffer(((struct data*)feed)->ctx,((struct data*)feed)->buffer, strlen((char*)((struct data*)feed)->buffer));

		    replace_end((char*)((struct data*)feed)->buffer);

		    printf("%s\n", (char*)((struct data*)feed)->buffer);
	    }

}


void replace_end(char* str)
{
	for(int i = 0; i < strlen(str); i++)
	{
		if (str[i] == '\n')
		{
			str[i] = '\0';
		}
	}
}
