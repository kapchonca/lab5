# Audio Streaming Server and Client

This project consists of a server and client application for streaming audio using Boost.Asio for network programming and SFML for audio processing. The server accepts connections from clients, handles audio streaming requests, and logs events, while the client connects to the server, sends audio track requests, and handles audio playback and user commands.

## Key Features

- **Server (AudioServer)**
  - Listens for incoming client connections.
  - Handles client requests to stream audio tracks.
  - Logs client connections, disconnections, and errors.

- **Client (AudioClient)**
  - Connects to the server and requests audio tracks.
  - Receives and plays audio tracks streamed from the server.
  - Handles user commands for pausing, unpausing, and exiting the session.

## Dependencies

- Boost.Asio: For network communication.
- SFML: For audio processing and playback.
- Boost.Log: For logging server events.

## How to Build and Run 

1. **Install Dependencies**:
   You can install dependencies using [Conan](https://conan.io/) or manually.

   **Using Conan**:
   ```bash
   conan install . --build=missing
   ```

   **Manually**:
   ```bash
   sudo apt install libsfml-dev libboost-all-dev
   ```

2. **Build the Server**:
   Navigate to the root directory of the project and run CMake.

   ```bash
   cd <root project directory>
   mkdir build
   cd build
   cmake ..
   make
   ./lab5
   ```

3. **Run the Server**:
   ```bash
   ./lab5
   ```
   
4. **Build the Client**:
   On a separate machine or in a separate terminal, navigate to the client directory and run CMake.

   ```bash
   cd <root project directory>
   cd src/client
   mkdir build
   cd build
   cmake ..
   make
   ./client
   ```

5. **Run the Client**:
   ```bash
   ./client <address> <port>
   ```

## Logging

The server logs events to both the console and log files located in the `../src/logs/` directory.

## User Commands (Client)

- `<track name>`: Request a track to play (the track has to be on the server's machine in the folder named `tracks` with the `.flac` extension).
- `pause`: Pauses the current track.
- `unpause`: Resumes the paused track.
- `exit`: Exits the client session.
