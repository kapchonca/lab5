#ifndef LAB5_H_SERVER_H_
#define LAB5_H_SERVER_H_

#include <SFML/Audio.hpp>
#include <boost/asio.hpp>
#include <iostream>

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
   * @brief Starts the server and waits for a client to connect.
   */
  void StartServer();

  /**
   * @brief Starts streaming audio data to the connected client.
   */
  void StartAudioStream();

  boost::asio::io_context io_context_;  ///< The Boost ASIO IO context.
  tcp::acceptor acceptor_;  ///< The TCP acceptor for incoming connections.
  tcp::socket socket_;  ///< The TCP socket for communication with the client.
};

#endif  // LAB5_H_SERVER_H_
