#include "BigUInt.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cmath>

const uint64_t BASE = 4294967296ULL;

BigUInt::BigUInt() {
    digits.push_back(0);
}

BigUInt::BigUInt(uint64_t n) {
    if (n == 0) {
        digits.push_back(0);
    }
    else {
        digits.push_back(static_cast<uint32_t>(n));
        uint32_t high = static_cast<uint32_t>(n >> 32);
        if (high > 0) digits.push_back(high);
    }
}

BigUInt::BigUInt(const std::string& str) {
    if (str.empty()) { digits.push_back(0); return; }

    std::string s = str;
    bool isHex = false;

    if (s.size() >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        isHex = true;
        s = s.substr(2);
    }

    if (isHex) {
        while (s.length() % 8 != 0) s = "0" + s;
        for (size_t i = 0; i < s.length(); i += 8) {
            std::string block = s.substr(s.length() - 8 - i, 8);
            uint32_t val = std::stoul(block, nullptr, 16);
            digits.push_back(val);
        }
    }
    else {
        digits.push_back(0);
        BigUInt tenPower(1000000000);
        size_t idx = 0;
        size_t firstBlock = s.length() % 9;

        if (firstBlock > 0) {
            uint32_t val = std::stoul(s.substr(0, firstBlock));
            *this = *this + BigUInt(val);
            idx = firstBlock;
        }

        while (idx < s.length()) {
            uint32_t val = std::stoul(s.substr(idx, 9));
            *this = *this * tenPower;
            *this = *this + BigUInt(val);
            idx += 9;
        }
    }
    stripZeros();
}

void BigUInt::stripZeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
}

// Lab1

BigUInt BigUInt::operator+(const BigUInt& other) const {
    BigUInt res;
    res.digits.clear();
    uint64_t carry = 0;
    size_t n = std::max(digits.size(), other.digits.size());

    for (size_t i = 0; i < n || carry; ++i) {
        uint64_t a = (i < digits.size()) ? digits[i] : 0;
        uint64_t b = (i < other.digits.size()) ? other.digits[i] : 0;
        uint64_t sum = a + b + carry;
        res.digits.push_back(static_cast<uint32_t>(sum));
        carry = sum >> 32;
    }
    return res;
}

BigUInt BigUInt::operator-(const BigUInt& other) const {
    if (*this < other) throw std::runtime_error("BigUInt subtraction underflow");
    BigUInt res;
    res.digits.clear();
    int64_t borrow = 0;

    for (size_t i = 0; i < digits.size(); ++i) {
        int64_t a = digits[i];
        int64_t b = (i < other.digits.size()) ? other.digits[i] : 0;
        int64_t diff = a - b - borrow;
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        res.digits.push_back(static_cast<uint32_t>(diff));
    }
    res.stripZeros();
    return res;
}

BigUInt BigUInt::operator*(const BigUInt& other) const {
    if (*this == BigUInt(0) || other == BigUInt(0)) return BigUInt(0);
    BigUInt res;
    res.digits.resize(digits.size() + other.digits.size(), 0);

    for (size_t i = 0; i < digits.size(); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry; ++j) {
            uint64_t cur = res.digits[i + j] +
                static_cast<uint64_t>(digits[i]) * (j < other.digits.size() ? other.digits[j] : 0) +
                carry;
            res.digits[i + j] = static_cast<uint32_t>(cur);
            carry = cur >> 32;
        }
    }
    res.stripZeros();
    return res;
}

void BigUInt::divMod(const BigUInt& dividend, const BigUInt& divisor, BigUInt& quotient, BigUInt& remainder) {
    if (divisor == BigUInt(0)) throw std::runtime_error("Division by zero");
    if (dividend < divisor) {
        quotient = BigUInt(0);
        remainder = dividend;
        return;
    }
    quotient = BigUInt(0);
    remainder = BigUInt(0);

    for (int i = dividend.bitLength() - 1; i >= 0; --i) {
        remainder.shiftLeft(1);
        if (dividend.getBit(i)) remainder.digits[0] |= 1;
        if (remainder >= divisor) {
            remainder = remainder - divisor;
            quotient.setBit(i);
        }
    }
    quotient.stripZeros();
    remainder.stripZeros();
}

BigUInt BigUInt::operator/(const BigUInt& other) const {
    BigUInt q, r; divMod(*this, other, q, r); return q;
}

BigUInt BigUInt::operator%(const BigUInt& other) const {
    BigUInt q, r; divMod(*this, other, q, r); return r;
}

BigUInt BigUInt::pow(const BigUInt& exponent) const {
    BigUInt res(1);
    BigUInt base = *this;
    for (int i = 0; i < exponent.bitLength(); ++i) {
        if (exponent.getBit(i)) res = res * base;
        base = base * base;
    }
    return res;
}

// Lab2

BigUInt BigUInt::gcd(const BigUInt& a, const BigUInt& b) {
    BigUInt x = a, y = b;
    while (y != BigUInt(0)) {
        BigUInt r = x % y;
        x = y; y = r;
    }
    return x;
}

BigUInt BigUInt::lcm(const BigUInt& a, const BigUInt& b) {
    if (a == BigUInt(0) || b == BigUInt(0)) return BigUInt(0);
    return (a * b) / gcd(a, b);
}

BigUInt BigUInt::powMod(const BigUInt& exponent, const BigUInt& modulus) const {
    if (modulus == BigUInt(0)) throw std::runtime_error("Modulo by zero");
    BigUInt res(1);
    BigUInt base = *this % modulus;
    for (int i = 0; i < exponent.bitLength(); ++i) {
        if (exponent.getBit(i)) res = (res * base) % modulus;
        base = (base * base) % modulus;
    }
    return res;
}

// v8

BigUInt BigUInt::calculateBarrettMu(const BigUInt& n) {
    size_t k = n.digits.size();
    BigUInt b2k(1);
    std::vector<uint32_t> shifted(2 * k, 0);
    shifted.push_back(1);
    b2k.digits = shifted;
    return b2k / n;
}

BigUInt BigUInt::barrettReduction(const BigUInt& x, const BigUInt& n, const BigUInt& mu) {
    if (x < n) return x;
    size_t k = n.digits.size();
    BigUInt q = x;
    if (k > 1) q.shiftRightWords(static_cast<int>(k) - 1);
    q = q * mu;
    q.shiftRightWords(static_cast<int>(k) + 1);

    BigUInt r = x - (q * n);
    while (r >= n) r = r - n;
    return r;
}

BigUInt BigUInt::getMontgomeryR(const BigUInt& n) {
    BigUInt R(1);
    size_t words = n.digits.size();
    std::vector<uint32_t> r_digits(words, 0);
    r_digits.push_back(1);
    R.digits = r_digits;
    return R;
}

BigUInt::GcdResult BigUInt::extendedGcd(const BigUInt& a, const BigUInt& b) {
    if (a == BigUInt(0)) {
        return { b, BigUInt(0), BigUInt(1), false, false };
    }
    GcdResult res = extendedGcd(b % a, a);

    BigUInt mul = (b / a) * res.x;
    BigUInt new_x;
    bool new_x_neg;

    if (res.y_neg == res.x_neg) {
        if (res.y < mul) {
            new_x = mul - res.y;
            new_x_neg = !res.y_neg;
        }
        else {
            new_x = res.y - mul;
            new_x_neg = res.y_neg;
        }
    }
    else {
        new_x = res.y + mul;
        new_x_neg = res.y_neg;
    }
    return { res.gcd, new_x, res.x, new_x_neg, res.x_neg };
}

BigUInt BigUInt::calculateMontgomeryInverse(const BigUInt& n, const BigUInt& R) {
    GcdResult res = extendedGcd(n, R);
    if (res.x_neg) return res.x;
    return R - res.x;
}

BigUInt BigUInt::montgomeryReduction(const BigUInt& T, const BigUInt& n, const BigUInt& n_prime, const BigUInt& R) {
    BigUInt m = (T % R) * n_prime;
    m = m % R;
    BigUInt t = (T + m * n) / R;
    if (t >= n) return t - n;
    return t;
}

// pluss

int BigUInt::compare(const BigUInt& other) const {
    if (digits.size() > other.digits.size()) return 1;
    if (digits.size() < other.digits.size()) return -1;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        if (digits[i] > other.digits[i]) return 1;
        if (digits[i] < other.digits[i]) return -1;
    }
    return 0;
}
bool BigUInt::operator==(const BigUInt& o) const { return compare(o) == 0; }
bool BigUInt::operator!=(const BigUInt& o) const { return compare(o) != 0; }
bool BigUInt::operator<(const BigUInt& o) const { return compare(o) < 0; }
bool BigUInt::operator>(const BigUInt& o) const { return compare(o) > 0; }
bool BigUInt::operator>=(const BigUInt& o) const { return compare(o) >= 0; }
bool BigUInt::operator<=(const BigUInt& o) const { return compare(o) <= 0; }

std::string BigUInt::toHex() const {
    if (digits.empty() || (digits.size() == 1 && digits[0] == 0)) return "0";
    std::stringstream ss;
    ss << std::hex << std::uppercase << digits.back();
    for (int i = static_cast<int>(digits.size()) - 2; i >= 0; --i) {
        ss << std::setw(8) << std::setfill('0') << digits[i];
    }
    return ss.str();
}

std::string BigUInt::toDec() const {
    if (*this == BigUInt(0)) return "0";
    BigUInt temp = *this;
    BigUInt ten(1000000000);
    std::vector<uint32_t> blocks;
    while (temp > BigUInt(0)) {
        BigUInt q, r; divMod(temp, ten, q, r);
        blocks.push_back(r.digits.empty() ? 0 : r.digits[0]);
        temp = q;
    }
    std::stringstream ss;
    ss << blocks.back();
    for (int i = static_cast<int>(blocks.size()) - 2; i >= 0; --i) {
        ss << std::setw(9) << std::setfill('0') << blocks[i];
    }
    return ss.str();
}

int BigUInt::bitLength() const {
    if (digits.empty()) return 0;
    int words = static_cast<int>(digits.size()) - 1;
    uint32_t msb = digits.back();
    int bits = 0;
    while (msb > 0) { msb >>= 1; bits++; }
    return words * 32 + bits;
}

bool BigUInt::getBit(int index) const {
    size_t wordIdx = index / 32;
    int bitIdx = index % 32;
    if (wordIdx >= digits.size()) return false;
    return (digits[wordIdx] >> bitIdx) & 1;
}

void BigUInt::setBit(int index) {
    size_t wordIdx = index / 32;
    int bitIdx = index % 32;
    if (wordIdx >= digits.size()) digits.resize(wordIdx + 1, 0);
    digits[wordIdx] |= (1U << bitIdx);
}

void BigUInt::shiftLeft(int bits) {
    if (bits == 0) return;
    int wordShift = bits / 32;
    int bitShift = bits % 32;
    std::vector<uint32_t> newDigits(digits.size() + wordShift + 1, 0);
    uint64_t carry = 0;
    for (size_t i = 0; i < digits.size(); ++i) {
        uint64_t val = (static_cast<uint64_t>(digits[i]) << bitShift) | carry;
        newDigits[i + wordShift] = static_cast<uint32_t>(val);
        carry = val >> 32;
    }
    if (carry) newDigits[digits.size() + wordShift] = static_cast<uint32_t>(carry);
    digits = newDigits;
    stripZeros();
}

void BigUInt::shiftRight(int bits) {
    int wordShift = bits / 32;
    int bitShift = bits % 32;
    if (wordShift >= static_cast<int>(digits.size())) { digits = { 0 }; return; }
    std::vector<uint32_t> newDigits;
    for (size_t i = wordShift; i < digits.size(); ++i) {
        uint32_t low = digits[i] >> bitShift;
        uint32_t high = 0;
        if (i + 1 < digits.size()) high = digits[i + 1] << (32 - bitShift);
        newDigits.push_back(low | high);
    }
    digits = newDigits;
    stripZeros();
}

void BigUInt::shiftRightWords(int words) {
    if (words >= static_cast<int>(digits.size())) digits = { 0 };
    else digits.erase(digits.begin(), digits.begin() + words);
}

std::ostream& operator<<(std::ostream& os, const BigUInt& num) {
    os << num.toHex();
    return os;
}