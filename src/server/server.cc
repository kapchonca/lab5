#include "../h/server.h"

void AudioServer::StartServer() {
  acceptor_.accept(socket_);
  std::cout << "Client connected\n";
  StartAudioStream();
  io_context_.run();
}

void AudioServer::StartAudioStream() {
  // Load your audio file into a buffer
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile("../tracks/bp.mp3")) {
    std::cerr << "Failed to load audio file\n";
    return;
  }

  // Create a sound instance and set its buffer
  sf::Sound sound;
  sound.setBuffer(buffer);

  // Play the sound
  sound.play();

  // Send audio data to the client in chunks
  const sf::Int16* samples = buffer.getSamples();
  std::size_t sampleCount = buffer.getSampleCount();
  const std::size_t chunkSize = 32;
  std::size_t offset = 0;

  while (offset < sampleCount) {
    std::size_t chunk = std::min(chunkSize, sampleCount - offset);
    boost::asio::write(socket_, boost::asio::buffer(samples + offset,
                                                    chunk * sizeof(sf::Int16)));
    offset += chunk;
  }
}
