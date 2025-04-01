#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

//// Helper Functions ////

// Convert an integer to binary (LSB first)
vector<bool> integerToBinary(int number) {
    vector<bool> binary;
    while (number > 0) {
        binary.push_back(number % 2); // Extract least significant bit
        number /= 2;
    }
    reverse(binary.begin(), binary.end()); // Reverse to get MSB first
    return binary;
}

// Convert binary (LSB first) to integer
int binaryToInteger(const vector<bool>& binary) {
    int result = 0;
    for (size_t i = 0; i < binary.size(); i++) {
        result = (result << 1) + binary[i]; // Shift left and add the bit
    }
    return result;
}

//// Half Adder ////
void halfAdder(bool A, bool B, bool &Sum, bool &Carry) {
    Sum = A ^ B;     // XOR for Sum
    Carry = A & B;   // AND for Carry
}

//// Full Adder ////
void fullAdder(bool A, bool B, bool Cin, bool &Sum, bool &Carry) {
    bool intermediateSum, intermediateCarry1, intermediateCarry2;
    halfAdder(A, B, intermediateSum, intermediateCarry1);
    halfAdder(intermediateSum, Cin, Sum, intermediateCarry2);
    Carry = intermediateCarry1 | intermediateCarry2; // OR for final Carry
}

//// Serial Adder ////
void serialAdder(const vector<bool>& A, const vector<bool>& B, vector<bool>& Sum, bool& Carry) {
    Carry = 0; // Initialize Carry
    size_t size = max(A.size(), B.size());
    vector<bool> paddedA = A, paddedB = B;
    paddedA.resize(size, 0);
    paddedB.resize(size, 0);
    Sum.resize(size);
    for (size_t i = size; i-- > 0;) {
        fullAdder(paddedA[i], paddedB[i], Carry, Sum[i], Carry);
    }
}

//// Parallel Adder ////
vector<bool> parallelAdder(const vector<bool>& A, const vector<bool>& B) {
    size_t size = max(A.size(), B.size());
    vector<bool> paddedA = A, paddedB = B;
    paddedA.resize(size, 0);
    paddedB.resize(size, 0);
    vector<bool> Sum(size);
    bool Carry = 0;

    for (size_t i = size; i-- > 0;) {
        fullAdder(paddedA[i], paddedB[i], Carry, Sum[i], Carry);
    }
    if (Carry) Sum.insert(Sum.begin(), Carry); // Add final carry if needed
    return Sum;
}

//// Parallel Subtractor ////
vector<bool> parallelSubtractor(const vector<bool>& A, const vector<bool>& B) {
    size_t size = max(A.size(), B.size());
    vector<bool> paddedA = A, paddedB = B;
    paddedA.resize(size, 0);
    paddedB.resize(size, 0);
    vector<bool> Difference(size);
    bool Borrow = 0;

    for (size_t i = size; i-- > 0;) {
        fullAdder(paddedA[i], !paddedB[i], Borrow, Difference[i], Borrow); // Subtraction as A - B = A + NOT(B)
    }
    return Difference;
}

//// Addition/Subtraction Logic Unit ////
void additionSubtractionALU(const vector<bool>& A, const vector<bool>& B, bool mode, vector<bool>& Result) {
    if (mode) {
        Result = parallelAdder(A, B); // Addition
    } else {
        Result = parallelSubtractor(A, B); // Subtraction
    }
}

//// Fast Adder ////
vector<bool> fastAdder(const vector<bool>& A, const vector<bool>& B) {
    return parallelAdder(A, B); // Built on Parallel Adder logic
}

//// Carry Lookahead Adder ////
vector<bool> carryLookaheadAdder(const vector<bool>& A, const vector<bool>& B) {
    size_t size = max(A.size(), B.size());
    vector<bool> paddedA = A, paddedB = B;
    paddedA.resize(size, 0);
    paddedB.resize(size, 0);

    vector<bool> Sum(size);
    vector<bool> Generate(size), Propagate(size), Carry(size + 1);

    // Step 1: Compute Generate and Propagate
    for (size_t i = 0; i < size; i++) {
        Generate[i] = paddedA[i] & paddedB[i];   // G = A AND B
        Propagate[i] = paddedA[i] ^ paddedB[i]; // P = A XOR B
    }

    // Step 2: Compute Carry
    Carry[0] = 0; // Initial carry
    for (size_t i = 0; i < size; i++) {
        Carry[i + 1] = Generate[i] | (Propagate[i] & Carry[i]); // C[i+1] = G[i] + P[i]C[i]
    }

    // Step 3: Compute Sum
    for (size_t i = 0; i < size; i++) {
        Sum[i] = Propagate[i] ^ Carry[i]; // S[i] = P[i] XOR C[i]
    }

    if (Carry[size]) Sum.insert(Sum.begin(), Carry[size]); // Append the final carry
    return Sum;
}

//// Main Function ////
int main() {
    int input1, input2;
    bool mode;

    // Take inputs from the user
    cout << "Enter the first integer: ";
    cin >> input1;
    cout << "Enter the second integer: ";
    cin >> input2;
    cout << "Enter mode (1 for Addition, 0 for Subtraction): ";
    cin >> mode;

    // Convert integers to binary
    vector<bool> binary1 = integerToBinary(input1);
    vector<bool> binary2 = integerToBinary(input2);

    cout << "\nBinary Representation of " << input1 << ": ";
    for (bool bit : binary1) cout << bit;
    cout << "\nBinary Representation of " << input2 << ": ";
    for (bool bit : binary2) cout << bit;

    vector<bool> result;

    // Perform addition or subtraction using the chosen ALU mode
    additionSubtractionALU(binary1, binary2, mode, result);

    // Display results
    if (mode) {
        cout << "\nAddition Result (Binary): ";
    } else {
        cout << "\nSubtraction Result (Binary): ";
    }
    for (bool bit : result) cout << bit;

    cout << "\nResult (Decimal): " << binaryToInteger(result) << "\n";

    // Fast Adder Example
    result = fastAdder(binary1, binary2);
    cout << "\nFast Adder Result (Binary): ";
    for (bool bit : result) cout << bit;

    cout << "\nResult (Decimal): " << binaryToInteger(result) << "\n";

    // Carry Lookahead Adder Example
    result = carryLookaheadAdder(binary1, binary2);
    cout << "\nCarry Lookahead Adder Result (Binary): ";
    for (bool bit : result) cout << bit;

    cout << "\nResult (Decimal): " << binaryToInteger(result) << "\n";

    return 0;
}
