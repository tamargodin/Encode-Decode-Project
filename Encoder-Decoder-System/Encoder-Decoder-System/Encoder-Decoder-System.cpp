// Encoder-Decoder-System.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Logger.h";
#include "Field.h";
#include "Polynomial.h"
#include "Encoder.h";
#include "Generator_polynomial.h"
#include "Decoder.h"
int main()
{
    wd_codec::Logger::init("logfile.txt");
    /* Finite Field Parameters */
    const std::size_t field_descriptor = 8;
    const std::size_t generator_polynomial_index = 120;
    const std::size_t generator_polynomial_root_count = 32;

    /* Reed Solomon Code Parameters */
    const std::size_t code_length = 255;
    const std::size_t fec_length = 32;
    const std::size_t data_length = code_length - fec_length;

    /* Instantiate Finite Field and Generator Polynomials */
    const wd_codec::galois::Field field(field_descriptor,
               wd_codec::galois::primitive_polynomial_size06,
               wd_codec::galois::primitive_polynomial06);
    wd_codec::galois::Polynomial generator_polynomial(field);

    //Generate G(X)
    wd_codec::create_root_generator_polynomial(field,
        generator_polynomial_index,
        generator_polynomial_root_count,
        generator_polynomial);
    wd_codec::Logger::log(wd_codec::INFO, " G(x)= ", generator_polynomial);
    //std::cout <<"generator_polynomial: "<< generator_polynomial;

    typedef wd_codec::reed_solomon::Encoder<code_length, fec_length, data_length> encoder_t;
    typedef wd_codec::reed_solomon::Decoder<code_length, fec_length, data_length> decoder_t;

    const encoder_t encoder(field, generator_polynomial);
    const decoder_t decoder(field, generator_polynomial_index);


    std::string message = "An expert is someone who knows more and more about less and "
        "less until they know absolutely everything about nothing";

    message.resize(code_length, 0x00);

    std::cout << "Original Message:  [" << message << "]" << std::endl;
    wd_codec::reed_solomon::Block<code_length, fec_length> block;

    //Encoding the message
    if (encoder.encode(block, message)) {
        std::cout << "\nEncode word: [";
        for (std::size_t i = 0; i < code_length; ++i)
        {
            std::cout << static_cast<char>(block[i]);
        }
        std::cout << "]\n";
    }

    else {
        //TODO: handle error.
        std::cout << "not good";
    }

    block[4] = '5';
    block[3] = '5';

    std::cout << "\nCorrupt word: [";
    for (std::size_t i = 0; i < code_length; ++i)
    {
        std::cout << static_cast<char>(block[i]);
    }
    std::cout << "]\n";
    if (!decoder.decode(block))
    {
        std::cout << "Error - Critical decoding failure! ";
        return 1;
    }

    std::cout << "\nDecode word: [";
    for (std::size_t i = 0; i < data_length; ++i)
    {
        std::cout << static_cast<char>(block[i]);
    }

    std::cout << "]\n"; 


    wd_codec::Logger::close();
}

