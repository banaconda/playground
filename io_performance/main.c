#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
#include <pthread.h>
#include <getopt.h>
#include <errno.h>

#include <sys/socket.h>

#include <sys/epoll.h>
#include <arpa/inet.h>

typedef enum {
    DATA = 1,
    END,
    ACK,
} command_t;

typedef struct event {

    command_t cmd;
    int seq;
    uint8_t buf[1024];
} event_t;

int
get_listen_socket(uint16_t port) {
    // base listen socket
    int listen_fd;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("listen socket");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int flag = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
        perror("setsockopt fail");
        exit(1);
    }

    if (bind(listen_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind fail");
        exit(1);
    }

    if (listen(listen_fd, 1) != 0) {
        perror("listen fail");
        exit(1);
    }

    return listen_fd;
}


#define TEST_NUM 10000
#define MAX_SOCKET_NUM  512
void
single_thread_non_blocking_test(uint16_t port)
{
    int listen_fd = get_listen_socket(port);

    int result[MAX_SOCKET_NUM * 2];
    memset(result, 0, sizeof(result));
    event_t **evts = malloc(sizeof(event_t*) * MAX_SOCKET_NUM * 2);
    for (int i = 0; i < MAX_SOCKET_NUM * 2; i++) {
        evts[i] = calloc(TEST_NUM + 1, sizeof(event_t));
    }

    clock_t start = 0;
    clock_t end = 0;

    // add listen socket to epoll
    int epoll_fd = epoll_create(MAX_SOCKET_NUM);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);

    struct epoll_event events[MAX_SOCKET_NUM];
    int sum[MAX_SOCKET_NUM];
    memset(sum, 0, sizeof(int) * MAX_SOCKET_NUM);
    while(1) {
        int count = epoll_wait(epoll_fd, events, MAX_SOCKET_NUM, -1);
        // printf("count = %d\n", count);
        if (count == -1) {
            perror("epoll_wait");
            exit(1);
        }

        for (int i = 0; i < count; i++) {
            int n = 0;
            int client_fd;
            if (events[i].data.fd == listen_fd) {
                struct sockaddr_in client;
                socklen_t client_socklen = sizeof(client);
                client_fd = accept(listen_fd, (struct sockaddr *)&client, &client_socklen);
                if (client_fd == -1) {
                    perror("accept fail");
                    exit(1);
                }

                int flags = fcntl(client_fd, F_GETFL);
                if (flags == -1) {
                    perror("F_GET");
                    exit(1);
                }
                flags |= O_NONBLOCK;
                if (fcntl(client_fd, F_SETFL, flags) == -1) {
                    perror("F_SET");
                    exit(1);
                }

                struct epoll_event event;
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                printf("init fd=%d, n=%d\n", client_fd, n);
            } else {
                client_fd = events[i].data.fd;

                event_t evt;
                if ((n = recv(client_fd, &evt, sizeof(event_t), 0)) <= 0) {
                    perror("recv");
                }
                if (n == sizeof(evt) && evt.seq >= 0 && evt.seq < TEST_NUM && client_fd > 0) {
                    if (start == 0) {
                        start = clock();
                    }
                    evts[client_fd][evt.seq] = evt;
                }

                if (evt.cmd == END) {
                    static int close_count = 0;
                    close_count++;
                    printf("close = %d, close_count=%d\n", client_fd, close_count);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);

                    if (close_count == MAX_SOCKET_NUM) {
                        goto __end;
                    }
                }
                // printf("cont fd=%d, n=%d\n", client_fd, n);
            }

            if (n <= 0 || client_fd < 0) {
                continue;
            }
            sum[client_fd] += n;
        }

    }

__end:
    end = clock();

    for (int i = 0; i < MAX_SOCKET_NUM * 2; i++) {
        for (int k = 0; k < TEST_NUM; k++) {
            if (evts[i][k].cmd != 0) {
                result[i]++;
            }
        }
    }

    for (int i = 0; i < MAX_SOCKET_NUM * 2; i++) {
        if (result[i] != 0) {
            printf("result[%d]=%d\n", i, result[i]);
        }
    }
    printf("clocks=%lu\n", end - start);
}

void *
recv_thread(void *arg)
{

    return NULL;
}

void
multi_thread_blocking_test(int thread_num, uint16_t port)
{
    int listen_fd = get_listen_socket(port);

    int result[MAX_SOCKET_NUM * 2];
    event_t **evts = malloc(sizeof(event_t*) * MAX_SOCKET_NUM * 2);
    for (int i = 0; i < MAX_SOCKET_NUM * 2; i++) {
        evts[i] = calloc(TEST_NUM + 1, sizeof(event_t));
    }

    for (int i = 0; i < MAX_SOCKET_NUM * 2; i++) {
        if (result[i] != 0) {
            printf("result[%d]=%d\n", i, result[i]);
        }
    }
}

void
multi_thread_non_blocking_test(int thread_num, uint16_t port)
{
}

void sender(uint16_t port) {
    int conn_fd[MAX_SOCKET_NUM];
    printf("socket\n");
    for (int i = 0; i < MAX_SOCKET_NUM; i++) {
        if ((conn_fd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("conn socket");
            exit(1);
        }
    }

    printf("connect\n");
    struct sockaddr_in server[MAX_SOCKET_NUM];
    for (int i = 0; i < MAX_SOCKET_NUM; i++) {
        server[i].sin_family = AF_INET;
        server[i].sin_port = htons(port);
        server[i].sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(conn_fd[i], (struct sockaddr *)&server[i], sizeof(server[i])) < 0) {
            perror("connect fail");
            exit(1);
        }
    }

    printf("send\n");
    event_t *evt = malloc(sizeof(event_t) * TEST_NUM);
    for (int i = 0; i < TEST_NUM; i++) {
        evt[i].cmd = DATA;
        evt[i].seq = i;
    }

    for (int i = 0; i < TEST_NUM; i++) {
        for (int k = 0; k < MAX_SOCKET_NUM; k++) {
            if (send(conn_fd[k], &evt[i], sizeof(event_t), 0) < 0) {
                perror("data send");
            }
        }
    }

    printf("end\n");
    evt[0].cmd = END;
    for (int i = 0; i < MAX_SOCKET_NUM; i++) {
        if (send(conn_fd[i], &evt[0], sizeof(event_t), 0) < 0) {
            perror("end send");
        }
    }
    printf("done\n");
    usleep(20000000);
}

static struct option long_options[] = {
    {"server", required_argument, 0, 's'},
    {"client", required_argument, 0, 'c'},
};

int
main(int argc, char **argv)
{
    int option_index = 0;
    int opt;
    while((opt = getopt_long(argc, argv, "s:c:", long_options, &option_index)) != -1) {
        switch (opt) {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case 's': {
            single_thread_non_blocking_test(atoi(optarg));
        } break;
        case 'c': {
            sender(atoi(optarg));
        } break;

        default:
            abort();
            break;
        }
    }
}