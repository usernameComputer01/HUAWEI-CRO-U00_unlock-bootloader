#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define OFF_TO_CROU00_ENCKEY 0x200
// #define OFF_TO_CROU00_VERIFY_STATUS 0x3F4

void
get_key(const unsigned char *in, unsigned char *out, bool is_dec)
{
    // From lk

    assert(in != NULL);
    assert(out != NULL);

#define op(dec, byte, first, second)                                                     \
    (dec ? (byte >> first) | (byte << second) : (byte << first) | (byte >> second))

    out[0] = in[0];
    out[1] = op(is_dec, in[1], 7, 1);
    out[2] = op(is_dec, in[2], 6, 2);
    out[3] = op(is_dec, in[3], 5, 3);
    out[4] = op(is_dec, in[4], 4, 4);
    out[5] = op(!is_dec, in[5], 5, 3);
    out[6] = op(!is_dec, in[6], 6, 2);
    out[7] = op(!is_dec, in[7], 7, 1);

    out[8]  = in[8];
    out[9]  = op(is_dec, in[9], 7, 1);
    out[10] = op(is_dec, in[10], 6, 2);
    out[11] = op(is_dec, in[11], 5, 3);
    out[12] = op(is_dec, in[12], 4, 4);
    out[13] = op(!is_dec, in[13], 5, 3);
    out[14] = op(!is_dec, in[14], 6, 2);

    out[15] = op(!is_dec, in[15], 7, 1);

    /* Only Decode
    out[0] = in[0];
    out[1] = (in[1] >> 7) | (in[1] << 1);
    out[2] = (in[2] >> 6) | (in[2] << 2);
    out[3] = (in[3] >> 5) | (in[3] << 3);
    out[4] = (in[4] >> 4) | (in[4] << 4);
    out[5] = (in[5] << 5) | (in[5] >> 3);
    out[6] = (in[6] << 6) | (in[6] >> 2);
    out[7] = (in[7] << 7) | (in[7] >> 1);

    out[8]  = in[8];
    out[9]  = (in[9] >> 7) | (in[9] << 1);
    out[10] = (in[10] >> 6) | (in[10] << 2);
    out[11] = (in[11] >> 5) | (in[11] << 3);
    out[12] = (in[12] >> 4) | (in[12] << 4);
    out[13] = (in[13] << 5) | (in[13] >> 3);
    out[14] = (in[14] << 6) | (in[14] >> 2);
    out[15] = (in[15] << 7) | (in[15] >> 1);
    */
}

void
usage_print(const char *arg)
{
    printf("[*] Usage: %s"
           " <u:0123456789AABBCCDDEEFF0A1B2C3D4E | l:0123456789ABCDEF | "
           "f:path/to/proinfo.bin>"
           " [ DEBUG (0|1) ]\n",
           arg);
}

int
main(int argc, char const *argv[])
{
    const int key_sz = 16;
    const char *scanarg;

    bool is_dec = true;
    unsigned char buf_to[key_sz];
    unsigned char buf_from[key_sz];

    if (argc < 2) {
        usage_print(argv[0]);
        return 1;
    }

    scanarg = argv[1];

    if (strlen(scanarg) < 2) {
        printf("[-] Wrong length arg\n");
        return 1;
    }

    switch (*scanarg) {
        case 'u':
            /* scan hex string to buf */
            scanarg += 2;
            if (strlen(scanarg) != key_sz * 2) {
                printf("[-] Wrong length enc key (key size not equal 32)\n");
                return 1;
            }

            for (int i = 0; i < key_sz * 2; i += 2) {
                char hex_byte[3] = {
                    scanarg[i + 0],
                    scanarg[i + 1],
                    0x00,
                };
                sscanf(hex_byte, "%hhx", &buf_from[i ? i / 2 : 0]);
            }

            is_dec = true;
            break;

        case 'l':
            /* scan hex string to buf */
            scanarg += 2;
            if (strlen(scanarg) != key_sz) {
                printf("[-] Wrong length unlock key (key size not equal 16)\n");
                return 1;
            }

            for (int i = 0; i < key_sz; ++i) {
                /* or checking the result buf for the presence of bit
                 * ((byte & 0xDF) == 0)
                 */
                if (!isxdigit(scanarg[i])) {
                    printf("[-] Wrong input key (need [a-fA-F0-9]) - (byte = %c)\n",
                           scanarg[i]);
                    return 1;
                }
            }

            memcpy(buf_from, scanarg, key_sz);

            is_dec = false;
            break;

        case 'f':
            /* read proinfo.bin */
            scanarg += 2;
            FILE *fp = fopen(scanarg, "rb");
            if (!fp) {
                printf("[-] Can't open %s - %s\n", scanarg, strerror(errno));
                return 1;
            }
            fseek(fp, OFF_TO_CROU00_ENCKEY, SEEK_SET);
            assert(fread(buf_from, 1, key_sz, fp) == key_sz);
            fclose(fp);
            break;

        default:
            usage_print(argv[0]);
            return 1;
    }

    get_key(buf_from, buf_to, is_dec);

    if (is_dec) {

        /* Print string unlock key */
        printf("[+] Key unlock: (");
        for (int i = 0; i < key_sz; ++i) {

            unsigned char c = buf_to[i];

            if (isalnum(c)) {
                printf("%c", c);
            } else {
                printf("<%#02x>", c);
            }
        }
        printf(")\n");

    } else {

        /* Print string unlock key (hex) */
        printf("[+] Key unlock (hex): (");
        for (int i = 0; i < key_sz; ++i) {
            printf("%02x", buf_to[i]);
        }
        printf(")\n");
    }

    /* Print debug */
    if (argc > 2 && argv[2][0] == '1') {
        printf("[*] (Debug) HEX: (");
        for (int i = 0; i < key_sz; ++i) {
            printf("%02X", buf_to[i]);
            if (i + 1 != key_sz) {
                printf(" ");
            }
        }
        printf(")\n");
    }

    return 0;
}
