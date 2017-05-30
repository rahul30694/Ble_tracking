#ifndef _DNS_H_
#define _DNS_H_

#define BUFFER_MAX_LEN          65536

//Types of DNS Resource Records
#define T_A 		1 	// Ipv4 address
#define T_NS 		2 	// Nameserver
#define T_CNAME 	5 	// canonical name
#define T_SOA 		6 	// start of authority zone
#define T_PTR 		12 	// domain name pointer
#define T_MX		15 	// Mail server

#endif