#include <iostream>
#include <QPdfWriter>

int main(int argc, char** argv)
{
    ::std::cout << "Hi EPUB" << ::std::endl;
    auto pdf = QPdfWriter("qrc:/test.epub");
    return 0;
}