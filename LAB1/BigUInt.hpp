#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cstdint>
#include <algorithm>

class BigUInt {
public:
    BigUInt();
    BigUInt(uint64_t n);
    explicit BigUInt(const std::string& str);

    // Lab1
    BigUInt operator+(const BigUInt& other) const;
    BigUInt operator-(const BigUInt& other) const;
    BigUInt operator*(const BigUInt& other) const;
    BigUInt operator/(const BigUInt& other) const;
    BigUInt operator%(const BigUInt& other) const;

    int compare(const BigUInt& other) const;
    bool operator==(const BigUInt& other) const;
    bool operator!=(const BigUInt& other) const;
    bool operator<(const BigUInt& other) const;
    bool operator>(const BigUInt& other) const;
    bool operator<=(const BigUInt& other) const;
    bool operator>=(const BigUInt& other) const;

    BigUInt pow(const BigUInt& exponent) const;
    std::string toHex() const;
    std::string toDec() const;

    // Lab2
    static BigUInt gcd(const BigUInt& a, const BigUInt& b);
    static BigUInt lcm(const BigUInt& a, const BigUInt& b);
    BigUInt powMod(const BigUInt& exponent, const BigUInt& modulus) const;

    // v8
    static BigUInt calculateBarrettMu(const BigUInt& n);
    static BigUInt barrettReduction(const BigUInt& x, const BigUInt& n, const BigUInt& mu);

    static BigUInt getMontgomeryR(const BigUInt& n);
    static BigUInt calculateMontgomeryInverse(const BigUInt& n, const BigUInt& R);
    static BigUInt montgomeryReduction(const BigUInt& T, const BigUInt& n, const BigUInt& n_prime, const BigUInt& R);

    struct GcdResult;
    static GcdResult extendedGcd(const BigUInt& a, const BigUInt& b);


    int bitLength() const;
    bool getBit(int index) const;
    void setBit(int index);
    void shiftLeft(int bits);
    void shiftRight(int bits);
    void shiftRightWords(int words);

private:
    std::vector<uint32_t> digits;

    void stripZeros();
    static void divMod(const BigUInt& dividend, const BigUInt& divisor, BigUInt& quotient, BigUInt& remainder);
};

std::ostream& operator<<(std::ostream& os, const BigUInt& num);

struct BigUInt::GcdResult {
    BigUInt gcd;
    BigUInt x;
    BigUInt y;
    bool x_neg;
    bool y_neg;
};