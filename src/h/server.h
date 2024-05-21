#ifndef LAB5_H_SERVER_H_
#define LAB5_H_SERVER_H_

#include <SFML/Audio.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <iostream>
#include <string>
#include <thread>

using boost::asio::ip::tcp;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

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
      : io_context_(), acceptor_(io_context_, endpoint) {
    InitializeLogger();
    StartServer();
  }

 private:
  /**
 * @brief Initialize the logging system.
 */
  void InitializeLogger();

  /**
     * @brief Starts the server and waits for clients to connect.
     */
  void StartServer();

  /**
     * @brief Handles a connected client.
     * @param socket The TCP socket for communication with the client.
     */
  void HandleClient(std::shared_ptr<tcp::socket> socket);

  /**
     * @brief Sends track metadata over a TCP socket.
     * @param socket A reference to the TCP socket used for communication.
     * @param buffer The sound buffer containing the audio track's data.
     */
  void SendTrackMetaData(std::shared_ptr<tcp::socket> socket,
                         const sf::SoundBuffer& buffer);

  /**
     * @brief Starts streaming audio data to the connected client.
     * @param socket The TCP socket for communication with the client.
     * @param track_name The name of the track to stream to the client.
     */
  void StartAudioStream(std::shared_ptr<tcp::socket> socket,
                        std::string track_name);

  /**
     * @brief Receives the name of the track to play from the client.
     * @param socket The TCP socket for communication with the client.
     * @return std::string The received name of the track.
     */
  std::string ReceiveTrackName(std::shared_ptr<tcp::socket> socket);

  /**
 * @brief Sends a string message to a peer over a TCP socket.
 * 
 * @param socket The TCP socket to send the message over.
 * @param message The string message to send.
 */
  void SendStringToPeer(std::shared_ptr<tcp::socket> socket,
                        const std::string& message);

  /**
 * @brief Receives a string message from a peer over a TCP socket.
 * 
 * @param socket The TCP socket to receive the message from.
 * @return The string message received.
 */
  std::string ReceiveStringFromPeer(std::shared_ptr<tcp::socket> socket);

  /**
 * @brief Get the IP address of the client as a string.
 *
 * @param socket The socket object representing the client connection.
 * @return The IP address of the client as a string.
 */
  std::string GetIpString(std::shared_ptr<tcp::socket> socket);

  boost::asio::io_context io_context_;  ///< The Boost ASIO IO context.
  tcp::acceptor acceptor_;  ///< The TCP acceptor for incoming connections.
  src::severity_logger<logging::trivial::severity_level>
      logger_;  ///< The Boost Log logger.
};

#endif  // LAB5_H_SERVER_H_
