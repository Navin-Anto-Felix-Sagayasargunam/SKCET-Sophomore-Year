#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

//// Helper Functions ////

// Convert an integer to binary (Two's Complement for negative numbers)
vector<bool> integerToBinary(int number, int bitWidth) {
    vector<bool> binary(bitWidth, 0); // Initialize with 0s
    bool isNegative = (number < 0);  // Check if the number is negative

    // Convert to binary (absolute value)
    number = abs(number);
    for (int i = bitWidth - 1; i >= 0; i--) {
        binary[i] = number % 2;
        number /= 2;
    }

    // If the number is negative, calculate the Two's Complement
    if (isNegative) {
        // Step 1: Invert bits
        for (int i = 0; i < bitWidth; i++) {
            binary[i] = !binary[i];
        }
        // Step 2: Add 1 to the LSB
        bool carry = 1;
        for (int i = bitWidth - 1; i >= 0; i--) {
            binary[i] = binary[i] ^ carry; // XOR with carry
            carry = binary[i] & carry;    // AND to propagate carry
        }
    }

    return binary;
}

// Convert binary (Two's Complement) to integer
int binaryToInteger(const vector<bool>& binary) {
    bool isNegative = binary[0]; // Sign bit (MSB)
    int bitWidth = binary.size();
    vector<bool> absBinary = binary;

    // If negative, convert from Two's Complement
    if (isNegative) {
        // Step 1: Invert bits
        for (int i = 0; i < bitWidth; i++) {
            absBinary[i] = !absBinary[i];
        }
        // Step 2: Add 1
        bool carry = 1;
        for (int i = bitWidth - 1; i >= 0; i--) {
            absBinary[i] = absBinary[i] ^ carry;
            carry = absBinary[i] & carry;
        }
    }

    // Convert to integer
    int result = 0;
    for (int i = 0; i < bitWidth; i++) {
        result = (result << 1) + absBinary[i];
    }

    return isNegative ? -result : result;
}

// Print binary representation
void printBinary(const vector<bool>& binary) {
    for (bool bit : binary) {
        cout << bit;
    }
}

//// Booth's Algorithm for Signed Multiplication ////
vector<bool> boothsAlgorithm(int multiplicand, int multiplier, int bitWidth) {
    // Step 1: Initialize Booth's Algorithm Registers
    vector<bool> A(bitWidth, 0); // Accumulator (A)
    vector<bool> Q = integerToBinary(multiplier, bitWidth); // Multiplier (Q)
    vector<bool> M = integerToBinary(multiplicand, bitWidth); // Multiplicand (M)
    bool QMinus1 = 0; // Q(-1) bit
    int count = bitWidth; // Iteration count

    cout << "Initial Values:\n";
    cout << "A: "; printBinary(A); cout << "\n";
    cout << "Q: "; printBinary(Q); cout << "\n";
    cout << "M: "; printBinary(M); cout << "\n";
    cout << "Q-1: " << QMinus1 << "\n";
    cout << "--------------------\n";

    // Step 2: Booth's Algorithm Iterations
    while (count > 0) {
        // Step 2.1: Check Q0 and Q(-1)
        if (Q.back() == 1 && QMinus1 == 0) {
            // Perform A = A - M (Subtract Multiplicand)
            bool borrow = 0;
            for (int i = bitWidth - 1; i >= 0; i--) {
                bool diff = A[i] ^ (!M[i]) ^ borrow;
                borrow = (!A[i] & (!M[i])) | (borrow & (!A[i] ^ (!M[i])));
                A[i] = diff;
            }
        } else if (Q.back() == 0 && QMinus1 == 1) {
            // Perform A = A + M (Add Multiplicand)
            bool carry = 0;
            for (int i = bitWidth - 1; i >= 0; i--) {
                bool sum = A[i] ^ M[i] ^ carry;
                carry = (A[i] & M[i]) | (carry & (A[i] ^ M[i]));
                A[i] = sum;
            }
        }

        // Step 2.2: Arithmetic Right Shift (ARS)
        QMinus1 = Q.back(); // Update Q(-1) with Q0
        Q.pop_back();
        Q.insert(Q.begin(), A.back()); // Shift Q with A's LSB
        A.insert(A.begin(), A[0]); // Sign-extend A
        A.pop_back();

        // Print current state
        cout << "A: "; printBinary(A); cout << "\n";
        cout << "Q: "; printBinary(Q); cout << "\n";
        cout << "Q-1: " << QMinus1 << "\n";
        cout << "--------------------\n";

        count--;
    }

    // Step 3: Combine A and Q as the final product
    vector<bool> product;
    product.insert(product.end(), A.begin(), A.end());
    product.insert(product.end(), Q.begin(), Q.end());
    return product;
}

//// Main Function ////
int main() {
    int multiplicand, multiplier;

    // Take inputs from the user
    cout << "Enter the multiplicand (signed integer): ";
    cin >> multiplicand;
    cout << "Enter the multiplier (signed integer): ";
    cin >> multiplier;

    // Determine the bit width (based on the magnitude of inputs)
    int bitWidth = max(8, max(to_string(abs(multiplicand)).length() * 4, to_string(abs(multiplier)).length() * 4));

    // Perform Booth's Algorithm for signed multiplication
    vector<bool> product = boothsAlgorithm(multiplicand, multiplier, bitWidth);

    // Display Results
    cout << "\nMultiplicand: " << multiplicand << " (Binary: ";
    printBinary(integerToBinary(multiplicand, bitWidth));
    cout << ")\n";

    cout << "Multiplier: " << multiplier << " (Binary: ";
    printBinary(integerToBinary(multiplier, bitWidth));
    cout << ")\n";

    cout << "Product (Binary): ";
    printBinary(product);
    cout << "\nProduct (Decimal): " << binaryToInteger(product) << "\n";

    return 0;
}
