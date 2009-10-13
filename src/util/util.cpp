/*
 * @author fcanas
 * @date 2009-10-12
 */

#include "util.h"

#include <cstring>
#include <sstream>

inline
unsigned char find_char (const char* haystack, size_t len, char needle)
{
    for (size_t i=0; i<len; ++i) {
        if (haystack[i] == needle) { return i; }
    }
    return -1;
}

std::string util::ip_addr_to_string (unsigned int addr)
{
    std::stringstream ss;
    unsigned char* c = (unsigned char*) &addr;
    for (size_t i=0; i<4; ++i) {
        ss << (c[3-i] & 0xffffffff);
        if (i < 3) { ss << "."; }
    }
    return ss.str();
}

size_t util::enc_base64 (const char* src, size_t src_offset, size_t src_len,
        char* dst, size_t dst_offset, size_t dst_len)
{
    /* El esquema es el sgte,
     * +---------+---------+---------+
     * |    M    |    a    |    n    |
     * +---------+---------+---------+
     * |    77   |    97   |   110   |
     * +---------+---------+---------+
     * |010011 01|0110 0001|01 101110|
     * +------+--+----+----+--+------+
     * |  19  |   22  |   5   |  46  |
     * +------+--+----+----+--+------+
     * |   T  |    W  |    F  |   u  |
     * +------+--+----+----+--+------+
     */
    char abc[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t dst_writ = 0;
    size_t src_pad;
    size_t i;

    if (dst_len == 0) { dst_len = -1; }

    /* Relleno necesario */
    src_pad = (src_len%3 == 0) ? 0 : 3 - src_len%3;

    /* Se lee de a 3, se escribe de a 4 */
    src_len -= src_len%3;
    dst_len -= dst_len%4;

    /* Si hay relleno, hay que guardar espacio en dst */
    if (src_pad != 0) { dst_len -= 4; }

    /* Aplicar offset */
    src = (src+src_offset);
    dst = (dst+dst_offset);

    for (i=0; i<src_len && dst_writ<dst_len; i+=3) {
        dst[dst_writ++] = abc[(src[i] >> 2) & 0x3f];
        dst[dst_writ++] = abc[
            ((src[i] << 4 ) & 0x30) |
            ((src[i+1] >> 4) & 0x0f)
            ];
        dst[dst_writ++] = abc[
            ((src[i+1] << 2) & 0x3c) |
            ((src[i+2] >> 6) & 0x03)
            ];
        dst[dst_writ++] = abc[src[i+2] & 0x3f];
    }

    switch (src_pad) {
        case 1:
            dst[dst_writ++] = abc[(src[i] >> 2) & 0x3f];
            dst[dst_writ++] = abc[
                ((src[i] << 4 ) & 0x30) |
                ((src[i+1] >> 4) & 0x0f)
                ];
            dst[dst_writ++] = abc[((src[i+1] << 2) & 0x3c)];
            dst[dst_writ++] = '=';
            break;
        case 2:
            dst[dst_writ++] = abc[(src[i] >> 2) & 0x3f];
            dst[dst_writ++] = abc[((src[i] << 4 ) & 0x30)];
            dst[dst_writ++] = '=';
            dst[dst_writ++] = '=';
            break;
        default:
            break;
    }
    return dst_writ;
}

size_t util::dec_base64 (const char* src, size_t src_offset, size_t src_len,
        char* dst, size_t dst_offset, size_t dst_len)
{
    char abc[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t dst_writ = 0;
    size_t i;
    unsigned char c[4];

    if (dst_len == 0) { dst_len = -1; }

    /* Se lee de a 4 */
    src_len -= src_len%4;

    /* Aplicar offset */
    src = (src+src_offset);
    dst = (dst+dst_offset);

    for (i=0; i<src_len && dst_writ<dst_len; i+=4) {
        c[0] = find_char(abc, 64, src[i]);
        c[1] = find_char(abc, 64, src[i+1]);
        c[2] = (src[i+2] == '=') ? 64 : find_char(abc, 64, src[i+2]);
        c[3] = (src[i+3] == '=') ? 64 : find_char(abc, 64, src[i+3]);
        dst[dst_writ++] =
            ((c[0] << 2) & 0xfc) |
            ((c[1] >> 4) & 0x03);
        if (c[2] == 64) { break; }
        dst[dst_writ++] =
            ((c[1] << 4) & 0xf0) |
            ((c[2] >> 2) & 0x0f);
        if (c[3] == 64) { break; }
        dst[dst_writ++] =
            ((c[2] << 6) & 0xc0) |
            (c[3] & 0x3f);
    }
    return dst_writ;
}

