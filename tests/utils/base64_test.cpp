#include <gtest/gtest.h>

#include "utils/base64.h"

namespace base64 = vibra::base64;

TEST(Base64Test, EncodesEmptyInput) {
  EXPECT_EQ(base64::encode("", 0), "");
}

TEST(Base64Test, EncodesInputsWithPadding) {
  EXPECT_EQ(base64::encode("f", 1), "Zg==");
  EXPECT_EQ(base64::encode("fo", 2), "Zm8=");
}

TEST(Base64Test, EncodesInputWithoutPadding) {
  EXPECT_EQ(base64::encode("foo", 3), "Zm9v");
  EXPECT_EQ(base64::encode("hello", 5), "aGVsbG8=");
}
