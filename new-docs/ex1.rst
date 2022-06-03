================================================
Building an Introductory/Trivial application
================================================

Consumer
---------
First, we will create our consumer. A consumer sends out Interests requesting data and will later validate the retrieved data. 
The implementation of a consumer utilizes the `ndn-cxx:Face <https://named-data.net/doc/ndn-cxx/current/doxygen/d9/d55/classndn_1_1Face.html>`_ class with the default transport (`ndn-cxx:UnixTransport <https://named-data.net/doc/ndn-cxx/current/doxygen/de/df0/classndn_1_1UnixTransport.html>`_).
The `ndn-cxx:Face <https://named-data.net/doc/ndn-cxx/current/doxygen/d9/d55/classndn_1_1Face.html>`_ class essentially provides an interface to the given transport, or if none is given, the default transport which uses the Unix stream socket (`ndn-cxx:UnixTransport <https://named-data.net/doc/ndn-cxx/current/doxygen/de/df0/classndn_1_1UnixTransport.html>`_). The consumer implementation also uses `ndn-cxx:ValidatorConfig <https://named-data.net/doc/ndn-cxx/current/doxygen/d9/df9/classndn_1_1security_1_1ValidatorConfig.html#details>`_ class which is a helper for validating retrieved data.

The following is the basic structure of a consumer, which includes a run function that sends an Interest as well as three callbacks to be called when Data/Nack is retrieved or the consumer’s Interest times out. We also provided a short main function at the end that runs the consumer. 

  .. code-block:: c

    #include <ndn-cx/face.hpp> 
    #include <ndn-cxx/security/validator-config.hpp> 

    #include <iostream> 

    namespace ndn {
    namespace examples {

    class Consumer {
        public: 
            Consumer() {}
            void run() {} 
        private: 
            void onData(const Interest&, const Data& data) {}
            void onNack(const Interest&, const lp::Nack& nack) const {} 
            void onTimeout(const Interest& interest) const {}
        private: 
            Face m_face; 
            ValidatorConfig m_validator{m_face}; 
    };

    } // end namespace examples
    } // end namespace ndn

    int main(int argc, char** argv) {
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

With this skeleton code, we will walk through the implementation of each of these functions, starting with the Consumer constructor. In the constructor, we need to provide the validator with the trust schema that our consumer will use to validate the retrieved data. We hard-coded this trust schema ahead of time, but for more details on how this was done, you can see `this <https://named-data.net/doc/ndn-cxx/current/tutorials/security-validator-config.html>`_ page. 

  .. code-block:: c

    Consumer() {
        m_validator.load("examples/trust-schema.conf"); 
    }

Next, our run function. First, we need to define the Interest name with the `Name class <https://named-data.net/doc/ndn-cxx/current/doxygen/da/d3e/classndn_1_1Name.html#details>`_, which indicates what Data we want to receive. In this example, we will be sending an Interest for /example/testApp/randomData. 

  .. code-block:: c

    Name interestName("/example/testApp/randomData"); 

Next, we want to append a version component to the Interest Name to request new data each time we run. We can either provide the version number or nullopt. If nullopt, then the current UNIX time in milliseconds is appended. In our example, we don’t provide a version number, so UNIX time is used. 

  .. code-block:: c

    interestName.appendVersion(); 

Now that we have the Interest Name, we can create our Interest using the `Interest class <https://named-data.net/doc/ndn-cxx/current/doxygen/d1/d81/classndn_1_1Interest.html>`_. We will enforce that the Interest can only be satisfied with fresh data (not stale data) and that the lifetime of an Interest is 6 seconds. Note that the default lifetime of an Interest is 4 seconds. 

  .. code-block:: c

    Interest interest(interestName); 
    interest.setMustBeFresh(true); 
    interest.setInterestLifetime(6_s); 

Finally, we can send the Interest with the expressInterest function. Then the processEvents function will block until the requested data is received or a timeout occurs. 

  .. code-block:: c

    std::cout << "Sending Interest " << interest << std::endl; 
    m_face.expressInterest(interest,
                std::bind(&Consumer::onData, this, _1, _2), 
                std::bind(&Consumer::onNack, this, _1, _2), 
                std::bind(&Consumer::onTimeout, this, _1)); 

    m_face.processEvents(); 

We have completed the implementation of the run function. Your run function should look like this now. 

  .. code-block:: c

    void run() {
        Name interestName("/example/testApp/randomData"); 
        interestName.appendVersion(); 

        Interest interest(interestName); 
        interest.setMustBeFresh(true); 
        interest.setInterestLifetime(6_s); 

        std::cout << "Sending Interest " << interest << std::endl; 
        m_face.expressInterest(interest,
                    std::bind(&Consumer::onData, this, _1, _2), 
                    std::bind(&Consumer::onNack, this, _1, _2), 
                    std::bind(&Consumer::onTimeout, this, _1)); 

        m_face.processEvents(); 
    }

Next, we will implement the three callback functions, onData, onNack, and onTimeout. All three callback functions have very similar implementations, but onData has an extra step where the consumer needs to verify the received data. To validate the data, we can just use our validator, then we can output it if successful. The following is what the three callback functions look like. 

  .. code-block:: c

    void onData(const Interest&, const Data& data)
    {
        std::cout << "Received Data " << data << std::endl;


        m_validator.validate(data,
                            [] (const Data& d) {
                                std::cout << "Data conforms to trust schema" << std::endl;
                                std::cout << "The received data: " << std::string(reinterpret_cast<const char*>(d.getContent().value()),
                                            d.getContent().value_size()) << std::endl;
                            },
                            [] (const Data&, const security::ValidationError& error) {
                                std::cout << "Error authenticating data: " << error << std::endl;
                            });
    }


    void onNack(const Interest&, const lp::Nack& nack) const {
        std::cout << "Received Nack with reason " << nack.getReason() << std::endl; 
    } 

    void onTimeout(const Interest& interest) const {
        std::cout << "Timeout for " << interest << std::endl; 
    } 

This completes the consumer implementation. This is what your consumer file should look like. 

  .. code-block:: c

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
            Name interestName("/example/testApp/randomData");
            interestName.appendVersion();

            Interest interest(interestName);
            interest.setMustBeFresh(true);
            interest.setInterestLifetime(6_s); // The default is 4 seconds

            std::cout << "Sending Interest " << interest << std::endl;
            m_face.expressInterest(interest,
                                std::bind(&Consumer::onData, this,  _1, _2),
                                std::bind(&Consumer::onNack, this, _1, _2),
                                std::bind(&Consumer::onTimeout, this, _1));

            m_face.processEvents();
        }

    private:
        void
        onData(const Interest&, const Data& data)
        {
            std::cout << "Received Data " << data << std::endl;


            m_validator.validate(data,
                                [] (const Data& d) {
                                std::cout << "Data conforms to trust schema" << std::endl;
                                std::cout << "The received data: " << std::string(reinterpret_cast<const char*>(d.getContent().value()),
                                                d.getContent().value_size()) << std::endl;
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

Producer
---------
Now, we will create our producer, which is a little more complicated than our consumer. A producer creates the packets that consumers request with their interests. For our particular producer, we produce packets for Interests with the prefix /example/testApp/randomData. 

In our example, after the producer receives an Interest, the producer creates a Data packet with the same name as the received Interest, adds content, and signs it with the /example/testApp identity. It is also possible to specify a particular key to be used during the signing with the `ndn-cxx:KeyChain class <https://named-data.net/doc/ndn-cxx/current/doxygen/de/d59/classndn_1_1security_1_1v2_1_1KeyChain.html>`_, or to simply choose the system default identity.

After the Data packet has been created and signed, it is returned to the requester. 

The following is the basic structure of the producer with a run function as well as two callback functions to be called when an Interest with the target prefix is received and when we fail to register the prefix with the NDN forwarder. We also provided a short main function that runs the producer. 

  .. code-block:: c

    #include <ndn-cxx/face.hpp> 
    #include <ndn-cxx/security/key-chain.hpp> 
    #include <ndn-cxx/security/signing-helpers.hpp> 

    #include <iostream> 

    namespace ndn {
    namespace examples { 

    class Producer {
        public: 
            void run() {}
        
        private: 
            void onInterest(const InterestFilter&, const Interest& interest) {} 
            void onRegisterFailed(const Name& prefix, const std::string& reason) {} 

        private: 
            Face m_face; 
            KeyChain m_keyChain; 
            ScopedRegisteredPrefixHandle m_certServeHandle; 
    }; 
    }
    } 

    int main(int argc, char** argv) {
        try {
            ndn::examples::Producer producer; 
            producer.run(); 
            return 0; 
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl; 
            return 1; 
        } 
    } 

Starting with the run function, we are going to first set the Interest filter that filters out Interests that this producer can create so that we only respond to the Interest with the specific prefix /example/testApp/randomData. 

  .. code-block:: c

    m_face.setInterestFilter("/example/testApp/randomData", 
			std::bind(&Producer::onInterest, this, _1, _2), 
			nullptr, 
			std::bind(&Producer::onRegisterFailed, this, _1, _2)); 

Next, we also need to filter out Interests with the required certificate that validates this data, in case a consumer requests the certificate. Then the processEvents function will block indefinitely and handle any incoming interests.

  .. code-block:: c

    auto cert = m_keyChain.getPib().getDefaultIdentity().getDefaultKey().getDefaultCertificate(); 
    m_certServeHandle = m_face.setInterestFilter(security::extractIdentityFromCertName(cert.getName()),
                              [this, cert] (auto&&...) {
                                m_face.put(cert);
                              },
                              std::bind(&Producer::onRegisterFailed, this, _1, _2));

    m_face.processEvents(); 

This completes the run function, which should now look like the following. 

  .. code-block:: c

    void run() {
        m_face.setInterestFilter("/example/testApp/randomData", 
                std::bind(&Producer::onInterest, this, _1, _2), 
                nullptr, 
                std::bind(&Producer::onRegisterFailed, this, _1, _2)); 

        auto cert = m_keyChain.getPib().getIdentity("/example/testApp").getDefaultKey().getDefaultCertificate(); 
    m_certServeHandle =          m_face.setInterestFilter(security::extractIdentityFromCertName(cert.getName())
        , [this, cert] (auto&&...) {
        m_face.put(cert); 
        }, 

        std::bind(&Producer::onRegisterFailed, this, _1, _2)); 

        m_face.processEvents(); 
    }

Next, we will implement the call back functions, starting with onInterest. When our producer receives an Interest with the correct prefix, we will first create the Data packet with the string “Hello World” and a freshness period of 10 seconds. 

  .. code-block:: c

    std::cout << ">> I: " << interest << std::endl; 

    static const std::string content("Hello, world!"); 

    auto data = make_shared<Data>(interest.getName()); 
    data->setFreshnessPeriod(10_s); 
    data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size()); 

Next we will sign the Data packet with the identity "/example/testApp". 

  .. code-block:: c

    m_keyChain.sign(*data, signingByIdentity("/example/testApp")); 

Finally, we will return the Data packet to the interface we received the interest from. 

  .. code-block:: c

    std::cout << "<< D: " << *data << std::endl; 
    m_face.put(*data); 

This completed the onInterest callback which should now look like the following. 

  .. code-block:: c

    void onInterest(const InterestFilter&, const Interest& interest) {
        std::cout << ">> I: " << interest << std::endl; 

        static const std::string content("Hello, world!"); 

        auto data = make_shared<Data>(interest.getName()); 
        data->setFreshnessPeriod(10_s); 
        data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size()); 

    m_keyChain.sign(*data); 

    std::cout << "<< D: " << *data << std::endl; 
        m_face.put(*data); 
    }

The onRegisterFailed callback is very simple. In the event that it fails to register the prefix with the NDN forwarder, we will print an error and shut down all pending operations and close the connection to the NDN forwarder. 

  .. code-block:: c

    void onRegisterFailed(const Name& prefix, const std::string& reason) {
        std::cerr << "ERROR: Failed to register prefix '" << prefix << "'with the local forwarder (" << reason << ")" << std::endl; 
        m_face.shutdown(); 
    } 

Now we have completed the producer implementation. Your producer should look like the following. 

  .. code-block:: c

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
        void
        run()
        {
            m_face.setInterestFilter("/example/testApp/randomData",
                                    std::bind(&Producer::onInterest, this, _1, _2),
                                    nullptr, // RegisterPrefixSuccessCallback is optional
                                    std::bind(&Producer::onRegisterFailed, this, _1, _2));

            auto cert = m_keyChain.getPib().getIdentity("/example/testApp").getDefaultKey().getDefaultCertificate();

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

            static const std::string content("Hello, world!");

            // Create Data packet
            auto data = make_shared<Data>(interest.getName());
            data->setFreshnessPeriod(10_s);
            data->setContent(make_span(reinterpret_cast<const uint8_t*>(content.data()), content.size()));

            m_keyChain.sign(*data, signingByIdentity("/example/testApp"));

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
        try {
            ndn::examples::Producer producer;
            producer.run();
            return 0;
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return 1;
        }
    }

Security configuration
----------------------
In order for the consumer example app to be able to properly authenticate data packets created by the producer app, you must configure the following parameters.

The following commands generate an example trust anchor:

  .. code-block:: sh

      ndnsec key-gen /example
      ndnsec cert-dump -i /example > /examples/example-trust-anchor.cert

Apps inside the trust zone need keys, certificate and trust policies to secure data communication. /example/testApp needs a key, and the key needs to be certified by /example. Create a key for the producer and sign it with the example trust anchor.

  .. code-block:: sh

      ndnsec key-gen /example/testApp
      ndnsec sign-req /example/testApp | ndnsec cert-gen -s /example -i example | ndnsec cert-install -

The command line above makes sure that the example app is under a trust zone, which is established through a trust anchor. The apps inside the trust zone share the generated trust anchor example-trust-anchor.cert.

Running the Application
------------------------
First, you must make sure that you have built ndn-cxx with examples:

  .. code-block:: sh

      ./waf configure --with-examples
      ./waf
      sudo ./waf install

If you encounter build errors, you need only run ./waf again after fixing the errors.

We must start NFD before continuing:

  .. code-block:: sh

      nfd-start

Now, in one terminal run:

  .. code-block:: sh

     ./build/examples/producer

In a different terminal, run:

  .. code-block:: sh
      
      ./build/examples/consumer

You should see appropriate output for both the consumer and the producer.
When you are done, you can terminate NFD with:

  .. code-block:: sh
      
      nfd-stop