Compile TCP client with ‘g++ mainclient.cpp -o mainclient’. Run with ‘./mainclient’
Compile TCP master server with ‘g++ mainserver.cpp -o mainserver’ 
Run with ‘./mainserver’. Carey’s wordlength code was used as a basis for the TCP master server.

The rest of the UDP micro servers are named:
  identity.cpp; This server echos the string back 
  Rachel’s echoserver code was used as a basis for the identity server
  reverse.cpp; This server reverses the string
  upper.cpp; This server capitalizes all letters in the string
  lower.cpp; This server changes all letters to lower case
  caesar.cpp; This server uses a shift key=13 to shift the letters
  yours.cpp; This server implements a multi-shift cipher for all the letters
They can all be compiled with ‘g++ {name} -o {name without .cpp}’ 

Run all the servers first, then run the mainclient
