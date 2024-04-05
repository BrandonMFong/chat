# Chat Server

## Supports
- Linux (debian 11)
- macOS (13.6.1)

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

## Protocol
- Handshake
    - Server and Client exchange information
- Message exchange
- Termination
    - User intending to leave will notify all users on server that they are leaving

## Development
- Dependencies
    - create an `external` folder at the base of the repo and clone these repos
        - https://github.com/BrandonMFong/libs
- Environment
	- `make` - build tool
	- `g++` - compiler
	- `vim` - file editor
- Supported OS
	- Linux (debian)
	- macOS (arm64)

