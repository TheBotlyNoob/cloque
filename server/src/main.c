#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>

#define HEADER_BUF_SIZE 256

enum HttpVerb {
  V_GET,
  V_HEAD,
  V_POST,
  V_PUT,
  V_DELETE,
  V_CONNECT,
  V_OPTIONS,
  V_TRACE,
  V_PATCH
};

struct Request {
  char path[HEADER_BUF_SIZE];
  uint8_t user_agent[HEADER_BUF_SIZE];
  uint16_t content_len;
  enum HttpVerb verb;
  // TODO: add more stuffs. for now it should be fine though?
};

enum HttpParserState {
  PARSER_VERB,
  PARSER_PATH,
  PARSER_USER_AGENT,
  PARSER_CONTENT_LEN,
};

int main(void) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;

  if (server_fd < 0) {
    perror("socket failed");
    return 1;
  }

  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = 8080;
  server_addr.sin_family = AF_INET;

  if (listen(server_fd, 10) < 0) {
    perror("listen failed");
    return 1;
  }

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind failed");
    return 1;
  }

  while (true) {
    struct Request request;
    uint8_t buf[HEADER_BUF_SIZE];
    int read, i;
    enum HttpParserState state = PARSER_VERB;

    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addrlen);
    if (client_fd < 0) {
      perror("accept failed");
      continue;
    }

    if (connect(client_fd, (struct sockaddr *)&client_addr, client_addrlen) !=
        0) {
      perror("connect failed");
      continue;
    };

    read = recv(client_fd, buf, HEADER_BUF_SIZE, 0);
    if (read == -1) {
      perror("recv failed");
      continue;
    }

    for (i = 0; i < read; i++) {
    }
  }
}
