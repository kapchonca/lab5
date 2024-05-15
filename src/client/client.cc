#include "./h/client.h"

void AudioClient::Connect(const tcp::resolver::results_type& endpoints) {
  boost::asio::connect(socket_, endpoints);
  std::cout << "Connected to server\n";
  while (socket_.is_open()) {
    if (SendTrackName()) {
      std::thread request_thread(&AudioClient::HandleClientRequest, this);
      BeginAudioStream();
      request_thread.join();
    }
  }
  std::cout << "Session closed" << std::endl;
}

void AudioClient::HandleClientRequest() {
  while (is_active_) {
    std::string input;
    std::cin >> input;
    if (input == "pause") {
      is_paused_ = true;
      std::cout << "The track has been paused\n";
    } else if (input == "unpause") {
      is_paused_ = false;
      std::cout << "The track has been unpaused\n";
    } else if (input == "exit" && is_active_) {
      std::cout << "Exiting...\n";
      is_active_ = false;
      if (socket_.is_open()) {
        socket_.shutdown(tcp::socket::shutdown_both);
        socket_.close();
        break;
      }
    } else if (is_active_) {
      std::cout << "Command is not supported. List of the available "
                   "commands:\n<pause, unpause, exit>\n";
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

bool AudioClient::FetchTrackPart(sf::SoundBuffer& buffer,
                                 std::pair<int, int>& metadata) {
  const int channel_count = metadata.first;
  const int sample_rate = metadata.second;
  std::vector<sf::Int16> audio_data(512000);  // Store 1 MB at a time
  boost::system::error_code error;
  size_t length = socket_.read_some(boost::asio::buffer(audio_data), error);

  if (error == boost::asio::error::eof) {
    std::cout << "Connection closed\nPress any key to quit" << std::endl;
    return false;
  } else if (error) {
    throw boost::system::system_error(error);
  }

  buffer.loadFromSamples(audio_data.data(), length / sizeof(sf::Int16),
                         channel_count, sample_rate);

  return length > 0;
}

void AudioClient::BeginAudioStream() {

  std::string raw_metadata = ReceiveStringFromPeer(socket_);
  std::pair<int, int> metadata = ExtractMetadata(raw_metadata);
  if (!metadata.first) {
    std::cerr << "Could not fetch song metadata\n";
    return;
  }

  // Create a music instance to play audio
  sf::Music music;
  sf::SoundBuffer buffer;

  // Receive and play audio data
  while (is_active_ && FetchTrackPart(buffer, metadata)) {

    // Load received audio data into a temporary buffer

    // Save temporary buffer to a temporary file
    buffer.saveToFile("temp_audio.wav");

    // Open the temporary file with the music instance
    music.openFromFile("temp_audio.wav");

    // Play the music
    music.play();

    // Wait until music playback is finished
    while (music.getStatus() == sf::SoundSource::Playing) {
      sf::sleep(sf::milliseconds(1));
      while (is_paused_) {
        music.pause();
        sf::sleep(sf::milliseconds(300));
        music.play();
      }
    }
  }
  music.stop();
  std::remove("temp_audio.wav");
  is_active_ = false;
  if (socket_.is_open()) {
    socket_.shutdown(tcp::socket::shutdown_both);
    socket_.close();
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
    std::cerr << "Usage: ./client <address> <port>\n";
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

void AudioClient::SendStringToPeer(tcp::socket& socket,
                                   const std::string& message) {
  size_t message_length = message.size();
  boost::asio::write(
      socket, boost::asio::buffer(&message_length, sizeof(message_length)));
  boost::asio::write(socket, boost::asio::buffer(message));
}

std::string AudioClient::ReceiveStringFromPeer(tcp::socket& socket) {
  size_t message_length;
  boost::asio::read(
      socket, boost::asio::buffer(&message_length, sizeof(message_length)));

  std::string message;
  message.resize(message_length);
  boost::asio::read(socket, boost::asio::buffer(&message[0], message_length));

  return message;
}