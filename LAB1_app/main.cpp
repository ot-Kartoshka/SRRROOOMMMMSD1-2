#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cassert>
#include "BigUInt.hpp"

using namespace std;

template<typename Func>
long long measure_time(Func f) {
    auto start = chrono::high_resolution_clock::now();
    f();
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

void demo_lab1() {
    cout << ("LAB1\n");

    BigUInt a("123456789012345678901234567890");
    BigUInt b("100000000000000000000");

    cout << "A = " << a.toDec() << "\n";
    cout << "B = " << b.toDec() << "\n\n";

    cout << "A + B = " << (a + b).toDec() << "\n";
    cout << "A - B = " << (a - b).toDec() << "\n";
    cout << "A * B = " << (a * b).toDec() << "\n";
    cout << "A / B = " << (a / b).toDec() << "\n";
    cout << "A % B = " << (a % b).toDec() << "\n";

    cout << "\n[SUCCESS]\n";
}

void demo_lab2() {
    cout << ("\nLAB2\n");

    BigUInt g1("123456");
    BigUInt g2("654321");
    cout << "GCD(" << g1.toDec() << ", " << g2.toDec() << ") = "
        << BigUInt::gcd(g1, g2).toDec() << "\n";
    cout << "LCM(" << g1.toDec() << ", " << g2.toDec() << ") = "
        << BigUInt::lcm(g1, g2).toDec() << "\n";

    BigUInt base("123");
    BigUInt exp("456");
    BigUInt mod("789");
    cout << "123^456 mod 789 = " << base.powMod(exp, mod).toDec() << "\n";

    BigUInt p("17");
    BigUInt a("5");
    BigUInt res = a.powMod(p - BigUInt(1), p);
    cout << "Fermat Test (5^16 mod 17) = " << res.toDec();
    if (res.toDec() == "1") cout << " [PASSED]\n";
    else cout << " [FAILED]\n";
}

void demo_variant8() {
    cout << ("\nV8\n");

    string hexN = "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";

    string hexN_odd = hexN.substr(0, hexN.size() - 1) + "1";

    BigUInt N(hexN_odd);
    BigUInt A(hexN_odd);

    A = A * BigUInt("0x123456789ABCDEF");

    cout << "Modulus size: " << N.toHex().length() * 4 << " bits\n";
    cout << "Comparisons over 1000 iterations:\n\n";

    int iterations = 1000;
    BigUInt resStd, resBar, resMont;

    auto tStd = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) resStd = A % N;
        });
    cout << "1. Standard Division (%):  " << tStd << " us\n";

    BigUInt mu = BigUInt::calculateBarrettMu(N);
    auto tBar = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) resBar = BigUInt::barrettReduction(A, N, mu);
        });
    cout << "2. Barrett Reduction:      " << tBar << " us  (Speedup: " << (double)tStd / tBar << "x)\n";

    BigUInt R = BigUInt::getMontgomeryR(N);
    BigUInt n_prime = BigUInt::calculateMontgomeryInverse(N, R);
    BigUInt A_mont = (A * R) % N;

    auto tMont = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) {
            resMont = BigUInt::montgomeryReduction(A_mont, N, n_prime, R);
        }
        });
    cout << "3. Montgomery Reduction:   " << tMont << " us  (Speedup: " << (double)tStd / tMont << "x)\n";

    if (resStd == resBar) cout << "\n[VERIFY]\n";
    else cout << "\n[ERROR] Barrett mismatch!\n";

    if (resStd == resMont) cout << "[VERIFY]\n";
    else cout << "[ERROR] Montgomery mismatch!\n";
}

void check_identities() {
    cout << ("\nIdentity Checks\n");
    BigUInt a("1234567890123456789"), b("6789012341248456168"), c("1357902456716451815");

    BigUInt left = (a + b) * c;
    BigUInt right = (a * c) + (b * c);

    cout << "(a + b) * c  = " << left.toDec() << endl;
    cout << "a*c + b*c    = " << right.toDec() << endl;
    cout << (left == right ? "PASSED" : "FAILED") << endl;
}
int main() {
    try {
        demo_lab1();
        demo_lab2();
        demo_variant8();
        check_identities();
        cout << "\nAll finish successfully.\n";
    }
    catch (const exception& ex) {
        cerr << "Critical Error: " << ex.what() << endl;
        return 1;
    }
    return 0;
}