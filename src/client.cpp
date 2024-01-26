/**
 * author: brando
 * date: 1/24/24
 */

#include <chat.h>
#include <client.hpp>
#include <messenger.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

void ClientThreadCallback(void * in) {
	const ChatConfig * config = (const ChatConfig *) in;

	int sockD = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port
        = htons(9001); // use some unused port number
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int connectStatus
        = connect(sockD, (struct sockaddr*)&servAddr,
                  sizeof(servAddr));

    if (connectStatus == -1) {
        printf("Error... %d\n", errno);
    }

    else {
		while (1) {
			char buf[MESSAGE_BUFFER_SIZE];
			recv(sockD, buf, sizeof(buf), 0);
			printf("recv: %s\n", buf);
		}
    }
}

int ClientRun(ChatConfig * config) {
	printf("client\n");
	BFThreadAsync(ClientThreadCallback, (void *) config);
	while (1) {}
	return 0;
}

