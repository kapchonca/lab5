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
     * @brief Streams audio data from the server.
     *
     * The function waits until the playback of each part is finished before fetching the next part.
    */
  void BeginAudioStream();

  /**
     * @brief Sends the track name to the server.
     */
  bool SendTrackName();

  /**
    * @brief Fetches a part of the audio track from the server.
    *
    * This function reads a chunk of audio data from the network socket and loads
    * it into the provided sound buffer.
    *
    * @param buffer The sound buffer to store the fetched audio data.
    * @param metadata A pair representing the audio metadata: first value is the
    *                 channel count, second value is the sample rate.
    * @return True if audio data was successfully fetched and loaded into the buffer,
    *         false if the connection was closed or an error occurred.
    */
  bool FetchTrackPart(sf::SoundBuffer& buffer, std::pair<int, int>& metadata);

  /**
     * @brief Handles client requests from the command line.
     *
     * This function continuously reads input from the command line and handles
     * client requests accordingly. Supported commands include "pause", "unpause" and "exit".
     */
  void HandleClientRequest();

  /**
 * @brief Sends a string message to a peer over a TCP socket.
 * 
 * @param socket The TCP socket to send the message over.
 * @param message The string message to send.
 */
  void SendStringToPeer(tcp::socket& socket, const std::string& message);

  /**
 * @brief Receives a string message from a peer over a TCP socket.
 * 
 * @param socket The TCP socket to receive the message from.
 * @return The string message received.
 */
  std::string ReceiveStringFromPeer(tcp::socket& socket);

  tcp::socket socket_;  ///< The TCP socket for communication with the server.
  bool is_paused_ = false;
  bool is_active_ = true;
};

#endif  // LAB5_H_CLIENT_H_
