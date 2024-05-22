#include "../h/server.h"

void AudioServer::InitializeLogger() {
  // Set up console logging
  logging::add_console_log(
      std::cout,
      keywords::format =
          (expr::stream << expr::attr<boost::posix_time::ptime>("TimeStamp")
                        << " [" << logging::trivial::severity << "] "
                        << expr::smessage));

  // Set up file logging
  logging::add_file_log(
      keywords::file_name = "../src/logs/server_%N.log",
      keywords::rotation_size = 10 * 1024 * 1024,  // 10 MB
      keywords::time_based_rotation =
          logging::sinks::file::rotation_at_time_point(0, 0, 0),
      keywords::open_mode = std::ios_base::app,
      keywords::format =
          (expr::stream << expr::attr<boost::posix_time::ptime>("TimeStamp")
                        << " [" << logging::trivial::severity << "] "
                        << expr::smessage));

  // Add common attributes
  logging::add_common_attributes();
  logging::core::get()->add_global_attribute(
      "Scope", logging::attributes::named_scope());
}

void AudioServer::StartServer() {
  boost::asio::thread_pool pool(std::thread::hardware_concurrency());
  while (true) {
    tcp::socket socket(io_context_);
    acceptor_.accept(socket);
    BOOST_LOG_SEV(logger_, logging::trivial::info)
        << "Client connected to server: " << socket.remote_endpoint();
    auto shared_socket = std::make_shared<tcp::socket>(std::move(socket));
    boost::asio::post(
        pool, [this, shared_socket]() { this->HandleClient(shared_socket); });

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  pool.join();
}

void AudioServer::HandleClient(std::shared_ptr<tcp::socket> socket) {
  std::string ip_client = GetIpString(socket);
  SendStringToPeer(socket,
                   "Welcome to Spopipy! What would you like to listen to "
                   "today?\n");
  while (socket->is_open()) {
    try {
      std::string track_name = ReceiveTrackName(socket);
      StartAudioStream(socket, track_name);
      logging::core::get()->flush();
    } catch (const boost::system::system_error& e) {
      // Check if the error is due to connection reset by peer or end of file
      if (e.code() == boost::asio::error::connection_reset) {
        BOOST_LOG_SEV(logger_, logging::trivial::warning)
            << "Client " << ip_client
            << " disconnected abruptly during the stream";
      } else if (e.code() == boost::asio::error::eof) {
        BOOST_LOG_SEV(logger_, logging::trivial::warning)
            << "CLient " << ip_client
            << " disconnected before the request for stream.";
      } else if (e.code() == boost::asio::error::broken_pipe) {
        BOOST_LOG_SEV(logger_, logging::trivial::warning)
            << "Client " << ip_client
            << " has safely requested to close the connection during the "
               "stream.";
      } else {
        BOOST_LOG_SEV(logger_, logging::trivial::error)
            << "Client " << ip_client
            << ": unexpected error reading from socket:" << e.what();
      }
      logging::core::get()->flush();
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
  sf::SoundBuffer buffer;
  std::string path = "../tracks/" + track_name + ".flac";
  if (!buffer.loadFromFile(path)) {
    BOOST_LOG_SEV(logger_, logging::trivial::info)
        << "Client " << socket->remote_endpoint()
        << " entered incorrect song name: \"" << track_name << "\"";
    std::string notification = "Incorrect song name!";
    SendStringToPeer(socket, notification);
    return;
  }

  BOOST_LOG_SEV(logger_, logging::trivial::info)
      << "Now playing \"" << track_name << "\" to client "
      << socket->remote_endpoint();
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
    BOOST_LOG_SEV(logger_, logging::trivial::info)
        << "Connection " << socket->remote_endpoint() << " successfully closed";
    socket->shutdown(tcp::socket::shutdown_both);
    socket->close();
  }
}

std::string AudioServer::ReceiveTrackName(std::shared_ptr<tcp::socket> socket) {
  std::string message = ReceiveStringFromPeer(socket);
  BOOST_LOG_SEV(logger_, logging::trivial::info)
      << "Received message from client " << socket->remote_endpoint() << ": \""
      << message << "\"";
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

std::string AudioServer::GetIpString(std::shared_ptr<tcp::socket> socket) {
  auto ip_client_raw = socket->remote_endpoint();
  std::ostringstream os;
  os << ip_client_raw;
  std::string ip_client = os.str();
  return ip_client;
}