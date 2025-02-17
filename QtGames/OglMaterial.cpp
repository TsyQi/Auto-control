#include "OglMaterial.h"
#include <QPainter>
#include <QPen>

#ifdef _WIN32
#define IMAGE_PATH "..\\WinNTKline\\KlineUtil\\image\\"
#else
#define IMAGE_PATH "../WinNTKline/KlineUtil/image/"
#endif

QOglMaterial::QOglMaterial(QWidget* parent)
    : QOpenGLWidget(parent)
{
#ifdef _GLVBO_
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);
#else
    xVal = yVal = zZoom = 0;
    tHigh = -1;
    mX = mY = 0;
#endif
}

QOglMaterial::~QOglMaterial()
{
    if (m_showSdl) {
        SDL_GL_quit();
    }
}

#ifdef _GLVBO_
/* 1.1 着色器代码 */
/* *********************************************
*   顶点着色器定义一个输入，它是 4 个成员的矢量 vPosition。
*   主函数声明着色器宣布着色器开始执行。着色器主体非常简单，
*   它复制输入 vPosition 属性到 gl_Position 输出变量中。
*   每个顶点着色器必须输出位置值到 gl_Position 变量中，
*   这个变量传入到管线的下一个阶段中。
*   matrix主要是模型视图矩阵，控制位置和旋转等
* ******************************************** */
/* 顶点着色器 */
static const char* vertexShaderSourceCore =
"attribute vec4 vPosition;\n"
"uniform highp mat4 matrix;\n"
"attribute lowp vec4 a_color;\n"
"varying lowp vec4 v_color;\n"
"void main() {\n"
"   v_color = a_color;\n"
"   gl_Position = matrix * vPosition;\n"
"}\n";

/* *********************************************
*   gl_FragColor，gl_FragColor是片段着色器最终的输出值，
*   本例中输出值来自外部传入的颜色数组。
* ******************************************** */

/* 片段着色器 */
static const char* fragmentShaderSourceCore =
"varying lowp vec4 v_color;\n"
"void main() {\n"
"   gl_FragColor = v_color;\n"
"}\n";

/* 2.1 图形顶点坐标 */
GLfloat vVertices[] = {
    0.0f, 0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
};
/* 2.2 图形顶点索引 */
GLuint tri_index[] = { 0, 1, 2 };
/* 2.3 顶点颜色数组 */
GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,0.5f,
    0.0f, 1.0f, 0.0f,0.5f,
    0.0f, 0.0f, 1.0f,0.5f
};

/**
* @brief 初始化模型信息vbo【显存】
*/
void QOglMaterial::initVbo()
{
    ///< 初始化顶点buffer并装载数据到显存
    glGenBuffers(1, &verVbo);
    glBindBuffer(GL_ARRAY_BUFFER, verVbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vVertices, GL_STATIC_DRAW);

    ///< 初始化索引buffer并装载数据到显存
    glGenBuffers(1, &v_indexVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_indexVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLuint), tri_index, GL_STATIC_DRAW);

    ///< 初始化颜色buffer并装载数据到显存
    glGenBuffers(1, &colorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
}
#endif

void QOglMaterial::coord()
{
    glDisable(GL_DEPTH_TEST);
    QPainter painter;
    painter.begin(this);
    QPen pen;
    pen.setColor(Qt::yellow);
    painter.setPen(pen);
    QString coord = QString("(x=%1,y=%2,h=%3,gl[%4,%5])")
        .arg(mX).arg(mY).arg(tHigh)
        .arg(xVal).arg(yVal);
    painter.drawText(10, 20, coord);
    painter.end();
    glEnable(GL_DEPTH_TEST);
}

void QOglMaterial::textOut(int left, int upon, QColor color, float th, QString family)
{
    glDisable(GL_DEPTH_TEST);
    QPen pen;
    pen.setColor(color);
    QFont font;
    if (family.isNull())
        family = font.defaultFamily();
    font.setFamily(family);
    QPainter painter;
    painter.begin(this);
    painter.setPen(pen);
    painter.setFont(font);
    painter.scale(th, th);
    painter.drawText(left, upon, text);
    painter.end();
    glEnable(GL_DEPTH_TEST);
}

void QOglMaterial::initializeGL()
{
    qDebug("+++ initializeGL +++");
    /* 0. 初始化函数，使能GL函数 */
    initializeOpenGLFunctions();
#ifdef _GLVBO_
    /* 创建项目对象链接着色器 */
    /* 1. 初始化最大的任务是装载顶点和片段着色器 */
    program = new QOpenGLShaderProgram(this);
    /* 一旦应用程序已经创建了顶点、片段着色器对象，
    * 它需要去创建项目对象，项目是最终的链接对象，
    * 每个着色器在被绘制前都应该联系到项目或者项目对象。
    * ***************************************** */
    /* 1.2 加载 */
    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCore)) {
        return;
    }
    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCore)) {
        return;
    }

    // 1.3 设置属性位置，将vPosition属性设置为位置0, vertex为位置1
    /*
    program->bindAttributeLocation("vertex", 1);
    program->bindAttributeLocation("vPosition", 0);
    program->bindAttributeLocation("a_color", 1);
    program->bindAttributeLocation("matrix", 2);
    */
    /* 1.4 链接项目检查错误 */
    if (!program->link())
        return;
    if (!program->bind())
        return;

    ///< 获取shaderprogram的id号，然后可以通过programid号获取一些属性...
    programid = program->programId();

    ///< 从shaderprogram获取变量标识，用到2种方式
    matrixLocation = glGetUniformLocation(programid, "matrix");
    vertexLocation = glGetAttribLocation(programid, "vPosition");
    clorLocation = program->attributeLocation("a_color");

    ///< 初始化vbo，对于实时变化的数据，要在paintGL()里每次调用！
    initVbo();
#else
#ifdef  OGL_KVIEW_H_
    kv.AdjustDraw(640, 480);
#else
    mPng.setPixels(IMAGE_PATH"atlas.png");
#if SDL_MAJOR_VERSION >= 2
    if (m_showSdl && (SDL_GL_init() == 0)) {
#ifndef MAX_PATH
#define MAX_PATH 256
#endif
        char fileName[MAX_PATH];
        sprintf(fileName, "%sspabandari.bmp", IMAGE_PATH);
        struct stCopyRect rect{ { 100,10,360,70 },{ 70,360,500,70 } };
        SDL_GL_loadImage(fileName, rect);
        rect.src = { 128,160,360,256 };
        rect.dst = { 1,1,256,180 };
        SDL_GL_loadImage(fileName, rect);
        struct TextCfg cfg;
        cfg.font = IMAGE_PATH"../font/Deng.ttf";
        cfg.style = TTF_STYLE_NORMAL;
        cfg.color = { 255, 0, 0, 255 };
        cfg.rect = { { 0,0,300,60 }, {100,256,256,24} };
#ifndef _WIN32
        const char* text = "这ge是打在窗口上的等线字体";
        SDL_GL_showText(text, cfg);
#endif
    } else {
        qDebug(m_showSdl ? "SDL_GL_init fail!" : "No SDL GL");
    }
#endif
#endif
#endif // _GLVBO_
}

void QOglMaterial::resizeGL(int width, int height)
{
#ifdef _GLVBO_
    /* 2.1 viewport 设定窗口的原点 origin (x, y)、宽度和高度 */
    glViewport(0, 0, width, height);

    ///< 模型矩阵重置
    m_projection.setToIdentity();
    ///< 透视投影【简单容错】
    qreal aspect = qreal(width) / qreal(height ? height : 1);
    m_projection.perspective(60.0f, aspect, 1.0f, 100.0f);
    ///< 增加模型矩阵，做一定偏移量，使物体刚开始渲染出来时就可以显示！
    m_projection.translate(0.0f, 0.0f, -2.0f);
#else
    if (height == 0) {
        height = 1;
    }
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLdouble aspectRatio = (GLfloat)width / (GLfloat)height;
    GLdouble zNear = 0.1;
    GLdouble zFar = 100.0;

    GLdouble rFov = 45.0 * _PI_ / 180.0;
    glFrustum(-zNear * tan(rFov / 2.0) * aspectRatio,
        zNear * tan(rFov / 2.0) * aspectRatio,
        -zNear * tan(rFov / 2.0),
        zNear * tan(rFov / 2.0),
        zNear, zFar);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
#endif
}

void QOglMaterial::paintGL()
{
#ifdef  OGL_KVIEW_H_
    kv.InitGraph();
    kv.DrawCoord(0, 0);
    kv.GetMarkDatatoDraw("../KlineUtil/data/SH600747.DAT");
    qDebug() << "(" << kv.lastmarket.price << ")";
#else
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
#ifdef _GLVBO_
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    ///< shader传入模型视图矩阵
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, m_projection.data());

    ///< shader绑定并启用颜色数组buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
    glEnableVertexAttribArray(clorLocation);
    glVertexAttribPointer(clorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

    ///< shader绑定并启用顶点数组buffer
    glBindBuffer(GL_ARRAY_BUFFER, verVbo);
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    ///< shader绑定并顶点索引数组buffer - 索引无需启用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_indexVbo);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    ///< 解绑buffer、关闭启用顶点、颜色数组
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(vertexLocation);
    glDisableVertexAttribArray(clorLocation);
#else
    glTranslatef(0, 0, 0);
    static uint32_t i = 0, j = 0;
    mPng.showPixels(i, j);
    i++;j++;
    if (i == 1024) i = 0;
    if (j == 768) j = 0;

    if (bingo) {
        textOut(30, 40, Qt::red, 3, "Helvetica");
        mPng.showFullPixels();
    } else {
        coord();
        textOut();

        glTranslatef(-1.0, 0, -8.0);
        glBegin(GL_QUADS);
        glVertex3f(0.0 + xVal, 0.4 + yVal, zZoom);
        glVertex3f(0.4 + xVal, 0.4 + yVal, zZoom);
        glVertex3f(0.4 + xVal, 0.0 + yVal, zZoom);
        glVertex3f(0.0 + xVal, 0.0 + yVal, zZoom);
        glEnd();

        glTranslatef(3, tHigh, -8.0);
        glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.8, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-0.8, -0.8, 0.0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.8, -0.8, 0.0);
        glEnd();
    }
#endif
#endif // OGL_KVIEW_H_
}
