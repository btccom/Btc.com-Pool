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
#ifndef STRATUM_CKB_H_
#define STRATUM_CKB_H_

#include "Stratum.h"
#include "CommonCkb.h"

#include "ckb/ckb.pb.h"
#include <uint256.h>

class ShareCkb : public sharebase::CkbMsg {
public:
  const static uint32_t CURRENT_VERSION =
      0x0cb0001u; // first 0cb0: Ckb, second 0001: version 1

  ShareCkb() {
    set_version(0);
    set_workerhashid(0);
    set_userid(0);
    set_status(0);
    set_timestamp(0);
    set_ip("0.0.0.0");
    set_sharediff(0);
    set_blockbits(0);
    set_height(0);
    set_nonce(0);
    set_sessionid(0);
    set_username("");
    set_workername("");
  }
  ShareCkb(const ShareCkb &r) = default;
  ShareCkb &operator=(const ShareCkb &r) = default;

  double score() const {

    if (!StratumStatus::isAccepted(status()) || sharediff() == 0 ||
        blockbits() == 0) {
      return 0.0;
    }

    // Network diff may less than share diff on testnet or regression test
    // network. On regression test network, the network diff may be zero. But no
    // matter how low the network diff is, you can only dig one block at a time.

    double networkDiff = 0.0;
    CkbDifficulty::BitsToDifficulty(blockbits(), &networkDiff);

    if (networkDiff < sharediff()) {
      return 1.0;
    } else {
      return (double)sharediff() / networkDiff;
    }
  }

  bool isValid() const {
    if (version() != CURRENT_VERSION) {
      return false;
    }
    if (userid() == 0 || workerhashid() == 0 || blockbits() == 0 ||
        sharediff() == 0) {
      return false;
    }
    return true;
  }

  string toString() const {
    return Strings::Format(
        "share(jobId: %" PRIu64 ",height: %" PRIu64
        ",..., ip: %s, userId: %d, "
        "workerId: %" PRId64
        ", time: %u/%s, "
        "shareDiff: %" PRIu64 ", blockBits: %" PRIu64 ", nonce: %016" PRIx64
        ", "
        "sessionId: %08x, status: %d/%s, username: %s, workername: %s)",
        jobid(),
        height(),
        ip().c_str(),
        userid(),
        workerhashid(),
        timestamp(),
        date("%F %T", timestamp()).c_str(),
        sharediff(),
        blockbits(),
        nonce(),
        sessionid(),
        status(),
        StratumStatus::toString(status()),
        username(),
        workername());
  }

  bool SerializeToBuffer(string &data, uint32_t &size) const {
    size = ByteSize();
    data.resize(size);

    if (!SerializeToArray((uint8_t *)data.data(), size)) {
      DLOG(INFO) << "base.SerializeToArray failed!" << std::endl;
      return false;
    }

    return true;
  }

  bool UnserializeWithVersion(const uint8_t *data, uint32_t size) {
    if (nullptr == data || size <= 0) {
      return false;
    }

    const uint8_t *payload = data;
    uint32_t version = *((uint32_t *)payload);

    if (version == CURRENT_VERSION) {
      if (!ParseFromArray(
              (const uint8_t *)(payload + sizeof(uint32_t)),
              size - sizeof(uint32_t))) {
        DLOG(INFO) << "share ParseFromArray failed!";
        return false;
      }
    } else {
      DLOG(INFO) << "unknow share received! data size: " << size;
      return false;
    }

    return true;
  }

  bool SerializeToArrayWithLength(string &data, uint32_t &size) const {
    size = ByteSize();
    data.resize(size + sizeof(uint32_t));

    *((uint32_t *)data.data()) = size;
    uint8_t *payload = (uint8_t *)data.data();

    if (!SerializeToArray(payload + sizeof(uint32_t), size)) {
      DLOG(INFO) << "base.SerializeToArray failed!";
      return false;
    }

    size += sizeof(uint32_t);
    return true;
  }

  bool SerializeToArrayWithVersion(string &data, uint32_t &size) const {
    size = ByteSize();
    data.resize(size + sizeof(uint32_t));

    uint8_t *payload = (uint8_t *)data.data();
    *((uint32_t *)payload) = version();

    if (!SerializeToArray(payload + sizeof(uint32_t), size)) {
      DLOG(INFO) << "SerializeToArray failed!";
      return false;
    }

    size += sizeof(uint32_t);
    return true;
  }

  size_t getsharelength() { return IsInitialized() ? ByteSize() : 0; }
};

class StratumJobCkb : public StratumJob {
public:
  StratumJobCkb();
  string serializeToJson() const override;
  bool unserializeFromJson(const char *s, size_t len) override;
  bool initFromRawJob(const string &msg);
  uint64_t height() const override { return height_; }

  string pow_hash_;
  string parent_hash_;
  uint64_t height_;
  uint64_t work_id_;
  string target_;
  uint64_t nTime_;
  uint64_t timestamp_;
};

class StratumServerCkb;
class StratumSessionCkb;

struct StratumTraitsCkb {
  using ServerType = StratumServerCkb;
  using SessionType = StratumSessionCkb;
  using LocalJobType = LocalJob;
  struct JobDiffType {
    uint64_t currentJobDiff_;
    std::set<uint64_t> jobDiffs_;

    JobDiffType &operator=(uint64_t diff) {
      jobDiffs_.insert(diff);
      currentJobDiff_ = diff;
      return *this;
    }
  };
};
#endif
