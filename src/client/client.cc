#include "../h/client.h"

void AudioClient::Connect(const tcp::resolver::results_type& endpoints) {
  boost::asio::connect(socket_, endpoints);
  std::cout << "Connected to server\n";
  while (true) {
    bool ok = SendTrackName();
    if (ok) {
      AcceptAudioStream();
    }
  }
}

void AudioClient::AcceptAudioStream() {
  // Create a sound instance to play audio
  sf::Sound sound;
  sf::SoundBuffer buffer;

  // Receive and play audio data
  while (true) {
    std::vector<sf::Int16> audioData(32);
    boost::system::error_code error;
    size_t length = socket_.read_some(boost::asio::buffer(audioData), error);
    if (error == boost::asio::error::eof) {
      break;  // Connection closed cleanly by peer
    } else if (error) {
      throw boost::system::system_error(error);
    }

    // Load received audio data into the sound buffer
    buffer.loadFromSamples(audioData.data(), length / sizeof(sf::Int16), 2,
                           48000);

    // Set the buffer to the sound and play it
    sound.setBuffer(buffer);
    sound.play();

    // Wait until sound playback is finished
    while (sound.getStatus() == sf::Sound::Playing) {
      sf::sleep(sf::milliseconds(500));
    }
  }
}

bool AudioClient::SendTrackName() {
  std::string message;
  std::cin >> message;

  size_t messageLength = message.size();

  // Send the length of the message first
  boost::asio::write(
      socket_, boost::asio::buffer(&messageLength, sizeof(messageLength)));

  // Then send the actual message data
  boost::asio::write(socket_, boost::asio::buffer(message));

  std::cout << "Message sent to server: " << message << std::endl;

  size_t receive_mes_len;
  boost::asio::read(
      socket_, boost::asio::buffer(&receive_mes_len, sizeof(receive_mes_len)));

  // Then read the actual message data
  std::string receive_mes;
  receive_mes.resize(receive_mes_len);
  boost::asio::read(socket_,
                    boost::asio::buffer(&receive_mes[0], receive_mes_len));
  std::cout << receive_mes << std::endl;
  if (receive_mes[0] == 'N') {
    return true;
  }
  return false;
}

int main() {
  try {
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("localhost", "8888");
    AudioClient client(io_context, endpoints);
    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
