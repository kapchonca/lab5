#include "../h/server.h"

void AudioServer::StartServer() {
  boost::asio::thread_pool pool(std::thread::hardware_concurrency());
  while (true) {
    tcp::socket socket(io_context_);
    acceptor_.accept(socket);
    std::cout << "Client connected:" << socket.local_endpoint() << std::endl;
    auto shared_socket = std::make_shared<tcp::socket>(std::move(socket));
    boost::asio::post(
        pool, [this, shared_socket]() { this->HandleClient(shared_socket); });

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  pool.join();
}

void AudioServer::HandleClient(std::shared_ptr<tcp::socket> socket) {
  SendStringToPeer(socket,
                   "Welcome to Spopipy! What would you like to listen to "
                   "today?\n");
  while (socket->is_open()) {
    try {
      std::string track_name = ReceiveTrackName(socket);
      StartAudioStream(socket, track_name);
    } catch (const boost::system::system_error& e) {
      // Check if the error is due to connection reset by peer or end of file
      if (e.code() == boost::asio::error::connection_reset) {
        std::cout << "Client disconnected (connection reset by peer).\n";
      } else if (e.code() == boost::asio::error::eof) {
        std::cout << "End of file.\n";
      } else if (e.code() == boost::asio::error::broken_pipe) {
        std::cout << "Client has closed the socket\n";
      } else {
        std::cerr << "Error reading from socket: " << e.what() << std::endl;
      }
      break;
    }
  }
}

void AudioServer::SendTrackMetaData(std::shared_ptr<tcp::socket> socket,
                                    const sf::SoundBuffer& buffer) {
  const int channel_count = buffer.getChannelCount();
  const int sample_rate = buffer.getSampleRate();
  std::string metadata =
      std::to_string(channel_count) + " " + std::to_string(sample_rate);
  SendStringToPeer(socket, metadata);
}

void AudioServer::StartAudioStream(std::shared_ptr<tcp::socket> socket,
                                   std::string track_name) {
  // Load audio file into a buffer
  // std::cout << "FIRST\n";
  // std::this_thread::sleep_for(std::chrono::seconds(10));
  sf::SoundBuffer buffer;
  // std::cout << "SECOND";
  // std::this_thread::sleep_for(std::chrono::seconds(10));

  std::string path = "../tracks/" + track_name + ".flac";
  if (!buffer.loadFromFile(path)) {
    std::string notification = "Incorrect song name!";
    SendStringToPeer(socket, notification);
    return;
  }
  // std::cout << "THIRD";

  std::string notification = "Now playing: " + track_name;
  SendStringToPeer(socket, notification);

  SendTrackMetaData(socket, buffer);

  // Send audio data to the client in chunks
  const sf::Int16* samples = buffer.getSamples();
  std::size_t sample_count = buffer.getSampleCount();
  const std::size_t chunk_size = 1024;
  std::size_t offset = 0;

  while (offset < sample_count) {
    std::size_t chunk = std::min(chunk_size, sample_count - offset);
    boost::asio::write(*socket, boost::asio::buffer(samples + offset,
                                                    chunk * sizeof(sf::Int16)));
    offset += chunk;
  }
  if (socket->is_open()) {
    socket->shutdown(tcp::socket::shutdown_both);
    socket->close();
  }
}

std::string AudioServer::ReceiveTrackName(std::shared_ptr<tcp::socket> socket) {
  std::string message = ReceiveStringFromPeer(socket);
  std::cout << "Received message from client: " << message << std::endl;
  return message;
}

void AudioServer::SendStringToPeer(std::shared_ptr<tcp::socket> socket,
                                   const std::string& message) {
  size_t message_length = message.size();
  boost::asio::write(
      *socket, boost::asio::buffer(&message_length, sizeof(message_length)));
  boost::asio::write(*socket, boost::asio::buffer(message));
}

std::string AudioServer::ReceiveStringFromPeer(
    std::shared_ptr<tcp::socket> socket) {
  size_t message_length;
  boost::asio::read(
      *socket, boost::asio::buffer(&message_length, sizeof(message_length)));

  std::string message;
  message.resize(message_length);
  boost::asio::read(*socket, boost::asio::buffer(&message[0], message_length));

  return message;
}