#include "../h/client.h"
#include "../h/shared.h"

void AudioClient::Connect(const tcp::resolver::results_type& endpoints) {
  boost::asio::connect(socket_, endpoints);
  std::cout << "Connected to server\n";
  while (true) {
    if (SendTrackName()) {
      AcceptAudioStream();
    }
  }
}

std::pair<int, int> AudioClient::ExtractMetadata(const std::string& metadata) {
  size_t spacePos = metadata.find_first_of(' ');
  if (spacePos != std::string::npos) {
    std::string firstNumberStr = metadata.substr(0, spacePos);
    std::string secondNumberStr = metadata.substr(spacePos + 1);

    int firstNumber = std::stoi(firstNumberStr);
    int secondNumber = std::stoi(secondNumberStr);
    return std::make_pair(firstNumber, secondNumber);
  }
  return std::make_pair(0, 0);
}

void AudioClient::AcceptAudioStream() {

  std::string raw_metadata = ReceiveStringFromPeer(socket_);
  std::pair<int, int> metadata = ExtractMetadata(raw_metadata);
  if (!metadata.first) {
    std::cerr << "Could not fetch song metadata\n";
    return;
  }

  // Create a music instance to play audio
  sf::Music music;
  const int channel_count = metadata.first;
  const int sample_rate = metadata.second;

  // Receive and play audio data
  while (true) {
    std::vector<sf::Int16> audio_data(1024000);  // Store 1 MB at a time
    boost::system::error_code error;
    size_t length = socket_.read_some(boost::asio::buffer(audio_data), error);
    if (error == boost::asio::error::eof) {
      break;  // Connection closed cleanly by peer
    } else if (error) {
      throw boost::system::system_error(error);
    }

    // Load received audio data into a temporary buffer
    sf::SoundBuffer buffer;
    buffer.loadFromSamples(audio_data.data(), length / sizeof(sf::Int16),
                           channel_count, sample_rate);

    // Save temporary buffer to a temporary file
    buffer.saveToFile("temp_audio.wav");

    // Open the temporary file with the music instance
    music.openFromFile("temp_audio.wav");

    // Play the music
    music.play();

    // Wait until music playback is finished
    while (music.getStatus() == sf::SoundSource::Playing) {
      sf::sleep(sf::milliseconds(1));
    }
  }
}

bool AudioClient::SendTrackName() {
  std::string message;
  std::cin >> message;

  SendStringToPeer(socket_, message);

  std::cout << "Message sent to server: " << message << std::endl;

  std::string receive_mes = ReceiveStringFromPeer(socket_);

  std::cout << receive_mes << std::endl;
  if (receive_mes[0] == 'N') {
    return true;
  }
  return false;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: ./lab5_test <address> <port>\n";
    return -1;
  }
  try {
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    AudioClient client(io_context, endpoints);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
