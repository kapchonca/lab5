#ifndef LAB5_H_SERVER_H_
#define LAB5_H_SERVER_H_

#include <SFML/Audio.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>

#include "../h/thread_pool.h"

using boost::asio::ip::tcp;

/**
 * @brief Class for an audio server that streams audio data to connected clients.
 */
class AudioServer {
 public:
  /**
     * @brief Constructs an AudioServer object.
     * @param endpoint The TCP endpoint to listen on.
     */
  AudioServer(const tcp::endpoint& endpoint)
      : io_context_(), acceptor_(io_context_, endpoint), socket_(io_context_) {
    StartServer();
  }

 private:
  /**
     * @brief Starts the server and waits for clients to connect.
     */
  void StartServer();

  /**
     * @brief Handles a connected client.
     * @param socket The TCP socket for communication with the client.
     */
  void HandleClient(tcp::socket socket);

  /**
     * @brief Sends track metadata over a TCP socket.
     * @param socket A reference to the TCP socket used for communication.
     * @param buffer The sound buffer containing the audio track's data.
     */
  void SendTrackMetaData(tcp::socket& socket, const sf::SoundBuffer& buffer);

  /**
     * @brief Starts streaming audio data to the connected client.
     * @param socket The TCP socket for communication with the client.
     * @param track_name The name of the track to stream to the client.
     */
  void StartAudioStream(tcp::socket& socket, std::string track_name);

  /**
     * @brief Receives the name of the track to play from the client.
     * @param socket The TCP socket for communication with the client.
     * @return std::string The received name of the track.
     */
  std::string ReceiveTrackName(tcp::socket& socket);

  boost::asio::io_context io_context_;  ///< The Boost ASIO IO context.
  tcp::acceptor acceptor_;  ///< The TCP acceptor for incoming connections.
  tcp::socket socket_;  ///< The TCP socket for communication with the client.
  ThreadPool thread_pool_{std::thread::hardware_concurrency()};
};

#endif  // LAB5_H_SERVER_H_
