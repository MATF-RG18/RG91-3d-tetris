#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <stdbool.h>

/* Definisemo granice parametara povrsi */
#define U_FROM (-4)
#define V_FROM (-4)
#define U_TO (4)
#define V_TO (4)
#define C_FROM (0)
#define C_TO (14)
#define MAX 30

/*Definisemo status figure ako je pala false ako nije true*/
/*bool fig_status=true;*/
/*Definisemo globalne promenljive koje koristimo za pomeranje figura na tastere strelica*/
float a=0.0,b=0.0;

/*Inicijalizacija niza za pakovanje random brojeva*/
int r[MAX];
int i=0;

/* Deklaracije callback funkcija. */
static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_arrow(int key, int x, int y);
void rotiraj(void);

/* Deklaracija funkcija za crtanje */
void figure(int r);
static void drawMreza();
static void drawFigura1();
static void drawFigura2();
static void drawFigura3();
static void drawFigura4();
static void drawFigura5();
static void drawFigura6();

/*Definisemo strukturu u kojoj cemo cuvati stanja rotacije */
struct rot_stanje {
    bool rotacije;    /* da li se rotacija desava */
    float x,y,z;      /* trenutne vrednosti rotacije*/
    int t_osa; /* trenutna osa rotacije: 0 for x, 1 for y, 2 for z */
} r_stanje;

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
    glutKeyboardFunc(on_keyboard);
    glutSpecialFunc(on_arrow);
    
    /*Inicijalizujemo stanje rotacije*/
    r_stanje.rotacije = false;
    r_stanje.x = r_stanje.y = r_stanje.z = 0.0f;
    r_stanje.t_osa = -1;
    
    /*Inicijalizujemo niz random brojeva koji cemo koristiti za crtanja figure*/
    srand(time(NULL));
    for(i=0; i<MAX; i++){
        r[i]=rand()/(RAND_MAX/6);
    }
    
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
    gluLookAt(0, 0, 4.5, 0, 0, 0, 0, 1, 0);

    /* Crtamo delove scene */
    drawMreza();
    /*Pomeramo teme kocke u koordinatni pocetak */
    glTranslatef(0.5,0.5,0.5);
    
    /* Primenjujemo translaciju ne klik strelice*/
    glTranslatef(a,b,0);
    
    /* Primenjujemo rotaciju  glScalef(0,0,0.2);*/
    glRotatef(r_stanje.x, 1, 0, 0);
    glRotatef(r_stanje.y, 0, 1, 0);
    glRotatef(r_stanje.z, 0, 0, 1);
    
    /*Ako smo dosli do kraja niza vracamo se na pocetak*/
    if(i == MAX){
        i=0;
    }
    /*Crtamo figuru*/
    figure(r[i]);
    
    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}
void figure(int r)
{
    /*Nasumicno biramo figuru koju cemo iscrtati*/
        
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
}

void rotiraj(void)
{
    /*Vrsimo animaciju rotiranja*/
    float increment = 90.0;
    switch (r_stanje.t_osa)
    {
    case 0:
        r_stanje.x += increment;
        r_stanje.rotacije = false;
        break;
    case 1:
        r_stanje.y += increment;
        r_stanje.rotacije = false;
        break;
    case 2:
        r_stanje.z += increment;
        r_stanje.rotacije = false;
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

static void on_keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 27:
        /* Zavrsava se program. */
        exit(0);
        break;        
    case 'a':
    case 'A':
        /* Rotacija po x osi */
        r_stanje.t_osa = 0;
        r_stanje.rotacije = true;
        rotiraj();
        break;

    case 's':
    case 'S':
        /* Rotacija po y osi */ 
        r_stanje.t_osa = 1;
        r_stanje.rotacije = true;
        rotiraj();
        break;
    
    case 'd':
    case 'D':
        /* Rotacija po z osi */
        r_stanje.t_osa = 2;
        r_stanje.rotacije= true;
        rotiraj();
        break;
  }
}

static void on_arrow(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
            b++;
            break;
        case GLUT_KEY_DOWN:
            b--;
            break;
        case GLUT_KEY_RIGHT:
            a++;
            break;
        case GLUT_KEY_LEFT:
            a--;
            break;
  }
  glutPostRedisplay();
}

static void drawMreza()
{
    /* definisemo ivicu polja u mrezi */
    float size = 0.1;
    /* smanjujemo dimenzije po z osi*/
    float resize = 0.2;
    int u,v,c;
    /* ukljucujemo iscrtavanje okvira kvadrata */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    c=C_FROM;
    for(v=V_FROM; v<V_TO; v++){
	/*crtamo mrezu strip po strip*/
    	glBegin(GL_QUAD_STRIP);
       	 	glColor3f(0.9, 0.9, 0.9);
		u=U_FROM;
		glVertex3f(u*size , -v*size, c*resize);
        glVertex3f(u*size , -(v+1)*size, c*resize);
		for(u=U_FROM+1; u<=U_TO; u++){
        		glVertex3f(u*size ,- v * size, c*resize);
        		glVertex3f(u*size, - (v+1) *size, c*resize);
	}

    	glEnd();
    }
    /*crtamo bocne strane mreze*/
    v=V_FROM;
    for(u=U_FROM; u < U_TO; u++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.9, 0.2, 0.7);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u*size , -v*size, c*resize);
            glVertex3f((u+1)*size , -v*size, c*resize);
        }
    glEnd();
    }
    /*Naspramna strana u odnosu na prethodno nacrtanu*/
    for(u=U_FROM; u < U_TO; u++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.2, 0.5, 0.7);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u*size , v*size, c*resize);
            glVertex3f((u+1)*size , v*size, c*resize);
        }
    glEnd();
    }
    /*treca strana*/
    u=U_FROM;
    for(v=V_FROM; v < V_TO; v++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.8, 0.8, 0.1);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(-u*size , -v*size, c*resize);
            glVertex3f(-u*size , -(v+1)*size, c*resize);
        }
    glEnd();
    }
    /*Naspremna prethodnoj i poslednja strana u nasoj mrezi*/
    for(v=V_FROM; v < V_TO; v++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.2, 0.6, 0.3);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u*size , -v*size, c*resize);
            glVertex3f(u*size , -(v+1)*size, c*resize);
        }
    glEnd();
    }

}
static void drawFigura1()
{
    glColor3f(0.2,.1,.9);
    glPushMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidCube(1);
    glPopMatrix();

}

static void drawFigura2()
{
    glColor3f(0.8,.6,.8);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glPushMatrix();
        glutSolidCube(1);
        
        glPushMatrix();
            glTranslatef(1,0,0);
            glutSolidCube(1);
        glPopMatrix();
    
        glPushMatrix();
            glTranslatef(-1,0,0);
            glutSolidCube(1);
        glPopMatrix();
    
        glTranslatef(-1,1,0);
        glutSolidCube(1);
    
    glPopMatrix();
}

static void drawFigura3()
{
    glColor3f(0.7,.1,.3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glPushMatrix();
        glutSolidCube(1);
    
        glTranslatef(0,1,0);
        glutSolidCube(1);
        glTranslatef(0,-1,0);
    
        glTranslatef(0,-1,0);
        glutSolidCube(1);
        glTranslatef(0,1,0);
    
        glTranslatef(1,0,0);
        glutSolidCube(1);
    glPopMatrix();
}

static void drawFigura4()
{
    glColor3f(0.3,.8,.4);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glPushMatrix();
        glutSolidCube(1);
    
        glTranslatef(-1,0,0);
        glutSolidCube(1);
        glTranslatef(1,0,0);
    
        glTranslatef(0,-1,0);
        glutSolidCube(1);
        glTranslatef(0,1,0);
    
        glTranslatef(1,-1,0);
        glutSolidCube(1);
    glPopMatrix();
}

static void drawFigura5()
{
    glColor3f(0.5,.9,.2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPushMatrix();
        glutSolidCube(1);
    
        glTranslatef(1,0,0);
        glutSolidCube(1);
        glTranslatef(-1,0,0);
    
        glTranslatef(0,-1,0);
        glutSolidCube(1);
        glTranslatef(0,1,0);
    
        glTranslatef(1,-1,0);
        glutSolidCube(1);
    glPopMatrix();
}

static void drawFigura6()
{
    glColor3f(0.5,.1,.2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glPushMatrix();
        glutSolidCube(1);
    
        glTranslatef(1,0,0);
        glutSolidCube(1);
        glTranslatef(-1,0,0);
    
        glTranslatef(2,0,0);
        glutSolidCube(1);
        glTranslatef(-2,0,0);
    
        glTranslatef(-1,0,0);
        glutSolidCube(1);
    glPopMatrix();
}
