# httpserver
Starts up a multi-threaded server that can accept PUT/GET/Head requests.

# SYNOPSIS
httpserver [port number] [options]

# DESCRIPTION
  - Starts up a server that can accept PUT/GET/Head requests.
  - On a PUT request, server will take data from the client socket and put the information on the server.
  - On a GET request, server will take data from the server, and put the information on the client socket.
  - On a HEAD request, server will read data from the server, and display the content length of the file.
  - The options supported are -N and -l
  - -N 
    - The number of threads the server will start with
  - -l
    - The name of the logging file to write to.

# Return Value
  - Server will return a header along with optional data to read if the request is a GET request.
  - Error code that are supported are:
    - 200 - OK
    - 201 - Created
    - 400 - Bad request
    - 403 - Forbidden
    - 404 - Not found
    - 500 - Internal Server Error

# Examples
   - httpserver 8080
     curl -s -T file.txt http://localhost:8080/server_file
      - Takes the contents in the client's file.txt, and stores the information on the server's server_file. Note that the server has 4 threads.
  - httpserver 8080 -N 10
    curl -s http://localhost:8080/server_file > file.txt
     - Takes the contents in the server's server_file, and stores the information on the client's file.txt. Note that the server has 10 threads.
  - httpserver 8080 -l logging.log
    curl -s -I http://localhost:8080/server_file
    - Reads the contents in the server's server_file, and return the content lengths of the file. Note that the requests will be written to logging.log.

# Author
   - Written by Vinson Chen.
