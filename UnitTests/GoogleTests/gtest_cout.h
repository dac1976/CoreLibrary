#ifndef GTEST_COUT_H
#define GTEST_COUT_H

#include <sstream>
#include <iostream>

class CustomGTestCout : public std::stringstream
{
public:
    ~CustomGTestCout()
    {
        std::cout << "\u001b[32m[   INFO   ] \u001b[33m" << str() << "\u001b[0m" << std::flush;
    }
};

#define GOUT(MESSAGE)                                                                              \
    do                                                                                             \
    {                                                                                              \
        CustomGTestCout() << MESSAGE << std::endl;                                                 \
    } while (false)

#endif /* GTEST_COUT_H */
