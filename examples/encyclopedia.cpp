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
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>

#include <iostream>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces should be used to prevent/limit name conflicts
namespace examples {

class Producer
{
public:
  Producer(std::string argument)
  {
    if (argument == "ndn") {
      dataName = "/example/ndn";
      content = "NDN is a network layer protocol centered around naming data.";
    }
    else if (argument == "penguin") {
      dataName = "/example/penguin";
      content = "The emperor penguin is the largest of all penguins on Earth.";
    }
    else {
      dataName = "/example/dinosaur";
      content = "Dinosaurs inhabited the Earth a long time ago.";
    }

    std::cout << dataName << std::endl;
    std::cout << content << std::endl;
  }

  void
  run()
  {
    m_face.setInterestFilter(dataName,
                             std::bind(&Producer::onInterest, this, _1, _2),
                             nullptr, // RegisterPrefixSuccessCallback is optional
                             std::bind(&Producer::onRegisterFailed, this, _1, _2));

    // Set up our keychain to use identity and default keys for our data name
    auto cert = m_keyChain.getPib().getIdentity(dataName).getDefaultKey().getDefaultCertificate();
    std::cout << "identity: " << m_keyChain.getPib().getIdentity(dataName).getName() << std::endl;
    std::cout << "key: " << m_keyChain.getPib().getIdentity(dataName).getDefaultKey().getName() << std::endl;

    m_certServeHandle = m_face.setInterestFilter(security::extractIdentityFromCertName(cert.getName()),
                             [this, cert] (auto&&...) {
                               m_face.put(cert);
                             },
                             std::bind(&Producer::onRegisterFailed, this, _1, _2));
    m_face.processEvents();
  }

private:
  void
  onInterest(const InterestFilter&, const Interest& interest)
  {
    std::cout << ">> I: " << interest << std::endl;

    static const std::string content(content);

    // Create Data packet
    auto data = make_shared<Data>(interest.getName());
    data->setFreshnessPeriod(10_s);
    data->setContent(make_span(reinterpret_cast<const uint8_t*>(content.data()), content.size()));

    // Sign Data packet with identity
    m_keyChain.sign(*data, signingByIdentity(dataName));

    // Return Data packet to the requester
    std::cout << "<< D: " << *data << std::endl;
    m_face.put(*data);
  }

  void
  onRegisterFailed(const Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix '" << prefix
              << "' with the local forwarder (" << reason << ")" << std::endl;
    m_face.shutdown();
  }

  // Private member variables for the Producer class
  std::string dataName;
  std::string content;


private:
  Face m_face;
  KeyChain m_keyChain;
  ScopedRegisteredPrefixHandle m_certServeHandle;
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  if (argc == 2 && (!strncmp(argv[1], "ndn", 3) || !strncmp(argv[1], "penguin", 7) || strncmp(argv[1], "dinosaur", 9))) {
    try {
      ndn::examples::Producer producer(argv[1]);
      producer.run();
      return 0;
    }
    catch (const std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }
  }
  else {
    std::cerr << "USAGE: encyclopedia <type>\nValid types:\n\tndn\n\tpenguin\n\tdinosaur" << std::endl;
    return 2;
  }
}
