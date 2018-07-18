/*
 The MIT License (MIT)

 Copyright (c) [2016] [BTC.COM]

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "gtest/gtest.h"
#include "Common.h"
#include "Utils.h"

#include "sserver/common/StratumServer.h"

// #include "Kafka.h"

#ifndef WORK_WITH_STRATUM_SWITCHER

TEST(StratumServer, SessionIDManager24Bits) {
  // KafkaProducer a("", "", 0);
  // a.produce("", 0);


  SessionIDManagerT<24> m(0xFFu);
  uint32_t j, sessionID;

  // fill all session ids
  for (uint32_t i = 0; i <= 0x00FFFFFEu; i++) {
    uint32_t id = (0xFFu << 24) | i;
    ASSERT_EQ(m.allocSessionId(&sessionID), true);
    ASSERT_EQ(sessionID, id);
  }
  ASSERT_EQ(m.ifFull(), true);

  // free the fisrt one
  j = 0xFF000000u;
  m.freeSessionId(j);
  ASSERT_EQ(m.ifFull(), false);
  ASSERT_EQ(m.allocSessionId(&sessionID), true);
  ASSERT_EQ(sessionID, j);
  ASSERT_EQ(m.ifFull(), true);

  // free the one
  j = 0xFFFFFFFEu;
  m.freeSessionId(j);
  ASSERT_EQ(m.ifFull(), false);
  ASSERT_EQ(m.allocSessionId(&sessionID), true);
  ASSERT_EQ(sessionID, j);
  ASSERT_EQ(m.ifFull(), true);
}

TEST(StratumServer, SessionIDManager16Bits) {
  SessionIDManagerT<16> m(0x99u);
  uint32_t j, sessionID;

  // fill all session ids
  for (uint32_t i = 0; i <= 0x0000FFFEu; i++) {
    uint32_t id = (0x99u << 16) | i;
    ASSERT_EQ(m.allocSessionId(&sessionID), true);
    ASSERT_EQ(sessionID, id);
  }
  ASSERT_EQ(m.ifFull(), true);

  // free the fisrt one
  j = 0x00990000u;
  m.freeSessionId(j);
  ASSERT_EQ(m.ifFull(), false);
  ASSERT_EQ(m.allocSessionId(&sessionID), true);
  ASSERT_EQ(sessionID, j);
  ASSERT_EQ(m.ifFull(), true);

  // free the one
  j = 0x0099FFFEu;
  m.freeSessionId(j);
  ASSERT_EQ(m.ifFull(), false);
  ASSERT_EQ(m.allocSessionId(&sessionID), true);
  ASSERT_EQ(sessionID, j);
  ASSERT_EQ(m.ifFull(), true);
}

TEST(StratumServer, SessionIDManager8Bits) {
  SessionIDManagerT<8> m(0x68u);
  uint32_t j, sessionID;

  // fill all session ids
  for (uint32_t i = 0; i <= 0x000000FEu; i++) {
    uint32_t id = (0x68u << 8) | i;
    ASSERT_EQ(m.allocSessionId(&sessionID), true);
    ASSERT_EQ(sessionID, id);
  }
  ASSERT_EQ(m.ifFull(), true);

  // free the fisrt one
  j = 0x00006800u;
  m.freeSessionId(j);
  ASSERT_EQ(m.ifFull(), false);
  ASSERT_EQ(m.allocSessionId(&sessionID), true);
  ASSERT_EQ(sessionID, j);
  ASSERT_EQ(m.ifFull(), true);

  // free the one
  j = 0x000068FEu;
  m.freeSessionId(j);
  ASSERT_EQ(m.ifFull(), false);
  ASSERT_EQ(m.allocSessionId(&sessionID), true);
  ASSERT_EQ(sessionID, j);
  ASSERT_EQ(m.ifFull(), true);
}

#endif // #ifndef WORK_WITH_STRATUM_SWITCHER
