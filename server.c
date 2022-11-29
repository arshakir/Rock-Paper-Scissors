#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct game {
  int p1fd;
  int p2fd;

  char p1name[256];
  char p2name[256];
};

struct player {
  int fd;
  char name[256];
  int choice;
};

void error(char* msg){
  perror(msg);
  exit(1);
}

void getPlayer(struct player *p, struct sockaddr_in *address, int sockfd, int* addrlen){
  if ((p->fd = accept(sockfd, (struct sockaddr *)address, (socklen_t *)addrlen)) < 0) {
      perror("Error on accept");
  }
  write(p->fd, "Enter your name: ", 17);

  int n = read(p->fd, p->name, 256);
  if (n < 0){
    perror("Error getting name");
  }
}

void getChoice(struct player *p){
  write(p->fd, "1) rock\n2) paper\n3) scissors\nEnter the number of your choice: ", 62);
  char buf[256];
  int n = read(p->fd, buf, 256);
  if (n < 0){
    perror("Error getting choice");
  }
  p->choice = atoi(buf);
}

char* win(struct player *p1, struct player *p2){
  if (p1->choice == p2-> choice){
    return "tie";
  }

  if ((p1->choice == 1 && p2->choice == 3) || (p1->choice == 2 && p2->choice == 1) || (p1->choice == 3 && p2->choice == 2)){
    return "player 1 wins";
  } else {
    return "player 2 wins";
  }
}


int main(int argc, char *argv[]) {

  //Initialize socket stuff
  if (argc < 2) {
    printf("No port given");
    return -1;
  }

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }

  struct sockaddr_in address;
  int addrlen = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(argv[1]));

  if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    return -1;
  }

  if (listen(sockfd, 3) < 0) {
    return -1;
  }

  struct player p1;
  struct player p2;

  getPlayer(&p1, &address, sockfd, &addrlen);

  write(p1.fd, "Waiting for player 2\n", 22);

  getPlayer(&p2, &address, sockfd, &addrlen);

  write(p1.fd, "Found player\n", 14);
  write(p2.fd, "Found player\n", 14);
  
  getChoice(&p1);
  getChoice(&p2);


  char* winner = win(&p1,&p2);

  write(p1.fd, winner, strlen(winner));
  write(p2.fd, winner, strlen(winner));

  close(p1.fd);
  close(p2.fd);
  shutdown(sockfd, SHUT_RDWR);
}
