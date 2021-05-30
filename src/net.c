#include "net.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <endian.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

struct addrinfo net_get_addr_struct(const int family, const int socktype, const int protocol, const int flags) {
  return (struct addrinfo) {
    .ai_family = family,
    .ai_socktype = socktype,
    .ai_protocol = protocol,
    .ai_flags = flags
  };
}

struct addrinfo* net_get_address(const char* const hostname, const char* const service, const struct addrinfo* const hints) {
  struct addrinfo* addr;
  int err = getaddrinfo(hostname, service, hints, &addr);
  if(err != 0) {
    errno = err;
    return NULL;
  }
  return addr;
}

const char* net_get_address_strerror(const int code) {
  return gai_strerror(code);
}

void net_get_address_free(struct addrinfo* const info) {
  freeaddrinfo(info);
}

int net_get_name(const void* const sockaddr, const socklen_t sockaddrlen, char* const hostname, const socklen_t len, const int flags) {
  return getnameinfo((struct sockaddr*) sockaddr, sockaddrlen, hostname, len, NULL, 0, NI_NAMEREQD | flags);
}

int net_string_to_address(void* const addr, const char* const buffer) {
  int err;
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    err = inet_pton(ipv4, buffer, &((struct sockaddr_in*) addr)->sin_addr.s_addr);
  } else {
    err = inet_pton(ipv6, buffer, ((struct sockaddr_in6*) addr)->sin6_addr.s6_addr);
  }
  if(err == 1) {
    return net_success;
  }
  return net_failure;
}

int net_address_to_string(void* const addr, char* const buffer) {
  const void* err;
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    err = inet_ntop(ipv4, &((struct sockaddr_in*) addr)->sin_addr.s_addr, buffer, 16);
  } else {
    err = inet_ntop(ipv6, ((struct sockaddr_in6*) addr)->sin6_addr.s6_addr, buffer, 41);
  }
  if(err == NULL) {
    return net_failure;
  }
  return net_success;
}

void net_set_family(void* const addr, const int family) {
  ((struct sockaddr*) addr)->sa_family = family;
}

int net_get_family(const void* const addr) {
  return ((struct sockaddr*) addr)->sa_family;
}

void net_set_any_addr(void* const addr) {
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    ((struct sockaddr_in*) addr)->sin_addr.s_addr = INADDR_ANY;
  } else {
    ((struct sockaddr_in6*) addr)->sin6_addr = in6addr_any;
  }
}

void net_set_loopback_addr(void* const addr) {
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    ((struct sockaddr_in*) addr)->sin_addr.s_addr = INADDR_LOOPBACK;
  } else {
    ((struct sockaddr_in6*) addr)->sin6_addr = in6addr_loopback;
  }
}

void net_set_addr(void* const addr, const void* const address) {
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    ((struct sockaddr_in*) addr)->sin_addr.s_addr = htons(*(uint32_t*) address);
  } else {
    (void) memcpy(((struct sockaddr_in6*) addr)->sin6_addr.s6_addr, address, 16);
  }
}

void* net_get_addr(const void* const addr) {
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    return &((struct sockaddr_in*) addr)->sin_addr.s_addr;
  } else {
    return ((struct sockaddr_in6*) addr)->sin6_addr.s6_addr;
  }
}

void net_set_port(void* const addr, const unsigned short port) {
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    ((struct sockaddr_in*) addr)->sin_port = htons(port);
  } else {
    ((struct sockaddr_in6*) addr)->sin6_port = htons(port);
  }
}

unsigned short net_get_port(void* const addr) {
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    return ntohs(((struct sockaddr_in*) addr)->sin_port);
  } else {
    return ntohs(((struct sockaddr_in6*) addr)->sin6_port);
  }
}

int net_get_ipv4_addrlen() {
  return sizeof(struct sockaddr_in);
}

int net_get_ipv6_addrlen() {
  return sizeof(struct sockaddr_in6);
}

int net_get_addrlen(const void* const addr) {
  if(((struct sockaddr*) addr)->sa_family == ipv4) {
    return net_get_ipv4_addrlen();
  } else {
    return net_get_ipv6_addrlen();
  }
}

int net_get_socket(const struct addrinfo* const addr) {
  return socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
}

int net_bind_socket(const int sfd, const void* const addr) {
  return bind(sfd, (struct sockaddr*) addr, net_get_addrlen(addr));
}

int net_connect_socket(const int sfd, const void* const addr) {
  return connect(sfd, (struct sockaddr*) addr, net_get_addrlen(addr));
}

int net_socket_setopt_true(const int sfd, const int level, const int option_name) {
  return setsockopt(sfd, level, option_name, &(int){1}, sizeof(int));
}

int net_socket_setopt_false(const int sfd, const int level, const int option_name) {
  return setsockopt(sfd, level, option_name, &(int){0}, sizeof(int));
}

int net_socket_reuse_addr(const int sfd) {
  return net_socket_setopt_true(sfd, SOL_SOCKET, SO_REUSEADDR);
}

int net_socket_dont_reuse_addr(const int sfd) {
  return net_socket_setopt_false(sfd, SOL_SOCKET, SO_REUSEADDR);
}

int net_socket_reuse_port(const int sfd) {
  return net_socket_setopt_true(sfd, SOL_SOCKET, SO_REUSEPORT);
}

int net_socket_dont_reuse_port(const int sfd) {
  return net_socket_setopt_false(sfd, SOL_SOCKET, SO_REUSEPORT);
}

int net_socket_get_family(const int sfd, int* const family) {
  return getsockopt(sfd, SOL_SOCKET, SO_DOMAIN, family, &(socklen_t){sizeof(int)});
}

int net_socket_get_socktype(const int sfd, int* const socktype) {
  return getsockopt(sfd, SOL_SOCKET, SO_TYPE, socktype, &(socklen_t){sizeof(int)});
}

int net_socket_get_protocol(const int sfd, int* const protocol) {
  return getsockopt(sfd, SOL_SOCKET, SO_PROTOCOL, protocol, &(socklen_t){sizeof(int)});
}

int net_socket_dont_block(const int sfd) {
  const int flags = fcntl(sfd, F_GETFL, 0);
  if(flags == -1) {
    return net_failure;
  }
  const int res = fcntl(sfd, F_SETFL, flags | O_NONBLOCK);
  if(res == -1) {
    return net_failure;
  }
  return net_success;
}

int net_socket_block(const int sfd) {
  int flags = fcntl(sfd, F_GETFL, 0);
  if(flags == -1) {
    return net_failure;
  }
  if((flags ^ O_NONBLOCK) != 0) {
    flags ^= O_NONBLOCK;
  }
  const int res = fcntl(sfd, F_SETFL, flags);
  if(res == -1) {
    return net_failure;
  }
  return net_success;
}

int net_socket_base_options(const int sfd) {
  int err = net_socket_dont_block(sfd);
  if(err != net_success) {
    return net_failure;
  }
  err = net_socket_reuse_addr(sfd);
  if(err != net_success) {
    return net_failure;
  }
  err = net_socket_reuse_port(sfd);
  if(err != net_success) {
    return net_failure;
  }
  return net_success;
}

#define epoll ((struct net_epoll*) net_epoll_thread_data)

static void net_epoll_thread(void* net_epoll_thread_data) {
  (void) pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  {
    sigset_t mask;
    (void) sigfillset(&mask);
    (void) pthread_sigmask(SIG_BLOCK, &mask, NULL);
  }
  struct epoll_event events[100];
  while(1) {
    int count = epoll_wait(epoll->sfd, events, 100, -1);
    for(int i = 0; i < count; ++i) {
      (void) pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      void* const clean_ptr = (uintptr_t) events[i].data.ptr & (UINTPTR_MAX ^ 3);
      switch((uintptr_t) events[i].data.ptr & 3) {
        case 0: {
          ((struct net_epoll_socket_base*) clean_ptr)->epoll_server->on_event(epoll, events + i, clean_ptr, net_esocket_eserver);
          break;
        }
        case 1: {
          ((struct net_epoll_socket_base*) clean_ptr)->on_event(epoll, events + i, clean_ptr, net_esocket_server);
          break;
        }
        case 2: {
          ((struct net_epoll_socket_base*) clean_ptr)->on_event(epoll, events + i, clean_ptr, net_esocket);
          break;
        }
        case 3: {
          ((struct net_epoll_server_base*) clean_ptr)->on_event(epoll, events + i, clean_ptr, net_eserver);
          break;
        }
      }
      (void) pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      pthread_testcancel();
    }
  }
}

#undef epoll

int net_epoll(struct net_epoll* const epoll) {
  {
    const int err = threads(&epoll->thread);
    if(err != threads_success) {
      return net_failure;
    }
  }
  const int sfd = epoll_create1(0);
  if(sfd == -1) {
    threads_free(&epoll->thread);
    return net_failure;
  }
  epoll->sfd = sfd;
  epoll->thread.func = net_epoll_thread;
  epoll->thread.data = epoll;
  return net_success;
}

int net_epoll_start(struct net_epoll* const epoll) {
  return threads_add(&epoll->thread, 1);
}

void net_epoll_stop(struct net_epoll* const epoll) {
  threads_shutdown(&epoll->thread);
}

void net_epoll_free(struct net_epoll* const epoll) {
  threads_free(&epoll->thread);
  (void) close(epoll->sfd);
}

int net_server_base(void* const server, const unsigned long item_size, const unsigned long descriptors_per_part, const unsigned long tolerance) {
  struct contmem sockets;
  int err = contmem(&sockets, descriptors_per_part, item_size, tolerance);
  if(err == contmem_out_of_memory) {
    return net_out_of_memory;
  }
  struct mufex mutex;
  err = mufex(&mutex);
  if(err != 0) {
    contmem_free(&sockets);
    errno = err;
    return net_failure;
  }
  ((struct net_server_base*) server)->sockets = sockets;
  (void) memcpy(&((struct net_server_base*) server)->mutex, &mutex, sizeof(pthread_mutex_t) + 4);
  return net_success;
}

void net_server_base_free(void* const server) {
  contmem_free(&((struct net_server_base*) server)->sockets);
  mufex_destroy(&((struct net_server_base*) server)->mutex);
}

/*
 *  ANY SERVER + EPOLL SOCKET
 */

int net_epoll_add_socket(struct net_epoll* const epoll, struct net_epoll_socket_base* const socket) {
  mufex_lock(&socket->server->mutex, mufex_not_shared);
  void* const mem = contmem_get(&socket->server->sockets);
  if(mem == NULL) {
    return net_out_of_memory;
  }
  (void) memcpy(mem, socket, socket->server->sockets.item_size);
  int err = epoll_ctl(epoll->sfd, EPOLL_CTL_ADD, socket->sfd, &((struct epoll_event) {
    .events = socket->events,
    .data = (epoll_data_t) {
      .ptr = (void*)((uintptr_t) mem | (socket->on_event != NULL ? 1 : 0))
    }
  }));
  if(err != 0) {
    (void) contmem_pop(&socket->server->sockets, mem);
    err = net_failure;
  } else {
    err = net_success;
  }
  mufex_unlock(&socket->server->mutex, mufex_not_shared);
  return err;
}

int net_epoll_mod_socket(struct net_epoll* const epoll, struct net_epoll_socket_base* const socket) {
  const int err = epoll_ctl(epoll->sfd, EPOLL_CTL_MOD, socket->sfd, &((struct epoll_event) {
    .events = socket->events,
    .data = (epoll_data_t) {
      .ptr = (void*)((uintptr_t) socket | (socket->on_event != NULL ? 1 : 0))
    }
  }));
  if(err != 0) {
    return net_failure;
  }
  return net_success;
}

int net_epoll_remove_socket(struct net_epoll* const epoll, struct net_epoll_socket_base* const socket) {
  mufex_lock(&socket->server->mutex, mufex_not_shared);
  int err = epoll_ctl(epoll->sfd, EPOLL_CTL_DEL, socket->sfd, NULL);
  if(err == 0) {
    const struct net_epoll_socket_base* const last = contmem_last(&socket->server->sockets);
    if(socket != last) {
      err = epoll_ctl(epoll->sfd, EPOLL_CTL_MOD, last->sfd, &((struct epoll_event) {
        .events = last->events,
        .data = (epoll_data_t) {
          .ptr = (void*)((uintptr_t) socket | (last->on_event != NULL ? 1 : 0))
        }
      }));
      if(err == 0) {
        (void) contmem_pop(&socket->server->sockets, socket);
        err = net_success;
      } else {
        err = net_failure;
      }
    } else {
      (void) contmem_pop(&socket->server->sockets, socket);
      err = net_success;
    }
  } else {
    err = net_failure;
  }
  mufex_unlock(&socket->server->mutex, mufex_not_shared);
  return err;
}

/*
 *  NO SERVER + EPOLL SOCKET
 */

static int net_epoll_modify_serverless_socket(struct net_epoll* const epoll, struct net_epoll_socket_base* const socket, const int method) {
  const int err = epoll_ctl(epoll->sfd, method, socket->sfd, &((struct epoll_event) {
    .events = socket->events,
    .data = (epoll_data_t) {
      .ptr = (void*)((uintptr_t) socket | 2)
    }
  }));
  if(err != 0) {
    return net_failure;
  }
  return net_success;
}

int net_epoll_add_serverless_socket(struct net_epoll* const epoll, struct net_epoll_socket_base* const socket) {
  return net_epoll_modify_serverless_socket(epoll, socket, EPOLL_CTL_ADD);
}

int net_epoll_mod_serverless_socket(struct net_epoll* const epoll, struct net_epoll_socket_base* const socket) {
  return net_epoll_modify_serverless_socket(epoll, socket, EPOLL_CTL_MOD);
}

int net_epoll_remove_serverless_socket(struct net_epoll* const epoll, struct net_epoll_socket_base* const socket) {
  return net_epoll_modify_serverless_socket(epoll, socket, EPOLL_CTL_DEL);
}

/*
 *  EPOLL SERVER + ANY SOCKET
 */

static int net_epoll_modify_server(struct net_epoll* const epoll, struct net_epoll_server_base* const server, const int method) {
  const int err = epoll_ctl(epoll->sfd, method, server->sfd, &((struct epoll_event) {
    .events = server->events,
    .data = (epoll_data_t) {
      .ptr = (void*)((uintptr_t) server | 3)
    }
  }));
  if(err != 0) {
    return net_failure;
  }
  return net_success;
}

int net_epoll_add_server(struct net_epoll* const epoll, struct net_epoll_server_base* const server) {
  return net_epoll_modify_server(epoll, server, EPOLL_CTL_ADD);
}

int net_epoll_mod_server(struct net_epoll* const epoll, struct net_epoll_server_base* const server) {
  return net_epoll_modify_server(epoll, server, EPOLL_CTL_MOD);
}

int net_epoll_remove_server(struct net_epoll* const epoll, struct net_epoll_server_base* const server) {
  return net_epoll_modify_server(epoll, server, EPOLL_CTL_DEL);
}