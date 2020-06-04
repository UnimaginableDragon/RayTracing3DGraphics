#include <iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<vector>
#include <windows.h>
#include <glut.h>
using namespace std;

#define pi (2*acos(0.0))

#define possine sin(pi/60)
#define poscosine cos(pi/60)

#define negsine sin(-pi/60)
#define negcosine cos(-pi/60)

int recursion_level;

#include "bitmap_image.hpp"
#include "base.hpp"

#define windowWidth 500
#define windowHeight 500
#define fov 80

Point3 pos(100, 100, 10);
Point3 l(-1/sqrt(2), -1/sqrt(2), 0);
Point3 r(-1/sqrt(2), 1/sqrt(2), 0);
Point3 u(0, 0, 1);

double look, up, rightVect, thick = 0.5;

int imageWidth, imageHeight;


void drawAxes() {
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    {
        glVertex3f( 100,0,0);
        glVertex3f(-100,0,0);

        glVertex3f(0,-100,0);
        glVertex3f(0, 100,0);

        glVertex3f(0,0, 100);
        glVertex3f(0,0,-100);
    }
    glEnd();
}


void drawSquare(double a) {
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,0);
        glVertex3f( a,-a,0);
        glVertex3f(-a,-a,0);
        glVertex3f(-a, a,0);
    }
    glEnd();
}

void drawPoint(Point3 pt) {

    glColor3f(1.0, 1.0, 0);

    glBegin(GL_QUADS);
    {
        glVertex3f(pt.x+thick, pt.y, pt.z+thick);
        glVertex3f(pt.x+thick, pt.y, pt.z-thick);
        glVertex3f(pt.x-thick, pt.y, pt.z-thick);
        glVertex3f(pt.x-thick, pt.y, pt.z+thick);
    }
    glEnd();



}
void loadTestData() {

    imageWidth = imageHeight = 768;

    recursion_level = 3;

    Point3 center(20,20,10);
    double radius = 10;

    Object *temp;

    temp = new Sphere(center, radius);
    temp->setColor(1,0,0);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);

    center = {0,15,15};
    radius = 5;

    temp = new Sphere(center, radius);
    temp->setColor(1,0,1);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    //objects.push_back(temp);


    Point3 a(20.0, 20.0, 20.0);
    Point3 b(40.0, 30.0, 10.0);
    Point3 c(50.0, 40.0, 0.0);

    temp = new Triangle(a, b, c);
    temp->setColor(0, 1, 0);
    temp->setCoEfficients(0.4, 0.2, 0.1, 0.3);
    temp->setShine(5);
    //objects.push_back(temp);


    double coeff[] = {1, 1, 1, 0, 0, 0, -20, -20, -20, 200};
    Point3 reff(0, 0, 0);

    temp = new GeneralQuadratic(coeff, reff, 0, 0, 5);
    temp->setColor(1, 0, 1);
    temp->setCoEfficients(0.4, 0.2, 0.1, 0.3);
    temp->setShine(3);
    //objects.push_back(temp);


    Point3 light1(-50,50,50);
    lights.push_back(light1);

    temp = new Floor(1000, 20);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);

}


void loadActualData() {

    freopen("scene.txt", "r", stdin);

    cin>>recursion_level;
    cin>>imageWidth;
    imageHeight = imageWidth;

    int numOfObjects;
    cin>>numOfObjects;

    string command;
    double a, b, c, radius;

    Object *temp;

    for (int i=0; i<numOfObjects; i++) {
        cin>>command;

        if (command == "sphere") {

            cin>>a>>b>>c;
            Point3 center(a, b, c);

            cin>>radius;
            temp = new Sphere(center, radius);

            cin>>a>>b>>c;
            temp->setColor(a, b, c);

            cin>>a>>b>>c>>radius;
            temp->setCoEfficients(a, b, c, radius);

            cin>>a;
            temp->setShine(a);

            objects.push_back(temp);
        }

        else if (command == "triangle") {

            cin>>a>>b>>c;
            Point3 A(a, b, c);

            cin>>a>>b>>c;
            Point3 B(a, b, c);

            cin>>a>>b>>c;
            Point3 C(a, b, c);

            temp = new Triangle(A, B, C);

            cin>>a>>b>>c;
            temp->setColor(a, b, c);

            cin>>a>>b>>c>>radius;
            temp->setCoEfficients(a, b, c, radius);

            cin>>a;
            temp->setShine(a);

            objects.push_back(temp);

        }

        else if (command == "general") {

            double coeff[10];
            for (int c=0; c<10; c++) {
                cin>>coeff[c];
            }

            cin>>a>>b>>c;
            Point3 reff(a, b, c);

            cin>>a>>b>>c;
            temp = new GeneralQuadratic(coeff, reff, a, b, c);

            cin>>a>>b>>c;
            temp->setColor(a, b, c);

            cin>>a>>b>>c>>radius;
            temp->setCoEfficients(a, b, c, radius);

            cin>>a;
            temp->setShine(a);

            objects.push_back(temp);

        }

    }

    cin>>numOfObjects;
    for (int i=0; i<numOfObjects; i++) {
        cin>>a>>b>>c;

        Point3 light(a, b, c);
        lights.push_back(light);
    }


    temp = new Floor(1000, 20);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);


}

void capture() {

    cout<<pos;

    Point3** frameBuffer;

    frameBuffer = new Point3* [imageWidth];

    Point3 color(0, 0, 0);

    for (int i=0; i<imageWidth; i++) {
        frameBuffer[i] = new Point3[imageHeight];
        for (int j=0; j<imageHeight; j++) {
            frameBuffer[i][j] = color;
        }
    }



    double planeDistance = (windowHeight/2)/tan(fov*pi/360);

    Point3 topLeft = pos + (l * planeDistance - r * (windowWidth/2) + u * (windowHeight/2));
    cout<<topLeft;

    double du = (windowWidth*1.0) / imageWidth;
    double dv = (windowHeight*1.0) / imageHeight;

    //cout<<du<<','<<dv<<endl;

    for (int i=0; i<imageWidth; i++) {
        for (int j=0; j<imageHeight; j++) {

            Point3 cornerDir = topLeft + r*j*du - u*i*dv;
            //cout<<cornerDir;

            Ray ray(pos, cornerDir - pos);

            int nearest=-1;
            double minT = 9999999;
            double color[3];

            for (int k=0; k < objects.size(); k++) {

                double t = objects[k]->intersect(&ray, color, 0);


                if(t <= 0) {
                    continue;
                } else if (t < minT) {
                    minT = t;
                    nearest = k;
                }

                //cout<<t<<endl;
            }

            if(nearest!=-1) {

                double t = objects[nearest]->intersect(&ray, color, 1);
                //cout<<i<<','<<j<<','<<t<<','<<color[0]<<','<<color[1]<<','<<color[2]<<endl;
                frameBuffer[i][j].x = color[0];
                frameBuffer[i][j].y = color[1];
                frameBuffer[i][j].z = color[2];


            }
        }
    }



    bitmap_image image(imageWidth, imageHeight);

    for (int i=0; i<imageWidth; i++) {
        for (int j=0; j<imageHeight; j++) {
            image.set_pixel(j, i, frameBuffer[i][j].x*255, frameBuffer[i][j].y*255, frameBuffer[i][j].z*255);
        }
    }

    image.save_image("output.bmp");

    cout<<"saved\n";
    exit(0);
}


void keyboardListener(unsigned char key, int x,int y) {


    switch(key) {

    case '1':

        l.x = l.x * poscosine - l.y * possine;
        l.y = l.x * possine + l.y * poscosine;

        look = sqrt(l.x*l.x + l.y*l.y + l.z*l.z);

        l.x /= look;
        l.y /= look;

        r.x = r.x * poscosine - r.y * possine;
        r.y = r.x * possine + r.y * poscosine;

        rightVect = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);

        r.x /= rightVect;
        r.y /= rightVect;

        break;

    case '2':

        l.x = l.x * negcosine - l.y * negsine;
        l.y = l.x * negsine + l.y * negcosine;

        look = sqrt(l.x*l.x + l.y*l.y + l.z*l.z);

        l.x /= look;
        l.y /= look;

        r.x = r.x * negcosine - r.y * negsine;
        r.y = r.x * negsine + r.y * negcosine;

        rightVect = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);

        r.x /= rightVect;
        r.y /= rightVect;

        break;

    case '3':

        l.x = l.x * poscosine + u.x * possine;
        l.y = l.y * poscosine + u.y * possine;
        l.z = l.z * poscosine + u.z * possine;

        u.x = u.x * poscosine - l.x * possine;
        u.y = u.y * poscosine - l.y * possine;
        u.z = u.z * poscosine - l.z * possine;

        break;

    case '4':

        l.x = l.x * negcosine + u.x * negsine;
        l.y = l.y * negcosine + u.y * negsine;
        l.z = l.z * negcosine + u.z * negsine;

        u.x = u.x * negcosine - l.x * negsine;
        u.y = u.y * negcosine - l.y * negsine;
        u.z = u.z * negcosine - l.z * negsine;


        break;

    case '5':

        r.y = r.y * poscosine - r.z * possine;
        r.z = r.y * possine + r.z * poscosine;

        rightVect = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);

        r.y /= rightVect;
        r.z /= rightVect;

        u.y = u.y * poscosine - u.z * possine;
        u.z = u.y * possine + u.z * poscosine;

        up = sqrt(u.x*u.x + u.y*u.y + u.z*u.z);

        u.y /= up;
        u.z /= up;

        break;

    case '6':

        r.y = r.y * negcosine - r.z * negsine;
        r.z = r.y * negsine + r.z * negcosine;

        rightVect = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);

        r.y /= rightVect;
        r.z /= rightVect;

        u.y = u.y * negcosine - u.z * negsine;
        u.z = u.y * negsine + u.z * negcosine;

        up = sqrt(u.x*u.x + u.y*u.y + u.z*u.z);

        u.y /= up;
        u.z /= up;

        break;

    case '0':
        capture();
        break;

    case 'q':
        exit(0);
        break;

    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y) {
    switch(key) {
    case GLUT_KEY_DOWN:		//down arrow key
        pos.x+=2;
        break;
    case GLUT_KEY_UP:		// up arrow key
        pos.x-=2;
        break;

    case GLUT_KEY_RIGHT:
        pos.y+=2;
        break;
    case GLUT_KEY_LEFT:
        pos.y-=2;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.z+=2;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.z-=2;
        break;

    default:
        break;
    }
}


void display() {

    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();

    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    //gluLookAt(100,100,100,	0,0,0,	0,0,1);
    //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    //gluLookAt(0,0,200,	0,0,0,	0,1,0);
    gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    drawAxes();
    //drawSquare(10);

    for (int i=0; i < objects.size(); i++) {
        objects[i]->draw();
    }

    for (int i=0; i < lights.size(); i++) {
        drawPoint(lights[i]);
    }



    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate() {
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init() {
    //codes for initialization
    //clear the screen
    glClearColor(0,0,0,0);

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(fov,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance

}

void freeMemory() {
    vector<Point3>().swap(lights);
    vector<Object*>().swap(objects);
}


int main(int argc, char **argv) {

    //freopen("out.txt", "w", stdout);
    //loadTestData();
    loadActualData();

    glutInit(&argc,argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("Ray Tracer");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);

    glutMainLoop();		//The main loop of OpenGL

    freeMemory();

    return 0;
}
