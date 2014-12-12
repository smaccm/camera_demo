#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char ** argv){
  if(argc != 2){
    std::cerr << "Usage: client <host>" << std::endl;
      return 1;
  }

  try{
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), atoi(argv[1])));

    for (;;)
    {
      tcp::socket socket(io_service);
      acceptor.accept(socket);
      std::string message = "hello client\n";

      boost::system::error_code ignored_error;
      boost::asio::write(socket, boost::asio::buffer(message),
        boost::asio::transfer_all(), ignored_error);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
