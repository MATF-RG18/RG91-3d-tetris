#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>

/* Definisemo granice parametara povrsi */
#define U_FROM (-4)
#define V_FROM (-4)
#define U_TO (4)
#define V_TO (4)
#define C_FROM (0)
#define C_TO (12)


/* Deklaracije callback funkcija. */
static void on_display(void);
static void on_reshape(int width, int height);

/* Deklaracija funkcija za crtanje */

static void drawMreza();
static void drawFigura1();
static void drawFigura2();
static void drawFigura3();
static void drawFigura4();
static void drawFigura5();
static void drawFigura6();

int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    /* Kreira se prozor. */
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se funkcije za obradu dogadjaja. */
    glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	
    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	
    /* Ulazi se u glavnu petlju. */
    glutMainLoop();
    
    return 0;
}
static void on_reshape(int width, int height)
{
    /* Postavlja se viewport. */
    glViewport(0, 0, width, height);

    /* Postavljaju se parametri projekcije. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 1500);
}

static void on_display(void)
{
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Postavlja se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

    /* Crtamo delove scene */
    drawMreza();
    
    /*Nasumicno biramo figuru koju cemo iscrtati*/
    srand(time(NULL));
    int r=rand()/(RAND_MAX/6);
        
    switch(r){
        case 0:
            drawFigura1();
            break;
        case 1:
            drawFigura2();
            break;
        case 2:
            drawFigura3();
            break;
        case 3:
            drawFigura4();
            break;
        case 4:
            drawFigura5();
            break;
        case 5:
            drawFigura6();
            break;
    }
    
    
    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

static void drawMreza()
{
    /* definisemo ivicu polja u mrezi */
    float size = 0.1;
    int u,v,c;
    /* ukljucujemo iscrtavanje okvira kvadrata */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    for(v=V_FROM; v<V_TO; v++){
	/*crtamo mrezu strip po strip*/
    	glBegin(GL_QUAD_STRIP);
       	 	glColor3f(0.9, 0.9, 0.9);
		u=U_FROM;
		glVertex3f(u*size , -v*size, 0.0);
        glVertex3f(u*size , -(v+1)*size, 0.0);
		for(u=U_FROM+1; u<=U_TO; u++){
        		glVertex3f(u*size ,- v * size, 0.0);
        		glVertex3f(u*size, - (v+1) *size, 0.0);
	}

    	glEnd();
    }
    /*crtamo bocne strane mreze*/
    v=V_FROM;
    for(u=U_FROM; u < U_TO; u++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.9, 0.2, 0.7);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u*size , -v*size, c*0.2);
            glVertex3f((u+1)*size , -v*size, c*0.2);
        }
    glEnd();
    }
    /*Naspramna strana u odnosu na prethodno nacrtanu*/
    for(u=U_FROM; u < U_TO; u++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.2, 0.5, 0.7);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u*size , v*size, c*0.2);
            glVertex3f((u+1)*size , v*size, c*0.2);
        }
    glEnd();
    }
    /*treca strana*/
    u=U_FROM;
    for(v=V_FROM; v < V_TO; v++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.8, 0.8, 0.1);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(-u*size , -v*size, c*0.2);
            glVertex3f(-u*size , -(v+1)*size, c*0.2);
        }
    glEnd();
    }
    /*Naspremna prethodnoj i poslednja strana u nasoj mrezi*/
    for(v=V_FROM; v < V_TO; v++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.2, 0.6, 0.3);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u*size , -v*size, c*0.2);
            glVertex3f(u*size , -(v+1)*size, c*0.2);
        }
    glEnd();
    }

}
static void drawFigura1(){
    glColor3f(0.2,.1,.9);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glutSolidCube(1);
}
static void drawFigura2(){
    glColor3f(0.1,.6,.3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glutSolidCube(1);
    
    glTranslatef(0,1,0);
    glutSolidCube(1);
    glTranslatef(0,-1,0);
    
    glTranslatef(0,-1,0);
    glutSolidCube(1);
    glTranslatef(0,1,0);
    
    glTranslatef(1,1,0);
    glutSolidCube(1);
    glTranslatef(-1,-1,0);
}

static void drawFigura3(){
    glColor3f(0.7,.1,.3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glutSolidCube(1);
    
    glTranslatef(0,1,0);
    glutSolidCube(1);
    glTranslatef(0,-1,0);
    
    glTranslatef(0,-1,0);
    glutSolidCube(1);
    glTranslatef(0,1,0);
    
    glTranslatef(1,0,0);
    glutSolidCube(1);
    glTranslatef(-1,0,0);
}
static void drawFigura4(){
    glColor3f(0.3,.8,.4);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glutSolidCube(1);
    
    glTranslatef(-1,0,0);
    glutSolidCube(1);
    glTranslatef(1,0,0);
    
    glTranslatef(0,-1,0);
    glutSolidCube(1);
    glTranslatef(0,1,0);
    
    glTranslatef(1,-1,0);
    glutSolidCube(1);
    glTranslatef(-1,1,0);
}
static void drawFigura5(){
    glColor3f(0.5,.9,.2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glutSolidCube(1);
    
    glTranslatef(1,0,0);
    glutSolidCube(1);
    glTranslatef(-1,0,0);
    
    glTranslatef(0,-1,0);
    glutSolidCube(1);
    glTranslatef(0,1,0);
    
    glTranslatef(1,-1,0);
    glutSolidCube(1);
    glTranslatef(-1,1,0);
}
static void drawFigura6(){
    glColor3f(0.5,.1,.2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glutSolidCube(1);
    
    glTranslatef(1,0,0);
    glutSolidCube(1);
    glTranslatef(-1,0,0);
    
    glTranslatef(2,0,0);
    glutSolidCube(1);
    glTranslatef(-2,0,0);
    
    glTranslatef(-1,0,0);
    glutSolidCube(1);
    glTranslatef(1,0,0);
}
