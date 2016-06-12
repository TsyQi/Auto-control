#include "transcoding.h"
#ifdef _LIBICONV_H

//remarks:��������UTF-8��ת
//[IN] dest:ת���ʽ,�硰GBK��,"UTF-8";pbSrc:ԭ���ʽ;input:ԭ���ַ���;ilen:ԭ���ַ�������;
//[OUT] output:ת���ַ���;olen:ת���ַ������ȡ�
int conv_charset(const char* dest,const char* pbSrc,const char *input,size_t ilen,char* output,size_t olen)
	{
		int convlen=(int)olen;
		iconv_t conv=iconv_open(dest,pbSrc);
		if(conv==(iconv_t)-1)
			return -1;
		memset(output,0,olen);
		if(iconv(conv,&input,&ilen,&output,&olen))
			return -1;
		iconv_close(conv);
		return (int)(convlen-olen);
	}
#endif // _LIBICONV_H
//remarks:�ַ���ת��Ϊ16������
//[IN]:string Դ�ַ���
//[OUT]:cbuf 16����ת���ַ�
int str_to_hex(char *string, char *cbuf)  
{  
	int len = strlen(string);
    unsigned char high, low;  
    int idx, ii=0;  
    for (idx=0; idx<len; idx+=2)   
    {  
        high = string[idx];  
        low = string[idx+1];  
          
        if(high>='0' && high<='9')  
            high = high-'0';  
        else if(high>='A' && high<='F')  
            high = high - 'A' + 10;  
        else if(high>='a' && high<='f')  
            high = high - 'a' + 10;  
        else  
            return -1;  
          
        if(low>='0' && low<='9')  
            low = low-'0';  
        else if(low>='A' && low<='F')  
            low = low - 'A' + 10;  
        else if(low>='a' && low<='f')  
            low = low - 'a' + 10;  
        else  
            return -1;  
          
        cbuf[ii++] = high<<4 | low;  
    }  
    return 0;  
}  
/*
// C prototype : void hex_to_str(unsigned char *dest, char *pbSrc)
// parameter(s): [OUT] pbDest - ���Ŀ���ַ���
//	[IN] pbSrc - ����16����������ʼ��ַ
//	��[IN] len - 16���������ֽ���
// void hex_to_str(unsigned char *dest, unsigned char *pbSrc, int len)��
// return value: 
// remarks : ��16������ת��ΪASCII���ʽ�ַ���
*/
void hex_to_str(unsigned char *dest, char *pbSrc)
{
	int i;
	char ddl,ddh;
	int len=sizeof(dest) * 4;
	if(dest==NULL)
		return;
	for (i=0; i<len; i++)
	{
		ddh = 48 + dest[i] / 16;
		ddl = 48 + dest[i] % 16;
		if (ddh > 57) ddh = ddh + 7 + 32;
		if (ddl > 57) ddl = ddl + 7 + 32;
		pbSrc[i*2] = ddh;
		pbSrc[i*2+1] = ddl;
	}
	pbSrc[len*2] = '\0';
}
