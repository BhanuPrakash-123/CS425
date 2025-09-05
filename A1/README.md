# Assignment 1: CS425
### Code by Aman(220413), Bhanu(220583), Rithwin(220537)

## Section 1: Pre-requisites
For this terminal-based chat application, a UNIX-based terminal is required for the running of the server. For Windows OS, any Linux distro using WSL can be used as a server. A client can be run on any terminal.
In addition, the presence of a GCC compiler is necessary along with the GNU make command for compiling this server.

## Section 2: Starting the application
Open the A1 folder in the Linux terminal and use the make command to compile.
Once done, use the "./server_grp" to start the server. If successful, this gives the message "starting server on port 12345".
Make sure that the server is started on a UNIX-based terminal so that the application doesn't give errors on the client side of the application.

Note: If started on a non-UNIX terminal, the client-side commands will give an invalid command error.

Once the server is started, we can log in via the clients.
To log in as clients,
1) Run the command "./client_grp" wherein you will be prompted for a username and password.
2) If successful, you will be greeted by the message "Login successful! Welcome "username"".
3) In case of failure, start from step 1 again.

Once in, the user has a plethora of options wherein they can create new groups, broadcast messages to all the users, send private messages, etc. The sky is the limit.

### Implemented
- Basic server functionality: TCP-based server that listens on a specific port.
- Accepts multiple concurrent client connections.
- Maintains a list of connected clients with their usernames.
- User authentication using users.txt, which is the only way to add or delete users of the application.
- Broadcast messages to all connected clients.
- Private messages to specific users.
- Group communication: create, join, leave, and send messages to groups.

### Not Implemented
- None from the assignment specifications but could have added an encryption or safety mechanism for the users.txt file.
- Were only able to stress test to a limited extent of 100 users via bash testing but are sure that the number of users who can simultaneously run the application could be much higher.

## Design Decisions

### Threading
- A new thread is created for each client connection to handle multiple clients concurrently. This allows the server to manage multiple clients without blocking.
- We used std::thread for threading and std::mutex for synchronization.

### Synchronization
- We used std::lock_guard<std::mutex> to ensure thread-safe access to shared resources like clients and groups maps.
- This helps prevent data races when multiple threads try to read or modify shared resources simultaneously.

## Implementation

### High-Level Idea
#### Programming
- *Server Initialization*: Create a socket, bind it to a port, and listen for incoming connections.
- *Client Handling*: For each client, create a new thread to handle authentication and message processing.
- *Message Processing*: Parse incoming messages and execute commands like broadcast, private message, group message, etc.

#### Designing

- *Auto Deletion of Groups*: Groups will be automatically deleted if there are no users online in the group, which helps limit the number of groups.
- *Private Chat*: The number of users cannot be manipulated from the client side, and the only way to change it is through the users.txt file.

### Code Flow
1. *Main Function*: Initializes the server and listens for incoming connections.
2. *Client Handler*: Authenticates the client and processes messages in a loop.
3. *Message Processing*: Parses commands and executes corresponding functions.

## Testing

### Correctness Testing
- Tested basic functionality by connecting multiple clients and sending various types of messages.
- Verified that authentication works correctly and only authenticated users can send messages.
- Tested all the functions in most cases for any errors and ensured that they work.

### Stress Testing
- Carried out stress testing via using bash scripts to automate user connections and message sending.
- Successfully tested the application with 100 simultaneous users.
- However, we were unable to determine the exact number of users who can simultaneously run the application beyond this.

## Restrictions

### Client Connections
- At max, 100 clients can connect to the server simultaneously based on the macro, but this can be changed by modifying the macro.

### Groups
- No explicit limit on the number of groups or group members.
- Message size is limited to 1024 bytes.

## Challenges

### Design Strategy
- Initially considered using processes for each connection but switched to threads for better performance and resource management.
- Faced issues with synchronization and resolved them using std::mutex and std::lock_guard.
- Were unsure if the groups should remain if the number of active members became zero.

### Bugs
- Encountered bugs related to message parsing and fixed them by improving string handling and validation.
- The groups remain if the client was disconnected from the terminal with the CTRL+C keybind even if the group size was reduced to zero.

## Contribution of Each Member

### Member Contributions
- *Aman*: 33.33% - README, Server functions, and debugging.
- *Bhanu*: 33.33% - Client functions, debugging, and testing.
- *Rithwin*: 33.33% - Server functions, testing, and design decisions.

## Sources Referred
- Beej's Guide to Network Programming.
- C++ reference documentation.
- Various online tutorials and blogs on socket programming and multithreading.
- GFG Socket Programming.

## Declaration
We declare that we did not indulge in plagiarism and the work submitted is our own.

## Feedback
- The assignment was challenging and helped us understand socket programming and multithreading.
- We understood the value of group management to work efficiently.
