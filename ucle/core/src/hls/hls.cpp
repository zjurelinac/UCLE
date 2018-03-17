#include <simulators/hls.h>

#include <cstdio>
#include <fstream>

#include <util/binary.h>


template <typename State, typename Config>
void ucle::hls::high_level_simulator::_pfile(std::string filename, address_t start_address) {
    std::ifstream pfile(filename);
    address_t address;

    unsigned word_t[4] = {0};

    std::string line;
    while (!pfile.eof()) {
        std::getline(pfile, line);

        if (sscanf(line.c_str(), "%8zX %2X %2X %2X %2X", &address, &word[0], &word[1], &word[2], &word[3]) == 5) {
            printf("%08zX %08X\n", address, word_from_bytes__le(word));
            adr_sp_.set<word>(address + start_address, word_from_bytes__le(word));
        }
    }

    // set state
}
