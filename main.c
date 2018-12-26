#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

#define TIMER_INTERVAL 20
#define TIMER_ID 1

/* Definisemo granice parametara povrsi */
#define U_FROM (-4)
#define V_FROM (-4)
#define U_TO (4)
#define V_TO (4)
#define C_FROM (0)
#define C_TO (10)
#define MAX 30
#define Z_MAX 11
#define XY_MAX 8

/*Definisemo status figure ako je pala false ako nije true*/
/*bool fig_status=true;*/
/*Definisemo globalne promenljive koje koristimo za pomeranje figura na tastere strelica*/
float a=0.5,b=0.5;
/*Ovda pratimo pomeranje po z osi*/
float q=10.5;
float q1=0.0;
/*Promenljive koje pamte koordinate misa*/
int mouse_x = 0;
int mouse_y = 0;

/* Dimenzije prozora */
static int window_width, window_height;

/* Kumulativana matrica rotacije. */
static float matrix[16];

/*Inicijalizacija niza za pakovanje random brojeva*/
int r[MAX];
int i=0;

/*Promenljiva kojom detektujemo da je figura spustna i iscrtamo je */
int drop = 0;

/* Deklaracije callback funkcija. */
static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_timer(int id);
static void on_arrow(int key, int x, int y);
static void on_mouse(int  button, int state, int x, int y);
static void on_motion(int x, int y);
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

/*Funkcije za alokaciju i dealokaiju matrice stanja*/
int ***mat;
int ***alloc_mat(int zlen, int ylen, int xlen);
void free_mat(int ***mat, int zlen, int ylen);

/*Pamtimo spustene figure i crtamo ih*/
void drawMatricaStanja(void);
void azurirajMatricaStanja(int p);

/*parametar za proveru da li je animacija pokrenuta*/
int animation_ongoing;

/*Vreme proteklo od pocetka animacije*/
float time_passed;

/*Definisemo strukturu u kojoj cemo cuvati stanja rotacije */
struct rot_stanje {
    bool rotacije;    /* da li se rotacija desava */
    float x,y,z;      /* trenutne vrednosti rotacije*/
    int t_osa; /* trenutna osa rotacije: 0 for x, 1 for y, 2 for z */
} r_stanje;

/*Definisemo minimume i maksimume da bismo napravili ogranicenja u okviru mreze*/
struct limits{
        int x_min,x_max;
        int y_min,y_max;
        int z_min,z_max;
}lim;

int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    /* Kreira se prozor. */
    glutInitWindowSize(700, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se funkcije za obradu dogadjaja. */
    glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    glutSpecialFunc(on_arrow);
    glutMouseFunc(on_mouse);
    glutMotionFunc(on_motion);
    
    /*Inicijalizijem globalne promenljive za pamcenje koordinata misa*/
    mouse_x = 0;
    mouse_y = 0;
    
    /* Inicijalizujemo matricu rotacije. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    
    /* Alociramo memoriju za matricu stanja mreze u kojoj pamtimo
     * prethodno spustene figure,i na osnovu toga vrsimo 
     * detekciju sudara sa trenutnom figurom */
    mat=alloc_mat(Z_MAX,XY_MAX,XY_MAX);
    if(mat == NULL)
        exit(EXIT_FAILURE);
    
    /*Inicijalizujemo stanje rotacije*/
    r_stanje.rotacije = false;
    r_stanje.x = r_stanje.y = r_stanje.z = 0.0f;
    r_stanje.t_osa = -1;
    
    /*Inicijalizujemo niz random brojeva koji cemo koristiti za crtanja figure*/
    srand(time(NULL));
    for(i=0; i<MAX; i++){
        r[i]=rand()/(RAND_MAX/6);
    }
    
	int u,v,c;
    /*Inicijalizujemo matricu stanja*/
	for(c = 0; c < Z_MAX; c++){
        for(v = 0; v < XY_MAX; v++){
            for(u = 0; u < XY_MAX; u++){
                mat[c][v][u]=0;
            }
        }
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
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
    
    /* Postavlja se viewport. */
    glViewport(0, 0, width, height);

    /* Postavljaju se parametri projekcije. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 1500);
}
static void koordinatni(void)
{
    /*Crtamo koordinatni sistem radi orijentacije 
     *prilikom izrade projekta*/
    glBegin(GL_LINES);
        glColor3f(1,0,0);
        glVertex3f(-10,0,0);
        glVertex3f(10,0,0);
        
        glColor3f(0,1,0);
        glVertex3f(0,-10,0);
        glVertex3f(0,10,0);
        
        glColor3f(0,0,1);
        glVertex3f(0,0,-10);
        glVertex3f(0,0,10);
    glEnd();
}
static void on_display(void)
{
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Postavlja se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 18, 0, 0, 0, 0, 1, 0);
    
    /* Primenjuje se matrica rotacije. */
    glMultMatrixf(matrix);
    
    koordinatni();
    
    /*Ako smo dosli do kraja RANDOM niza  vracamo se na pocetak*/
    if(i == MAX)
        i=0;
    
    /* Crtamo delove scene */
    drawMreza();

 
    /*Crtamo umanjeno sledecu figuru i smestamo je u gornji desni ugao pored mreze*/
    glPushMatrix();
        glTranslatef(4.8,2,10.5);
        glScalef(0.5,0.5,0.5);
        figure(r[i+1]);
    glPopMatrix();
    
    glPushMatrix();
        /* Pomeramo teme kocke u koordinatni pocetak i primenjujemo translaciju ne klik strelice*/
        glTranslatef(a,b,q+q1);
    
        /* Primenjujemo rotaciju*/
        glRotatef(r_stanje.x, 1, 0, 0);
        glRotatef(r_stanje.y, 0, 1, 0);
        glRotatef(r_stanje.z, 0, 0, 1);
        
        /*Crtamo koordinatni sistem figure radi orijentacije
        * prilikom izrade projekta*/
        koordinatni();
        
        /*Crtamo matricu stanja
        drawMatricaStanja();*/
        
        /*Crtamo figuru*/
        figure(r[i]);
    glPopMatrix();
    
    /*Zaustavljanje*/
    int pom=drop;
    if(q <= 0.5){
        animation_ongoing = 0;
        time_passed = 0;
        drop++;
        if(drop > pom){
            pom = drop;
            azurirajMatricaStanja(r[i]);
            a=0.5;
            b=0.5;
            q=10.5;
            q1=0;
            i++;
            animation_ongoing=1;
        }
     }
     
     
    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

void figure(int p)
{
    /*Nasumicno biramo figuru koju cemo iscrtati*/
        
    switch(p){
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
        free_mat(mat, Z_MAX, XY_MAX);
        exit(0);
        break;
    case ' ':
        /*Pokrecemo animaciju.*/
        if(!animation_ongoing){
            animation_ongoing = 1;
            glutTimerFunc(TIMER_INTERVAL,on_timer, TIMER_ID);
        }
        break;
    case 'p':
    case 'P':
        /* Zaustavlja se animacija. */
        animation_ongoing = 0;
        break;        
    case 'a':
    case 'A':
        /* Rotacija po x osi */
        if(animation_ongoing){
            r_stanje.t_osa = 0;
            r_stanje.rotacije = true;
            rotiraj();
        }
        break;

    case 's':
    case 'S':
        /* Rotacija po y osi */ 
        if(animation_ongoing){
            r_stanje.t_osa = 1;
            r_stanje.rotacije = true;
            rotiraj();
        }
        break;
    
    case 'd':
    case 'D':
        /* Rotacija po z osi */
        if(animation_ongoing){
            r_stanje.t_osa = 2;
            r_stanje.rotacije= true;
            rotiraj();
        }
        break;
  }
}

static void on_timer(int id)
{
    /*Povecavamo proteklo vreme.*/
    time_passed +=0.005;
    
    
    glutPostRedisplay();
    
    if(animation_ongoing){
        q = q-0.04;
        glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
    }
}

static void on_arrow(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
            if(lim.y_max+b < V_TO && animation_ongoing)
                b++;
            break;
        case GLUT_KEY_DOWN:
            if(lim.y_min+b > V_FROM && animation_ongoing)
                b--;
            break;
        case GLUT_KEY_RIGHT:
            if(lim.x_max+a < U_TO && animation_ongoing)
                a++;
            break;
        case GLUT_KEY_LEFT:
            if(lim.x_min+a > U_FROM && animation_ongoing)
                a--;
            break;
  }
  glutPostRedisplay();
}

static void on_mouse(int buttun, int state, int x, int y)
{
    /* Pamti se pozicija pokazivaca misa. */
    mouse_x = x;
    mouse_y = y;
}

static void on_motion(int x, int y)
{
    /* Promene pozicije pokazivaca misa. */
    int delta_x, delta_y;

    /* Izracunavaju se promene pozicije pokazivaca misa. */
    delta_x = x - mouse_x;
    delta_y = y - mouse_y;

    /* Pamti se nova pozicija pokazivaca misa. */
    mouse_x = x;
    mouse_y = y;

    /* Izracunava se nova matrica rotacije. */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glLoadIdentity();
        /*Vrsimo rotiranje samo po y osi*/
        glRotatef(45 * (float) delta_x / window_width, 0, 1, 0);
        glRotatef(45 * (float) delta_y / window_height, 0, 1, 0);
        glMultMatrixf(matrix);

        glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    glPopMatrix();

    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

}

static void drawMreza()
{
    /*mreza se nalazi na saponu od -4 do 4 po x osi (oznaka u) i y osi (oznaka v) i na rasponu od -6 do 4 po z osi (oznaka c)*/
    int u,v,c;
    /* ukljucujemo iscrtavanje okvira kvadrata */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    c=C_FROM;
    for(v=V_FROM; v<V_TO; v++){
	/*crtamo mrezu strip po strip*/
    	glBegin(GL_QUAD_STRIP);
       	 	glColor3f(0.9, 0.9, 0.9);
		u=U_FROM;
		glVertex3f(u , -v, c);
        glVertex3f(u , -(v+1), c);
		for(u=U_FROM+1; u<=U_TO; u++){
        		glVertex3f(u , - v , c);
        		glVertex3f(u , -(v+1) , c);
        }

    	glEnd();
    }
    /*crtamo bocne strane mreze*/
    v=V_FROM;
    for(u=U_FROM; u < U_TO; u++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.9, 0.2, 0.7);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u , -v, c);
            glVertex3f((u+1) , -v, c);
        }
    glEnd();
    }
    /*Naspramna strana u odnosu na prethodno nacrtanu*/
    for(u=U_FROM; u < U_TO; u++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.2, 0.5, 0.7);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u , v, c);
            glVertex3f((u+1) , v, c);
        }
    glEnd();
    }
    /*treca strana*/
    u=U_FROM;
    for(v=V_FROM; v < V_TO; v++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.8, 0.8, 0.1);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(-u , -v , c);
            glVertex3f(-u , -(v+1) , c);
        }
    glEnd();
    }
    /*Naspremna prethodnoj i poslednja strana u nasoj mrezi*/
    for(v=V_FROM; v < V_TO; v++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.2, 0.6, 0.3);
        for(c=C_FROM; c <= C_TO; c++){
            glVertex3f(u , -v , c);
            glVertex3f(u , -(v+1) , c);
        }
    glEnd();
    }

}

static void drawFigura1()
{
    /*Inicijalizujemo min i max za svaku osu figure 1*/
    lim.x_min=0;
    lim.x_max=1;
    lim.y_min=0;
    lim.y_max=1;
    lim.z_min=0;
    lim.z_max=1;
    
    glColor3f(0.2,.1,.9);
    glPushMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidCube(1);
    glPopMatrix();

}

static void drawFigura2()
{
    /*Inicijalizujemo min i max za svaku osu figure 2*/
    lim.x_min=-1;
    lim.x_max=2;
    lim.y_min=0;
    lim.y_max=2;
    lim.z_min=0;
    lim.z_max=1;

    glColor3f(0.9,0.9,0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glPushMatrix();
        glPushMatrix();
            glutSolidCube(1);
        glPopMatrix();
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
    /*Inicijalizujemo min i max za svaku osu figure 3*/
    lim.x_min=0;
    lim.x_max=2;
    lim.y_min=-1;
    lim.y_max=2;
    lim.z_min=0;
    lim.z_max=1;
    
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
    /*Inicijalizujemo min i max za svaku osu figure 4*/
    lim.x_min=-1;
    lim.x_max=2;
    lim.y_min=-1;
    lim.y_max=1;
    lim.z_min=0;
    lim.z_max=1;
    
    glColor3f(0.0,.8,.9);
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
    /*Inicijalizujemo min i max za svaku osu figure 5*/
    lim.x_min=0;
    lim.x_max=2;
    lim.y_min=-1;
    lim.y_max=1;
    lim.z_min=0;
    lim.z_max=1;
    
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
    /*Inicijalizujemo min i max za svaku osu figure 6*/
    lim.x_min=0;
    lim.x_max=1;
    lim.y_min=-1;
    lim.y_max=3;
    lim.z_min=0;
    lim.z_max=1;
    
    glColor3f(1.0,0.6,0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glPushMatrix();
        glutSolidCube(1);
    
        glTranslatef(0,1,0);
        glutSolidCube(1);
        glTranslatef(0,-1,0);
    
        glTranslatef(0,2,0);
        glutSolidCube(1);
        glTranslatef(0,-2,0);
    
        glTranslatef(0,-1,0);
        glutSolidCube(1);
    glPopMatrix();
}

void drawMatricaStanja(void)
{
 int u,v,c;
    /*Crtamomo matricu stanja*/
	for(c = 0; c < Z_MAX; c++){
        for(v = 0; v < XY_MAX; v++){
            for(u = 0; u < XY_MAX; u++){
                if(mat[c][v][u]==1){
                    glColor3f(0.2,0.4,0.2);
                    glTranslatef(u-4,v-4,c-4);
                    glutSolidCube(1);
                }
            }
        }
    }   
}

void azurirajMatricaStanja(int p)
{
int c,b1,a1;
c=q+q1;
b1=4+b;
a1=4+a;
mat[c][b1][a1]=1;

switch(p){
        case 1:
            mat[c][b1][a1+1]=1;
            mat[c][b1][a1-1]=1;
            mat[c][b1+1][a1-1]=1;
            break;
        case 2:
            mat[c][b1+1][a1]=1;
            mat[c][b1][a1+1]=1;
            mat[c][b1-1][a1]=1;
            break;
        case 3:
            mat[c][b1][a1-1]=1;
            mat[c][b1-1][a1]=1;
            mat[c][b1-1][a1+1]=1;
            break;
        case 4:
            mat[c][b1][a1+1]=1;
            mat[c][b1-1][a1]=1;
            mat[c][b1-1][a1+1]=1;
            break;
        case 5:
            mat[c][b1+2][a1]=1;
            mat[c][b1+1][a1]=1;
            mat[c][b1-1][a1]=1;
            break;
    }
}

int ***alloc_mat(int zlen, int ylen, int xlen)
{
    
    int u, v;

    if ((mat = malloc(zlen * sizeof(*mat))) == NULL) {
        perror("malloc 1");
        return NULL;
    }

    for (u=0; u < zlen; ++u)
        mat[u] = NULL;

    for (u=0; u < zlen; ++u)
        if ((mat[u] = malloc(ylen * sizeof(*mat[u]))) == NULL) {
            perror("malloc 2");
            free_mat(mat, zlen, ylen);
            return NULL;
        }

    for (u=0; u < zlen; ++u)
        for (v=0; v < ylen; ++v)
            mat[u][v] = NULL;

    for (u=0; u < zlen; ++u)
        for (v=0; v < ylen; ++v)
            if ((mat[u][v] = malloc(xlen * sizeof (*mat[u][v]))) == NULL) {
                perror("malloc 3");
                free_mat(mat, zlen, ylen);
                return NULL;
            }

    return mat;
}
void free_mat(int ***mat, int zlen, int ylen)
{
    int u, v;

    for (u=0; u < zlen; ++u) {
        if (mat[u] != NULL) {
            for (v=0; v < ylen; ++v)
                free(mat[u][v]);
            free(mat[u]);
        }
    }
    free(mat);
}
