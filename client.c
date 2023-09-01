#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define TEST_SIZE (1024 * 1024 * 10) // 10 MB

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 创建客户端套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // 设置服务器地址信息
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.10"); // 替换为服务器的IP地址

    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // 准备测试数据
    char *data = malloc(TEST_SIZE);
    memset(data, 'A', TEST_SIZE);

    // 发送数据并测试带宽
    while (1) {
        struct timeval start, end;
        gettimeofday(&start, NULL);

        size_t sent_bytes = 0;
        while (sent_bytes < TEST_SIZE) {
            ssize_t sent = send(sockfd, data + sent_bytes, TEST_SIZE - sent_bytes, 0);
            if (sent == -1) {
                perror("send");
                close(sockfd);
                free(data);
                return 1;
            }
            sent_bytes += sent;
        }

        gettimeofday(&end, NULL);

        double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        double bandwidth = TEST_SIZE / (1024 * 1024) / elapsed_time; // MB/s

        printf("Transferred %d bytes in %.2f seconds. Bandwidth: %.2f MB/s\n", TEST_SIZE, elapsed_time, bandwidth);
    }

    // 关闭套接字和释放内存
    close(sockfd);
    free(data);

    return 0;
}