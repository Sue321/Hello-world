// bmpfunc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdlib.h"
typedef struct TagBitMapFileHeader
{
	/*unsigned short bfType; */   //说明文件的类型，该值必需是0x4D42（十六进制ASCII码4D代表“B”，42代表“M”），也就是字符'BM'。
  //其中不包含文件类型信息（由于结构体的内存结构决定，要是加了的话将不能正确读取文件信息）
	unsigned long bfSize;      //说明该位图文件的大小，用字节为单位
	unsigned short bfReserved1;     //保留，必须设置为0
	unsigned short bfReserved2;      //保留，必须设置为0
	unsigned long bfOffBits;         //说明从文件头开始到实际的图象数据之间的字节的偏移量。
	                                 //这个参数是非常有用，因为位图信息头和调色板的长度会根据不同情况而变化，
		                             //可以用这个偏移值迅速的从文件中读取到位数据。
} bmfh;

typedef struct TagBitMapInfoHeader
{
	unsigned long biSize;  //说明BITMAPINFOHEADER结构所需要的字数，40字节
	long biWidth;         //说明图象的宽度，以象素为单位。
	long biHeight;        // 说明图象的高度，以象素为单位。					
	unsigned short biPlanes;   //为目标设备说明位面数，其值将总是被设为1。
	unsigned short biBitCount;   //说明比特数/象素，其值为1、4、8、16、24、或32。	                            
	unsigned long biCompression;   //说明图象数据压缩的类型,没有压缩的类型：BI_RGB。
	unsigned long biSizeImage;     //说明图象的大小，以字节为单位。当用BI_RGB格式时，可设置为0。
	long biXPelsPerMeter;      //说明目标设备水平分辨率，用象素/米表示。可设置为0
	long biYPelsPerMeter;      //说明目标设备垂直分辨率，用象素/米表示。同上
	unsigned long biClrUsed;   //说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）。
	unsigned long biClrImportant;//说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。
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
			fread(&fileType,sizeof(unsigned short),1,fp);  //文件类型在文件头前面
			printf("文件类型：%x\n", fileType);  
			fread(&bmpFileHeader, sizeof(bmfh), 1, fp);		
			fread(&bmpInfoHeader, sizeof(bmih), 1, fp);
			printf("bmp文件信息头\n"); 
			
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
			//malloc(size)动态分配一个长度为size的连续空间，返回的指针指向分配域的第一个字节
			if (img->imageData) {
				printf("分配成功: %x\n", img->imageData);
			}
			
			
			for (int i = img->height-1; i >=0; i--)
			{
				for (int j = 0; j < img->width; j=j+1)
				{    
					for (int k = 0; k < 3;k++) 
					{
						fread(&pix, sizeof(unsigned char), 1, fp);
						//将文件读入的数据存储到第一个参数地址
						img->imageData[i*step + j*3 + k] = pix;
						//地址不断减小		
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
		printf("文件大小：%d\n", bmpFileHeader.bfSize);
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfOffBits = 54;
		fwrite(&bmpFileHeader,sizeof(bmfh),1,fw);
		
		bmpInfoHeader.biSize = 40;
		bmpInfoHeader.biWidth = img->width/2;
		printf("图片宽度：%d\n", bmpInfoHeader.biWidth);
		bmpInfoHeader.biHeight = img->height;
		printf("图片宽度：%d\n",bmpInfoHeader.biHeight);
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
					//从最后一个数据开始赋值
					fwrite(&pix, sizeof(unsigned char), 1, fw);
					//将第一个参数地址开始的数据向文件输出
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
		printf("修改成功\n");
	}
	
	while(true);
}

