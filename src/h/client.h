#ifndef LAB5_H_CLIENT_H_
#define LAB5_H_CLIENT_H_

#include <SFML/Audio.hpp>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

/**
 * @brief Class to accept and play audio received from the server
 */
class AudioClient {
 public:
  /**
     * @brief Constructor for AudioClient class.
     * @param io_context The Boost.Asio I/O context object.
     * @param endpoints The resolved endpoints for server connection.
     */
  AudioClient(boost::asio::io_context& io_context,
              const tcp::resolver::results_type& endpoints)
      : socket_(io_context) {
    Connect(endpoints);
  }

 private:
  /**
     * @brief Connects to the server.
     * @param endpoints The resolved endpoints for server connection.
     */
  void Connect(const tcp::resolver::results_type& endpoints);

  /**
     * @brief Accepts audio stream from the server.
     */
  void AcceptAudioStream();

  /**
     * @brief Sends the track name to the server.
     */
  void SendTrackName();

  tcp::socket socket_;  ///< The TCP socket for communication with the server.
};

#endif  // LAB5_H_CLIENT_H_
