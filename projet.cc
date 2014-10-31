
/*******************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <termios.h>
#include <fcntl.h>

using namespace std;

/* retourne 0 si aucune touche n'est appuyée, 
 * sinon retourne le code ASCII de la touche */
int read_keybord();

/********SCREEN_DISPLAY************/

#define H 20
#define L 30

char screen[H][L];

bool loseGame=false;

void screen_display(){
  system("clear");
  cout << "          CASSE BRIQUE \n";
  cout << "      ...Projet de AP1... \n\n";
  cout << "Appuyez 'q' pour quitter le jeux \n\n";

  for(int i=0;i<H; i++) {
    for (int j=0; j<L; j++) {
      cout << screen [i][j];
    }
    cout << endl;
  }

}

/**************CADRE*************/

void clear_screen() {
  for(int i=0;i<H; i++){ 
    for (int j=0; j<L; j++){
      if (i==0 || i==H-1 || j==0 || j==L-1)
	screen[i][j]= '*';
      else
	screen[i][j]= ' ';
    }
  }

}

/************BARRE**************/

int bG=L/2-2; // limite du barre a gauche
int bD=L/2+2; // limite du barre a droite

int key;

void barre(){
  if (key=='a') {
    if (bG>1) { // possibilite d'aller a gauche
      bG -= 2; bD -= 2;
    }
  }

  if (key=='d') { 
    if (bD<L-3) { // possibilite d'aller a droite
    bG += 2; bD += 2; 
    }
  }
  
  for(int i=0;i<H;i++) {
    for (int j=0; j<L; j++) {
      if (i==H-3 && j>=bG && j<=bD)
	screen[i][j]='~'; // barre
    }   
  }
  
}


/**********UPDATE_GAME*************/

int x = 1, a=1; // situation du depart
int y = 1, b=1;

int cpt=0;
 
void update_game() {
    screen[x][y]='o';
    if (x==1) a=1;
    if (y==1) b=1;
    if (y==L-2) b=-1;
    if (x==H-2) loseGame=true;
    x += a;
    y += b;

    if (screen[x][y]=='~' && screen[x][y-1]=='~') { // ball >< DROITE de la barre
      a=-1; b=1;
      cpt+=1;
    }

    if (screen[x][y]=='~' && screen[x][y+1]=='~') { // ball >< GAUCHE de la barre
      a=-1; b=-1;
      cpt+=1;
    }

}

/**********LOSE_GAME***************/

void lose_game() {
  cout << "\n\tDESOLE! \n\n";
  cout << "\tPoints: "<< cpt << "\n\n";
  exit(0);
}

/**************MAIN**************/
/* Mettre votre code source ici */

int main(){

  do{
    key = read_keybord();	
    clear_screen();
    barre();
    update_game();				
    screen_display();
    usleep(10 * 10000);	

    if (loseGame==true) 
      lose_game();
  } 
  while(key != 'q');

}

/******************************/

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

int read_keybord() {
  int key;
  if (kbhit())
    key = getc(stdin);
  else
    key = 0;
  return key;
}

/******************************/
