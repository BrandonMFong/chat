* Protocol I am planning to use is SIP or SIMPLE. Realized it may be nice for me to learn an industry standard as opposed to making my own. First pass will not implement the entire protocol, will use the spec as inspiration until I get a robust demo
* Decided to isolate the `Socket` class. May be a good idea to pluck this work out into its own library
* Each socket connection will have their own dedicated thread that polls the `recv` call.  This allows us to concurrently handle incoming data from multiple correspondents
* Each socket connection will have an agent representing the remote user on the other end. Having an agent seems to be the best way to id a user by the socket connection. I view an agent like a lawyer representing a client.
