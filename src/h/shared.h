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
  size_t message_len = message.size();
  boost::asio::write(socket,
                     boost::asio::buffer(&message_len, sizeof(message_len)));
  boost::asio::write(socket, boost::asio::buffer(message));
}

/**
 * @brief Receives a string message from a peer over a TCP socket.
 * 
 * @param socket The TCP socket to receive the message from.
 * @return The string message received.
 */
std::string ReceiveStringFromPeer(tcp::socket& socket) {
  size_t messageLength;
  boost::asio::read(socket,
                    boost::asio::buffer(&messageLength, sizeof(messageLength)));

  std::string message;
  message.resize(messageLength);
  boost::asio::read(socket, boost::asio::buffer(&message[0], messageLength));

  return message;
}

#endif  // LAB5_H_SHARED_H_
