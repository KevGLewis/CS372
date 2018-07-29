Testing Machine: Due to issues with the flip server, I was not able to fully test on a flip machine. However, it should work on either flip1 or flip2


Client Instructions:
----Compilation: Run the compileall script (./compileall). This will create a chat client executable.

----Execution: To run the program, call the chatclient program with the following in-line variables:

	USAGE: ./chatclient <host> <PORTNUM>

----More Information: You will initially be prompted for a user handle when starting the program. This handle will be prepended onto your message when communicating with the server. The client sends the initial message to the server, then waits for the server to respond. If you wish to quit, send "\quit" on your turn.

Server Instructions:
----Compilation: Does not require any compilation

----Execution: Run the server as follows:

	USAGE ./chatserver <PORTNUM>

----MoreInformation: The server will wait for the client to connect. Once connected, the client sends the first message. Afterward the server and client alternate sending messages. If you wish to disconnect from the client, send "\quit" on your turn. This will end the client connection and set the server to wait for another client to connect. To close the server, press CTRL-C.