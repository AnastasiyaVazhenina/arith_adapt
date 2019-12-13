#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <exception>
#include <string>

#define BITS_IN_REGISTER 16
#define TOP_VALUE (((long) 1 << BITS_IN_REGISTER) - 1)
#define FIRST_QTR (TOP_VALUE / 4 + 1)
#define HALF (2 * FIRST_QTR)
#define NO_OF_CHARS 256
#define EOF_SYMBOL (NO_OF_CHARS + 1)
#define NO_OF_SYMBOLS (NO_OF_CHARS + 1)
#define MAX_FREQUENCY 16383

class Arith_adapt {

    unsigned char index_to_char[NO_OF_SYMBOLS];

    int char_to_index[NO_OF_CHARS];

    int cum_freq[NO_OF_SYMBOLS + 1];

    int freq[NO_OF_SYMBOLS + 1] = {0,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 124, 1, 1, 1, 1, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1236, 1, 21, 9, 3, 1, 25, 15, 2, 2, 2, 1, 79, 19, 60, 1,
                                   15, 15, 8, 5, 4, 7, 5, 4, 4, 6, 3, 2, 1, 1, 1, 1,
                                   1, 24, 15, 22, 12, 15, 10, 9, 16, 16, 8, 6, 12, 23, 13, 11,
                                   14, 1, 14, 28, 29, 6, 3, 11, 1, 3, 1, 1, 1, 1, 1, 3,
                                   1, 491, 85, 173, 232, 744, 127, 110, 293, 418, 6, 39, 250, 139, 429, 446,
                                   111, 5, 388, 375, 531, 152, 57, 97, 12, 101, 5, 2, 1, 2, 3, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   1};

    long low;
    long high;
    long value;

    long bits_to_follow;
    int buffer;
    int bits_to_go;
    int garbage_bits;

    FILE *in{};
    FILE *out{};;

    void start_model() {
        for (int i = 0; i < NO_OF_CHARS; ++i) {
            char_to_index[i] = i + 1;
            index_to_char[i + 1] = i;
        }
        cum_freq[NO_OF_SYMBOLS] = 0;
        for (int i = NO_OF_SYMBOLS; i > 0; --i) {
            cum_freq[i - 1] = cum_freq[i] + freq[i];
        }
        if (cum_freq[0] > MAX_FREQUENCY)
            std::exit(1);
    }

    void start_inputing_bits() {
        bits_to_go = 0;
        garbage_bits = 0;
    }

    int input_bit() {
        if (bits_to_go == 0) {
            buffer = std::getc(in);
            if (buffer == EOF) {
                garbage_bits += 1;
                if (garbage_bits > (BITS_IN_REGISTER - 2)) {
                    throw std::runtime_error("Error in file");
                }
            }
            bits_to_go = 8;
        }
        int t = buffer & 1;
        buffer >>= 1;
        bits_to_go -= 1;
        return t;
    }

    void start_outputing_bits() {
        buffer = 0;
        bits_to_go = 8;
    }


    void output_bit(int bit) {
        buffer >>= 1;
        if (bit)
            buffer |= 0x80;
        bits_to_go -= 1;
        if (bits_to_go == 0) {
            std::putc(buffer, out);
            bits_to_go = 8;
        }
    }

    void done_outputing_bits() {
        std::putc(buffer >> bits_to_go, out);
    }

    void output_bit_plus_follow(int bit) {
        output_bit(bit);
        while (bits_to_follow > 0) {
            bit = !bit;
            output_bit(bit);
            --bits_to_follow;
        }
    }

    void start_encoding() {
        low = 0;
        high = TOP_VALUE;
        bits_to_follow = 0;
    }

    void done_encoding() {
        ++bits_to_follow;
        if (low < FIRST_QTR)
            output_bit_plus_follow(0);
        else
            output_bit_plus_follow(1);
    }

    void start_decoding() {
        for (int i = 0; i < BITS_IN_REGISTER; ++i) {
            value = 2 * value + input_bit();
        }
        low = 0;
        high = TOP_VALUE;
    }

    void encode_symbol(int symbol) {
        long range = (high - low) + 1;
        high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1;
        low = low + (range * cum_freq[symbol]) / cum_freq[0];
        for (;;) {
            if (high < HALF) {
                output_bit_plus_follow(0);
                low = 2 * low;
                high = 2 * high + 1;
            } else if (low >= HALF) {
                output_bit_plus_follow(1);
                low = 2 * (low - HALF);
                high = 2 * (high - HALF) + 1;
            } else
                break;
        }
    }

    int decode_symbol() {
        int symbol = 0;
        long range = (high - low) + 1;
        int cum = (((value - low) + 1) * cum_freq[0] - 1) / range;
        for (symbol = 1; cum_freq[symbol] > cum; ++symbol);
        high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1;
        low = low + (range * cum_freq[symbol]) / cum_freq[0];
        for (;;) {
            if (high < HALF) {
                value = 2 * value + input_bit();
                low = 2 * low;
                high = 2 * high + 1;
            } else if (low > HALF) {
                value = 2 * (value - HALF) + input_bit();
                low = 2 * (low - HALF);
                high = 2 * (high - HALF) + 1;
            } else
                break;
        }
        return symbol;
    }

public:
    void encode(const char *input, const char *output) {
        in = std::fopen(input, "r+b");
        out = std::fopen(output, "w+b");
        if (in == nullptr || out == nullptr)
            return;
        start_model();
        start_outputing_bits();
        start_encoding();
        for (;;) {
            int ch = std::getc(in);
            if (std::feof(in))
                break;
            int symbol = char_to_index[ch];
            encode_symbol(symbol);
        }
        int eof = EOF_SYMBOL;
        encode_symbol(eof);
        done_encoding();
        done_outputing_bits();
        std::fclose(in);
        std::fclose(out);
    }

    void decode(const char *input, const char *output) {
        in = std::fopen(input, "r+b");
        out = std::fopen(output, "w+b");
        if (in == nullptr || out == nullptr)
            return;
        start_model();
        start_inputing_bits();
        start_decoding();
        for (;;) {
            int symbol = decode_symbol();
            if (symbol == EOF_SYMBOL)
                break;
            int ch = index_to_char[symbol];
            std::putc(ch, out);
        }
        std::fclose(in);
        std::fclose(out);
    }

    Arith_adapt() : low(0), high(0), value(0), bits_to_follow(0), buffer(0), bits_to_go(0), garbage_bits(0) {}
};

int main(int argc, char* argv[]) {
    if (argc < 4)
    {
        std::cout << "\n Using: Arith_adapt infile outfile outfile_decode\n";
        return 0;
    }
    std::string input = argv[1];
    std::string output = argv[2];
    std::string output_decode = argv[3];
    Arith_adapt model;
    model.encode(input.c_str(), output.c_str());
    model.decode(output.c_str(), output_decode.c_str());
    return 0;
}
