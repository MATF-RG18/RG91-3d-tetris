#include <stdlib.h>
#include <GL/glut.h>

/* Definisemo granice parametara povrsi */
#define U_FROM -4
#define V_FROM -4
#define U_TO 4
#define V_TO 4


/* Deklaracije callback funkcija. */
static void on_display(void);

/* Deklaracija funkcija za crtanje */

static void drawMreza();

int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    
    /* Kreira se prozor. */
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se funkcije za obradu dogadjaja. */
    glutDisplayFunc(on_display);

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0, 0, 0, 0);

    /* Ulazi se u glavnu petlju. */
    glutMainLoop();
    
    return 0;
}

static void on_display(void)
{
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Crtamo delove scene */
    drawMreza();

    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

static void drawMreza()
{
    /* definisemo ivicu i visinu trougla */
    float size = 0.1;
    int u,v;
    /* ukljucujemo iscrtavanje sa popunom */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    for(v=V_FROM; v<V_TO; v++){
    	glBegin(GL_QUAD_STRIP);
        /* iscrtavamo skoro crnu popunu glavnog trougla
         * preko prethodno nacrtanih zraka */
       	 	glColor3f(0.9, 0.9, 0.9);
		u=U_FROM;
		glVertex2f(u*size , -v*size);
        	glVertex2f(u*size , -(v+1)*size);
		for(u=U_FROM+1; u<=U_TO; u++){
        		glVertex2f(u*size ,- v * size);
        		glVertex2f(u*size, - (v+1) *size);
	}

    	glEnd();
    }


}
