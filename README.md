# Multi-Client Chat Server  
### C • TCP Sockets • POSIX Threads • Concurrency

A lightweight multi-client chat system written in C.  
Multiple clients connect to a central server and exchange messages in real time.  
Each client uses two pthreads (send + receive), and the server uses one thread per client.

---

## Features
- Multi-client TCP chat with real-time message broadcasting  
- Server spawns a pthread for every client  
- Client uses separate send/receive threads  
- Length-prefixed message protocol  
- Graceful shutdown on `Ctrl+C`  
- Thread-safe logging with mutexes  
- Clean modular C architecture  

---

## Build + Quickstart (copy & paste)


#### Git clone the repository

```bash
# 1. Clone the repository
git clone https://github.com/CapeGenius/Multi-Client-Chat-Server.git
cd Multi-Client-Chat-Server
```

#### Create executables for the router, client, and server

1. Create executable for router
```bash
gcc -o router router.c
```

2. Create executable for client
```bash
cd client
gcc -o client client.c client_helpers.c
```

3. Create executable for server
```bash
cd server
gcc -o server server.c logger.c server_setup.c fd_list.c
```

#### Now, you can run your project:

```bash
# 2. Build the project using router executable
./router

# 3. Start the server 
After running ./router in terminal you will get

' Do you want to run a server (1) or client (2):
1) Server
2) Client
Enter choice: '

Enter 1. This will start a server listening on port 8080

# 4. In another terminal, start a client
After running ./router in terminal you will get

' Do you want to run a server (1) or client (2):
1) Server
2) Client
Enter choice: '

Enter 2. 

Then you will get

Type Remote IP Address:

Enter in the remote IP address you want to connect to.

Finally, enter in the associated port.

# 5. Open more terminals and run more clients to chat

## Creators
This project was created by:
- **Ahan Trivedi**
- **Akshat Jain]**
- **Rohan Bendapudi**

