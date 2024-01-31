/**
 * author: brando
 * date: 1/24/24
 */

#include <chat.h>
#include <client.hpp>
#include <messenger.hpp>
#include <io.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

void ClientThreadCallback(void * in) {
	ChatConfig * config = (ChatConfig *) in;

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
    } else {
		/*
		while (1) {
			char buf[MESSAGE_BUFFER_SIZE];
			recv(sockD, buf, sizeof(buf), 0);
			printf("recv: %s\n", buf);
		}
		*/

		IOTools iotool;
		iotool.config = config;
		iotool.cd = sockD;

		BFThreadAsyncID inid = BFThreadAsync(IOIn, (void *) &iotool);
		//BFThreadAsyncID outid = BFThreadAsync(IOOut, (void *) &iotool);

		while (1) {}

		BFThreadAsyncIDDestroy(inid);
		//BFThreadAsyncIDDestroy(outid);
    }
}

int ClientRun(ChatConfig * config) {
	printf("client\n");
	BFThreadAsyncID tid = BFThreadAsync(ClientThreadCallback, (void *) config);

	int error = MessengerRun(config);

	BFThreadAsyncIDDestroy(tid);

	return error;
}

