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
    std::string track_name = ReceiveTrackName(socket);
    StartAudioStream(socket, track_name);
  }
}

void AudioServer::StartAudioStream(tcp::socket& socket,
                                   std::string track_name) {
  // Load audio file into a buffer
  sf::SoundBuffer buffer;
  std::string path = "../tracks/" + track_name + ".mp3";
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

  // Play the sound
  sound.play();

  // Send audio data to the client in chunks
  const sf::Int16* samples = buffer.getSamples();
  std::size_t sample_count = buffer.getSampleCount();
  const std::size_t chunk_size = 32;
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
