#include <stdio.h>

#include "cpack.h"

void callback(const cp_buf *buf){
    char *ch;
    cp_buf_to_ch(buf, &ch);
    printf("resp: %s\r\n", ch);
    free(ch);
}

int main(int argc, char **argv){

    struct cp_client *client;
    cp_client_init(
        &client, "http://127.0.0.1:8080", "main.db", callback);

    char *p1 = "hello, world!";
    char *p2 = "hello, foo!";
    char *p3 = "hello, bar!";
    char *p4 = "do you copy?";

    cp_buf *buf1 = cp_buf_init();
    cp_buf *buf2 = cp_buf_init();
    cp_buf *buf3 = cp_buf_init();
    cp_buf *buf4 = cp_buf_init();
    cp_buf_append(buf1, p1, strlen(p1));
    cp_buf_append(buf2, p2, strlen(p2));
    cp_buf_append(buf3, p3, strlen(p3));
    cp_buf_append(buf4, p4, strlen(p4));

    cp_commit_packet(client, buf1, 0);
    cp_commit_packet(client, buf2, 1);
    cp_commit_packet(client, buf3, 2);
    cp_commit_packet(client, buf4, 2);

    cp_start_loop(client);

    cp_client_free(client);

}