// bmpfunc.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdlib.h"
typedef struct TagBitMapFileHeader
{
	/*unsigned short bfType; */   //˵���ļ������ͣ���ֵ������0x4D42��ʮ������ASCII��4D����B����42����M������Ҳ�����ַ�'BM'��
  //���в������ļ�������Ϣ�����ڽṹ����ڴ�ṹ������Ҫ�Ǽ��˵Ļ���������ȷ��ȡ�ļ���Ϣ��
	unsigned long bfSize;      //˵����λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ
	unsigned short bfReserved1;     //��������������Ϊ0
	unsigned short bfReserved2;      //��������������Ϊ0
	unsigned long bfOffBits;         //˵�����ļ�ͷ��ʼ��ʵ�ʵ�ͼ������֮����ֽڵ�ƫ������
	                                 //��������Ƿǳ����ã���Ϊλͼ��Ϣͷ�͵�ɫ��ĳ��Ȼ���ݲ�ͬ������仯��
		                             //���������ƫ��ֵѸ�ٵĴ��ļ��ж�ȡ��λ���ݡ�
} bmfh;

typedef struct TagBitMapInfoHeader
{
	unsigned long biSize;  //˵��BITMAPINFOHEADER�ṹ����Ҫ��������40�ֽ�
	long biWidth;         //˵��ͼ��Ŀ�ȣ�������Ϊ��λ��
	long biHeight;        // ˵��ͼ��ĸ߶ȣ�������Ϊ��λ��					
	unsigned short biPlanes;   //ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1��
	unsigned short biBitCount;   //˵��������/���أ���ֵΪ1��4��8��16��24����32��	                            
	unsigned long biCompression;   //˵��ͼ������ѹ��������,û��ѹ�������ͣ�BI_RGB��
	unsigned long biSizeImage;     //˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0��
	long biXPelsPerMeter;      //˵��Ŀ���豸ˮƽ�ֱ��ʣ�������/�ױ�ʾ��������Ϊ0
	long biYPelsPerMeter;      //˵��Ŀ���豸��ֱ�ֱ��ʣ�������/�ױ�ʾ��ͬ��
	unsigned long biClrUsed;   //˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ�����
	unsigned long biClrImportant;//˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��
}bmih;


typedef struct Image
{
	int width;
	int height;
	int channels;
	unsigned char* imageData;
}image;

image* readImage(const char* path)
{
	        unsigned short fileType;
			bmfh bmpFileHeader;
			bmih bmpInfoHeader;
			int step = 0;
			int offset = 0;
			unsigned char pix;
			image*  img;
			FILE*  fp;

			img = (image*)malloc(sizeof(image));
			fp = fopen(path,"rb");
			if (!fp)
			{
				free(img);
				return NULL;
			}
			fread(&fileType,sizeof(unsigned short),1,fp);  //�ļ��������ļ�ͷǰ��
			printf("�ļ����ͣ�%x\n", fileType);  
			fread(&bmpFileHeader, sizeof(bmfh), 1, fp);		
			fread(&bmpInfoHeader, sizeof(bmih), 1, fp);
			printf("bmp�ļ���Ϣͷ\n"); 
			
			img->width = bmpInfoHeader.biWidth;
			img->height = bmpInfoHeader.biHeight;
			img->channels = 3;
			step = 3 * bmpInfoHeader.biWidth ;
			offset = (step) % 4;
			if (offset != 0)
			{
				offset = 4 - offset;				
			}

			img->imageData = (unsigned char *)malloc(sizeof(unsigned char)* bmpInfoHeader.biWidth* bmpInfoHeader.biHeight*3);
			//malloc(size)��̬����һ������Ϊsize�������ռ䣬���ص�ָ��ָ�������ĵ�һ���ֽ�
			if (img->imageData) {
				printf("����ɹ�: %x\n", img->imageData);
			}
			
			
			for (int i = img->height-1; i >=0; i--)
			{
				for (int j = 0; j < img->width; j=j+1)
				{    
					for (int k = 0; k < 3;k++) 
					{
						fread(&pix, sizeof(unsigned char), 1, fp);
						//���ļ���������ݴ洢����һ��������ַ
						img->imageData[i*step + j*3 + k] = pix;
						//��ַ���ϼ�С		
					}						
				}
				if (offset != 0)
				{
					fread(&pix, sizeof(unsigned char), 1, fp);
				}
			}

	return img;

}

bool writeImage(const char* path,image* img)
{
	unsigned short fileType = 0x4D42;
	bmfh bmpFileHeader;
	bmih bmpInfoHeader;
	int step = img->width * 3;
	int offset = step / 2 % 4;
	if (offset != 0)
	{
		offset = 4 - offset;
	}
	unsigned char pix = 0;
	FILE *fw;
	fw = fopen(path, "wb");
	if (!fw)
	{
		return false;
	}
	
	fwrite(&fileType,sizeof(unsigned short),1,fw);
	
		bmpFileHeader.bfSize = img->height*(img->width/2 + offset)*3 + 54;
		printf("�ļ���С��%d\n", bmpFileHeader.bfSize);
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfOffBits = 54;
		fwrite(&bmpFileHeader,sizeof(bmfh),1,fw);
		
		bmpInfoHeader.biSize = 40;
		bmpInfoHeader.biWidth = img->width/2;
		printf("ͼƬ��ȣ�%d\n", bmpInfoHeader.biWidth);
		bmpInfoHeader.biHeight = img->height;
		printf("ͼƬ��ȣ�%d\n",bmpInfoHeader.biHeight);
		bmpInfoHeader.biPlanes = 1;
		bmpInfoHeader.biBitCount = 24;
		bmpInfoHeader.biCompression = 0;
		bmpInfoHeader.biSizeImage = 0;
		bmpInfoHeader.biXPelsPerMeter = 0;
		bmpInfoHeader.biYPelsPerMeter = 0;
		bmpInfoHeader.biClrUsed = 0;
		bmpInfoHeader.biClrImportant = 0;
		fwrite(&bmpInfoHeader,sizeof(bmih),1,fw);

	     

		for (int i =img->height-1; i>=0; i--)
		{
			for(int j=0;j< img->width/2 ;j=j+1)
			{
				for(int k = 0; k < 3;k++) 
				{
					pix = img->imageData[i*step + j*3 +k];
					//�����һ�����ݿ�ʼ��ֵ
					fwrite(&pix, sizeof(unsigned char), 1, fw);
					//����һ��������ַ��ʼ���������ļ����
				}		
			}	
			if (offset!=0) 
			{
				for (int k =0;k<offset;k++) 
				{
					pix = 0;
					fwrite(&pix, sizeof(unsigned char), 1, fw);
				}
				
			}
		}
	
	fclose(fw);
	return true;
}


int main()
{
	image* img = readImage("img.bmp");
	bool result = writeImage("result.bmp",img);
	if(result)
	{
		printf("�޸ĳɹ�\n");
	}
	
	while(true);
}

