* protocol I am planning to use is SIP or SIMPLE. Realized it may be nice for me to learn an industry standard as opposed to making my own
* Decided to isolate the `Socket` class. May be a good idea to pluck this work out into its own library
* each socket connection will have their own dedicated thread that polls the `recv` call.  This allows us to concurrently handle incoming data from multiple correspondents
* first pass wont include a full implementation of SIP or of any protocol. My goal to get proof of concept out the door with a working application.
