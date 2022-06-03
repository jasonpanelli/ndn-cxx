==================
Building a Multiple Producer Application
==================

As a prerequisite for this application and tutorial, please see :doc:`ex1`.

In a normal network, it is common to have multiple producers, or to request more than one type of data. Here we will see how to perform this with NDN, through an encyclopedia-inspired application. The consumer in this application can request information from 3 different producers about 3 different topics - NDN, penguins, and dinosaurs.

Consumer
---------
We will start with the implementation of our consumer, which will not be too different from the trivial consumer. In fact, the structure is completely identical:

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

The constructor is identical to the trivial example. These apps were written to use the same trust-schema.conf file that has been hard coded. Once again, see `this page <https://named-data.net/doc/ndn-cxx/current/tutorials/security-validator-config.html>`_ to see how this file works.

  .. code-block:: c
      
    Consumer()
    {
        m_validator.load("examples/trust-schema.conf");
    }

The changes come with the control we allow our consumer to have over the data it seeks. Let's start with our run function. In the trivial consumer, we consumed only one data name. In our more sophisticated encyclopedia consumer, we can consume more than just one data name. We process the data we want in our run function before we perform any other tasks.

  .. code-block:: c
      
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

Next, we must choose our data names based on what the user selected.

  .. code-block:: c
      
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

Then, similar to our trivial consumer, we must construct our interest. This time, we do not append the version or care if this data is fresh. This will allow us to see one of the advantages of NDN - data can be cached in the network. If the data has made it to the network, and the producer goes down, we might still be able to fetch the data from the network. (As a side note, you can try this by running the producers, requesting the data, killing the producers, then requesting the data again).

  .. code-block:: c
      
    Name interestName(dataName);
    Interest interest(interestName);

The last step is to simply send the Interest and then wait for a response, timeout, or nack.

  .. code-block:: c
      
    std::cout << "Sending Interest " << interest << std::endl;
    m_face.expressInterest(interest,
                        std::bind(&Consumer::onData, this,  _1, _2),
                        std::bind(&Consumer::onNack, this, _1, _2),
                        std::bind(&Consumer::onTimeout, this, _1));

    // processEvents will block until the requested data is received or a timeout occurs
    m_face.processEvents();

The completed run function should look like this:

  .. code-block:: c
      
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

The onData, onNack, and onTimeout functions are essentially the same as the trivial application:

  .. code-block:: c
      
    void onData(const Interest&, const Data& data)
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

    void onNack(const Interest&, const lp::Nack& nack) const
    {
        std::cout << "Received Nack with reason " << nack.getReason() << std::endl;
    }

    void onTimeout(const Interest& interest) const
    {
        std::cout << "Timeout for " << interest << std::endl;
    }

This gives a complete consumer:

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

Our producer is once again similar to the trivial application. For a more compact design, we implement producers for all 3 different data names in a single file. The only change to our layout is the inclusion of dataName and content member variables in order for the class to know which dataName to produce data for and what content to produce for that dataName.

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
        // Private member variables for the Producer class
        std::string dataName;
        std::string content;


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

In order to initialize this, we provide this main function:

  .. code-block:: c
      
    int
    main(int argc, char** argv)
    {
        if (argc == 2 && (!strncmp(argv[1], "ndn", 3) || !strncmp(argv[1], "penguin", 7) || !strncmp(argv[1], "dinosaur", 8))) {
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

This main function takes a commandline argument, "ndn", "penguin", or "dinosaur", to determine which type of producer it is. We pass this information into the constructor, which is defined as follows in order to set up our member variables:

  .. code-block:: c
      
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

Our run function is very similar to the trivial producer. The only difference is to pass our dataName variable to the interest filter and certificate in order to receive the appropriate packets and certificate.

  .. code-block:: c
      
    void
    run()
    {
        m_face.setInterestFilter(dataName + "/information",
                                std::bind(&Producer::onInterest, this, _1, _2),
                                nullptr, // RegisterPrefixSuccessCallback is optional
                                std::bind(&Producer::onRegisterFailed, this, _1, _2));

        // Set up our keychain to use identity and default keys for our data name
        auto cert = m_keyChain.getPib().getIdentity(dataName).getDefaultKey().getDefaultCertificate();

        m_certServeHandle = m_face.setInterestFilter(security::extractIdentityFromCertName(cert.getName()),
                                [this, cert] (auto&&...) {
                                    m_face.put(cert);
                                },
                                std::bind(&Producer::onRegisterFailed, this, _1, _2));
        m_face.processEvents();
    }

Similarly, our onInterest and onRegisterFailed functions are nearly identical to the trivial application except for taking the proper dataName and setting the proper content.

  .. code-block:: c
      
    void
    onInterest(const InterestFilter&, const Interest& interest)
    {
        std::cout << ">> I: " << interest << std::endl;

        static const std::string c = content;

        // Create Data packet
        auto data = make_shared<Data>(interest.getName());
        data->setFreshnessPeriod(10_s);
        data->setContent(make_span(reinterpret_cast<const uint8_t*>(c.data()), c.size()));

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

Our full encyclopedia producer should look like this:

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
            m_face.setInterestFilter(dataName + "/information",
                                    std::bind(&Producer::onInterest, this, _1, _2),
                                    nullptr, // RegisterPrefixSuccessCallback is optional
                                    std::bind(&Producer::onRegisterFailed, this, _1, _2));

            // Set up our keychain to use identity and default keys for our data name
            auto cert = m_keyChain.getPib().getIdentity(dataName).getDefaultKey().getDefaultCertificate();

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

            static const std::string c = content;

            // Create Data packet
            auto data = make_shared<Data>(interest.getName());
            data->setFreshnessPeriod(10_s);
            data->setContent(make_span(reinterpret_cast<const uint8_t*>(c.data()), c.size()));

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
        if (argc == 2 && (!strncmp(argv[1], "ndn", 3) || !strncmp(argv[1], "penguin", 7) || !strncmp(argv[1], "dinosaur", 8))) {
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

Security Configuration
-----------------------
Since we are dealing with multiple data names, we must prepare keys for each one of them. 

First, generate the trust anchor if you haven't already:

  .. code-block:: sh
      
      ndnsec key-gen /example
      ndnsec cert-dump -i /example > examples/example-trust-anchor.cert

Next, we can generate the keys we need:

  .. code-block:: sh
      
      ndnsec key-gen /example/penguin
      ndnsec key-gen /example/dinosaur
      ndnsec key-gen /example/ndn

Now, we sign each of our keys with our trust anchor:

  .. code-block:: sh
      
      ndnsec sign-req /example/penguin | ndnsec cert-gen -s /example -i example | ndnsec cert-install -
      ndnsec sign-req /example/dinosaur | ndnsec cert-gen -s /example -i example | ndnsec cert-install -
      ndnsec sign-req /example/ndn | ndnsec cert-gen -s /example -i example | ndnsec cert-install -

The schema of our trust is determined in our examples/trust-anchor.conf file, so our signing must match with what is in this file.

Running the Application
-------------------------
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
      
      ./build/examples/producer-encyclopedia ndn

In a different terminal, run:

  .. code-block:: sh
      
      ./build/examples/producer-encyclopedia penguin

In a different terminal, run:

  .. code-block:: sh
      
      ./build/examples/producer-encyclopedia dinosaur

In a different terminal, run:

  .. code-block:: sh
      
      ./build/examples/consumer-encyclopedia

You should see appropriate output for both the consumer and the producers.

Once you have requested data once from a producer, you can try to kill that producer and request the data again. Because we did not add version numbers, the consumer will be requesting the exact same data name, and if it is still cached in the network, then the network should still be able to deliver it to the consumer, even though the producer is not running. In fact, if the producer is still alive, you should only see output when it receives an interest the first time the consumer requests data from it. Subsequent requests will like be satisfied by caches in the network.

When you are done, you can terminate NFD with:

  .. code-block:: sh
      
      nfd-stop