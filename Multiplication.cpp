#include <iostream>
#include <vector>
#include <algorithm> // For reverse function

using namespace std;

//// Helper Functions ////

// Convert an integer to binary
vector<bool> integerToBinary(int number) {
    vector<bool> binary;
    bool isNegative = (number < 0); // Check if the number is negative
    number = abs(number); // Work with the absolute value

    while (number > 0) {
        binary.push_back(number % 2); // Extract least significant bit
        number /= 2;
    }
    reverse(binary.begin(), binary.end()); // Reverse to get MSB first

    if (isNegative) {
        // Two's complement for negative numbers
        for (size_t i = 0; i < binary.size(); i++) {
            binary[i] = !binary[i]; // Flip bits
        }

        // Add 1 to complete two's complement
        bool carry = 1;
        for (size_t i = binary.size(); i-- > 0;) {
            binary[i] = binary[i] ^ carry;
            carry = binary[i] & carry;
        }

        if (carry) binary.insert(binary.begin(), 1); // Overflow bit for carry
    }
    return binary;
}

// Convert binary to integer
int binaryToInteger(const vector<bool>& binary) {
    bool isNegative = binary[0]; // Check the sign bit
    vector<bool> absBinary = binary;

    if (isNegative) {
        // Two's complement conversion for negative numbers
        for (size_t i = 0; i < absBinary.size(); i++) {
            absBinary[i] = !absBinary[i]; // Flip bits
        }

        // Add 1 to complete conversion
        bool carry = 1;
        for (size_t i = absBinary.size(); i-- > 0;) {
            absBinary[i] = absBinary[i] ^ carry;
            carry = absBinary[i] & carry;
        }
    }

    // Calculate the integer value
    int result = 0;
    for (size_t i = 0; i < absBinary.size(); i++) {
        result = (result << 1) + absBinary[i];
    }

    return isNegative ? -result : result;
}

//// Multiplication Function ////
vector<bool> signedMultiplication(int multiplicand, int multiplier) {
    bool isNegativeResult = (multiplicand < 0) ^ (multiplier < 0); // XOR to determine result sign
    vector<bool> binaryMultiplicand = integerToBinary(abs(multiplicand));
    vector<bool> binaryMultiplier = integerToBinary(abs(multiplier));
    size_t size = binaryMultiplicand.size() + binaryMultiplier.size();
    vector<bool> product(size, 0); // Initialize product with zeros

    // Binary multiplication (shift-and-add method)
    for (size_t i = binaryMultiplier.size(); i-- > 0;) {
        if (binaryMultiplier[i]) {
            vector<bool> shiftedMultiplicand = binaryMultiplicand; // Copy multiplicand
            shiftedMultiplicand.resize(size, 0); // Pad with zeros for alignment
            for (size_t j = shiftedMultiplicand.size(); j-- > 0;) {
                product[j] = product[j] ^ shiftedMultiplicand[j];
                bool carry = product[j] & shiftedMultiplicand[j];
                if (carry) product.insert(product.begin(), 1); // Overflow bit for carry
            }
        }
        binaryMultiplicand.insert(binaryMultiplicand.end(), 0); // Left shift multiplicand
    }

    if (isNegativeResult) {
        // Convert result to two's complement for negative numbers
        for (size_t i = 0; i < product.size(); i++) {
            product[i] = !product[i]; // Flip bits
        }

        // Add 1 to complete two's complement
        bool carry = 1;
        for (size_t i = product.size(); i-- > 0;) {
            product[i] = product[i] ^ carry;
            carry = product[i] & carry;
        }

        if (carry) product.insert(product.begin(), 1); // Overflow bit for carry
    }

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

    // Perform signed multiplication
    vector<bool> product = signedMultiplication(multiplicand, multiplier);

    // Convert inputs to binary
    vector<bool> binaryMultiplicand = integerToBinary(multiplicand);
    vector<bool> binaryMultiplier = integerToBinary(multiplier);

    // Display results
    cout << "\nBinary Representation of Multiplicand (" << multiplicand << "): ";
    for (bool bit : binaryMultiplicand) cout << bit;
    cout << "\nBinary Representation of Multiplier (" << multiplier << "): ";
    for (bool bit : binaryMultiplier) cout << bit;

    cout << "\nProduct (Binary): ";
    for (bool bit : product) cout << bit;

    cout << "\nProduct (Decimal): " << binaryToInteger(product) << "\n";

    return 0;
}
