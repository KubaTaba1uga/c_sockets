# C sockets

Resources:
 - https://beej.us/guide/bgnet/html//index.html

## Intro 
Repository for learning about sockets in C.

Each nested directory contain some server application.
Application task is to counts fibbonaci numbers using one algo.

The challenge is to process as many request per second as possible.
Exploring sockets and parallel computaion is a goal here.

There is a script wriiten specially for testing requests per second:
  `python count_requests_per_second.py`

There is a script wriiten specially for testing one big request time:
  `python count_request_time.py`

## Socket Types

There are all kinds of sockets. There are DARPA Internet addresses (Internet Sockets), path names on a local node (Unix Sockets), CCITT X.25 addresses (X.25 Sockets that you can safely ignore), and probably many others depending on which Unix flavor you run. This document deals only with the first: Internet Sockets.

## Stream Socket Intro

Stream sockets are reliable two-way connected communication streams. If you output two items into the socket in the order “1, 2”, they will arrive in the order “1, 2” at the opposite end. They will also be error-free.

How do stream sockets achieve this high level of data transmission quality?

They use a protocol called “The Transmission Control Protocol”, otherwise known as “TCP” (see RFC 7939 for extremely detailed info on TCP). TCP makes sure your data arrives sequentially and error-free. 

## Datagram Socket Intro

If you send a datagram, it may arrive. It may arrive out of order. If it arrives, the data within the packet will be error-free.

Datagram sockets also use IP for routing, but they don’t use TCP; they use the “User Datagram Protocol”, or “UDP” (see RFC 76811).

Why are they connectionless? Well, basically, it’s because you don’t have to maintain an open connection as you do with stream sockets. You just build a packet, slap an IP header on it with destination information, and send it out. No connection needed. They are generally used either when a TCP stack is unavailable or when a few dropped packets here and there don’t mean the end of the Universe. Sample applications: tftp (trivial file transfer protocol, a little brother to FTP), dhcpcd (a DHCP client), multiplayer games, streaming audio, video conferencing, etc.

## Standard C library

The entrypoint to sockets is `getaddrinfo`. According to man pages:
Given node and service, which identify an Internet host and a service, getaddrinfo() returns one or more `addrinfo` structures, each of which contains an Internet address that can be specified in a call to bind(2) or connect(2). 

Addr info looks like this:
```
struct addrinfo {
	int              ai_flags;
	int              ai_family;
	int              ai_socktype;
	int              ai_protocol;
	socklen_t        ai_addrlen;
	struct sockaddr *ai_addr;
	char            *ai_canonname;
	struct addrinfo *ai_next;
};
```
```
struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
}; 
```

To deal with struct sockaddr, programmers created a parallel structure: struct sockaddr_in (“in” for “Internet”) to be used with IPv4.

And this is the important bit: a pointer to a struct sockaddr_in can be cast to a pointer to a struct sockaddr and vice-versa. So even though connect() wants a struct sockaddr*, you can still use a struct sockaddr_in and cast it at the last minute!

```
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};
```
sin_zero (which is included to pad the structure to the length of a struct sockaddr) should be set to all zeros with the function memset().

```
struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};
```

First, let’s say you have a struct sockaddr_in ina, and you have an IP address “10.12.110.57” or “2001:db8:63b3:1::3490” that you want to store into it. The function you want to use, inet_pton(), converts an IP address in numbers-and-dots notation into either a struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6. (“pton” stands for “presentation to network”—you can call it “printable to network” if that’s easier to remember.) The conversion can be made as follows:

```
struct sockaddr_in sa; // IPv4
struct sockaddr_in6 sa6; // IPv6

inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); // IPv4
inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr)); // IPv6
```

or in the other way

```
// IPv4:
char ip4[INET_ADDRSTRLEN];// space to hold the IPv4

struct sockaddr_in sa;// pretend this is loaded with string something

inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);

printf("The IPv4 address is: %s\n", ip4);
```
