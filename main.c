#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>


#define TIMER_INTERVAL 20
#define TIMER_ID 1
/* Definisemo granice parametara povrsi */
#define X_FROM (-4)
#define Y_FROM (-4)
#define X_TO (4)
#define Y_TO (4)
#define Z_FROM (0)
#define Z_TO (10)
/*Definisemo maksimalnu duinu niza random brojeva*/
#define MAX 30
/*Definisemo velicinu matrice*/
#define Z_MAX 12
#define XY_MAX 8
/*Definisemo oznaku za osu po kojoj trenutno vrsimo rotaciju*/
#define X_OSA 0
#define Y_OSA 1
#define Z_OSA 2
/*Definisemo ozneke za figure*/
#define FIGURA_DOT 0
#define FIGURA_EL 1
#define FIGURA_TRIANGLE 2
#define FIGURA_ZE 3
#define FIGURA_SQUERE 4
#define FIGURA_LINE 5

/*Definisemo globalne promenljive koje koristimo za pomeranje figura na tastere strelica,
 inicijalno ih postavljamo na 0.5 jer se kocka crta u koordinatnom pocetku a nama treba da bude u polju */
int x_pomeraj=0,y_pomeraj=0;
/*Ovda pratimo pomeranje po z osi inicijalno postavljamo da se pocetna figura nalazi na z=11*/
int z_pomeraj=10;

/*Potrebna nam je pomocna promenljiva koja ce da kontrolise da za svaku novu figuru samo
 *prvi put postavi granice i posle ih azurira na adekvatan nacin*/
int granice=0;

/*Promenljive koje pamte koordinate misa*/
int mouse_x = 0;
int mouse_y = 0;

/* Dimenzije prozora */
static int window_width, window_height;

/* Kumulativana matrica rotacije. */
static float matrix[16];

/*Inicijalizacija niza za pakovanje random brojeva*/
int randNiz[MAX];
int rand_brojac=0;

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

static void inicijalizacija(void);
static void init_lights();
static void set_material(int id);

void zaustavljanjeFigure(void);
void vratiNaInit(void);
void rotiraj(void);

/* Deklaracija funkcija za crtanje */
void crtanjeDelovaScene(void);
void figure(int r);
static void drawMreza();
static void drawFiguraDot();
static void drawFiguraEl();
static void drawFiguraTriangle();
static void drawFiguraZe();
static void drawFiguraSquere();
static void drawFiguraLine();

/*Na osnovu ovih granicnih funkcija odredjujemo koliko figuru mozemo pomerati levo,
 * desno, gore ili dole, a da ona ostane u mrezi bez i nakon rotacije*/
void graniceFigure(int oznaka_figure);
void azurirajGranice(int oznaka_figure);
void azurirajEl(void);
void azurirajTriangle(void);
void azurirajZe(void);
void azurirajSquere(void);
void azurirajLine(void);
/*Proveravamoo da li je figura nakon rotacije ostala u okvirima mreze*/
void proveriFigIn(void);

/*Funkcije za alokaciju i dealokaiju matrice stanja*/
int ***matStanja;
int ***alloc_mat(int zlen, int ylen, int xlen);
void free_mat(int ***matStanja, int zlen, int ylen);

/*Pamtimo spustene figure i crtamo ih*/
void drawMatricaStanja(void);
void azurirajMatricaStanja(int oznaka_figure);
void pamtimoEl(int a, int b);
void pamtimoTriangle(int a, int b);
void pamtimoZe(int a, int b);
void pamtimoSquere(int a, int b);
void pamtimoLine(int a, int b);

void najniziDeoFigure(int oznaka_figure);

/*parametar za proveru da li je animacija pokrenuta*/
int animation_ongoing;

/*Vreme proteklo od pocetka animacije*/
int time_passed;

/*Brojimo rotacije u funkciji rotiraj(), vrednosti: 0,1,2,3*/
int rot_brojac=0;

/*Definisemo strukturu u kojoj cemo cuvati stanja rotacije */
struct rot_stanje {
    bool rotacije;    /* da li se rotacija desava */
    float x,y,z;      /* trenutne vrednosti rotacije*/
    int t_osa; /* trenutna osa rotacije: 0 for x, 1 for y, 2 for z */
} r_stanje;

/*Definisemo maksimalni pomeraj u odnosu na pocetni polozaj figure da bismo napravili ogranicenja u okviru mreze*/
struct limits{
        int levo;
        int desno;
        int gore;
        int dole;
}lim;

/*Pratimo najnizu kocku figure*/
struct lowest{
    int x,y,z;
}low[4];

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
    
    /*Vrsimo potrebne inicijalizacije*/
    inicijalizacija();
    
    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0.1, 0.1, 0.1, 0);
	glEnable(GL_DEPTH_TEST);
    
    /*Ukljucujemo normalizaciju*/
    glEnable(GL_NORMALIZE);
	
    /* Ulazi se u glavnu petlju. */
    glutMainLoop();

    return 0;
}
static void inicijalizacija(void)
{
    /*Inicijalizijem promenljive za pamcenje koordinata misa*/
    mouse_x = 0;
    mouse_y = 0;
    
    /* Inicijalizujemo matricu rotacije. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    
    /*Inicijalizujemo stanje rotacije*/
    r_stanje.rotacije = false;
    r_stanje.x = r_stanje.y = r_stanje.z = 0.0f;
    r_stanje.t_osa = -1;
    
    /*Inicijalizujemo sve najnize tacke figure,kojih ukupno moze da ima 4, na 0*/
    int i;
    for(i=0;i<4;i++){
        low[i].x=0;
        low[i].y=0;
        low[i].z=0;
    }
    
    /*Inicijalizujemo niz random brojeva koji cemo koristiti za crtanja figure*/
    srand(time(NULL));
    for(rand_brojac=0; rand_brojac<MAX; rand_brojac++){
        randNiz[rand_brojac]=rand()/(RAND_MAX/6);
    }
        
    /* Alociramo memoriju za matricu stanja mreze u kojoj pamtimo
     * prethodno spustene figure,i na osnovu toga vrsimo 
     * detekciju sudara sa trenutnom figurom */
    matStanja=alloc_mat(Z_MAX,XY_MAX,XY_MAX);
    if(matStanja == NULL)
        exit(EXIT_FAILURE);
    
    /*Inicijalizujem matricu stanja na nule*/
    int u,v,c;
 
	for(c = 0; c < Z_MAX; c++)
        for(v = 0; v < XY_MAX; v++)
            for(u = 0; u < XY_MAX; u++)
                matStanja[c][v][u]=0;
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

static void on_display(void)
{
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Postavlja se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 18, 0, 0, 0, 0, 1, 0);
    
    /*Obavljamo normalizaciju vektora normale*/
    glNormal3f(0,0,1);
    
    /* Primenjuje se matrica rotacije. */
    glMultMatrixf(matrix);
    
    /*Podesavamo osvetljenje i materijale*/
    init_lights();
    set_material(16);
    
    /*Ako smo dosli do kraja RANDOM niza  vracamo se na pocetak*/
    if(rand_brojac == MAX)
        rand_brojac=0;

    /* Crtanje delova scene izdvojeno u funkciji */
    crtanjeDelovaScene();
    
    /*Zaustavljanje figure izdvojeno u funkciji*/
    zaustavljanjeFigure();
     
    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

void crtanjeDelovaScene(void)
{
    /*Crtamo mrezu 3D tetrisa*/
    drawMreza();
    
    /*Crtamo matricu stanja iliti popunjenost osnove*/
    drawMatricaStanja();
    
    glPushMatrix();
        /*Podesavamo materijale za svaku figuru zasebno*/
        set_material(randNiz[rand_brojac]);
        /* Pomeramo teme kocke u koordinatni pocetak i */
        glTranslatef(0.5,0.5,0.5);
        /*Primenjujemo translaciju ne klik strelice, 
         *a oznaka z_pomeraj pomaze da pratimo pad figure */
        glTranslatef(x_pomeraj,y_pomeraj,z_pomeraj);
        
        /* Primenjujemo rotaciju*/
        glRotatef(r_stanje.x, 1, 0, 0);
        glRotatef(r_stanje.y, 0, 1, 0);
        glRotatef(r_stanje.z, 0, 0, 1);
        
        /*Crtamo figuru*/
        figure(randNiz[rand_brojac]);
    glPopMatrix();
    
        /*Crtamo umanjeno sledecu figuru i smestamo je u gornji desni ugao pored mreze*/
    glPushMatrix();
        /*Podesavamo materijale sledece figure*/
        set_material(randNiz[rand_brojac+1]);
        glTranslatef(4.8,2,10.5);
        glScalef(0.5,0.5,0.5);
        figure(randNiz[rand_brojac+1]);
    glPopMatrix(); 
}

void zaustavljanjeFigure(void)
{
/*Zaustavljanje
    int pom=drop;*/
    
/*Provera koalizije*/
int u,v,c;
int i;
 
for(c = Z_MAX-1; c >= 0; c--){
    for(v = 0; v < XY_MAX; v++){
        for(u = 0; u < XY_MAX; u++){
            for(i=0;i<4;i++){
                if((matStanja[c][v][u]==1 && c==(z_pomeraj+low[i].z-1) && v==(low[i].y+y_pomeraj) && u==(low[i].x+x_pomeraj)) || z_pomeraj <= 0){
                    animation_ongoing = 0;
                    time_passed = 0;
                    drop++;                
                    azurirajMatricaStanja(randNiz[rand_brojac]);
                    vratiNaInit();
                    rand_brojac++;
                    animation_ongoing=1;
                }
            }
        }
    }
}

}

void vratiNaInit(void)
{
/*Ponistavamo pomeranja vrsena nad prethodnom figurom*/
x_pomeraj=0;
y_pomeraj=0;
z_pomeraj=10;
/*Ponistavamo rotacije vrsene nad prethodnom figurom*/
r_stanje.x=0;
r_stanje.y=0;
r_stanje.z=0;  

/*Ponistavamo koordinate najnizih kocki prethodne figure*/
int i;
for(i=0;i<4;i++){
    low[i].x=0;
    low[i].y=0;
    low[i].z=0;
}

granice=0;
}

void figure(int oznaka_figure)
{    
    /*Nasumicno biramo figuru koju cemo iscrtati*/
    switch(oznaka_figure){
        case FIGURA_DOT:
            drawFiguraDot();
            break;
        case FIGURA_EL:
            drawFiguraEl();
            break;
        case FIGURA_TRIANGLE:
            drawFiguraTriangle();
            break;
        case FIGURA_ZE:
            drawFiguraZe();
            break;
        case FIGURA_SQUERE:
            drawFiguraSquere();
            break;
        case FIGURA_LINE:            
            drawFiguraLine();
            break;
    }
}

void rotiraj(void)
{
/*Vrsimo animaciju rotiranja, a ugao za koji rotiramo je 90*/
float increment = 90.0;

switch (r_stanje.t_osa){
    case X_OSA:
        r_stanje.x += increment;
        r_stanje.rotacije = false;
        rot_brojac++;
        azurirajGranice(randNiz[rand_brojac]);
        break;
    case Y_OSA:
        r_stanje.y += increment;
        r_stanje.rotacije = false;
        rot_brojac++;
        azurirajGranice(randNiz[rand_brojac]);
        break;
    case Z_OSA:
        r_stanje.z += increment;
        r_stanje.rotacije = false;
        rot_brojac++;
        azurirajGranice(randNiz[rand_brojac]);
        break;
}
/*Vracamo brojac na nula jer smo vratili figuru u pocetni polozaj
*i pravimo mogucnost da ako nismo nasli odgovarajuci polozaj
* pokusamo rotaciju po nekoj drugoj osi*/
if(rot_brojac == 4)
    rot_brojac=0;

/*Ako je figura pre rotacije bila na granici moze se desiti da posle rotacije izadje van okvira mreze, vrsimo proveru pa ako jeste vracamo je u mrezu*/
proveriFigIn();

/*Forsiramo ponovno iscrtavanje na ekranu*/
glutPostRedisplay();
}

void proveriFigIn(void)
{
/*Proveravamo da li je figura ostala u mrezi, ako nije vracamo je*/
if(lim.gore < y_pomeraj)
    y_pomeraj-=y_pomeraj-lim.gore;
else if(-lim.dole > y_pomeraj)
    y_pomeraj+=-(y_pomeraj-lim.dole);
else if(lim.desno < x_pomeraj)
    x_pomeraj-=x_pomeraj-lim.desno;
else if(-lim.levo > x_pomeraj)
    x_pomeraj+=-(x_pomeraj-lim.levo);
}

static void on_keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 27:
        /* Zavrsava se program. */
        printf("\n****Palo je %d figura!****\n\n",drop);
        free_mat(matStanja, Z_MAX, XY_MAX);
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
        if(animation_ongoing && (rot_brojac==0 || r_stanje.t_osa ==0)){
            r_stanje.t_osa = 0;
            r_stanje.rotacije = true;
            rotiraj();
        }
        break;

    case 's':
    case 'S':
        /* Rotacija po y osi */ 
        if(animation_ongoing && (rot_brojac==0 || r_stanje.t_osa ==1)){
            r_stanje.t_osa = 1;
            r_stanje.rotacije = true;
            rotiraj();
        }
        break;
    
    case 'd':
    case 'D':
        /* Rotacija po z osi */
        if(animation_ongoing && (rot_brojac==0 || r_stanje.t_osa ==2)){
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
    time_passed +=1;
    
    
    glutPostRedisplay();
    
    if(animation_ongoing){
        if(time_passed % 30 == 0)
            z_pomeraj = z_pomeraj-1;
        glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
    }
}

static void on_arrow(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
            /*Na komandu strelice nagore povecavamo y_pomeraj i time pomeramo figuru nagore 
             *ukoliko granica nije prekoracena i ukoliko je animacija pokrenuta*/
            if(lim.gore > y_pomeraj && animation_ongoing)
                y_pomeraj++;
            break;
        case GLUT_KEY_DOWN:
            /*Na komandu strelice nadole umanjujemo y_pomeraj i time pomeramo figuru nadole 
             *ukoliko granica nije prekoracena i ukoliko je animacija pokrenuta*/
            if(-lim.dole < y_pomeraj && animation_ongoing)
                y_pomeraj--;
            break;
        case GLUT_KEY_RIGHT:
            /*Na komandu strelice nadesno povecavamo x_pomeraj i time pomeramo figuru nadesno 
             *ukoliko granica nije prekoracena i ukoliko je animacija pokrenuta*/
            if(lim.desno > x_pomeraj && animation_ongoing)
                x_pomeraj++;
            break;
        case GLUT_KEY_LEFT:
            /*Na komandu strelice nalevo umanjujemo x_pomeraj i time pomeramo figuru nalevo 
             *ukoliko granica nije prekoracena i ukoliko je animacija pokrenuta*/
            if(-lim.levo < x_pomeraj && animation_ongoing)
                x_pomeraj--;
            break;
  }
  /*Forsiramo ponovno iscrtavanje scene*/
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
    c=Z_FROM;
    for(v=Y_FROM; v<Y_TO; v++){
	/*crtamo mrezu strip po strip*/
    	glBegin(GL_QUAD_STRIP);
       	 	glColor3f(0.9, 0.9, 0.9);
		u=X_FROM;
		glVertex3f(u , -v, c);
        glVertex3f(u , -(v+1), c);
		for(u=X_FROM+1; u<=X_TO; u++){
        		glVertex3f(u , - v , c);
        		glVertex3f(u , -(v+1) , c);
        }

    	glEnd();
    }
    /*crtamo bocne strane mreze*/
    v=Y_FROM;
    for(u=X_FROM; u < X_TO; u++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.9, 0.2, 0.7);
        for(c=Z_FROM; c <= Z_TO; c++){
            glVertex3f(u , -v, c);
            glVertex3f((u+1) , -v, c);
        }
    glEnd();
    }
    /*Naspramna strana u odnosu na prethodno nacrtanu*/
    for(u=X_FROM; u < X_TO; u++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.2, 0.5, 0.7);
        for(c=Z_FROM; c <= Z_TO; c++){
            glVertex3f(u , v, c);
            glVertex3f((u+1) , v, c);
        }
    glEnd();
    }
    /*treca strana*/
    u=X_FROM;
    for(v=Y_FROM; v < Y_TO; v++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.8, 0.8, 0.1);
        for(c=Z_FROM; c <= Z_TO; c++){
            glVertex3f(-u , -v , c);
            glVertex3f(-u , -(v+1) , c);
        }
    glEnd();
    }
    /*Naspremna prethodnoj i poslednja strana u nasoj mrezi*/
    for(v=Y_FROM; v < Y_TO; v++){
      glBegin(GL_QUAD_STRIP);
        glColor3f(0.2, 0.6, 0.3);
        for(c=Z_FROM; c <= Z_TO; c++){
            glVertex3f(u , -v , c);
            glVertex3f(u , -(v+1) , c);
        }
    glEnd();
    }

}

static void drawFiguraDot()
{
    glColor3f(0.2,.1,.9);
    glPushMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutSolidCube(1);
    glPopMatrix();

    /*Kontrolismo da se samo prvi put postavi granice i posle azuriraju na adekvatan nacin*/
    if(granice == 0){
        graniceFigure(FIGURA_DOT);   
    }
}

static void drawFiguraEl()
{
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
    
    /*Kontrolismo da se samo prvi put postavi granice i posle azuriraju na adekvatan nacin*/
    if(granice == 0){
        graniceFigure(FIGURA_EL);   
    }
}

static void drawFiguraTriangle()
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
    
    /*Kontrolismo da se samo prvi put postavi granice i posle azuriraju na adekvatan nacin*/
    if(granice == 0){
        graniceFigure(FIGURA_TRIANGLE);   
    }
}

static void drawFiguraZe()
{
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
    
    /*Kontrolismo da se samo prvi put postavi granice i posle azuriraju na adekvatan nacin*/
    if(granice == 0){
        graniceFigure(FIGURA_ZE);   
    }
}

static void drawFiguraSquere()
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
    
    /*Kontrolismo da se samo prvi put postavi granice i posle azuriraju na adekvatan nacin*/
    if(granice == 0){
        graniceFigure(FIGURA_SQUERE);   
    }
}

static void drawFiguraLine()
{ 
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
    
    /*Kontrolismo da se samo prvi put postavi granice i posle azuriraju na adekvatan nacin*/
    if(granice == 0){
        graniceFigure(FIGURA_LINE);   
    }
}

void graniceFigure(int oznaka_figure)
{
/*Inicijalizujemo za svaku figuru makimalan pomeraj do granice*/
    switch(oznaka_figure){
        case FIGURA_DOT:    
            lim.levo=4;
            lim.desno=3;
            lim.gore=3;
            lim.dole=4;
            granice++;
            /*Odradjujemo polozaj u mrezi najnizih kocki figure*/
            najniziDeoFigure(oznaka_figure);
            break;
        case FIGURA_EL:
            lim.levo=3;
            lim.desno=2;
            lim.gore=2;
            lim.dole=4;
            granice++;
            /*Odradjujemo polozaj u mrezi najnizih kocki figure*/
            najniziDeoFigure(oznaka_figure);
            break;
        case FIGURA_TRIANGLE:
            lim.levo=4;
            lim.desno=2;
            lim.gore=2;
            lim.dole=3;
            granice++;
            /*Odradjujemo polozaj u mrezi najnizih kocki figure*/
            najniziDeoFigure(oznaka_figure);
            break;
        case FIGURA_ZE:
            lim.levo=3;
            lim.desno=2;
            lim.gore=3;
            lim.dole=3;
            granice++;
            /*Odradjujemo polozaj u mrezi najnizih kocki figure*/
            najniziDeoFigure(oznaka_figure);
            break;
        case FIGURA_SQUERE:
            lim.levo=4;
            lim.desno=2;
            lim.gore=3;
            lim.dole=3;
            granice++;
            /*Odradjujemo polozaj u mrezi najnizih kocki figure*/
            najniziDeoFigure(oznaka_figure);
            break;
        case FIGURA_LINE:
            lim.levo=4;
            lim.desno=3;
            lim.gore=1;
            lim.dole=3;
            granice++;
            /*Odradjujemo polozaj u mrezi najnizih kocki figure*/
            najniziDeoFigure(oznaka_figure);
            break;
    }
}

void najniziDeoFigure(int oznaka_figure)
{
/*Inicijalizujemo za svaku figuru makimalan pomeraj do granice*/
    switch(oznaka_figure){
        case FIGURA_DOT:    
            /*Definisemo najnizu tacku za detekciju sudara*/
            low[0].x=4;
            low[0].y=4;
            break;
        case FIGURA_EL:
            /*Definisemo najnizu tacku za detekciju sudara,koordinate odgovaraju polju u mrezi*/
            low[0].x=4;
            low[0].y=4;
            low[1].x=3;
            low[1].y=4;
            low[2].x=3;
            low[2].y=3;
            low[3].x=5;
            low[3].y=4;
            break;
        case FIGURA_TRIANGLE:
            low[0].x=4;
            low[0].y=4;
            low[1].x=3;
            low[1].y=4;
            low[2].x=4;
            low[2].y=3;
            low[3].x=5;
            low[3].y=4;
            break;
        case FIGURA_ZE:
            low[0].x=4;
            low[0].y=4;
            low[1].x=3;
            low[1].y=4;
            low[2].x=4;
            low[2].y=5;
            low[3].x=5;
            low[3].y=5;
            break;
        case FIGURA_SQUERE:
            low[0].x=4;
            low[0].y=4;
            low[1].x=5;
            low[1].y=4;
            low[2].x=4;
            low[2].y=5;
            low[3].x=5;
            low[3].y=5;
            break;
        case FIGURA_LINE:
            low[0].x=4;
            low[0].y=4;
            low[1].x=4;
            low[1].y=3;
            low[2].x=4;
            low[2].y=2;
            low[3].x=4;
            low[3].y=5;
            break;
    }
}

void azurirajGranice(int oznaka_figure)
{
    /*Azuriramo granicne vrednosti posle zadate rotacije
     *da nam figure ne bi izasle van okvira mreze*/
    switch(oznaka_figure){
        case FIGURA_DOT:
            /*Rotacija nema efekat na ovu figuru pa necemo razmatrati promene 
             *granicnih vrednosti*/
            break;
        case FIGURA_EL:
            azurirajEl();
            break;
        case FIGURA_TRIANGLE:
            azurirajTriangle();
            break;
        case FIGURA_ZE:
            azurirajZe();
            break;
        case FIGURA_SQUERE:
            azurirajSquere();
            break;
        case FIGURA_LINE:
            azurirajLine();
            break;
    }
}

void azurirajLine(void)
{
/*Obradjujemo promenu granice ove figure posle svake rotacije*/
switch(r_stanje.t_osa){
    case X_OSA:
        if(rot_brojac == 1){
            lim.gore +=2;
            lim.dole +=1;
        }else if(rot_brojac == 2){
            lim.gore -=1;
            lim.dole -=2;
        }else if(rot_brojac == 3){
            lim.gore +=1;
            lim.dole +=2;
        }else if(rot_brojac == 4){
            lim.levo=4;
            lim.desno=3;
            lim.gore=1;
            lim.dole=3;
        }
        break;
    case Y_OSA:
        /*Rotacija po y osi nema efekat na ovu figuru*/
        break;
    case Z_OSA:
        if(rot_brojac == 1){
            lim.levo -=2;
            lim.desno -=1;
            lim.gore +=2;
            lim.dole +=1;
        }else if(rot_brojac == 2){
            lim.levo +=2;
            lim.desno +=1;
            lim.gore -=1;
            lim.dole -=2;
        }else if(rot_brojac == 3){
            lim.levo -=1;
            lim.desno -=2;
            lim.gore +=1;
            lim.dole +=2;
        }else if(rot_brojac == 4){
            lim.levo=4;
            lim.desno=3;
            lim.gore=1;
            lim.dole=3;
        }
        break;
    }
}

void azurirajSquere(void)
{
/*Obradjujemo promenu granice ove figure posle svake rotacije*/
switch(r_stanje.t_osa){
    case X_OSA:
        if(rot_brojac == 1){
            lim.dole +=1;
        }else if(rot_brojac == 2){
            lim.gore -=1;
        }else if(rot_brojac == 3){
            lim.gore +=1;
        }else if(rot_brojac == 4){
            lim.levo=4;
            lim.desno=2;
            lim.gore=3;
            lim.dole=3;
        }
        break;
    case Y_OSA:
        if(rot_brojac == 1){
            lim.desno +=1;
        }else if(rot_brojac == 2){
            lim.levo -=1;
        }else if(rot_brojac == 3){
            lim.levo +=1;
        }else if(rot_brojac == 4){
            lim.levo=4;
            lim.desno=2;
            lim.gore=3;
            lim.dole=3;
        }
        break;
    case Z_OSA:
        if(rot_brojac == 1){
            lim.gore -=1;
            lim.dole +=1;
        }else if(rot_brojac == 2){
            lim.desno +=1;
            lim.levo -=1;
        }else if(rot_brojac == 3){
            lim.gore +=1;
            lim.dole -=1;
        }else if(rot_brojac == 4){
            lim.levo=4;
            lim.desno=2;
            lim.gore=3;
            lim.dole=3;
        }
        break;
    }
}

void azurirajZe(void)
{
/*Obradjujemo promenu granice ove figure posle svake rotacije*/
switch(r_stanje.t_osa){
        case X_OSA:
            if(rot_brojac == 1){
                lim.dole +=1;
            }else if(rot_brojac == 2){
                lim.gore -=1;
            }else if(rot_brojac == 3){
                lim.gore +=1;
            }else if(rot_brojac == 4){
                lim.levo=3;
                lim.desno=2;
                lim.gore=3;
                lim.dole=3;
            }
            break;
        case Y_OSA:
            if(rot_brojac == 1){
                lim.levo +=1;
                lim.desno +=1;
            }else if(rot_brojac == 2){
                lim.levo -=1;
                lim.desno -=1;
            }else if(rot_brojac == 3){
                lim.levo +=1;
                lim.desno +=1;
            }else if(rot_brojac == 4){
                lim.levo=3;
                lim.desno=2;
                lim.gore=3;
                lim.dole=3;
            }
            break;
        case Z_OSA:
            if(rot_brojac == 1){
                lim.gore -=1;
                lim.levo +=1;
            }else if(rot_brojac == 2){
                lim.dole +=1;
                lim.levo -=1;
            }else if(rot_brojac == 3){
                lim.desno +=1;
                lim.dole -=1;
            }else if(rot_brojac == 4){
                lim.levo=3;
                lim.desno=2;
                lim.gore=3;
                lim.dole=3;
            }
    
            break;
    }    
}

void azurirajTriangle(void)
{
/*Obradjujemo promenu granice ove figure posle svake rotacije*/
switch(r_stanje.t_osa){
        case X_OSA:
            if(rot_brojac == 1){
                lim.gore +=1;
                lim.dole +=1;
            }else if(rot_brojac == 2){
                lim.gore -=1;
                lim.dole -=1;
            }else if(rot_brojac == 3){
                lim.gore +=1;
                lim.dole +=1;
            }else if(rot_brojac == 4){
                lim.levo=4;
                lim.desno=2;
                lim.gore=2;
                lim.dole=3;
            }
            break;
        case Y_OSA:
            if(rot_brojac == 1){
                lim.desno +=1;
            }else if(rot_brojac == 2){
                lim.levo -=1;
            }else if(rot_brojac == 3){
                lim.levo +=1;
            }else if(rot_brojac == 4){
                lim.levo=4;
                lim.desno=2;
                lim.gore=2;
                lim.dole=3;
            }
            break;
        case Z_OSA:
            if(rot_brojac == 1){
                lim.dole +=1;
                lim.levo -=1;
            }else if(rot_brojac == 2){
                lim.dole -=1;
                lim.desno +=1;
            }else if(rot_brojac == 3){
                lim.desno -=1;
                lim.gore +=1;
            }else if(rot_brojac == 4){
                lim.levo=4;
                lim.desno=2;
                lim.gore=2;
                lim.dole=3;
            }
    
            break;
    }
}

void azurirajEl(void)
{
    /*Obradjujemo promenu granice ove figure posle svake rotacije*/
    switch(r_stanje.t_osa){
        case X_OSA:
            if(rot_brojac == 1){
                lim.gore +=1;
            }else if(rot_brojac == 2){
                lim.dole -=1;
            }else if(rot_brojac == 3){
                lim.dole +=1;
            }else if(rot_brojac == 4){
                lim.levo=3;
                lim.desno=2;
                lim.gore=2;
                lim.dole=4;
            }
            break;
        case Y_OSA:
            if(rot_brojac == 1){
                lim.levo +=1;
                lim.desno +=1;
            }else if(rot_brojac == 2){
                lim.levo -=1;
                lim.desno -=1;
            }else if(rot_brojac == 3){
                lim.levo +=1;
                lim.desno +=1;
            }else if(rot_brojac == 4){
                lim.levo=3;
                lim.desno=2;
                lim.gore=2;
                lim.dole=4;
            }
            break;
        case Z_OSA:
            if(rot_brojac == 1){
                lim.dole -=1;
                lim.desno +=1;
            }else if(rot_brojac == 2){
                lim.gore +=1;
                lim.desno -=1;
            }else if(rot_brojac == 3){
                lim.levo +=1;
                lim.gore -=1;
            }else if(rot_brojac == 4){
                lim.levo=3;
                lim.desno=2;
                lim.gore=2;
                lim.dole=4;
            }
            break;
    }
}

void drawMatricaStanja(void)
{
 int u,v,c;
 
 glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    /*Crtamo matricu stanja*/
	for(c = 0; c < Z_MAX; c++){
        set_material(6+c);
        for(v = 0; v < XY_MAX; v++){
            for(u = 0; u < XY_MAX; u++){
                if(matStanja[c][v][u]==1){
                    glPushMatrix();
                        glTranslatef(0.5,0.5,0.5);
                        glTranslatef(u-4,v-4,c);
                        glutSolidCube(1);
                    glPopMatrix();
                }
            }
        }
    }   
}

void azurirajMatricaStanja(int oznaka_figure)
{
int a,b,c;
c=z_pomeraj;
/*Promenljive x_pomeraj i y_pomeraj uvecavamo za 4 zato sto u nasoj mrezi X i Y
 *imaju vrednosti od -4 do 4 a u matrici se pamte i rasporedjuju od 0 do 8*/
b=4+y_pomeraj;
a=4+x_pomeraj;

/*Pamtimo glavnu kocku, a po potrebi za ostale figure nadogradjujemo */
matStanja[c][b][a]=1;

switch(oznaka_figure){
        case FIGURA_EL:
            pamtimoEl(a,b);
            break;
        case FIGURA_TRIANGLE:
            pamtimoTriangle(a,b);
            break;
        case FIGURA_ZE:
            pamtimoZe(a,b);
            break;
        case FIGURA_SQUERE:
            pamtimoSquere(a,b);
            break;
        case FIGURA_LINE:
            pamtimoLine(a,b);
            break;
    }
}

void pamtimoEl(int a, int b)
{
if(rot_brojac == 0){
    matStanja[z_pomeraj][b][a+1]=1;
    matStanja[z_pomeraj][b][a-1]=1;
    matStanja[z_pomeraj][b+1][a-1]=1;
}
}

void pamtimoTriangle(int a, int b)
{
if(rot_brojac == 0){
    matStanja[z_pomeraj][b+1][a]=1;
    matStanja[z_pomeraj][b][a+1]=1;
    matStanja[z_pomeraj][b-1][a]=1;
}    
}

void pamtimoZe(int a, int b)
{
if(rot_brojac == 0){
    matStanja[z_pomeraj][b][a-1]=1;
    matStanja[z_pomeraj][b-1][a]=1;
    matStanja[z_pomeraj][b-1][a+1]=1;
}
}

void pamtimoSquere(int a, int b)
{
if(rot_brojac == 0){
    matStanja[z_pomeraj][b][a+1]=1;
    matStanja[z_pomeraj][b-1][a]=1;
    matStanja[z_pomeraj][b-1][a+1]=1;
}
}

void pamtimoLine(int a, int b)
{
if(rot_brojac == 0){
    matStanja[z_pomeraj][b+2][a]=1;
    matStanja[z_pomeraj][b+1][a]=1;
    matStanja[z_pomeraj][b-1][a]=1;
}
}

int ***alloc_mat(int zlen, int ylen, int xlen)
{
    /*Alokacija memorije za cuvanje matrice stanja mreze*/
    int u, v;

    if ((matStanja = malloc(zlen * sizeof(*matStanja))) == NULL) {
        perror("malloc 1");
        return NULL;
    }

    for (u=0; u < zlen; ++u)
        matStanja[u] = NULL;

    for (u=0; u < zlen; ++u)
        if ((matStanja[u] = malloc(ylen * sizeof(*matStanja[u]))) == NULL) {
            /*Ako alokacija nije uspela oslobadjamo vec alocirano i saljemo poruku za gresku*/
            perror("malloc 2");
            free_mat(matStanja, zlen, ylen);
            return NULL;
        }

    for (u=0; u < zlen; ++u)
        for (v=0; v < ylen; ++v)
            matStanja[u][v] = NULL;

    for (u=0; u < zlen; ++u)
        for (v=0; v < ylen; ++v)
            if ((matStanja[u][v] = malloc(xlen * sizeof (*matStanja[u][v]))) == NULL) {
                /*Ako alokacija nije uspela oslobadjamo vec alocirano i saljemo poruku za gresku*/
                perror("malloc 3");
                free_mat(matStanja, zlen, ylen);
                return NULL;
            }

    return matStanja;
}
void free_mat(int ***matStanja, int zlen, int ylen)
{
    int u, v;

    for (u=0; u < zlen; ++u) {
        if (matStanja[u] != NULL) {
            for (v=0; v < ylen; ++v)
                free(matStanja[u][v]);
            free(matStanja[u]);
        }
    }
    free(matStanja);
}

static void init_lights()
{
    /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
    GLfloat light_position[] = { 0, -1, 1, 0 };

    /* Ambijentalna boja svetla. */
    GLfloat light_ambient[] = { 0.5, 0.6, 0.3, 1 };

    /* Difuzna boja svetla. */
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.9, 1 };

    /* Spekularna boja svetla. */
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
    
    
    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

}

static void set_material(int id)
{
    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0.2, 0.4, 0.1, 1 };

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 1, 1, 1, 1 };

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 0.8, 0.8, 0.8, 1 };

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 30;
    
    /* Odredjujemo koeficijente ambijentalne i difuzne refleksije materijala za svaku figure. */
    switch (id) {
        case FIGURA_DOT:
            diffuse_coeffs[0] = 0.2;
            diffuse_coeffs[1] = 0.1;
            diffuse_coeffs[2] = 0.9;
            /* Koeficijenti ambijentalne refleksije materijala za datu figuru. */
            ambient_coeffs[0] = 0.0;
            ambient_coeffs[1] = 0.0;
            ambient_coeffs[2] = 0.5;
            break;
        case FIGURA_EL:
            /* Koeficijenti difuzne refleksije materijala za datu figuru. */
            diffuse_coeffs[0] = 0.9;
            diffuse_coeffs[1] = 0.9;
            diffuse_coeffs[2] = 0.0;
            /* Koeficijenti ambijentalne refleksije materijala za datu figuru. */
            ambient_coeffs[0] = 0.3;
            ambient_coeffs[1] = 0.3;
            ambient_coeffs[2] = 0.0;
            break;
        case FIGURA_TRIANGLE:
            /* Koeficijenti difuzne refleksije materijala za datu figuru. */
            diffuse_coeffs[0] = 0.7;
            diffuse_coeffs[1] = 0.1;
            diffuse_coeffs[2] = 0.3;
            /* Koeficijenti ambijentalne refleksije materijala za datu figuru. */
            ambient_coeffs[0] = 0.3;
            ambient_coeffs[1] = 0.0;
            ambient_coeffs[2] = 0.1;
            break;
        case FIGURA_ZE:
            /* Koeficijenti difuzne refleksije materijala za datu figuru. */
            diffuse_coeffs[0] = 0.0;
            diffuse_coeffs[1] = 0.8;
            diffuse_coeffs[2] = 0.9;
            /* Koeficijenti ambijentalne refleksije materijala za datu figuru. */
            ambient_coeffs[0] = 0.0;
            ambient_coeffs[1] = 0.3;
            ambient_coeffs[2] = 0.5;
            break;
        case FIGURA_SQUERE:
            /* Koeficijenti difuzne refleksije materijala za datu figuru. */
            diffuse_coeffs[0] = 0.5;
            diffuse_coeffs[1] = 0.9;
            diffuse_coeffs[2] = 0.2;
            /* Koeficijenti ambijentalne refleksije materijala za datu figuru. */
            ambient_coeffs[0] = 0.1;
            ambient_coeffs[1] = 0.5;
            ambient_coeffs[2] = 0.0;
            break;
        case FIGURA_LINE:
            /* Koeficijenti difuzne refleksije materijala za datu figuru. */
            diffuse_coeffs[0] = 0.7;
            diffuse_coeffs[1] = 0.3;
            diffuse_coeffs[2] = 0.0;
            /* Koeficijenti ambijentalne refleksije materijala za datu figuru. */
            ambient_coeffs[0] = 0.3;
            ambient_coeffs[1] = 0.1;
            ambient_coeffs[2] = 0.0;
            break;
        case 6:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.0;
            diffuse_coeffs[1] = 0.0;
            diffuse_coeffs[2] = 0.6;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.0;
            ambient_coeffs[1] = 0.0;
            ambient_coeffs[2] = 0.2;
            break;
        case 7:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.8;
            diffuse_coeffs[1] = 0.0;
            diffuse_coeffs[2] = 0.4;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.5;
            ambient_coeffs[1] = 0.0;
            ambient_coeffs[2] = 0.1;
            break;
        case 8:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.8;
            diffuse_coeffs[1] = 0.7;
            diffuse_coeffs[2] = 0.0;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.5;
            ambient_coeffs[1] = 0.4;
            ambient_coeffs[2] = 0.0;
            break;
        case 9:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.0;
            diffuse_coeffs[1] = 0.7;
            diffuse_coeffs[2] = 0.0;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.0;
            ambient_coeffs[1] = 0.2;
            ambient_coeffs[2] = 0.0;
            break;
        case 10:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.0;
            diffuse_coeffs[1] = 0.6;
            diffuse_coeffs[2] = 0.4;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.0;
            ambient_coeffs[1] = 0.3;
            ambient_coeffs[2] = 0.1;
            break;
        case 11:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.6;
            diffuse_coeffs[1] = 0.6;
            diffuse_coeffs[2] = 0.0;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.3;
            ambient_coeffs[1] = 0.3;
            ambient_coeffs[2] = 0.0;
            break;
        case 12:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.1;
            diffuse_coeffs[1] = 0.9;
            diffuse_coeffs[2] = 0.0;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.0;
            ambient_coeffs[1] = 0.3;
            ambient_coeffs[2] = 0.0;
            break;
        case 13:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.0;
            diffuse_coeffs[1] = 0.1;
            diffuse_coeffs[2] = 0.5;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.0;
            ambient_coeffs[1] = 0.0;
            ambient_coeffs[2] = 0.2;
            break;
        case 14:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.7;
            diffuse_coeffs[1] = 0.0;
            diffuse_coeffs[2] = 0.1;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.4;
            ambient_coeffs[1] = 0.0;
            ambient_coeffs[2] = 0.1;
            break;
        case 15:
            /* Koeficijenti difuzne refleksije materijala za naslagane figure. */
            diffuse_coeffs[0] = 0.1;
            diffuse_coeffs[1] = 0.0;
            diffuse_coeffs[2] = 0.9;
            /* Koeficijenti ambijentalne refleksije materijala za naslagane figure. */
            ambient_coeffs[0] = 0.0;
            ambient_coeffs[1] = 0.0;
            ambient_coeffs[2] = 0.6;
            break;
    }

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}
