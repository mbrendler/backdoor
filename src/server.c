#include "server.h"
#include "commands.h"
#include "lib.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void server_handle_connection(const int cfd) {
  char buffer[4096] = { 0 };
  const size_t buffer_size = sizeof(buffer) / sizeof(*buffer);
  if (0 > read(cfd, buffer, buffer_size - 1)) {
    log_perror("server read");
    return;
  }
  if (command_valid(buffer)) {
    log_info("run '%s'", buffer);
    system(buffer);
  }
}

int server_run(const char* address, int port) {
  const int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    log_pfatal("server socket");
    return 1;
  }

  struct sockaddr_in servaddr = {
    .sin_family = AF_INET,
    .sin_addr = { .s_addr = inet_addr(address) },
    .sin_port = htons(port)
  };

  if (0 != bind(sfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
    log_pfatal("server bind");
    return 1;
  }

  if (0 != listen(sfd, 5)) {
    log_pfatal("server listen");
    return 1;
  }

  while (1) {
    struct sockaddr_in cli;
    socklen_t client_socket_len = sizeof(cli);
    const int cfd = accept(sfd, (struct sockaddr*)&cli, &client_socket_len);
    if (cfd < 0) {
      log_pfatal("server accept");
      return 1;
    }

    server_handle_connection(cfd);
    close(cfd);
  }

  // close(sfd);
}
