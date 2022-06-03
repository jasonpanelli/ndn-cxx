/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/validator-config.hpp>

#include <iostream>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces should be used to prevent/limit name conflicts
namespace examples {

class Consumer
{
public:
  Consumer()
  {
    m_validator.load("examples/trust-schema.conf");
  }

  void
  run()
  {
    int input;
    try
    {
      std::cout << "What data would you like?\n";
      std::cout << "[1] Information about NDN\n";
      std::cout << "[2] Information about the penguins\n";
      std::cout << "[3] Information about dinosaurs\n";
      std::cout << "Your answer: ";

      std::string s;
      getline(std::cin, s);
      input = std::stoi(s);
      if (input < 1 || input >3) throw 1;
    }
    catch (...)
    {
      std::cerr << "You must answer with 1, 2, or 3.\n";
      return;
    }

    std::string dataName;
    if (input == 1) {
      dataName = "/example/ndn/information";
    }
    else if (input == 2) {
      dataName = "/example/penguin/information";
    }
    else {
      dataName = "/example/dinosaur/information";
    }

    Name interestName(dataName);

    Interest interest(interestName);

    std::cout << "Sending Interest " << interest << std::endl;
    m_face.expressInterest(interest,
                           std::bind(&Consumer::onData, this,  _1, _2),
                           std::bind(&Consumer::onNack, this, _1, _2),
                           std::bind(&Consumer::onTimeout, this, _1));

    // processEvents will block until the requested data is received or a timeout occurs
    m_face.processEvents();
  }

private:
  void
  onData(const Interest&, const Data& data)
  {
    std::cout << "Received Data " << data << std::endl;

    m_validator.validate(data,
                         [] (const Data& d) {
                           std::cout << "\n\nYour requested information:\n" << std::endl;
                           std::string output(reinterpret_cast<const char*>(d.getContent().value()),
                                                           d.getContent().value_size());
                           std::cout << output << std::endl;

                         },
                         [] (const Data&, const security::ValidationError& error) {
                           std::cout << "Error authenticating data: " << error << std::endl;
                         });
  }

  void
  onNack(const Interest&, const lp::Nack& nack) const
  {
    std::cout << "Received Nack with reason " << nack.getReason() << std::endl;
  }

  void
  onTimeout(const Interest& interest) const
  {
    std::cout << "Timeout for " << interest << std::endl;
  }

private:
  Face m_face;
  ValidatorConfig m_validator{m_face};
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  try {
    ndn::examples::Consumer consumer;
    consumer.run();
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
}
