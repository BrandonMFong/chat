# Chat Server

## Supports
- Linux
- macOS

## Description
I always wanted to make a chat server. I want to make the experience easy such that people that want to chat can just use nc or telnet

## Usage
Lobby:
![lobby](docs/lobby.png)

Chatroom:
![chatroom](docs/chatroom.png)

- launch
	- as server: `chat server`
	- as client: `chat -ip4 <server's ip address>`
- for help, enter `help` command

## Development
- Dependencies
	- `external/libs`: manual build is required
	- in `external/openssl`: manual build is required
			
- Environment
	- `make` - build tool
	- `g++` - compiler
	- `vim` - file editor
- Build
	- run `make help` for help
- Supported OS
	- Linux (debian)
	- macOS (arm64)

