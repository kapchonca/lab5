#include "../h/server.h"
#include "../h/shared.h"

void AudioServer::StartServer() {
  while (true) {
    tcp::socket socket(io_context_);
    acceptor_.accept(socket);
    std::cout << "Client connected\n";

    std::thread(&AudioServer::HandleClient, this, std::move(socket)).detach();
  }
}

void AudioServer::HandleClient(tcp::socket socket) {
  while (true) {
    try {
      std::string track_name = ReceiveTrackName(socket);
      StartAudioStream(socket, track_name);
    } catch (const boost::system::system_error& e) {
      // Check if the error is due to connection reset by peer or end of file
      if (e.code() == boost::asio::error::connection_reset) {
        std::cout << "Client disconnected (connection reset by peer).\n";
      } else if (e.code() == boost::asio::error::eof) {
        std::cout << "End of file.\n";
      } else {
        std::cerr << "Error reading from socket: " << e.what() << std::endl;
      }
      break;
    }
  }
}

void AudioServer::SendTrackMetaData(tcp::socket& socket,
                                    const sf::SoundBuffer& buffer) {
  const int channel_count = buffer.getChannelCount();
  const int sample_rate = buffer.getSampleRate();
  std::string metadata =
      std::to_string(channel_count) + " " + std::to_string(sample_rate);
  SendStringToPeer(socket, metadata);
}

void AudioServer::StartAudioStream(tcp::socket& socket,
                                   std::string track_name) {
  // Load audio file into a buffer
  sf::SoundBuffer buffer;
  std::string path = "../tracks/" + track_name + ".flac";
  if (!buffer.loadFromFile(path)) {
    std::cerr << "Failed to load audio file: " << track_name << std::endl;
    std::string notification = "Incorrect song name!";
    SendStringToPeer(socket, notification);
    return;
  }
  std::string notification = "Now playing: " + track_name;
  SendStringToPeer(socket, notification);

  // Create a sound instance and set its buffer
  sf::Sound sound;
  sound.setBuffer(buffer);

  SendTrackMetaData(socket, buffer);

  // Send audio data to the client in chunks
  const sf::Int16* samples = buffer.getSamples();
  std::size_t sample_count = buffer.getSampleCount();
  const std::size_t chunk_size = 1024;
  std::size_t offset = 0;

  while (offset < sample_count) {
    std::size_t chunk = std::min(chunk_size, sample_count - offset);
    boost::asio::write(socket, boost::asio::buffer(samples + offset,
                                                   chunk * sizeof(sf::Int16)));
    offset += chunk;
  }
}

std::string AudioServer::ReceiveTrackName(tcp::socket& socket) {
  std::string message = ReceiveStringFromPeer(socket);
  std::cout << "Received message from client: " << message << std::endl;
  return message;
}
