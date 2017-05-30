#include "dns.h"
#include "utils.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define QUERY_INTERNET_CLASS    1

/*
 * +---------------------+
 * | Header              |
 * +---------------------+
 * | Question            | The question for the name server
 * +---------------------+
 * | Answer              | RRs answering the question
 * +---------------------+
 * | Authority           | RRs pointing toward an authority
 * +---------------------+
 * | Additional          | RRs holding additional information
 * +---------------------+
 *
 */

struct dns_header_s {
	uint16_t id;		// identification number
	uint8_t rd:1;		// recursion desired
	uint8_t tc:1;		// truncated message
	uint8_t aa:1;		// authoritive answer
	uint8_t opcode:4;	// purpose of message
	uint8_t qr:1;		// query/response flag
	uint8_t rcode:4;	// response code
	uint8_t cd:1;		// checking disabled
	uint8_t ad:1;		// authenticated data
	uint8_t z:1;		// its z! reserved
	uint8_t ra:1;		// recursion available

	uint16_t q_count;	// number of question entries
	uint16_t ans_count;	// number of answer entries
	uint16_t auth_count;// number of authority entries
	uint16_t add_count;	// number of resource entries
} __attribute__((packed));

struct dns_question_s {
	uint16_t qtype;
	uint16_t qclass;
} __attribute__((packed));

//Constant sized fields of the resource record structure
struct dns_rdata_s {
	uint16_t type;
	uint16_t _class;
	uint32_t ttl;
	uint16_t data_len;
} __attribute__((packed));


//Pointers to resource record contents
struct resource_record_s {
    char *name;
    struct dns_rdata_s *resource;
    char *rdata;
} __attribute__((packed));
 
//Structure of a Query
struct dns_query_s {
    char *name;
    struct dns_question_s *ques;
} __attribute__((packed));


int convToDnsNameFormat(char *qname, const char *hostname);
int readName(char *dest, char* src, int offset);
 
//On the i386 the host byte order is Least Significant Byte first, 
//whereas the network byte order, as used on the Internet, is Most Significant Byte first.

uint16_t getid()
{
	return 0x1234;
}


int create_dns_query(char *buf, char *hostname, uint16_t query_type)
{
	struct dns_header_s *dns = (struct dns_header_s *)buf;
	int len = sizeof(struct dns_header_s);

	dns->id = (uint16_t)hton_ui16(getid());
    dns->qr = 0; 		//This is a query
    dns->opcode = 0; 	//This is a standard query
    dns->aa = 0; 		//Not Authoritative
    dns->tc = 0; 		//This message is not truncated
    dns->rd = 1; 		//Recursion Desired
    dns->ra = 0; 		//Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;

    dns->q_count = hton_ui16(1); //Only Question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    char *qname = buf + len;
    len += (convToDnsNameFormat(qname, hostname) + 1);

    struct dns_question_s *question = (struct dns_question_s *)(buf + len);
    question->qclass = hton_ui16(QUERY_INTERNET_CLASS);
    question->qtype = hton_ui16(query_type);

    len += sizeof(struct dns_question_s);
	return len;
}
    
int handle_dns_response(char *buf, const char *hostname, struct ipv4_s *ip)
{
	struct dns_header_s *dns = (struct dns_header_s *)buf;
	unsigned len = sizeof(struct dns_header_s);

	char *qname = (buf + len);
	len += (strlen(qname) + 1);

	len += sizeof(struct dns_question_s);

    int ans_count = ntoh_ui16(dns->ans_count);
    int auth_count = ntoh_ui16(dns->auth_count);
    int add_count = ntoh_ui16(dns->add_count);
   
    int i;
    char name[100];
    for (i=0; i<ans_count; i++) {
        len = len + readName(name, buf, len);

        struct dns_rdata_s *resource = (struct dns_rdata_s *)(buf + len);
        len = len + sizeof(struct dns_rdata_s);
        
        if (ntoh_ui16(resource->type) == T_A) {
            if (!strcmp(hostname, name))
                util::inet_aton(buf + len, ip);
            len = len + ntoh_ui16(resource->data_len);
        }
    }
	return 0;
}
 

int readName(char *dest, char* src, int offset)
{
    unsigned char *ptr = (unsigned char *)(src + offset);
    bool jumped=false;
    int i=0, j=0, k=0;
    int count=1;
 
    //read the names in 3www6google3com format
    while (*ptr) {
        if (*ptr >= 192) {
            offset = ((*ptr) << 8) + *(ptr + 1) - 49152; //49152 = 11000000 00000000 ;)
            ptr = (unsigned char *)(src + offset - 1);
            jumped = true; //we have jumped to another location so counting wont go up!
        } else {
            dest[k++]=*ptr;
        }
        ptr = ptr + 1;
        if(!jumped)
            count++; //if we havent jumped to another location then we can count up
    }
    dest[k]='\0'; //string complete
    if (jumped)
        count++; //number of steps we actually moved forward in the packet
 
    //now convert 3www6google3com0 to www.google.com
    int len = strlen(dest);
    for (i=0; i<len; i++) {
        k=dest[i];
        for (j=0; j<k; j++, i++)
            dest[i]=dest[i+1];
        dest[i]='.';
    }
    dest[i-1]='\0'; //remove the last dot
    return count;
}

 
int convToDnsNameFormat(char* dns_format, const char* host_format) 
{
    int i, j, k, count;
    for (i=0, j=0, k=0, count=0; ;i++) {
    	if (host_format[i] && (host_format[i] != '.')) {
    		count++;
    		continue;
    	}
    	int temp = count;
    	if (count > 9) {
    		temp = temp/10;
    		dns_format[j++] = temp;
    		temp = count - (temp*10);
    	}
    	dns_format[j++] = temp;
    	for (k=i-count; (i - k) > 0; k++)
    		dns_format[j++] = host_format[k];
    	count = 0;
    	if (!host_format[i])
    		break;
    }
    dns_format[j] = '\0';
    return j;
}


#include <sys/socket.h>    //you know what this is for
#include <arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define DNS_PORT_NUMBER 	53

char buf[BUFFER_MAX_LEN];
char hostname[] = "www.google.com";
char dns_ip_addr[20];

char *get_dns_ip(void)
{
    strcpy(dns_ip_addr , "208.67.222.222");
    return dns_ip_addr;
}

int main(int argc, char *argv[])
{
	int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock_fd < 0) {
		perror("Error: In opening Socket for DNS\n");
		return -1;
	}

	struct sockaddr_in dns_sock;
	//memset(&dns_sock, 0, sizeof(struct sockaddr_in));

	dns_sock.sin_family = AF_INET;
    dns_sock.sin_port = hton_ui16(DNS_PORT_NUMBER);
    dns_sock.sin_addr.s_addr = inet_addr(get_dns_ip());

    int len = create_dns_query(buf, hostname, T_A);

    if (sendto(sock_fd, buf, len, 0, (struct sockaddr*)&dns_sock, sizeof(dns_sock)) < 0) {
        perror("Error: Cannot Send\n");
        return -1;
    }
     
    len = sizeof(dns_sock);
    if (recvfrom(sock_fd, buf, BUFFER_MAX_LEN, 0, (struct sockaddr*)&dns_sock , (socklen_t*)&len) < 0) {
        perror("Error: In Receving\n");
        return -1;
    }
    
    struct ipv4_s ip;
    handle_dns_response(buf, hostname, &ip);
    util::printIP(&ip);
 
    return 0;
}