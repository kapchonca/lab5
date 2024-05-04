#ifndef LAB5_H_SHARED_H_
#define LAB5_H_SHARED_H_

#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

/**
 * @brief Sends a string message to a peer over a TCP socket.
 * 
 * @param socket The TCP socket to send the message over.
 * @param message The string message to send.
 */
void SendStringToPeer(tcp::socket& socket, const std::string& message) {
  size_t message_length = message.size();
  boost::asio::write(
      socket, boost::asio::buffer(&message_length, sizeof(message_length)));
  boost::asio::write(socket, boost::asio::buffer(message));
}

/**
 * @brief Receives a string message from a peer over a TCP socket.
 * 
 * @param socket The TCP socket to receive the message from.
 * @return The string message received.
 */
std::string ReceiveStringFromPeer(tcp::socket& socket) {
  size_t message_length;
  boost::asio::read(
      socket, boost::asio::buffer(&message_length, sizeof(message_length)));

  std::string message;
  message.resize(message_length);
  boost::asio::read(socket, boost::asio::buffer(&message[0], message_length));

  return message;
}

#endif  // LAB5_H_SHARED_H_
