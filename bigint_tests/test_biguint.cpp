#include "pch.h"
#include <gtest/gtest.h>
#include <random>
#include <string>
#include "BigUInt.hpp"

class BigUIntTest : public ::testing::Test {
protected:
    std::mt19937_64 rng;
    BigUIntTest() : rng(42) {} 

    std::string randomHex(int length) {
        const char hex_chars[] = "0123456789ABCDEF";
        std::string s = "0x";
        s += hex_chars[rng() % 15 + 1]; 
        for (int i = 1; i < length; ++i) {
            s += hex_chars[rng() % 16];
        }
        return s;
    }
};

TEST_F(BigUIntTest, IO_DefaultConstructor) {
    BigUInt a;
    EXPECT_EQ(a.toDec(), "0");
    EXPECT_EQ(a.toHex(), "0");
}

TEST_F(BigUIntTest, IO_Uint64Constructor) {
    BigUInt a(123456789ULL);
    EXPECT_EQ(a.toDec(), "123456789");

    // Max uint64
    BigUInt b(0xFFFFFFFFFFFFFFFFULL);
    EXPECT_EQ(b.toHex(), "FFFFFFFFFFFFFFFF");
}

TEST_F(BigUIntTest, IO_StringHexParsing) {
    BigUInt a("0xABC");
    
    BigUInt b("0xabc"); 
    BigUInt c("0xABC");
    
    EXPECT_EQ(a, b);
    EXPECT_EQ(b, c);
    EXPECT_EQ(a.toDec(), "2748");
}

TEST_F(BigUIntTest, IO_StringDecParsing) {
    std::string big = "1234567890123456789012345678901234567890";
    BigUInt a(big);
    EXPECT_EQ(a.toDec(), big);
}


TEST_F(BigUIntTest, Cmp_Equality) {
    BigUInt a("100");
    BigUInt b("100");
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST_F(BigUIntTest, Cmp_Inequality) {
    BigUInt a("100");
    BigUInt b("101");
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b > a);
    EXPECT_FALSE(a == b);
}

TEST_F(BigUIntTest, Cmp_LargeNumbers) {
    BigUInt a("1");
    a.shiftLeft(100); 
    BigUInt b("1");
    b.shiftLeft(99);  
    EXPECT_TRUE(a > b);
}

TEST_F(BigUIntTest, Add_Simple) {
    BigUInt a("10");
    BigUInt b("20");
    EXPECT_EQ((a + b).toDec(), "30");
}

TEST_F(BigUIntTest, Add_Commutativity) {
    BigUInt a(randomHex(50));
    BigUInt b(randomHex(50));
    EXPECT_EQ(a + b, b + a);
}

TEST_F(BigUIntTest, Add_CarryPropagation) {
    BigUInt a("0xFFFFFFFF");
    BigUInt b("1");
    EXPECT_EQ((a + b).toHex(), "100000000");

    BigUInt c("0xFFFFFFFFFFFFFFFF");
    EXPECT_EQ((c + b).toHex(), "10000000000000000");
}

TEST_F(BigUIntTest, Add_Zero) {
    BigUInt a("12345");
    BigUInt z("0");
    EXPECT_EQ(a + z, a);
}

TEST_F(BigUIntTest, Sub_Simple) {
    BigUInt a("50");
    BigUInt b("30");
    EXPECT_EQ((a - b).toDec(), "20");
}

TEST_F(BigUIntTest, Sub_BorrowPropagation) {
    BigUInt a("0x10000");
    BigUInt b("1");
    EXPECT_EQ((a - b).toHex(), "FFFF");

    BigUInt c("0x100000000"); 
    EXPECT_EQ((c - b).toHex(), "FFFFFFFF");
}

TEST_F(BigUIntTest, Sub_ZeroResult) {
    BigUInt a("12345");
    EXPECT_EQ((a - a).toDec(), "0");
}

TEST_F(BigUIntTest, Sub_UnderflowException) {
    BigUInt a("10");
    BigUInt b("20");
    EXPECT_THROW(a - b, std::runtime_error);
}

TEST_F(BigUIntTest, Mul_Simple) {
    BigUInt a("12");
    BigUInt b("12");
    EXPECT_EQ((a * b).toDec(), "144");
}

TEST_F(BigUIntTest, Mul_ByZero) {
    BigUInt a(randomHex(50));
    BigUInt z("0");
    EXPECT_EQ((a * z).toDec(), "0");
}

TEST_F(BigUIntTest, Mul_ByOne) {
    BigUInt a(randomHex(50));
    BigUInt one("1");
    EXPECT_EQ(a * one, a);
}

TEST_F(BigUIntTest, Mul_LargeNumbers) {
    BigUInt a("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    BigUInt res = a * a;
    BigUInt x("10000000000000000"); 
    BigUInt y = x * x;
    EXPECT_EQ(y.toDec(), "100000000000000000000000000000000");
}

TEST_F(BigUIntTest, Div_Simple) {
    BigUInt a("100");
    BigUInt b("25");
    EXPECT_EQ((a / b).toDec(), "4");
    EXPECT_EQ((a % b).toDec(), "0");
}

TEST_F(BigUIntTest, Div_WithRemainder) {
    BigUInt a("100");
    BigUInt b("30");
    EXPECT_EQ((a / b).toDec(), "3");
    EXPECT_EQ((a % b).toDec(), "10");
}

TEST_F(BigUIntTest, Div_ByLarger) {
    BigUInt a("10");
    BigUInt b("20");
    EXPECT_EQ((a / b).toDec(), "0");
    EXPECT_EQ((a % b).toDec(), "10");
}

TEST_F(BigUIntTest, Div_ByZero) {
    BigUInt a("123");
    BigUInt z("0");
    EXPECT_THROW(a / z, std::runtime_error);
    EXPECT_THROW(a % z, std::runtime_error);
}

TEST_F(BigUIntTest, DivMod_IdentityProperty) {
    for (int i = 0; i < 50; ++i) {
        BigUInt A(randomHex(20 + rng() % 30));
        BigUInt B(randomHex(1 + rng() % 10));

        BigUInt Q = A / B;
        BigUInt R = A % B;

        EXPECT_TRUE(R < B);
        EXPECT_EQ((Q * B) + R, A);
    }
}

TEST_F(BigUIntTest, Pow_Basic) {
    BigUInt a("2");
    EXPECT_EQ(a.pow(BigUInt(10)).toDec(), "1024");
}

TEST_F(BigUIntTest, Pow_ZeroExp) {
    BigUInt a(randomHex(20));
    EXPECT_EQ(a.pow(BigUInt(0)).toDec(), "1");
}

TEST_F(BigUIntTest, Pow_OneExp) {
    BigUInt a(randomHex(20));
    EXPECT_EQ(a.pow(BigUInt(1)), a);
}

TEST_F(BigUIntTest, GCD_Simple) {
    EXPECT_EQ(BigUInt::gcd(BigUInt(12), BigUInt(18)).toDec(), "6");
    EXPECT_EQ(BigUInt::gcd(BigUInt(17), BigUInt(13)).toDec(), "1"); 
}

TEST_F(BigUIntTest, GCD_WithZero) {
    BigUInt a("12345");
    EXPECT_EQ(BigUInt::gcd(a, BigUInt(0)), a);
    EXPECT_EQ(BigUInt::gcd(BigUInt(0), a), a);
}

TEST_F(BigUIntTest, LCM_Simple) {
    EXPECT_EQ(BigUInt::lcm(BigUInt(4), BigUInt(6)).toDec(), "12");
}

TEST_F(BigUIntTest, LCM_Relation) {
    BigUInt a("1234");
    BigUInt b("5678");
    BigUInt mul = a * b;
    BigUInt lcm_gcd = BigUInt::lcm(a, b) * BigUInt::gcd(a, b);
    EXPECT_EQ(mul, lcm_gcd);
}

TEST_F(BigUIntTest, PowMod_Basic) {
    EXPECT_EQ(BigUInt(5).powMod(BigUInt(3), BigUInt(13)).toDec(), "8");
}

TEST_F(BigUIntTest, PowMod_FermatTest) {
    BigUInt p("101");
    BigUInt a("73");
    EXPECT_EQ(a.powMod(p - BigUInt(1), p).toDec(), "1");
}

TEST_F(BigUIntTest, PowMod_ModOne) {
    BigUInt a("12345");
    EXPECT_EQ(a.powMod(BigUInt("10"), BigUInt("1")).toDec(), "0");
}

TEST_F(BigUIntTest, Variant8_BarrettMu) {
    BigUInt N("123456");

    EXPECT_NO_THROW(BigUInt::calculateBarrettMu(N));
}

TEST_F(BigUIntTest, Variant8_Barrett_Stress) {
    for (int i = 0; i < 50; ++i) {
        BigUInt N(randomHex(10)); 
        BigUInt A(randomHex(20)); 

        BigUInt mu = BigUInt::calculateBarrettMu(N);
        BigUInt expected = A % N;
        BigUInt actual = BigUInt::barrettReduction(A, N, mu);

        ASSERT_EQ(actual, expected) << "Barrett mismatch for N=" << N.toHex();
    }
}

TEST_F(BigUIntTest, Variant8_MontgomeryR) {
    BigUInt N("0x123456");
    BigUInt R = BigUInt::getMontgomeryR(N);
    BigUInt check("1");
    check.shiftLeft(32);
    EXPECT_EQ(R, check);
}

TEST_F(BigUIntTest, Variant8_Montgomery_Stress) {
    for (int i = 0; i < 50; ++i) {
        std::string sN = randomHex(10);
        char last = sN.back();
        int val = (last >= '0' && last <= '9') ? (last - '0') : (last - 'A' + 10);
        if (val % 2 == 0) sN.back() = (val + 1 >= 16) ? 'F' : "0123456789ABCDEF"[val + 1];

        BigUInt N(sN);
        BigUInt A(randomHex(20));

        BigUInt R = BigUInt::getMontgomeryR(N);
        BigUInt n_prime = BigUInt::calculateMontgomeryInverse(N, R);

        BigUInt A_mont = (A * R) % N;

        BigUInt res = BigUInt::montgomeryReduction(A_mont, N, n_prime, R);

        ASSERT_EQ(res, A % N) << "Montgomery mismatch for N=" << sN;
    }
}

TEST_F(BigUIntTest, Variant8_ExtendedGCD) {
    BigUInt a("30");
    BigUInt b("20");
    auto res = BigUInt::extendedGcd(a, b);
    EXPECT_EQ(res.gcd.toDec(), "10");
}

TEST_F(BigUIntTest, BitOps_ShiftLeft) {
    BigUInt a("1");
    a.shiftLeft(10); 
    EXPECT_EQ(a.toDec(), "1024");
}

TEST_F(BigUIntTest, BitOps_ShiftRight) {
    BigUInt a("1024");
    a.shiftRight(1); 
    EXPECT_EQ(a.toDec(), "512");
}

TEST_F(BigUIntTest, BitOps_GetSetBit) {
    BigUInt a(0);
    a.setBit(0);
    EXPECT_EQ(a.toDec(), "1");
    a.setBit(10);
    EXPECT_EQ(a.getBit(10), true);
    EXPECT_EQ(a.getBit(9), false);
}