# Server (ircserv)

## Overview

`ircserv` is a simple IRC server developed in C++98 that allows multiple users to connect, chat in channels, and send/receive messages using the IRC protocol.

## Features

- Supports multiple users
- Channel creation and management
- Basic user authentication
- Private messaging
- Operators with privileges
- Built-in quiz bot for multiple-choice questions

## Installation

1. Clone the repository:
   ```sh
   git clone <repository_url>
   cd ircserv
   ```
2. Compile the server:
   ```sh
   make
   ```
3. Run the server:
   ```sh
   ./ircserv <port> <password>
   ```
   - `<port>`: The port number the server will listen on.
   - `<password>`: The password required to connect.

## How to Use

### Connecting to the Server

Use an IRC client (e.g., HexChat, irssi, WeeChat) to connect:

```sh
/server <server_ip> <port> <password>
```

### Basic Commands

- Register a nickname:
  ```
  /nick <your_nickname>
  ```
- Set a username:
  ```
  /user <username> 0 * :<realname>
  ```
- Join a channel:
  ```
  /join #channel
  ```
- Send a message to a channel:
  ```
  /privmsg #channel <message>
  ```
- Send a private message:
  ```
  /privmsg <nickname> <message>
  ```
- Quit the server:
  ```
  /quit
  ```

### Operator Commands

- Kick a user from a channel:
  ```
  /kick #channel <user>
  ```
- Invite a user to a channel:
  ```
  /invite <user> #channel
  ```
- Change or view the channel topic:
  ```
  /topic #channel <new_topic>
  ```
- Change channel modes:
  ```
  /mode #channel <mode> [parameters]
  ```
  - `i`: Invite-only mode
  - `t`: Topic restricted to operators
  - `k`: Set a channel password
  - `o`: Grant/revoke operator privilege
  - `l`: Set user limit

## Running the Bot

To run the quiz bot, execute:
```sh
./bot <port> <password> <bot_nickname>
```
- `<port>`: The same port as the server.
- `<password>`: Must match the server’s password.
- `<bot_nickname>`: The bot's nickname.

### Quiz Bot Features
- Asks multiple-choice questions.
- Users interact with bot using `/privmsg <bot_nickname> <answer>`.
- Provides instant feedback on answers.
- The bot must be launched before interacting.
