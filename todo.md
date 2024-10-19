```
x.x
[] user data window editor
[] ability to change user name
[] window dynamic resizing
[] get list of users in chatroom
[] add to homebrew package manager

0.3
[] improved controls (replicate nano commands)
[] chat configuration implemented in ncurses
[] user typing

0.2.1
[x] issue with sending messages between two machines that are on macos and linux
	[] test on two macs
[] build a macos universal binary
[] properly quit to make sure the receiver isn't overworking

0.2
[x] cross platform
[x] configure a local openssl usage in this repo
[x] chatroom encryption
	[x] make chatroom symmetric key
	[x] make user assymetic keys
	[x] symmetric key for chatroom exchange
	[x] encrypt chatroom message
		[x] create a step before joining a chatroom. Use it to ask for permission and grant chat key
		[x] add logging to the enrollment logging
		[x] fix issue with packet size
		[x] there is an issue with using the chatroom key to decrypt
			- https://github.com/openssl/openssl/discussions/24924
[x] separate sources for libs

0.1
[x] make linux build
[x] make macos build
[x] test release builds
[x] versioning mechanism
[x] license
[x] fix cpu usage. possibly make all while loops more efficient by using locks
[x] figure out why macos ui is buggy when message is sent
[x] chatrooms
[x] create chatroom
[x] join chatroom
[x] keep record of users in chatroom
[x] finish help window
[x] create custom name
[x] use the correct index for join
[x] fix command line args
[x] make ip and port configurable
[x] notify when user joins/leaves chatroom
[x] broadcast that chatroom list updated
[x] make sure command permissions are set
[x] make error dialog
[x] make sure multiple users can join
[x] gracefully exit server after "quit"
[x] ensure user data is shared when joining server
[x] message character limit
[x] make user list
[x] review for any leaks
[x] make instream packet queue be application specific
[x] backspace does not work on macos machines
```

