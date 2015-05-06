
#include "rtp.h"
#include "pbm.h"

#define DATA_SIZE 100000000
#define PAYLOAD_SIZE 1000

int main(int argc, const char * argv[]){
    int                 sock, ch, i;
    struct sockaddr_in  local;
    struct sockaddr_in  to;
    u_int32             rtp_stream_address;
    unsigned long       data_size;
    char                *image_data = malloc(DATA_SIZE);
    struct image_header *ih;
    char 				ch_rows[4],ch_cols[4];
    int 				rows,cols;

    if (argc != 3) {
        printf("\nUsage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
//    printf("Enter data to send: ");
    ////////////reading data (header + data)//////////////////
    data_size = 0;
    while ((ch = getchar()) != EOF){
    	image_data[data_size] = (unsigned char) ch;
        data_size++;
    }

    memcpy(ch_rows,image_data+sizeof(ih->format),sizeof(ih->rows)-1);
    ch_rows[sizeof(ih->rows)-1]=0;
    rows = atoi(ch_rows);
    memcpy(ch_cols,image_data+sizeof(ih->format)+sizeof(ih->rows),sizeof(ih->cols)-1);
    ch_cols[sizeof(ih->cols)-1]=0;
    cols = atoi(ch_cols);

    d_Sampling_img(image_data, rows,cols);

//-----====================================-------------------
    // initialize RTP stream address */
    rtp_stream_address = inet_addr(argv[1]);

    // if we got a valid RTP stream address... */
    if (rtp_stream_address != 0) {
        sock = socket(AF_INET, SOCK_DGRAM, 0); // create new socket for sending datagrams
        if (sock >= 0) {
            // prepare local address
            memset(&local, 0, sizeof(local));
            local.sin_family      = AF_INET;
            local.sin_port        = htons(INADDR_ANY);
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            // bind to local address
            if (bind(sock, (struct sockaddr *)&local, sizeof(local)) == 0) {
                // prepare RTP stream address
                memset(&to, 0, sizeof(to));
                to.sin_family      = AF_INET;
                to.sin_port        = htons(atoi(argv[2]));
                to.sin_addr.s_addr = rtp_stream_address;
                // send RTP packets
                rtp_send_packets( sock, &to, image_data, data_size, PAYLOAD_SIZE);
            }
            close(sock);
        }
    }
    free(image_data);
    return 0;
}
