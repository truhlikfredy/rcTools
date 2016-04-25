#include <string>
#include <iostream>
#include <cstdarg>


//template <typename... Rest>
//void Output(Rest... rest) {
//    va_list arguments;
//    unsigned int arg;
//
//    va_start(arguments, rest);
//    while (rest != NULL) {
//        std::cout << arg << std::endl;
//        arg = va_arg(arguments, unsigned int *);
//    }
//    va_end(rest);
//}


void Output(int num,...) {
    va_list arguments;
    int arg;

    std::cout << "Arguments: " << num << std::endl;

    va_start(arguments, num);
    for (unsigned char i = 0; i < num; i++) {
        arg = va_arg(arguments, int);
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    va_end(arguments);
}

int main() {
    Output(3, 2,4,6);
    Output(5, 3,1,4,2,6);
    return 0;
}