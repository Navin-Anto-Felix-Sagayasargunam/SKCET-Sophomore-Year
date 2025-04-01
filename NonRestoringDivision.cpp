#include <iostream>
#include <vector>
#include <cmath>
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

//// Non-Restoring Division Algorithm ////
pair<vector<bool>, vector<bool>> nonRestoringDivision(int dividend, int divisor, int bitWidth) {
    // Step 1: Initialize Registers
    vector<bool> A(bitWidth, 0); // Accumulator (A)
    vector<bool> Q = integerToBinary(dividend, bitWidth); // Dividend (Q)
    vector<bool> M = integerToBinary(divisor, bitWidth); // Divisor (M)
    int count = bitWidth; // Number of iterations

    cout << "Initial Values:\n";
    cout << "A: "; printBinary(A); cout << "\n";
    cout << "Q: "; printBinary(Q); cout << "\n";
    cout << "M: "; printBinary(M); cout << "\n";
    cout << "--------------------\n";

    // Step 2: Non-Restoring Division Iterations
    while (count > 0) {
        // Step 2.1: Left Shift A and Q
        A.insert(A.begin(), Q.front()); // Shift Q's MSB into A's LSB
        Q.erase(Q.begin());
        Q.push_back(0); // Append 0 to Q

        // Print current state after shift
        cout << "After Left Shift:\n";
        cout << "A: "; printBinary(A); cout << "\n";
        cout << "Q: "; printBinary(Q); cout << "\n";

        // Step 2.2: Subtract or Add Divisor
        if (A[0] == 0) {
            // A is non-negative: Subtract M from A
            vector<bool> tempA = A; // Temporary accumulator for addition
            bool borrow = 0;
            for (int i = bitWidth - 1; i >= 0; i--) {
                bool diff = A[i] ^ (!M[i]) ^ borrow;
                borrow = (!A[i] & (!M[i])) | (borrow & (!A[i] ^ (!M[i])));
                A[i] = diff;
            }
            cout << "After Subtraction (A - M):\n";
        } else {
            // A is negative: Add M to A
            bool carry = 0;
            for (int i = bitWidth - 1; i >= 0; i--) {
                bool sum = A[i] ^ M[i] ^ carry;
                carry = (A[i] & M[i]) | (carry & (A[i] ^ M[i]));
                A[i] = sum;
            }
            cout << "After Addition (A + M):\n";
        }

        cout << "A: "; printBinary(A); cout << "\n";

        // Step 2.3: Update Q
        Q.back() = (A[0] == 0) ? 1 : 0; // Append 1 if A is non-negative, otherwise append 0

        // Print current state
        cout << "Updated State:\n";
        cout << "A: "; printBinary(A); cout << "\n";
        cout << "Q: "; printBinary(Q); cout << "\n";
        cout << "--------------------\n";

        count--;
    }

    return {A, Q}; // Return remainder (A) and quotient (Q)
}

//// Main Function ////
int main() {
    int dividend, divisor;

    // Take inputs from the user
    cout << "Enter the dividend (signed integer): ";
    cin >> dividend;
    cout << "Enter the divisor (signed integer): ";
    cin >> divisor;

    // Determine the bit width (based on magnitude of inputs)
    int bitWidth = max(8, max(to_string(abs(dividend)).length() * 4, to_string(abs(divisor)).length() * 4));

    // Perform Non-Restoring Division
    auto [remainder, quotient] = nonRestoringDivision(dividend, divisor, bitWidth);

    // Display Results
    cout << "\nDividend: " << dividend << " (Binary: ";
    printBinary(integerToBinary(dividend, bitWidth));
    cout << ")\n";

    cout << "Divisor: " << divisor << " (Binary: ";
    printBinary(integerToBinary(divisor, bitWidth));
    cout << ")\n";

    cout << "Quotient (Binary): ";
    printBinary(quotient);
    cout << "\nQuotient (Decimal): " << binaryToInteger(quotient) << "\n";

    cout << "Remainder (Binary): ";
    printBinary(remainder);
    cout << "\nRemainder (Decimal): " << binaryToInteger(remainder) << "\n";

    return 0;
}
