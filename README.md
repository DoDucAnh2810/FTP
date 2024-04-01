FTP Server
==========

Utilization:
------------
1. **make** : Create the executables, distribute them to clusters. Intialize the content of clusters.
2. **make start_server** : Start clusters and master server in the background.
3. **make client** : Lauch a client from the executable in the "Clients" directory. All files are downloaded to that directory.
4. **make end_server** : Terminate running clusters and master server. (Must be called in the same terminal as "make start_server")
5. **make clean** : Delete the executables and clear the content of clusters.

Client's commands:
------------------
**get 'filepath'** : Download the file located at 'filepath' on the server.
**bye** : Terminate the client.
**ls** : List the content of the current directory of the server.
**pwd** : Print the current working directory of the server.
**cd 'dirpath'** : Change the current working directory to 'dirpath'.

Server's possible responses:
----------------------------
### Error (printed in red):
+ No such file
+ Permission denied
+ Unknown error
+ Permission denied
+ Is a directory
+ Empty command line
+ Empty get request
+ Unsupported option/argument
+ Failed to change directory
+ Unknown command
### "get" request's response protocol:
Successful request (printed in green)
[size of file]
[file content]
End
### Other commands:
Successful command (not printed)
[command output]

Clusters (slave servers):
-------------------------
There are four clusters located within the "Clusters" directory:
+ Nord
+ East
+ West
+ South
They contain a copy of 'ftpserver' and identical files (after running "make"). They are assigned clients based on a rolling basis by the master server (executable located in the "Master" directory).

File structures:
----------------
### Directories:
+ Clients: Contains the executable 'ftpclient'. Clients created by "make client" download files to this directory.
+ Clusters: Contains four clusters as mentionned above.
+ Files: Contains the original files to be distribute to clusters as testing data.
+ Master: Contains the executable 'ftpmaster' and a JSON file tracking the information of the four clusters.
+ scirpts: Contains shell scripts used by the Makefile.
### Modules:
+ cJSON: Helps with parsing JSON files.
+ csapp: Wrapper for UNIX calls.
+ utils: Contains utilities used by the main programs.
### Main programs:
+ ftpclient.c: A command-line based FTP client.
+ ftpserver.c: Receives request from client(s) and send back appropriate responses.
+ ftpmaster.c: A load balancer, distributes clients to clusters.


Tests:
------
### Correctness:
TODO by Maxim
### Concurrency:
TODO by Duc

