#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <mutex>
#include <cstring>
#include <fstream>
#include <sstream>

#define PORT 12345
#define MAX_CLIENTS 100

 // this is for client sockets and username
std::map<int, std::string> clients; 

 // Stores username and password
std::map<std::string, std::string> users;

// this is to protect shared data
std::mutex client_mutex;            

std::map<std::string, std::set<std::string>> groups;
std::mutex group_mutex;

//to load user credentails from file
void load_users() {
    std::ifstream file("users.txt");
    if (!file) {
        std::cerr << "Error: Could not open users.txt\n";
        return;
    }

    std::string line, usr, pwd;
    while (std::getline(file, line)) {
        size_t delimiter = line.find(':');
        if (delimiter != std::string::npos) {
            usr = line.substr(0, delimiter);
            pwd = line.substr(delimiter + 1);
            if(pwd.back() == '\r' || pwd.back() == '\n') {
                pwd.pop_back();
            }
            users[usr] = pwd;
        }
    }
    file.close();
}

//privatee message function
void send_private_msg(const std::string& sender, const std::string& recipient, const std::string& msg) {
    std::lock_guard<std::mutex> lock(client_mutex);
    for (const auto& [socket, user] : clients) {
        if (user == recipient) {
            std::string full_msg = "(Private) " + sender + ": " + msg;
            send(socket, full_msg.c_str(), full_msg.length(), 0);
            return;
        }
    }
    // Send error message back to the sender
    for (const auto& [socket, user] : clients) {
        if (user == sender) {
            std::string error_msg = "Error: User " + recipient + " is not online or does not exist.";
            send(socket, error_msg.c_str(), error_msg.length(), 0);
            return;
        }
    }
}
//to broadcast message to all clients
void broadcast_msg(const std::string& msg, int sender_socket) {
    std::lock_guard<std::mutex> lock(client_mutex);
    for (const auto& [socket, user] : clients) {
        if (socket != sender_socket) {
            send(socket, msg.c_str(), msg.length(), 0);
        }
    }
}

//to create group
void create_group(const std::string& usr, const std::string& group_name, int client_socket) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(group_name) == groups.end()) {
        groups[group_name] = {usr};
        std::string msg = "Group '" + group_name + "' created successfully!\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
    } else {
        std::string msg = "Error: Group '" + group_name + "' already exists!\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
    }
}

//to join group
void join_group(const std::string& usr, const std::string& group_name, int client_socket) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(group_name) != groups.end()) {
        groups[group_name].insert(usr);
        std::string msg = "You have joined the group '" + group_name + "'.\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
    } else {
        std::string msg = "Error: Group '" + group_name + "' does not exist!\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
    }
}

//to leave group
void leave_group(const std::string& usr, const std::string& group_name, int client_socket) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(group_name) != groups.end() && groups[group_name].count(usr)) {
        groups[group_name].erase(usr);
        std::string msg = "You have left the group '" + group_name + "'.\n";
        send(client_socket, msg.c_str(), msg.length(), 0);

        if (groups[group_name].empty()) {
            groups.erase(group_name);
        }
    } else {
        std::string msg = "Error: You are not a member of group '" + group_name + "'.\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
    }
}


//to send a group message
void send_group_msg(const std::string& sender, const std::string& group_name, const std::string& msg) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(group_name) != groups.end() && groups[group_name].count(sender)) {
        std::string full_msg = "(Group " + group_name + ") " + sender + ": " + msg;
        
        std::lock_guard<std::mutex> client_lock(client_mutex);
        for (const auto& user : groups[group_name]) {
            for (const auto& [socket, usr] : clients) {
                if (usr == user && usr != sender) {
                    send(socket, full_msg.c_str(), full_msg.length(), 0);
                }
            }
        }
    }
    else {
        // to send error message back to the sender
        std::lock_guard<std::mutex> client_lock(client_mutex);
        for (const auto& [socket, usr] : clients) {
            if (usr == sender) {
                std::string error_msg = "Error: You are not part of the group " + group_name + " or the group doesn't exist.";
                send(socket, error_msg.c_str(), error_msg.length(), 0);
                return;
            }
        }
    }
}

//this is to remove client
void remove_client(int client_socket, const std::string& usr) {
    std::lock_guard<std::mutex> lock(client_mutex);
    clients.erase(client_socket);
    
    //to remove the user from all groups
    std::lock_guard<std::mutex> group_lock(group_mutex);
    for (auto& [group_name, members] : groups) {
        members.erase(usr);
    }
    close(client_socket);
}

void handle_client(int client_socket) {
    char buffer[1024];

    std::string msg = "Enter username: ";
    send(client_socket, msg.c_str(), msg.length(), 0);

    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }
    buffer[bytes_received] = '\0';
    std::string usr(buffer);

    msg = "Enter password: ";
    send(client_socket, msg.c_str(), msg.length(), 0);

    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        std::cout << usr << " has disconnected.\n";
        remove_client(client_socket, usr);
        return;
    }

    buffer[bytes_received] = '\0';
    std::string pwd(buffer);

    // Validation
    if (users.find(usr) == users.end() || users[usr] != pwd) {
        std::string error_msg = "Error: Authentication failed. Disconnecting...\n";
        send(client_socket, error_msg.c_str(), error_msg.length(), 0);
        close(client_socket);
        return;
    }

    // Prevention of duplicate logins
    {
        std::lock_guard<std::mutex> lock(client_mutex);
        for (const auto& [sock, name] : clients) {
            if (name == usr) {
                std::string msg = "Error: Username already logged in!\n";
                send(client_socket, msg.c_str(), msg.length(), 0);
                close(client_socket);
                return;
            }
        }
        clients[client_socket] = usr;
    }

    // success in authentication
    std::string success_msg = "Login successful! Welcome " + usr + "\n";
    send(client_socket, success_msg.c_str(), success_msg.length(), 0);
    std::string join_msg = usr + " has joined the chat.\n";
    broadcast_msg(join_msg, client_socket);

    std::cout << usr << " has joined the chat.\n";

    // messaging 
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;

        buffer[bytes_received] = '\0';
        std::string msg(buffer);

        if (msg.rfind("/broadcast ", 0) == 0) {
            if (msg.length() > 11) {
                std::string msg_content = usr + ": " + msg.substr(11);
                broadcast_msg(msg_content, client_socket);
            }
        } 
        else if (msg.rfind("/msg ", 0) == 0) {
            size_t first_space = msg.find(' ', 5);
            if (first_space != std::string::npos) {
                std::string recipient = msg.substr(5, first_space - 5);
                std::string msg_content = msg.substr(first_space + 1);
                send_private_msg(usr, recipient, msg_content);
            }
        }
        else if (msg.rfind("/create_group ", 0) == 0) {
            if (msg.length() > 14) {
                std::string group_name = msg.substr(14);
                create_group(usr, group_name, client_socket);
            }
        } 
        else if (msg.rfind("/join_group ", 0) == 0) {
            if (msg.length() > 12) {
                std::string group_name = msg.substr(12);
                join_group(usr, group_name, client_socket);
            }
        } 
        else if (msg.rfind("/leave_group ", 0) == 0) {
            if (msg.length() > 13) {
                std::string group_name = msg.substr(13);
                leave_group(usr, group_name, client_socket);
            }
        } 
        else if (msg.rfind("/group_msg ", 0) == 0) {
            size_t first_space = msg.find(' ', 11);
            if (first_space != std::string::npos) {
                std::string group_name = msg.substr(11, first_space - 11);
                std::string group_msg = msg.substr(first_space + 1);
                send_group_msg(usr, group_name, group_msg);
            }
        } 
        else {
            std::string error_msg = "Error: Invalid command. Please use a valid command.\n";
            send(client_socket, error_msg.c_str(), error_msg.length(), 0);
        }
    }

    // Removal of client on disconnect
    remove_client(client_socket, usr);
    std::cout << usr << " has disconnected.\n";
}

int main() {
    load_users();
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Creation of server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    //to set up server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // to bind socket to port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Binding failed\n";
        return 1;
    }

    // this is to listen for connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        std::cerr << "Listening failed\n";
        return 1;
    }

    std::cout << "Server started on port " << PORT << "...\n";

    while (true) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            std::cerr << "Failed to accept client\n";
            continue;
        }

        std::cout << "New client connected\n";
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }

    close(server_socket);
    return 0;
}