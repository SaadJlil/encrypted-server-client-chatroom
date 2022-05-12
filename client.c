// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include "aes.h"
#include <stdlib.h>
#include <pthread.h>
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
void* contact1(void* data1);
void* contact(void* data);

int main(int argc, char const* argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr*)&serv_addr,
		sizeof(serv_addr))
	< 0) {
	printf("\nConnection Failed \n");
	return -1;
    }
    uint8_t key[] = "secret key 123";
    uint8_t *in;
    uint8_t iv[16] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
    size_t in_size = 1023;
    char * input_mess;
    size_t mess_size = 32;
   
    input_mess = (char*)malloc(32*40); 
    in = input_mess;

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CTR_xcrypt_buffer(&ctx, in, strlen((char*)in));

    struct data1* feed1 = (struct data1*)malloc(sizeof(struct data1));
    feed1->new_socket = &sock;
    feed1->in = in;
    feed1->ctx = &ctx;
    feed1->key = key;
    feed1->iv = iv;
    feed1->mess_size = &mess_size;
    feed1->input_mess = input_mess;

    struct data* feed = (struct data*)malloc(sizeof(struct data));
    feed->new_socket = &sock;
    feed->buffer = buffer;
    feed->ctx = &ctx;
    feed->key = key;
    feed->iv = iv;

    
    pthread_t threads[3];
    int thing1 = pthread_create(&threads[0], NULL, contact1,(void*) feed1);	
    int thing2 = pthread_create(&threads[1], NULL, contact,(void*) feed);	
    pthread_exit(NULL);




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
		send(*(((struct data1*)feed1)->new_socket), (char*) (((struct data1*)feed1)->in), strlen((((struct data1*)feed1)->in)), 0);
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

