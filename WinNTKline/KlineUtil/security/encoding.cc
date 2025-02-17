#include "encoding.h"

#ifdef _LIBICONV_H
////////////////////////////////////////////
//[RMK]: 国标码与UTF-8互转
//[IN]:  dest:   转码后格式,如“GBK”,"UTF-8";
//       pbSrc:  原码格式;
//       input:  原码字符串;
//       ilen:   原码字符串长度;
//[OUT]: output: 转码字符串;
//       olen:   转码字符串长度。
//[RET]:         不为-1成功
////////////////////////////////////////////
int conv_charset(const char* dest, const char* pbSrc, const char* input, size_t ilen, char* output, size_t olen)
{
    int convlen = (int)olen;
    iconv_t conv = iconv_open(dest, pbSrc);
    if (conv == (iconv_t)-1)
        return -1;
    memset(output, 0, olen);
    if (iconv(conv, &input, &ilen, &output, &olen))
        return -1;
    iconv_close(conv);
    return (int)(convlen - olen);
}
#endif // _LIBICONV_H

/**************** 二进制取反 ****************
输入:
    const unsigned char *src  二进制数据
    int length                待转换的二进制数据长度
输出:
    unsigned char *dst        取反后的二进制数据
返回:
    0 success
************************************************/
int convert(unsigned char* dst, const unsigned char* src, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        dst[i] = src[i] ^ 0xFF;
    }
    return 0;
}
/**************** 求权 ****************
输入:
    int base       进制基数
    int times      权级数
返回:
    unsigned long  当前数据位的权
********************************/
unsigned long power(int base, int times)
{
    int i;
    unsigned long rslt = 1;
    for (i = 0; i < times; i++)
        rslt *= base;
    return rslt;
}
/**************** ASCII码格式字符转换为16进制格式 ****************
输入:
    asc  源字符
    len  字符长度
输出:
    hex  16进制(HEX)格式字符
****************************************************************/
void asc_to_hex(unsigned char* hex, unsigned char* asc, int len)
{
    for (int i = 0; i < len; i++) {
        unsigned char ch;
        ch = asc[i];
        if (ch >= '0' && ch <= '9') {
            ch -= '0';
        }
        if (ch >= 'a' && ch <= 'f') {
            ch = ch - 'a';
            ch = ch + 10;
        }
        if (ch >= 'A' && ch <= 'F') {
            ch = ch - 'A';
            ch = ch + 10;
        }
        if (i % 2 == 0) {
            hex[i / 2] = ch << 4;
        } else {
            hex[i / 2] = hex[i / 2] | ch;
        }
    }
}
/**************** 16进制格式转换为ASCII码格式 ****************
输入:
    hex  源字符
    len  字符长度
输出:
    asc ASCII码格式字符
****************************************************************/
void hex_to_asc(unsigned char* asc, unsigned char* hex, int len)
{
    for (int i = 0; i < len; i++) {
        int ch = hex[i] >> 4;
        if (ch >= 0 && ch <= 9) {
            asc[i * 2] = ch + '0';
        }
        if (ch >= 0xA && ch <= 0xF) {
            asc[i * 2] = ch - 0xa + 'A';
        }
        ch = hex[i] & 0x0F;
        if (ch >= 0 && ch <= 9) {
            asc[i * 2 + 1] = ch + '0';
        }
        if (ch >= 0xA && ch <= 0xF) {
            asc[i * 2 + 1] = ch - 0xa + 'A';
        }
    }
}
/**************** ASCII码格式字符转换为EBCD格式(二进制编码的十进制交换码) ****************
输入:
    asc  源字符
    len  字符长度
输出:
    ebcd EBCD格式字符
********************************************************************************/
void asc_to_ebcd(unsigned char* ebcd, unsigned char* asc, int len)
{
    for (int i = 0; i < len; i++) {
        if ((asc[i] >= '0') && (asc[i] <= '9'))
            ebcd[i] = asc[i] - '0' + 0xF0;
        if ((asc[i] >= 'A') && (asc[i] <= 'F'))
            ebcd[i] = asc[i] - 'A' + 0xC1;
        if ((asc[i] >= 'a') && (asc[i] <= 'f'))
            ebcd[i] = asc[i] - 'a' + 0xC1;
    }
}
/**************** ebcd格式字符转换为ASCII码格式 ****************
输入:
    ebcd  源字符
    len   字符长度
输出:
    asc   ASCII格式字符
****************************************************************/
void ebcd_to_asc(unsigned char* asc, unsigned char* ebcd, int len)
{
    for (int i = 0; i < len; i++) {
        if ((ebcd[i] >= 0xF0) && (ebcd[i] <= 0xF9))
            asc[i] = ebcd[i] - 0xF0 + '0';
        if ((ebcd[i] >= 0xC1) && (ebcd[i] <= 0xC6))
            asc[i] = ebcd[i] - 0xC1 + 'A';
    }
}
/**************** 将ASCII字符串展开为BIN字符串 ****************
输入:
    ASCBuf: ASC字符串
    ASCLen: ASC字符串的长度
输出:
    BinBuf: BIN字符串(即ASC码-'0')
****************************************************************/
void ASC2Bin(unsigned char* ASCBuf, unsigned char ASCLen, unsigned char* BinBuf)
{
    unsigned char i;
    for (i = 0; i <= ASCLen; i++) {
        BinBuf[i] = ASCBuf[i] - 0x30;
    }
}
/**************** 将BIN字符串转变为ASC字符串 ****************
输入:
    BinBuf: BIN字符串
    BinLen: BIN字符串的长度
输出:
    ASCBuf: ASC字符串(即BIN码+'0')
****************************************************************/
void Bin2ASC(unsigned char* BinBuf, unsigned char BinLen, unsigned char* ASCBuf)
{
    unsigned char i;
    for (i = 0; i <= BinLen; i++) {
        ASCBuf[i] = BinBuf[i] + 0x30;
    }
}
/**************** 将BCD字符串展开为BIN字符串 ****************
输入:
    BCDBuf: BCD字符串
    BCDLen: BCD字符串的长度
输出:
    BinBuf: BIN字符串(即ASC码-'0')
****************************************************************/
void BCD2Bin(unsigned char* BCDBuf, unsigned char BCDLen, unsigned char* BinBuf)
{
    for (unsigned char i = BCDLen; i != 0; i--) {
        unsigned char j = BCDBuf[i - 1];
        BinBuf[i * 2 - 1] = j & 0x0F;
        BinBuf[i * 2 - 2] = j >> 4;
    }
}
/**************** 将BIN字符串展开为BCD字符串 ****************
输入:
    BinBuf: BIN字符串(即ASC码-'0')
    BinLen: BIN字符串的长度
输出:
    BCDBuf: BCD字符串
****************************************************************/
void Bin2BCD(unsigned char* BinBuf, unsigned char BinLen, unsigned char* BCDBuf)
{
    unsigned char i, j;
    for (i = 0, j = 0; i < BinLen; i += 2) {
        unsigned char k = BinBuf[i] << 4;
        BCDBuf[j++] = k + BinBuf[i + 1];
    }
}
/**************** 将BCD码转成10进制数 ****************
输入：
    const unsigned char *BCD  待转换的BCD码
    int length                BCD码数据长度
返回：
    unsigned long             当前数据位的权
原理：
    压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,
    进制为100, 先求每个字符所表示的十进制值，然后乘以权
****************************************************************/
unsigned long BCD2Dec(const unsigned char* BCD, int length)
{
    unsigned long dec = 0;
    for (int i = 0; i < length; i++) {
        int tmp = ((BCD[i] >> 4) & 0x0F) * 10 + (BCD[i] & 0x0F);
        dec += tmp * power(100, length - 1 - i);
    }
    return dec;
}
/**************** 将十进制转为BCD码 ****************
输入：
    int Dec             待转换的十进制数据
    int length          BCD码数据长度
输出：
    unsigned char *BCD  转换后的BCD码
返回：
    0 success
************************************************/
int Dec2BCD(int Dec, unsigned char* BCD, int length)
{
    for (int i = (length - 1); i >= 0; i--) {
        int temp = Dec % 100;
        BCD[i] = ((temp / 10) << 4) + ((temp % 10) & 0x0F);
        Dec /= 100;
    }
    return 0;
}
/**************** 十进制转十六进制 ****************
输入：
    int dec             待转换的十进制数据
    int length          转换后的十六进制数据长度
输出：
    unsigned char *hex  转换后的十六进制数据
返回：
    0 success
************************************************/
int Dec2Hex(int dec, unsigned char* hex, int length)
{
    int i;
    for (i = length - 1; i >= 0; i--) {
        hex[i] = (dec % 256) & 0xFF;
        dec /= 256;
    }
    return 0;
}
/**************** 十六进制转为十进制 ****************
输入：
    const unsigned char *hex  待转换的十六进制数据
    int length                十六进制数据长度
返回：
    int rslt                  转换后的十进制数据
原理：
    十六进制每个字符位所表示的十进制数的范围是0 ~255,
    进制为256, 左移8位(<<8)等价乘以256
************************************************/
unsigned long Hex2Dec(const unsigned char* hex, int length)
{
    int i;
    unsigned long rslt = 0;
    for (i = 0; i < length; i++) {
        rslt += (unsigned long)(hex[i]) << (8 * (length - 1 - i));
    }
    return rslt;
}
// 功能：HEX格式转为BCD码
void Hex2BCD(char* HEX, char* BCD)
{
    int i = 0;
    while (HEX[i]) {
        BCD[i] = (HEX[i] / 10 * 16) + (HEX[i] % 10);
        i++;
    }
    BCD[i] = '\0';
}
/*
 * 将12位的字符序列号转换成6位
 */
static void pci_test_Get_SerialNum(int iInLen, int iOutLen, unsigned char* pucInPut, unsigned char* pucOutPut)
{
    if ((NULL == pucInPut) || (NULL == pucOutPut)) {
        printf("pci_test_Get_SerialNum Failed! p1:%p, p2:%p\n", pucInPut, pucOutPut);
        return;
    }
    if (iInLen < (iOutLen * 2)) {
        printf("pci_test_Get_SerialNum Failed! iInLen:%d, iOutLen:%d\n", iInLen, iOutLen);
        return;
    }
    int j = 0;
    for (int i = 0; i < iOutLen; i++) {
        if (j >= iInLen)
            break;
        unsigned char ucHTmp = pucInPut[j] & 0x0F;
        j++;
        if (j >= iInLen)
            break;
        unsigned char ucLTmp = pucInPut[j] & 0x0F;
        pucOutPut[i] = (ucHTmp << 4) + ucLTmp;
        j++;
    }
}
