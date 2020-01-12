//
//  Display a bouncing object
//
#define GL_SILENCE_DEPRECATION
#include <fstream>
#include "Angel.h"

typedef vec4  color4;
typedef vec4  point4;

///------------------initial positions, velocities, and other constants such as gravitional force and surface effect---------------///
////LOGIC:xVelocity remains the same through out the motion, yVelocity is affected from the gravitional force, and each hit to the ground.////
//Update the xPosition and yPosition according to velocities.
float xPosition=-4;
float yPosition=3.75;

float g = -0.001;
const float surfaceEffect = 0.01;

float xVelocitySetter = 0.00300;
float xVelocity=0.00300;
float yVelocity=0;

int colorNumber = 4; //initially blue
int objectIdentifier = 0; //  /0 is cube / 1 is sphere / 2 is bunny / set from the menu //for correctly calling the multiple vertex array objects


//////--------------------------------------------------- CUBE ENTITIES  ---------------------------------------------------------////////
const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.33, -0.33,  0.33, 1.0 ),
    point4( -0.33,  0.33,  0.33, 1.0 ),
    point4(  0.33,  0.33,  0.33, 1.0 ),
    point4(  0.33, -0.33,  0.33, 1.0 ),
    point4( -0.33, -0.33, -0.33, 1.0 ),
    point4( -0.33,  0.33, -0.33, 1.0 ),
    point4(  0.33,  0.33, -0.33, 1.0 ),
    point4(  0.33, -0.33, -0.33, 1.0 )
};
// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  //0 black
    color4( 1.0, 0.0, 0.0, 1.0 ),  //1 red
    color4( 1.0, 1.0, 0.0, 1.0 ),  //2 yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  //3 green
    color4( 0.0, 0.0, 1.0, 1.0 ),  //4 blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  //5 magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  //6 white
    color4( 0.0, 1.0, 1.0, 1.0 )   //7 cyan
};
// quad generates two triangles for each face
int Index = 0;
void
quad( int a, int b, int c, int d )
{
    points[Index] = vertices[a]; Index++;
    points[Index] = vertices[b]; Index++;
    points[Index] = vertices[c]; Index++;
    points[Index] = vertices[a]; Index++;
    points[Index] = vertices[c]; Index++;
    points[Index] = vertices[d]; Index++;
}
// generate 12 triangles: 36 vertices
void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//////--------------------------------------------------- SPHERE ENTITIES  ---------------------------------------------------------////////
const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096;
// (4 faces)^(NumTimesToSubdivide + 1)
const int NumVerticesSphere = 3 * NumTriangles;

point4 pointSphere[NumVerticesSphere];
vec3 normals[NumVerticesSphere];

int IndexSphere = 0;
void
triangle( const point4& a, const point4& b, const point4& c )
{
    vec3 normal = normalize( cross(b - a, c - b) );
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = a; IndexSphere++;
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = b; IndexSphere++;
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = c; IndexSphere++;
}
point4
unit( const point4& p )
{
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    point4 t;
    if ( len > DivideByZeroTolerance ) {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    return t;
}
void
divide_triangle( const point4& a, const point4& b,
                const point4& c, int count )
{
    if ( count > 0 ) {
        point4 v1 = unit( a + b );
        point4 v2 = unit( a + c );
        point4 v3 = unit( b + c );
        divide_triangle( a, v1, v2, count - 1 );
        divide_triangle( c, v2, v3, count - 1 );
        divide_triangle( b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else {
        triangle( a, b, c );
    }
}
void
tetrahedron( int count )
{
    point4 v[4] = {
        vec4( 0.0, 0.0, 1.0, 1.0 ),
        vec4( 0.0, 0.942809, -0.333333, 1.0 ),
        vec4( -0.816497, -0.471405, -0.333333, 1.0 ),
        vec4( 0.816497, -0.471405, -0.333333, 1.0 )
    };
    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
}
//////--------------------------------------------------- BUNNY ENTITIES  ---------------------------------------------------------////////
//using ifsteream reads vertices and triangles from BunnyVertex.txt and BunnyTriangles.txt files.
const int NumVerticesBunny = 4922;
const int NumTrianglesBunny = 9840;

point4 verticesBunny[NumVerticesBunny];
vec4 trianglesBunny[NumTrianglesBunny];

//assigns the vertice's and triangle's datas to corresponding arrays.
void readBunny(){
    std::ifstream bunnyVertexDataFile;
    bunnyVertexDataFile.open("BunnyVertex.txt");
    if(!bunnyVertexDataFile) std::cout<<"Unable to open BunnyVertex.txt"<<std::endl;
    float dummy1=0.0, dummy2=0.0, dummy3=0.0, dummy4=0.0;
    int i=0;
    while(bunnyVertexDataFile >> dummy1 >> dummy2 >> dummy3){
        verticesBunny[i] = vec4(dummy1,dummy2,dummy3,1.0);
        i++;
    }
    bunnyVertexDataFile.close();
    std::ifstream bunnyTriangleDataFile;
    bunnyTriangleDataFile.open("BunnyTriangles.txt");
    if(!bunnyTriangleDataFile) std::cout<<"Unable to open BunnyTriangles.txt"<<std::endl;
    i=0;
    while(bunnyTriangleDataFile >> dummy1 >> dummy2 >> dummy3 >> dummy4){
        trianglesBunny[i] = point4(dummy1,dummy2,dummy3,dummy4);
        i++;
    }
    bunnyTriangleDataFile.close();
    
}
//--------------------------------------------------------------------------------------------------------//
// Array of rotation angles (in degrees) for each coordinate axis
//this is needed for rotating the bunny's initial prespective
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Xaxis;
//needed rotating for bunny's initial perspective. -90 degrees in x axis, and 90 degrees in z axis
GLfloat  Theta[NumAxes] = { -90, 0.0, 90.0 };


// ModelView, Projection, and ColorUpdater matrices uniform location
GLuint  ModelView, Projection, ColorUpdater;
GLuint vao[3];
//----------------------------------------------------------------------------
// OpenGL initialization
//Used multiple vertex array objects to change between the object types
void
init()
{
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    //--------------------------------------------CUBE RELATED-------------------------------------------------//
    colorcube();
    // Create a vertex array object
    glGenVertexArrays( 3, vao );
    glBindVertexArray( vao[0] );
    // Create and initialize a buffer object
    GLuint buffer1;
    glGenBuffers( 1, &buffer1 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer1 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    //------------------------------------------SPHERE RELATED-------------------------------------------------//
    // Subdivide a tetrahedron into a sphere
    tetrahedron( NumTimesToSubdivide );
    glBindVertexArray( vao[1] );
    // Create and initialize a buffer object
    GLuint buffer2;
    glGenBuffers( 1, &buffer2 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer2 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(pointSphere) + sizeof(normals),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointSphere), pointSphere );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointSphere), sizeof(normals), normals );
    // set up vertex arrays
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    //-----------------------------------------BUNNY RELATED----------------------------------------------------//
    readBunny();
    glBindVertexArray(vao[2]);
    // Create and initialize a buffer object
    GLuint buffer3;
    glGenBuffers(1,&buffer3);
    glBindBuffer( GL_ARRAY_BUFFER, buffer3 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(verticesBunny) + sizeof(trianglesBunny),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(verticesBunny), verticesBunny );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(verticesBunny), sizeof(trianglesBunny), trianglesBunny);
    // set up vertex arrays
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    //------------------------------------------------------------------------------------------------------//
    // Retrieve transformation, and color uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    ColorUpdater =glGetUniformLocation( program, "ColorUpdater" );
    
    // Set current program object
    glUseProgram( program );
    // Enable hiddden surface removal
    glEnable( GL_DEPTH_TEST );
    // Set state variable "clear color" to clear buffer with.
    glClearColor(1.0, 1.0, 1.0, 1.0 ); // white background
}
//----------------------------------------------------------------------------
//Since objects have different number of vertices, choose according to current object type while drawing.
int numVerticestoBeDrawn[3]={NumVertices,NumVerticesSphere,NumVerticesBunny};
//--------------------------------------------------------------------------------------------------------//
//Updates the position, view of the object, and its color
void
display( void )
{
    glBindVertexArray( vao[objectIdentifier] ); //vao[0] is cube vao[1] is sphere vao[2] is bunny set from the menu
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4 model_view;
    if(objectIdentifier==2){ //if it is Bunny, need to rotate and scale differently than the cube and sphere
        //update the x and y positions to latest ones with displacement function *20 for preserving the proper moving
        vec3 displacementBunny( xPosition*20, yPosition*20, 0.0 );
        model_view = ( Scale(0.01, 0.01, 0.01) * Translate( displacementBunny ) * //calculates the new position and looking of the bunny
                      RotateX( Theta[Xaxis] ) *
                      RotateY( Theta[Yaxis] ) *
                      RotateZ( Theta[Zaxis] ) );
    }else{
        vec3 displacementCubeSphere( xPosition, yPosition, 0.0 ); //update the x and y positions to latest ones with displacement function
        model_view = (Scale(0.2, 0.2, 0.2) * Translate(displacementCubeSphere)); //calculates the new position
    }
    glUniform4fv(ColorUpdater,1,vertex_colors[colorNumber]); //sets the color
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view ); //sets the new position and looking
    glDrawArrays( GL_TRIANGLES, 0, numVerticestoBeDrawn[objectIdentifier] ); //draws the new vertices
    glutSwapBuffers();
}

//--------------------------------------------------------------------------------------------------------//
//Rearrenges the size and position of the object when the window size is changed.
void reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    // Set projection matrix
    mat4  projection;
    GLfloat hOVERw =(GLfloat)h/(GLfloat)w;
    GLfloat wOVERh =(GLfloat)w/(GLfloat)h;
    if (w <= h)
        projection = Ortho(-1.0, 1.0, -1.0 * hOVERw, 1.0 * hOVERw, -1.0, 1.0);
    else
        projection = Ortho(-1.0* wOVERh, 1.0 *wOVERh, -1.0, 1.0, -1.0, 1.0);
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
    glDrawArrays( GL_TRIANGLES, 0, numVerticestoBeDrawn[objectIdentifier] ); //should change this in accordance with cube, sphere or bunny
    glutSwapBuffers();
}
//--------------------------------------------------------------------------------------------------------//
//Sets the keyboard functionalities
//When Q or q button is pressed program terminates
//When I or i button is pressed program resets the position of the current object
//When H or h button is pressed program prints the instructions to the console
void
keyboard( unsigned char key,int x, int y )
{
    if(key == 'Q' | key == 'q')
        exit(0);
    if(key == 'I' | key == 'i'){
        //when I or i is pressed reset the application to initial positions
        xPosition=-4;
        xVelocity=xVelocitySetter;
        yPosition=3.75;
        yVelocity=0;
        g=-0.001;
    }
    if(key == 'h' | key == 'H'){
        std::cout<< " This is a simple bouncing object application"<<std::endl;
        std::cout<< " You can control the horizontal velocity, object type, its drawing style and its color"<<std::endl;
        std::cout<< " ------------------------------------------"<<std::endl;
        std::cout<< " By pressing Q or q you can exit from the program" << std::endl;
        std::cout<< " By pressing I or i you can reset the object positions" << std::endl;
        std::cout<< " By pressing right mouse button you can reach the menu" <<std::endl;
        std::cout<< " Under the menu you can control the displaying options" <<std::endl;
    }
}
//--------------------------------------------------------------------------------------------------------//
//Controls the animation inputs. At the end of each timer, updates the xPosition and yPosition.
//Velocity in the x axis remains constant.
//Velocity in the y axis decreases by the surfaceEffect in each hit to the ground. Gravitation g is added to yVelocity.
void timer( int p )
{
    xPosition = xPosition+xVelocity; //increment x position with respect to x velocity
    if(yPosition<-3.75) {
        yVelocity *= -1;
        yVelocity-=surfaceEffect;
        if(yVelocity<0){
            g=0;
            yVelocity=0;
            xVelocity=0;
        }
    }
    yVelocity = yVelocity + g;
    yPosition = yPosition + yVelocity;
    glutPostRedisplay();
    glutTimerFunc(10,timer,0);
}

//--------------------------------------------MENU RELATED------------------------------------------------//
//initial object type is cube. One can set it to cube, sphere or bunny from menu
void polygonMenu (int n){
    objectIdentifier=n;
}
//initial drawing type is set to lines. One can set it to fill or point mode from menu
void drawingModeMenu (int n){
    if(n==0){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }else if(n==1){
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }else if(n==2){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
//initial color is set to blue. One can change the color (black, red, yellow, green, blue, magenta, white, cyan) from menu
void colorMenu(int n){
    colorNumber = n;
}
//initial velocity is set to 0.003. One can change the velocity(0.001, 0.002, 0.003, 0.004, 0.005) from menu
void velocityMenu (int n){
    if(n==0) xVelocitySetter=0.00100;
    else if(n==1)xVelocitySetter=0.00200;
    else if(n==2)xVelocitySetter=0.00300;
    else if(n==3)xVelocitySetter=0.00400;
    else if(n==4)xVelocitySetter=0.00500;
    xVelocity=xVelocitySetter;
    
}

void mainMenu(int n){}

//creates the menu layout. One can reach the menu by pressing the right button of the mouse
void CreateMenu(){
    
    int hvelocityMenu = glutCreateMenu(velocityMenu);
    glutAddMenuEntry("1", 0);
    glutAddMenuEntry("2", 1);
    glutAddMenuEntry("3", 2);
    glutAddMenuEntry("4", 3);
    glutAddMenuEntry("5", 4);
    
    int pMenu = glutCreateMenu(polygonMenu);
    glutAddMenuEntry("CUBE", 0);
    glutAddMenuEntry("SPHERE", 1);
    glutAddMenuEntry("BUNNY", 2);
    
    int drawingMenu = glutCreateMenu(drawingModeMenu);
    glutAddMenuEntry("WIREFRAME LINES", 0);
    glutAddMenuEntry("POINTS", 1);
    glutAddMenuEntry("SOLID MODE", 2);
    
    int cMenu = glutCreateMenu(colorMenu);
    glutAddMenuEntry("Black", 0);
    glutAddMenuEntry("Red", 1);
    glutAddMenuEntry("Yellow", 2);
    glutAddMenuEntry("Green", 3);
    glutAddMenuEntry("Blue", 4);
    glutAddMenuEntry("Magenta", 5);
    glutAddMenuEntry("White", 6);
    glutAddMenuEntry("Cyan", 7);
    
    glutCreateMenu(mainMenu);
    glutAddSubMenu("Horizontal Velocity", hvelocityMenu);
    glutAddSubMenu("Polygon Type", pMenu);
    glutAddSubMenu("Drawing Mode", drawingMenu);
    glutAddSubMenu("Color Type", cMenu);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


//--------------------------------------------------------------------------------------------------------//

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode(  GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize( 512, 512 );
    glutInitWindowPosition( 50, 50 );
    glutCreateWindow( "Bouncing Object" );
    glewExperimental = GL_TRUE;
    glewInit();
    init();
    //-------------------------------callback functions-----------------------------------//
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc(keyboard);
    glutTimerFunc(5,timer,0);
    //------------------------------------------------------------------------------------//
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//initial drawing type is set to lines.
    CreateMenu();
    
    glutMainLoop();
    return 0;
}


