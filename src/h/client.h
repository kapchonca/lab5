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
     * @brief Extracts metadata from a string and returns it as a pair of integers.
     * @param metadata A constant reference to the string containing the metadata.
     * @return A pair of integers representing the channel count and sample rate respectively.
     */
  std::pair<int, int> ExtractMetadata(const std::string& metadata);

  /**
     * @brief Accepts audio stream from the server.
     */
  void AcceptAudioStream();

  /**
     * @brief Sends the track name to the server.
     */
  bool SendTrackName();

  tcp::socket socket_;  ///< The TCP socket for communication with the server.
};

#endif  // LAB5_H_CLIENT_H_
