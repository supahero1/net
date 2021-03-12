/*
   Copyright 2021 shädam

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef W_pOOj__2VkXM_ew9zpRf2pZH_H8ggTe
#define W_pOOj__2VkXM_ew9zpRf2pZH_H8ggTe 1

#ifdef __cplusplus
extern "C" {
#endif

#include "../def.h"
#include "../net/net_avl.h"
#include "../distr/distr.h"

#include <netdb.h>
#include <stdint.h>
#include <signal.h>

/*struct HTTP_URL {
  char* protocol;
  char* host;
  char* origin;
  char* port;
  char* pathname;
  char* query;
  char* hash;
  
  uint16_t origin_len;
  uint16_t pathname_len;
  uint16_t query_len;
  uint16_t hash_len;
  uint8_t protocol_len;
  uint8_t host_len;
  uint8_t port_len;
};

__nonnull((1))
extern struct HTTP_URL ParseHTTP_URL(const char* const);

struct HTTPSocket {
  uint8_t state;
};*/

enum NETFlags { NET_CONNECTING, NET_OPEN, NET_CLOSED };

extern void SocketNoBlock(const int sfd);

__nonnull((4))
extern int GetAddrInfo(const char* const, const char* const, const int, struct addrinfo** const restrict);

struct ANET_GAILookup {
  void (*callback)(struct addrinfo*, int);
  char* hostname;
  char* service;
  int flags;
};

struct ANET_GAIArray {
  struct ANET_GAILookup* arr;
  uint_fast32_t count;
};

__nonnull((1))
extern int AsyncGetAddrInfo(struct ANET_GAIArray* const);

struct NETSocket {
  struct sockaddr addr;
  char* canonname;
  socklen_t addrlen;
  enum NETFlags state;
  int flags;
  int family;
  int socktype;
  int protocol;
  int sfd;
};

__nonnull((1))
extern int SyncTCPConnect(struct addrinfo* const, struct NETSocket* restrict);

__nonnull((1))
extern int SyncTCP_GAIConnect(const char* const, const char* const, struct NETSocket* restrict);

__nonnull((1))
extern int SyncTCP_IP_GAIConnect(const char* const, const char* const, struct NETSocket* restrict);

__nonnull((1))
extern int SyncTCPListen(struct addrinfo* const, struct NETSocket* restrict);

__nonnull((1))
extern int SyncTCP_GAIListen(const char* const, struct NETSocket* restrict);

struct NETConnectionManager {
  pthread_t thread;
  struct net_avl_tree avl_tree;
  int epoll;
};

__nonnull((1))
extern int InitConnectionManager(struct NETConnectionManager* const, const uint32_t);

__nonnull((1))
extern int AddSocket(struct NETConnectionManager* const, const int, void (*)(int, uint32_t));

__nonnull((1))
extern int DeleteSocket(struct NETConnectionManager* const, const int);

__nonnull((1))
extern void FreeConnectionManager(struct NETConnectionManager* const);

struct ANET {
  void (*callback)(struct NETSocket*, int);
  struct addrinfo* addrinfo;
};

__nonnull((1))
extern int AsyncTCPConnect(struct ANET* const);

__nonnull((1))
extern int AsyncTCPListen(struct ANET* const);

#ifdef __cplusplus
}
#endif

#endif // W_pOOj__2VkXM_ew9zpRf2pZH_H8ggTe