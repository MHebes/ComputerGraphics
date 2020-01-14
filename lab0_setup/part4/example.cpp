#include <iostream>
#include <string>

int main()
{
    int myArray[10];

    for (int i = 0; i < 10; ++i) {
        myArray[i] = i;

        std::cout << i << " ";
    }

    std::string s = "hello";

    for (char c : s) {
        std::cout << c;
    }
}
