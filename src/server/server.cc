#include "../h/server.h"

void AudioServer::StartServer() {
  acceptor_.accept(socket_);
  std::cout << "Client connected\n";
  std::string track_name = RecieveTrackName();
  StartAudioStream(track_name);
}

void AudioServer::StartAudioStream(std::string track_name) {
  // Load your audio file into a buffer
  sf::SoundBuffer buffer;
  std::string path = "../tracks/" + track_name + ".mp3";
  if (!buffer.loadFromFile(path)) {
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

std::string AudioServer::RecieveTrackName() {

  // Read the length of the message first
  size_t messageLength;
  boost::asio::read(socket_,
                    boost::asio::buffer(&messageLength, sizeof(messageLength)));

  // Then read the actual message data
  std::string message;
  message.resize(messageLength);
  boost::asio::read(socket_, boost::asio::buffer(&message[0], messageLength));

  std::cout << "Received message from client: " << message << std::endl;

  return message;
}
