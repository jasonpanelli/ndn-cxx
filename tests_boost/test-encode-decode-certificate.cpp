/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include <ndn-cpp/security/certificate/certificate.hpp>
#include <ndn-cpp/security/certificate/public-key.hpp>

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif

#include <cryptopp/cryptlib.h>
#include <cryptopp/rsa.h>
#include <cryptopp/files.h>

#include <fstream>
#include <boost/test/output_test_stream.hpp>

using namespace std;
using namespace ndn;

using namespace CryptoPP;

BOOST_AUTO_TEST_SUITE(TestCertificate)

const uint8_t CertificateRaw[] = {
};

const unsigned char PRIVATE_KEY_DER[] = {
0x30, 0x82, 0x02, 0x74, 0x02, 0x01, 0x00, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01,
0x05, 0x00, 0x04, 0x82, 0x02, 0x5e, 0x30, 0x82, 0x02, 0x5a, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0x9e, 0x06, 0x3e,
0x47, 0x85, 0xb2, 0x34, 0x37, 0xaa, 0x85, 0x47, 0xac, 0x03, 0x24, 0x83, 0xb5, 0x9c, 0xa8, 0x05, 0x3a, 0x24, 0x1e, 0xeb,
0x89, 0x01, 0xbb, 0xe9, 0x9b, 0xb2, 0xc3, 0x22, 0xac, 0x68, 0xe3, 0xf0, 0x6c, 0x02, 0xce, 0x68, 0xa6, 0xc4, 0xd0, 0xa7,
0x06, 0x90, 0x9c, 0xaa, 0x1b, 0x08, 0x1d, 0x8b, 0x43, 0x9a, 0x33, 0x67, 0x44, 0x6d, 0x21, 0xa3, 0x1b, 0x88, 0x9a, 0x97,
0x5e, 0x59, 0xc4, 0x15, 0x0b, 0xd9, 0x2c, 0xbd, 0x51, 0x07, 0x61, 0x82, 0xad, 0xc1, 0xb8, 0xd7, 0xbf, 0x9b, 0xcf, 0x7d,
0x24, 0xc2, 0x63, 0xf3, 0x97, 0x17, 0xeb, 0xfe, 0x62, 0x25, 0xba, 0x5b, 0x4d, 0x8a, 0xc2, 0x7a, 0xbd, 0x43, 0x8a, 0x8f,
0xb8, 0xf2, 0xf1, 0xc5, 0x6a, 0x30, 0xd3, 0x50, 0x8c, 0xc8, 0x9a, 0xdf, 0xef, 0xed, 0x35, 0xe7, 0x7a, 0x62, 0xea, 0x76,
0x7c, 0xbb, 0x08, 0x26, 0xc7, 0x02, 0x01, 0x11, 0x02, 0x81, 0x80, 0x04, 0xa5, 0xd4, 0xa7, 0xc0, 0x2a, 0xe3, 0x6b, 0x0c,
0x8b, 0x73, 0x0c, 0x96, 0xae, 0x40, 0x1b, 0xee, 0x04, 0xf1, 0x18, 0x4c, 0x5b, 0x43, 0x29, 0xad, 0x3a, 0x3b, 0x93, 0xa3,
0x60, 0x17, 0x9b, 0xa8, 0xbb, 0x68, 0xf4, 0x1e, 0x33, 0x3f, 0x50, 0x32, 0xf7, 0x13, 0xf8, 0xa9, 0xe6, 0x7d, 0x79, 0x44,
0x00, 0xde, 0x72, 0xed, 0xf2, 0x73, 0xfa, 0x7b, 0xae, 0x2a, 0x71, 0xc0, 0x40, 0xc8, 0x37, 0x6f, 0x38, 0xb2, 0x69, 0x1f,
0xa8, 0x83, 0x7b, 0x42, 0x00, 0x73, 0x46, 0xe6, 0x4c, 0x91, 0x7f, 0x13, 0x06, 0x69, 0x06, 0xd8, 0x3f, 0x22, 0x15, 0x75,
0xf6, 0xde, 0xcd, 0xb0, 0xbc, 0x66, 0x61, 0x91, 0x08, 0x9b, 0x2b, 0xb2, 0x00, 0xa9, 0x67, 0x05, 0x39, 0x40, 0xb9, 0x37,
0x85, 0x88, 0x4f, 0x76, 0x79, 0x63, 0xc0, 0x88, 0x3c, 0x86, 0xa8, 0x12, 0x94, 0x5f, 0xe4, 0x36, 0x3d, 0xea, 0xb9, 0x02,
0x41, 0x00, 0xb6, 0x2e, 0xbb, 0xcd, 0x2f, 0x3a, 0x99, 0xe0, 0xa1, 0xa5, 0x44, 0x77, 0xea, 0x0b, 0xbe, 0x16, 0x95, 0x0e,
0x64, 0xa7, 0x68, 0xd7, 0x4b, 0x15, 0x15, 0x23, 0xe2, 0x1e, 0x4e, 0x00, 0x2c, 0x22, 0x97, 0xae, 0xb0, 0x74, 0xa6, 0x99,
0xd0, 0x5d, 0xb7, 0x1b, 0x10, 0x34, 0x13, 0xd2, 0x5f, 0x6e, 0x56, 0xad, 0x85, 0x4a, 0xdb, 0xf0, 0x78, 0xbd, 0xf4, 0x8c,
0xb7, 0x9a, 0x3e, 0x99, 0xef, 0xb9, 0x02, 0x41, 0x00, 0xde, 0x0d, 0xa7, 0x48, 0x75, 0x90, 0xad, 0x11, 0xa1, 0xac, 0xee,
0xcb, 0x41, 0x81, 0xc6, 0xc8, 0x7f, 0xe7, 0x25, 0x94, 0xa1, 0x2a, 0x21, 0xa8, 0x57, 0xfe, 0x84, 0xf2, 0x5e, 0xb4, 0x96,
0x35, 0xaf, 0xef, 0x2e, 0x7a, 0xf8, 0xda, 0x3f, 0xac, 0x8a, 0x3c, 0x1c, 0x9c, 0xbd, 0x44, 0xd6, 0x90, 0xb5, 0xce, 0x1b,
0x12, 0xf9, 0x3b, 0x8c, 0x69, 0xf6, 0xa9, 0x02, 0x93, 0x48, 0x35, 0x0a, 0x7f, 0x02, 0x40, 0x6b, 0x2a, 0x8c, 0x96, 0xd0,
0x7c, 0xd2, 0xfc, 0x9b, 0x52, 0x28, 0x46, 0x89, 0xac, 0x8d, 0xef, 0x2a, 0x80, 0xef, 0xea, 0x01, 0x6f, 0x95, 0x93, 0xee,
0x51, 0x57, 0xd5, 0x97, 0x4b, 0x65, 0x41, 0x86, 0x66, 0xc2, 0x26, 0x80, 0x1e, 0x3e, 0x55, 0x3e, 0x88, 0x63, 0xe2, 0x66,
0x03, 0x47, 0x31, 0xd8, 0xa2, 0x4e, 0x68, 0x45, 0x24, 0x0a, 0xca, 0x17, 0x61, 0xd5, 0x69, 0xca, 0x78, 0xab, 0x21, 0x02,
0x41, 0x00, 0x8f, 0xae, 0x7b, 0x4d, 0x00, 0xc7, 0x06, 0x92, 0xf0, 0x24, 0x9a, 0x83, 0x84, 0xbd, 0x62, 0x81, 0xbc, 0x2c,
0x27, 0x60, 0x2c, 0x0c, 0x33, 0xe5, 0x66, 0x1d, 0x28, 0xd9, 0x10, 0x1a, 0x7f, 0x4f, 0xea, 0x4f, 0x78, 0x6d, 0xb0, 0x14,
0xbf, 0xc9, 0xff, 0x17, 0xd6, 0x47, 0x4d, 0x4a, 0xa8, 0xf4, 0x39, 0x67, 0x3e, 0xb1, 0xec, 0x8f, 0xf1, 0x71, 0xbd, 0xb8,
0xa7, 0x50, 0x3d, 0xc7, 0xf7, 0xbb, 0x02, 0x40, 0x0d, 0x85, 0x32, 0x73, 0x9f, 0x0a, 0x33, 0x2f, 0x4b, 0xa2, 0xbd, 0xd1,
0xb1, 0x42, 0xf0, 0x72, 0xa8, 0x7a, 0xc8, 0x15, 0x37, 0x1b, 0xde, 0x76, 0x70, 0xce, 0xfd, 0x69, 0x20, 0x00, 0x4d, 0xc9,
0x4f, 0x35, 0x6f, 0xd1, 0x35, 0xa1, 0x04, 0x95, 0x30, 0xe8, 0x3b, 0xd5, 0x03, 0x5a, 0x50, 0x21, 0x6d, 0xa0, 0x84, 0x39,
0xe9, 0x2e, 0x1e, 0xfc, 0xe4, 0x82, 0x43, 0x20, 0x46, 0x7d, 0x0a, 0xb6
};

const uint8_t PUBLIC_KEY[] = {
0x30, 0x81, 0x9d, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81,
0x8b, 0x00, 0x30, 0x81, 0x87, 0x02, 0x81, 0x81, 0x00, 0x9e, 0x06, 0x3e, 0x47, 0x85, 0xb2, 0x34, 0x37, 0xaa, 0x85, 0x47,
0xac, 0x03, 0x24, 0x83, 0xb5, 0x9c, 0xa8, 0x05, 0x3a, 0x24, 0x1e, 0xeb, 0x89, 0x01, 0xbb, 0xe9, 0x9b, 0xb2, 0xc3, 0x22,
0xac, 0x68, 0xe3, 0xf0, 0x6c, 0x02, 0xce, 0x68, 0xa6, 0xc4, 0xd0, 0xa7, 0x06, 0x90, 0x9c, 0xaa, 0x1b, 0x08, 0x1d, 0x8b,
0x43, 0x9a, 0x33, 0x67, 0x44, 0x6d, 0x21, 0xa3, 0x1b, 0x88, 0x9a, 0x97, 0x5e, 0x59, 0xc4, 0x15, 0x0b, 0xd9, 0x2c, 0xbd,
0x51, 0x07, 0x61, 0x82, 0xad, 0xc1, 0xb8, 0xd7, 0xbf, 0x9b, 0xcf, 0x7d, 0x24, 0xc2, 0x63, 0xf3, 0x97, 0x17, 0xeb, 0xfe,
0x62, 0x25, 0xba, 0x5b, 0x4d, 0x8a, 0xc2, 0x7a, 0xbd, 0x43, 0x8a, 0x8f, 0xb8, 0xf2, 0xf1, 0xc5, 0x6a, 0x30, 0xd3, 0x50,
0x8c, 0xc8, 0x9a, 0xdf, 0xef, 0xed, 0x35, 0xe7, 0x7a, 0x62, 0xea, 0x76, 0x7c, 0xbb, 0x08, 0x26, 0xc7, 0x02, 0x01, 0x11
};

const uint8_t CERT[] = {
0x30, 0x81, 0xff, 0x30, 0x22, 0x18, 0x0f, 0x32, 0x30, 0x31, 0x33, 0x31, 0x32, 0x32, 0x36, 0x32, 0x33, 0x32, 0x32, 0x35, 0x34, 0x5a, 0x18, 0x0f, 0x32, 0x30, 0x31, 0x33, 0x31, 0x32, 0x32, 0x36, 0x32, 0x33, 0x32, 0x32, 0x35, 0x34, 0x5a, 0x30, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x29, 0x13, 0x09, 0x54, 0x45, 0x53, 0x54, 0x20, 0x4e, 0x41, 0x4d, 0x45, 0x30, 0x81, 0x9d, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8b, 0x00, 0x30, 0x81, 0x87, 0x02, 0x81, 0x81, 0x00, 0x9e, 0x06, 0x3e, 0x47, 0x85, 0xb2, 0x34, 0x37, 0xaa, 0x85, 0x47, 0xac, 0x03, 0x24, 0x83, 0xb5, 0x9c, 0xa8, 0x05, 0x3a, 0x24, 0x1e, 0xeb, 0x89, 0x01, 0xbb, 0xe9, 0x9b, 0xb2, 0xc3, 0x22, 0xac, 0x68, 0xe3, 0xf0, 0x6c, 0x02, 0xce, 0x68, 0xa6, 0xc4, 0xd0, 0xa7, 0x06, 0x90, 0x9c, 0xaa, 0x1b, 0x08, 0x1d, 0x8b, 0x43, 0x9a, 0x33, 0x67, 0x44, 0x6d, 0x21, 0xa3, 0x1b, 0x88, 0x9a, 0x97, 0x5e, 0x59, 0xc4, 0x15, 0x0b, 0xd9, 0x2c, 0xbd, 0x51, 0x07, 0x61, 0x82, 0xad, 0xc1, 0xb8, 0xd7, 0xbf, 0x9b, 0xcf, 0x7d, 0x24, 0xc2, 0x63, 0xf3, 0x97, 0x17, 0xeb, 0xfe, 0x62, 0x25, 0xba, 0x5b, 0x4d, 0x8a, 0xc2, 0x7a, 0xbd, 0x43, 0x8a, 0x8f, 0xb8, 0xf2, 0xf1, 0xc5, 0x6a, 0x30, 0xd3, 0x50, 0x8c, 0xc8, 0x9a, 0xdf, 0xef, 0xed, 0x35, 0xe7, 0x7a, 0x62, 0xea, 0x76, 0x7c, 0xbb, 0x08, 0x26, 0xc7, 0x02, 0x01, 0x11, 0x30, 0x25, 0x30, 0x23, 0x06, 0x06, 0x2b, 0x06, 0x01, 0x05, 0x20, 0x01, 0x01, 0x01, 0xff, 0x04, 0x16, 0x30, 0x14, 0x04, 0x0c, 0x2f, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x2f, 0x6b, 0x69, 0x74, 0x74, 0x79, 0x02, 0x01, 0x00, 0x02, 0x01, 0x0a
};

const std::string CERT_INFO = "Certificate name:\n"
  "  /\n"
  "Validity:\n"
  "  NotBefore: 20131226T232254.000000\n"
  "  NotAfter: 20131226T232254.000000\n"
  "Subject Description:\n"
  "  2.5.4.41: TEST NAME\n"
  "Public key bits:\n"
  "MIGdMA0GCSqGSIb3DQEBAQUAA4GLADCBhwKBgQCeBj5HhbI0N6qFR6wDJIO1nKgF\n"
  "OiQe64kBu+mbssMirGjj8GwCzmimxNCnBpCcqhsIHYtDmjNnRG0hoxuImpdeWcQV\n"
  "C9ksvVEHYYKtwbjXv5vPfSTCY/OXF+v+YiW6W02Kwnq9Q4qPuPLxxWow01CMyJrf\n"
  "7+0153pi6nZ8uwgmxwIB\n";

BOOST_AUTO_TEST_CASE (Encode)
{
  ndn::Certificate c;

  // validity
  c.setNotBefore(1388100174000); // 12/26/2013 @ 11:22pm
  c.setNotAfter(1388100174000); // 12/26/2013 @ 11:22pm

  // subject
  c.addSubjectDescription(CertificateSubjectDescription("2.5.4.41", "TEST NAME"));

  // publicKeyInfo
  ndn::PublicKey key(PUBLIC_KEY, sizeof(PUBLIC_KEY));
  c.setPublicKeyInfo(key);

  // extensions
  BOOST_REQUIRE_NO_THROW({
    std::string extenstionValue;
    StringSink sink(extenstionValue);
    DERSequenceEncoder seq(sink);
    {
      std::string name("/hello/kitty");
      DEREncodeOctetString(seq, reinterpret_cast<const uint8_t*>(name.c_str()), name.size());
      // trustClass
      DEREncodeUnsigned<uint32_t>(seq, 0);
      // trustLevel
      DEREncodeUnsigned<uint32_t>(seq, 10);
    }
    seq.MessageEnd();
  
    c.addExtension(CertificateExtension("1.3.6.1.5.32.1", true,
                                        reinterpret_cast<const uint8_t*>(extenstionValue.c_str()), extenstionValue.size()));
  });
  // RSA::PublicKey p;
  // StringSource source(T, sizeof(T), true);
  // p.Load(source);

  BOOST_REQUIRE_NO_THROW(c.encode());
  
  // ofstream of("cert.out");
  // of.write((const char*)c.getContent().value(), c.getContent().value_size());
  
  // const Block &wire = i.wireEncode();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(CERT, CERT+sizeof(CERT),
                                c.getContent().value_begin(), c.getContent().value_end());

  std::ostringstream os;
  os << c << std::endl;
  std::string info(os.str());
  BOOST_REQUIRE_EQUAL_COLLECTIONS(CERT_INFO.begin(), CERT_INFO.end(),
                                  info.begin(), info.end());
}

const unsigned char REAL_CERT[] = {
0x30, 0x82, 0x01, 0x63, 0x30, 0x22, 0x18, 0x0f, 0x32, 0x30, 0x31, 0x33, 0x31, 0x31, 0x30, 0x31, 0x31, 0x37, 0x31, 0x31, 0x32, 0x32, 0x5a, 0x18, 0x0f, 0x32, 0x30, 0x31, 0x34, 0x31, 0x31, 0x30, 0x31, 0x31, 0x37, 0x31, 0x31, 0x32, 0x32, 0x5a, 0x30, 0x19, 0x30, 0x17, 0x06, 0x03, 0x55, 0x04, 0x29, 0x13, 0x10, 0x4e, 0x44, 0x4e, 0x20, 0x54, 0x65, 0x73, 0x74, 0x62, 0x65, 0x64, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x30, 0x82, 0x01, 0x20, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0d, 0x00, 0x30, 0x82, 0x01, 0x08, 0x02, 0x82, 0x01, 0x01, 0x00, 0xd3, 0xac, 0x7e, 0x7a, 0x5c, 0x33, 0x58, 0x21, 0xda, 0xe0, 0x8d, 0xdb, 0xca, 0xb6, 0x02, 0x30, 0x02, 0x15, 0xc5, 0x0a, 0x51, 0x54, 0xbb, 0x8e, 0x5e, 0x9d, 0x21, 0xf8, 0x14, 0xbe, 0xe4, 0x63, 0x60, 0x31, 0x53, 0xe2, 0xef, 0xee, 0x34, 0xa3, 0x8c, 0xd2, 0x24, 0x6f, 0xa4, 0x89, 0x4f, 0x02, 0x20, 0x7d, 0x66, 0xb6, 0x3f, 0x11, 0x40, 0x0c, 0xc1, 0x5f, 0xd8, 0x45, 0x23, 0x95, 0x40, 0xc8, 0xe0, 0xbc, 0x9d, 0x2f, 0x03, 0xf1, 0x83, 0x9f, 0x07, 0x0b, 0x76, 0xc9, 0x10, 0xd9, 0x3e, 0x0b, 0x75, 0x13, 0x93, 0xe9, 0xc9, 0x85, 0x01, 0x88, 0x36, 0x2e, 0xab, 0xfc, 0xe6, 0x24, 0x32, 0xfc, 0xc6, 0x3c, 0x40, 0x97, 0x1a, 0xcc, 0xcd, 0x53, 0xaa, 0x0f, 0xfb, 0xa3, 0xfe, 0xf9, 0x24, 0x70, 0x13, 0x3f, 0x4f, 0x5b, 0x7d, 0x43, 0xaa, 0x75, 0x0a, 0x94, 0x72, 0xab, 0xe1, 0x8c, 0x45, 0xb5, 0x78, 0x10, 0x01, 0xef, 0x1f, 0xb3, 0x05, 0x6f, 0xa6, 0xc3, 0xac, 0x7f, 0x6d, 0xf0, 0x31, 0xc4, 0x83, 0xb3, 0x4f, 0x50, 0x26, 0x92, 0x40, 0x1a, 0xdd, 0xec, 0xfb, 0xcb, 0xef, 0x63, 0xfe, 0x41, 0xd8, 0x8d, 0x1f, 0xdc, 0xec, 0xfc, 0x48, 0x95, 0xcc, 0x09, 0x1e, 0x30, 0x6e, 0x22, 0x9e, 0x24, 0x97, 0x2e, 0xe6, 0x0c, 0xdf, 0x3d, 0x20, 0x32, 0xaa, 0x9c, 0xc9, 0x45, 0x14, 0xaf, 0xaa, 0xf5, 0x17, 0xd2, 0x01, 0x98, 0x33, 0xbe, 0x2a, 0x9f, 0x7b, 0x9d, 0x98, 0x7c, 0x54, 0x22, 0xfe, 0x72, 0x72, 0x04, 0xc3, 0x2c, 0xc0, 0x14, 0x0b, 0xa9, 0x40, 0x7e, 0x46, 0xa1, 0x75, 0x16, 0x1a, 0x27, 0x9e, 0xf2, 0x82, 0x96, 0xc0, 0x7d, 0xaf, 0x18, 0x75, 0xfb, 0xbb, 0xab, 0x16, 0x66, 0xc0, 0xa9, 0xd7, 0x93, 0x4c, 0x48, 0x6d, 0xce, 0x0b, 0x88, 0xd4, 0x21, 0x93, 0x84, 0x89, 0x55, 0x05, 0xd5, 0x02, 0x01, 0x11
};

const std::string REAL_CERT_INFO = "Certificate name:\n"
"  /tmp\n"
"Validity:\n"
"  NotBefore: 20131101T171122.000000\n"
"  NotAfter: 20141101T171122.000000\n"
"Subject Description:\n"
"  2.5.4.41: NDN Testbed Root\n"
"Public key bits:\n"
"MIIBIDANBgkqhkiG9w0BAQEFAAOCAQ0AMIIBCAKCAQEA06x+elwzWCHa4I3byrYC\n"
"MAIVxQpRVLuOXp0h+BS+5GNgMVPi7+40o4zSJG+kiU8CIH1mtj8RQAzBX9hFI5VA\n"
"yOC8nS8D8YOfBwt2yRDZPgt1E5PpyYUBiDYuq/zmJDL8xjxAlxrMzVOqD/uj/vkk\n"
"cBM/T1t9Q6p1CpRyq+GMRbV4EAHvH7MFb6bDrH9t8DHEg7NPUCaSQBrd7PvL72P+\n"
"QdiNH9zs/EiVzAkeMG4iniSXLuYM3z0gMqqcyUUUr6r1F9IBmDO+Kp97nZh8VCL+\n"
"cnIEwyzAFAupQH5GoXUWGiee8oKWwH2vGHX7u6sWZsCp15NMSG3OC4jUIZOEiVUF\n"
"1QIB\n";

BOOST_AUTO_TEST_CASE (Decode)
{
  ndn::Data d("/tmp");
  d.setContent(REAL_CERT, sizeof(REAL_CERT));

  ndn::Certificate c(d);

  std::ostringstream os;
  os << c << std::endl;
  std::string info(os.str());
  BOOST_REQUIRE_EQUAL_COLLECTIONS(REAL_CERT_INFO.begin(), REAL_CERT_INFO.end(),
                                  info.begin(), info.end());
}

BOOST_AUTO_TEST_SUITE_END()
