#include "rtp.h"
#include "pbm.h"
#define DATA_SIZE 100000000 //We assume not more than this number of bytes will be recieved

int main(int argc, const char * argv[]) {
	int                 sock, i;
	struct sockaddr_in  local;
	int                 timeout;
	unsigned long       number_of_recv_bytes;
	struct image_header *ih;
	char 				ch_rows[4],ch_cols[4];
	int 				rows,cols;

	if (argc != 2) { // not write port number.
		printf("Usage: %s <port>", argv[0]);
		exit(1);
	}
	char *data = (char *) malloc(DATA_SIZE);
	// create new socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock >= 0) {
		// prepare local address
		memset(&local, 0, sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(atoi(argv[1]));
		local.sin_addr.s_addr = htonl(INADDR_ANY);

		// bind to local address
		if (bind(sock, (struct sockaddr *) &local, sizeof(local)) == 0) {
			// set recv timeout
			timeout = RTP_RECV_TIMEOUT;
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
					sizeof(timeout));

			//recieve data here;
			number_of_recv_bytes = rtp_recv_packets(sock, data);
		}
		close(sock);
	}
	//not make socket, execute putchar data
	for (i = 0; i < number_of_recv_bytes; i++) {
		putchar(data[i]);
	}


	memcpy(ch_rows,data+sizeof(ih->format),sizeof(ih->rows)-1);
	ch_rows[sizeof(ih->rows)-1]=0;
	rows = atoi(ch_rows);
	memcpy(ch_cols,data+sizeof(ih->format)+sizeof(ih->rows),sizeof(ih->cols)-1);
	ch_cols[sizeof(ih->cols)-1]=0;
	cols = atoi(ch_cols);

	u_Sampling_img(data,rows,cols);
	free(data);
	return 0;
}

