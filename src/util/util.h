/*
 * @author fcanas
 * @date 2009-10-12
 */

#include <string>

namespace util
{
    /** Retorna un string de la forma "X.X.X.X" a partir de la representación
     * binaria.
     *
     * @param   addr    la dirección, en host byte order
     * @return  la dirección representada en un string de la forma "X.X.X.X"
     */
    std::string ip_addr_to_string (unsigned int addr);

    size_t enc_base64 (const char* src, size_t src_offset, size_t src_len,
            char* dst, size_t dst_offset, size_t dst_len=0); 

    size_t dec_base64 (const char* src, size_t src_offset, size_t src_len,
            char* dst, size_t dst_offset, size_t dst_len=0); 
}

