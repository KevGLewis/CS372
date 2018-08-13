Testing Machine: I ran it on Flip2/3


Server Instructions:
----Compilation: Run the compileall script (./compileall). This will create a chat client executable.

----Execution: To run the program, call the ftserver program with the following in-line variables:

	USAGE: ./ftserver <PORTNUM>

----More Information: Waits on a client to connect to the server, then will send a file, file list, or error message back. Enter CTRL-C to quit.

Client Instructions:
----Compilation: Does not require any compilation

----Execution: Run the client as follows:

	USAGE ./ftclient <host> <PORTNUM> <COMMAND (Either -l or -g <FILENAME>) <PORTNUM>

----MoreInformation: The client connects to the server and establishes a connection. Once established, it sends the command to the client. At which point, the server will complete the command or return an error message