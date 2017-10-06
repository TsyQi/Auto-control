#include "ShowPNG.h"

ShowPNG::ShowPNG()
{
}

ShowPNG::~ShowPNG()
{
}

unsigned char* pixels = NULL;
png_uint_32 width, height;
int color_type;

//��ȡÿһ�����õ��ֽ�������Ҫ����4�ı���  
int getRowBytes(int width) {
	//�պ���4�ı���  
	if ((width * 3) % 4 == 0) {
		return width * 3;
	}
	else {
		return ((width * 3) / 4 + 1) * 4;
	}
}

//��ʾͼƬ  
void ShowPNG::Show() {
	glClear(GL_COLOR_BUFFER_BIT); 
	glTranslatef(-1.0, 0, -8.0);
	//ͼƬ�Ƿ���͸����  
	if (color_type == PNG_COLOR_TYPE_RGB) {
		glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}
	else if (color_type == PNG_COLOR_TYPE_RGBA) {
		glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}
	glFlush();
}

int ShowPNG::getPixels(const char* filename)
{
	png_structp png_ptr;
	png_infop info_ptr;
	int bit_depth;
	FILE *fp;

	qDebug() << "lpng16(" << PNG_LIBPNG_VER_STRING << "), zlib(" << ZLIB_VERSION << ")";

	if ((fp = fopen(filename, "rb")) == NULL) {
		return EXIT_FAILURE;
	}
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		fclose(fp);
		return EXIT_FAILURE;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return EXIT_FAILURE;
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/* If we get here, we had a problem reading the file */
		return EXIT_FAILURE;
	}
	/* Set up the input control if you are using standard C streams */
	png_init_io(png_ptr, fp);
	//��ȡPNG�ļ�  
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	//��ȡPNGͼƬ�����Ϣ  
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		NULL, NULL, NULL);
	qDebug() << "[" << width << "*" << height << "]";

	//�������PNG����  
	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
	//����pixel��С  
	unsigned int size = 0;
	if (color_type == PNG_COLOR_TYPE_RGB) {
		size = getRowBytes(width) * height;
	}
	else if (color_type == PNG_COLOR_TYPE_RGBA) {
		size = width * height * 4;
	}
	else {
		return EXIT_FAILURE;
	}
	//����ѿռ�  
	pixels = (unsigned char*)malloc(size);
	int i;
	for (i = 0; i < height; i++) {
		//����ÿ�е����ݵ�pixel��  
		//openglԭ�����·�������ʱҪ����һ��  
		if (color_type == PNG_COLOR_TYPE_RGB) {
			memcpy(pixels + getRowBytes(width) * i, row_pointers[height - i - 1], width * 3);
		}
		else if (color_type == PNG_COLOR_TYPE_RGBA) {
			memcpy(pixels + i * width * 4, row_pointers[height - i - 1], width * 4);
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(fp);
	return EXIT_SUCCESS;
}

GLuint ShowPNG::CreateTextureFromPng(const char* filename)
{
	unsigned char header[8];     //8  
	int k;   //ѭ������  
	GLuint textureID; //��ͼ����  
	int width, height; //��¼ͼƬ����͸�  
	png_byte color_type; //ͼƬ�����ͣ��������Ƿ��ǿ�����ͨ����  
	png_byte bit_depth; //�ֽ����  

	png_structp png_ptr; //ͼƬ  
	png_infop info_ptr; //ͼƬ����Ϣ  
	int number_of_passes; //����ɨ��  
	png_bytep * row_pointers;//ͼƬ����������  
	int row, col, pos;  //�ı�png�������е����⡣  
	GLubyte *rgba;

	FILE *fp = fopen(filename, "rb");//��ֻ����ʽ���ļ���Ϊfile_name���ļ�  
	if (NULL == fp)//������Ӧ���ܵĴ�����  
	{
		printf("error: %s\n", strerror(errno));//�رմ򿪵��ļ�������Ĭ����ͼ  
		return 0;//Ӧ�õ���һ������Ĭ����ͼ����ID�ĺ���  
	}
	//��ȡ�ļ�ͷ�ж��Ƿ���PNG��ʽͼƬ.������������Ӧ����  
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		fclose(fp);
		return 0; //ÿ����������һ���ģ�  
	}

	//����libpng��libpng-manual.txt��˵��ʹ���ĵ� �����������ʼ��png_structp �� png_infop  
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); //�������ǰ󶨴����Լ�����ĺ�����������Ϊ��  
	if (!png_ptr)//������Ӧ����ʼ��ʧ�ܵĴ���  
	{
		fclose(fp);
		return 0;
	}
	//���ݳ�ʼ����png_ptr��ʼ��png_infop  
	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
	{
		//��ʼ��ʧ���Ժ�����png_structp  
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}

	//����libpng������˵���ȶ����������
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		//�ͷ�ռ�õ��ڴ棡Ȼ��ر��ļ�����һ����ͼID
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}
	//ͨ��i/o���ƺ���png_init_io���ļ� 
	png_init_io(png_ptr, fp);
	//����libpng�ļ��ӵڼ�����ʼmissing  
	png_set_sig_bytes(png_ptr, 8);
	//�򵥵Ĳ�����ȡʵ��ͼƬ��Ϣ
	png_read_info(png_ptr, info_ptr);
	//���ͼƬ����Ϣ width height ��ɫ����  �ֽ����  
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	//���ͼƬ����alphaͨ������  
	// if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)  
	// png_set_swap_alpha(png_ptr);  
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	//����ɨ��ͼƬ 
	number_of_passes = png_set_interlace_handling(png_ptr);
	//����ȡ������Ϣ���µ�info_ptr  
	png_read_update_info(png_ptr, info_ptr);
	//���ļ�  
	if (setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		return 0;
	}
	rgba = (GLubyte*)malloc(width * height * 4);
	//ʹ�ö�̬����  ���ó���  
	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

	for (k = 0; k < height; k++)
		row_pointers[k] = NULL;

	//ͨ��ɨ���������ÿһ�н��õ������ݸ�ֵ����̬����         
	for (k = 0; k<height; k++)
		//row_pointers[k] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));  
		row_pointers[k] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr,
			info_ptr));
	//����PNG���������� ��-��-�Ӷ����� ����ͼ��Ҫ�����ض��� ��-��-�ӵ׵��� ��������Ҫ���������ݽ���һ���������� ��ͼƬ  
	png_read_image(png_ptr, row_pointers);

	pos = (width * height * 4) - (4 * width);
	for (row = 0; row < height; row++)
	{
		for (col = 0; col < (4 * width); col += 4)
		{
			rgba[pos++] = row_pointers[row][col];        // red  
			rgba[pos++] = row_pointers[row][col + 1];    // green  
			rgba[pos++] = row_pointers[row][col + 2];    // blue  
			rgba[pos++] = row_pointers[row][col + 3];    // alpha  
		}
		pos = (pos - (width * 4) * 2);
	}

	//����������ͼ��Ч  
	glEnable(GL_TEXTURE_2D);
	//��������   
	glGenTextures(1, &textureID);
	//������  
	glBindTexture(GL_TEXTURE_2D, textureID); //������󶨵�����  
											 //������ͼ������Ļ��Ч�� Ĭ��ֻ������  
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//�����������õ�ͼƬ����  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	free(row_pointers);
	fclose(fp);
	return textureID;
}

void ShowPNG::loadGLTextures(const char* filename)
{
	if (filename == NULL)
		return;
#ifdef QT_GUI_LIB
	QImage tex, buf;
	if (!buf.load(filename))
	{
		qWarning("Could not read image file, using single-color instead.");
		QImage dummy(128, 128, QImage::Format_RGB32);
#ifdef QCOLOR_H
		dummy.fill(QColor("darkCyan").rgb());
#endif
		buf = dummy;
	}
#ifdef QGL_H
	tex = QGLWidget::convertToGLFormat(buf);
#endif
	glGenTextures(3, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
#endif
}

void ShowPNG::Show(const char* filename)
{
	if (filename == NULL)
		return;
	glEnable(GL_TEXTURE_2D);
	if (NULL == texture[0])
		texture[0] = CreateTextureFromPng(filename);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

