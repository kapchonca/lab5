#include <SFML/Audio.hpp>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;
/**
 * @brief Class to accept and play audio received from the server
 */
class AudioClient {
 public:
  AudioClient(boost::asio::io_context& io_context,
              const tcp::resolver::results_type& endpoints)
      : socket_(io_context) {
    Connect(endpoints);
  }

 private:
  void Connect(const tcp::resolver::results_type& endpoints) {
    boost::asio::connect(socket_, endpoints);
    std::cout << "Connected to server\n";
    AcceptAudioStream();
  }

  void AcceptAudioStream() {
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
        sf::sleep(sf::milliseconds(300));
      }
    }
  }

  tcp::socket socket_;
};

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
