
#include <sys/socket.h>
#include <string>

const int BUFSIZE = 1024;
class Socket {
    public:
        Socket();
        Socket(int fd);
        ~Socket();
        int SetAddrPortReuse();
        int Bind(int port);
        int Listen();
        int Accept();
        int Connect(std::string addr, bool keep_alive = false);
        int Send(char* msg);
        int Recev();
        int SetSocketNonBlocking();
        int Fd(){
            return socket_fd;
        }
        char* ReadBuf(){
            return read_buf;
        }
        
    private:
        int socket_fd;
        struct sockaddr_in* address;
        char read_buf[BUFSIZ];
};

struct KeepConfig {
    /** The time (in seconds) the connection needs to remain 
     * idle before TCP starts sending keepalive probes (TCP_KEEPIDLE socket option)
     */
    int keepidle;
    /** The maximum number of keepalive probes TCP should 
     * send before dropping the connection. (TCP_KEEPCNT socket option)
     */
    int keepcnt;

    /** The time (in seconds) between individual keepalive probes.
     *  (TCP_KEEPINTVL socket option)
     */
    int keepintvl;
};