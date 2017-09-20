#include <stdio.h>

#include "cpack.h"

// HTTP

#include "mongoose.h"

typedef struct _http_opt{
    void *p;
    int exit_flag;
    void(*callback)(const cp_buf *buf, void *p);
} _http_opt;

static void _http_ev_handler(struct mg_connection *c, int ev, void *p){
    _http_opt *opt = c->user_data;
    if(ev == MG_EV_HTTP_REPLY){
        c->flags |= MG_F_CLOSE_IMMEDIATELY;

        // Invoke response handle function
        struct http_message *hm = (struct http_message *)p;
        if(hm->body.len){
            cp_buf *buf = cp_buf_init();
            cp_buf_append(buf, hm->body.p, hm->body.len);
            opt->callback(buf, opt->p);
            cp_buf_free(buf);
        }
        opt->exit_flag = 1;
    } else if(ev == MG_EV_CLOSE){
        opt->exit_flag = 1;
    }
}

void _http_post(const char *url, const cp_buf *post_data, 
    void(*callback)(const cp_buf *buf, void *p), void *p){
    struct mg_mgr mgr;

    struct mg_connect_opts connect_opts;
    memset(&connect_opts, 0, sizeof(connect_opts));
    _http_opt opt;
    opt.p = p;
    opt.exit_flag = 0;
    opt.callback = callback;
    connect_opts.user_data = &opt;

    mg_mgr_init(&mgr, NULL);
    mg_connect_http_opt2(&mgr, _http_ev_handler, connect_opts, 
        url, NULL, post_data->data, post_data->size);

    while(opt.exit_flag == 0){
        mg_mgr_poll(&mgr, 500);
    }
    mg_mgr_free(&mgr);
}

// End HTTP

void callback(const cp_buf *buf){
    char *ch;
    cp_buf_to_ch(buf, &ch);
    printf("resp: %s\r\n", ch);
    free(ch);
}

void response_handler(const cp_buf *buf, void *p){
    cp_client *client = p;
    cp_parse_body(client, buf, callback);
}

int main(int argc, char **argv){

    struct cp_client *client;
    cp_client_init(&client, "main.db");

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

    int t = 100;

    while(--t){
        cp_sleep(500);

        cp_buf *body;
        int rc = cp_generate_body(client, &body);
        if(rc){
            break;
        }
        if(body->size){
            _http_post("http://127.0.0.1:8080", body, response_handler, client);
        }
        cp_buf_free(body);
    }

    cp_client_free(client);

}