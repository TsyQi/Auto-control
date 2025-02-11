#pragma once
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32
#include <GL/glu.h>
#include "../com/auto_tchar.h"
#include "../GL/glaux.h"
#include "../com/Glalgorithm.h"
#include "../Def/MacroDef.h"

#define INTS int
#define REAL float /* 4 byte 		*/
#define __error   0
#define NoError   1
#define FALSE     0
#define TRUE      1
#define MINREAL   1.0e-5
#define MAXREAL   1.0e10
#define MAXINTS   (0x7f<<24)
#define zero(x)   fabs(x) < MINREAL
#define upint(x)  (INTS)(x+1-MINREAL)
#define addr(i,j) (long)(j)*cam.width+i
#define MIN(x,y)  ((x)<(y)?(x):(y))

#define LEFT_EG  0x01
#define RIGHT_EG 0x02
#define HORIZ    0x04
#define DCV      0x08
#define UCV      0x10

#define MAX_TEX  20

#define	CHARS	signed	 char		/* 1 byte signed char	*/
#define	CHARU	unsigned char		/* 1 byte unsigned char	*/
#define	INT2S	signed   short		/* 2 byte signed int	*/
#define INT2U	unsigned short		/* 2 byte unsigned int	*/
#define	INT4S	signed   long		/* 4 byte signed int	*/
#define	INT4U	unsigned long		/* 4 byte unsigned int	*/
#define INT8U	unsigned long long  /* 8 byte unsigned int	*/		

/* for material type	*/
#define DIFFUSE		1
#define SPECULAR	2
#define ENERGY		8
#define SAMEKD		16
#define SAMEKS		32
#define SAMEKT		64

/* for image type	*/
#define COLOR2		1
#define COLOR16		4
#define COLOR256	8
#define HIGHCOLOR	16
#define TRUECOLOR	24
#define ALPHACOLOR	32

/* Patch type		*/
#define cTRIANGLE	't'
#define cQUADRIC	'q'

/* Texture mapping type */
#define CLAMP_TEXTURE		0
#define REPEAT_TEXTURE	1

// The following code for mouse routines was contributed.
// These are used for the motion function.
#define DEFAULT 0
#define FORWARD 1
#define UPDOWN 2
#define TURNLEFT 3
#define MOVELR	4
#define LOOKUP 5

// Initial eye position and vector of sight.
static GLfloat speed = 0;
/* Data struct		*/
typedef struct texture_2d {
    char	fname[30];
    INT2U	type;	/* CLAMP_TEXTURE or REPEAT_TEXTEXTURE */
}   TEXTURE_2D;

typedef struct point3d  /* point in 3D */
{
    FLOAT	x, y, z;	/* 3D coordinate*/
    FLOAT	r, g, b;	/* color        */
    FLOAT	u, v;		/* texture coordinate */
}   POINT3D;


typedef struct surface	/* surface */
{
    INT4U	points;     /* point number		*/
    INT4U	triangle;	/* triangle number	*/
    INT4U	quadric;	/* quadrangle number*/
    POINT3D* pointlist;	/* points list		*/
    INT4U* patchlist;	/* patches list(list of point No.)*/
    INT4U	texId;      /* texture index	*/
}   SURFACE;

typedef struct object	/* OBJECT */
{
    INT4U	SurfNum;    /* surface number and list size */
    SURFACE* surfaceList;  /* surfaces list in the object  */
}   OBJECT;

class House {
public:
    // This is the holding space for the landscape colours.
    int Width = 0;
    int Height = 0;
    HGLRC m_hGLContext;
    int m_GLPixelIndex = 0;
    // Mouse position and button.
    int oldmx = 0, oldmy = 0, mb = 0;
    typedef GlAlgorithm::GLPoint Point;
    typedef GlAlgorithm::GLColour Color3f;
protected:
    int wide = 0;
    int tall = 0;

    TEXTURE_2D** TextureList = NULL;
    OBJECT* objectList = NULL;  /* ObjectList[0]:isolated surfaces*/

    INT4S         objectNum = 0;

    char          gEnergyFile[30] = {};
    char	      sLookAtFN[100] = {};
    char	      ImageName[30] = {};

    unsigned short int comp = 32;   // Scale modifier.

    unsigned short int texture_mapping = FALSE,
        land_fogging = TRUE, flat_shading = TRUE;

    float angle = 0;
    float Near = 0;
    float ex = 0, ey = 0, ez = 0;
    float cx = 0, cy = 0, cz = 0;
    float ux = 0, uy = 0, uz = 0;

    unsigned char* ImageDatas[MAX_TEX] = {};
    INT2U m_lxs[MAX_TEX], m_lys[MAX_TEX] = {};
    int   texNum = 0;
public:
    House();
    // OpenGL specific
    BOOL CreateViewGLContext(HDC hDC);
    virtual ~House();
    void    MoveEye(int type, GLfloat amount = 0, int update = 0);
    void	InitTex(int TexIndex);
    void	KillTex();
    void    LoadAllTexture();
    void    CleanAllTexture();
    void    CleanList();
    void    InitLookAt();
    void    ReadData();
    unsigned char* OpenTexImage(INT2U TexIndex, INT2U* slx, INT2U* sly);
    void    InitRenderWin();
    void    InitGeometry(void);
    void    Render(void);
private:
    void ReadUntil(FILE* fp, const char* target, char* buffer, size_t bufferSize);
    void MoveForward(GLfloat amount);
    void MoveLeftRight(GLfloat amount);
    void TurnLeft(GLfloat amount);
    void ResetView();
    void UpdateView();
};
