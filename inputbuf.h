//
// Created by Chase Overcash on 4/8/2019.
//

#ifndef CSE340_PROJ4_CHASEOVERCASH_INPUTBUF_H
#define CSE340_PROJ4_CHASEOVERCASH_INPUTBUF_H

#include <string>

class InputBuffer {
public:
    void GetChar(char&);
    char UngetChar(char);
    std::string UngetString(std::string);
    bool EndOfInput();

private:
    std::vector<char> input_buffer;
};

#endif //CSE340_PROJ4_CHASEOVERCASH_INPUTBUF_H
