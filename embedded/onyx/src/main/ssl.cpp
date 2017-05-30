#include <iostream>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

//http://fm4dd.com/openssl/sslconnect.htm
//https://wiki.openssl.org/index.php/Simple_TLS_Server
//https://wiki.openssl.org/index.php/SSL/TLS_Client
//gcc -lssl -lcrypto -o sslconnect sslconnect.c
//http://simplestcodings.blogspot.in/2010/08/secure-server-client-using-openssl-in-c.html

//https://www.openssl.org/docs/manmaster/ssl/SSL_CTX_load_verify_locations.html

#define DEFAULT_PORT_NUMBER                     443

int create_socket(char *, uint16_t port_num);

int openSSL_client_init()
{
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();

    if (SSL_library_init() < 0)
        return -1;
    return 0;
}

int openSSL_create_client_ctx(SSL_CTX **ctx)
{
    const SSL_METHOD *method = SSLv23_client_method();

    if ((*ctx = SSL_CTX_new(method)) == NULL)
        return -1;


    //SSL_CTX_set_options(*ctx, SSL_OP_NO_SSLv2);

    return 0;
}


static int verify(const char* certfile, const char* CAfile);
static X509 *load_cert(const char *file);
static int check(X509_STORE *ctx, X509 *x);

int verify(X509 *peer_certi, const char* CAfile)
{
    int ret=0;
    X509_STORE *cert_ctx=NULL;
    X509_LOOKUP *lookup=NULL;

    cert_ctx=X509_STORE_new();
    if (cert_ctx == NULL) 
        goto end;

    lookup=X509_STORE_add_lookup(cert_ctx, X509_LOOKUP_file());
    if (lookup == NULL)
        goto end;

    if (!X509_LOOKUP_load_file(lookup, CAfile,X509_FILETYPE_PEM))
        goto end;

    lookup=X509_STORE_add_lookup(cert_ctx, X509_LOOKUP_hash_dir());
    if (lookup == NULL)
        goto end;

    X509_LOOKUP_add_dir(lookup,NULL,X509_FILETYPE_DEFAULT);

    ret = check(cert_ctx, peer_certi);
end:
    if (cert_ctx != NULL) X509_STORE_free(cert_ctx);

    return ret;
}


static int check(X509_STORE *ctx, X509 *x)
{
    int i=0,ret=0;
    X509_STORE_CTX *csc;

    csc = X509_STORE_CTX_new();
    if (csc == NULL)
        goto end;
    X509_STORE_set_flags(ctx, 0);
    if(!X509_STORE_CTX_init(csc,ctx,x,0))
        goto end;
    i=X509_verify_cert(csc);
    X509_STORE_CTX_free(csc);

end:
    ret = (i > 0);

    return(ret);
}

#define LOAD_CERTI
int main(int argc, char *argv[])
{
    BIO *outbio = NULL;

    X509 *cert;
    X509_NAME *certname = NULL;

    SSL_CTX *ctx;
    SSL *ssl;
    int server = 0;
    int ret, i;

    if (openSSL_client_init()) {
        std :: cerr << "Could not initialize the OpenSSL library !" << std :: endl;
        return -1;
    }

    outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

    if (openSSL_create_client_ctx(&ctx)) {
        std :: cerr << "Unable to create a new SSL context structure." << std :: endl;
        return -1;
    }

//#ifdef LOAD_CERTI
    std :: cout << "Adding Certifcate" << std :: endl;
    if (SSL_CTX_load_verify_locations(ctx, "/home/abhiarora/Startup/Lock/embedded/onyx/src/main/ca.cert.pem", NULL) <= 0) {
        std :: cerr << "Unable to Load certificate" << std :: endl;
        return -1;
    }
//#endif
    //SSL_CTX_set_verify(&ctx, SSL_VERIFY_PEER, NULL);

    ssl = SSL_new(ctx);
    server = create_socket(argv[1], atoi(argv[2]));

    if (server < 0) {
        std :: cerr << "Error: Can't create TCP session" << std :: endl;
        return -1;
    }
    std :: cout << "Successfully made the TCP connection to: " << argv[1] << " port: " << atoi(argv[2]) << std :: endl;

    SSL_set_fd(ssl, server);

    if (SSL_connect(ssl) != 1) {
        std :: cerr << "Error: Could not build a SSL session to: " << argv[1] << std :: endl;
        return -1;
    }

    std :: cout << "Successfully enabled SSL/TLS session to: " << argv[1] << std :: endl;
    //SSL_SESSION *ss = SSL_get_session(ssl);

    cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL) {
        std :: cerr << "Error: Could not get a certificate from: " <<  argv[1] << std :: endl;
        return -1;
    }

    certname = X509_NAME_new();
    certname = X509_get_subject_name(cert);

    long res = SSL_get_verify_result(ssl);
    if(!(X509_V_OK == res)) 
        std :: cerr << "-------------Not Secured!-----------"    << std :: endl;

    if (verify(cert, "/home/abhiarora/Startup/Lock/embedded/src/main/ca-cert.pem") != 1) {
        std :: cerr << "Failed to verify!" << std :: endl;
    }

    std :: cout << "Displaying the certificate subject data:" << std :: endl;
    X509_NAME_print_ex(outbio, certname, 0, 0);
    std :: cout << std :: endl;


    char msg[100000] = "GET / HTTP/1.1\r\nHOST: www.siliconbolt.com\r\n\r\n";
    SSL_write(ssl, msg, strlen(msg));
    SSL_read(ssl, msg, 100000);
    std :: cout << "Message is " << msg << std :: endl;

    SSL_read(ssl, msg, 100000);
    std :: cout << "Message is " << msg << std :: endl;


    SSL_free(ssl);
    close(server);
    X509_free(cert);
    SSL_CTX_free(ctx);
    std :: cout << "Finished SSL/TLS connection with server" << std :: endl;
    return 0;
}


int create_socket(char *ip_cstr, uint16_t port_num)
{
    int fd;
    struct sockaddr_in dest_addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port_num);
    dest_addr.sin_addr.s_addr = inet_addr(ip_cstr);

    memset(&(dest_addr.sin_zero), '\0', 8);

    if (connect(fd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1)
        return -1;

    return fd;
}