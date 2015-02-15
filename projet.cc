/*******************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <termios.h>
#include <fcntl.h>
#include <ctime>
#include <string.h>
using namespace std;

/* retourne 0 si aucune touche n'est appuyée,
* sinon retourne le code ASCII de la touche */
int read_keybord();

#define H 20
#define L 30
bool depart;
int key;
int choix;
int niveau=1;
int vitesse=14;
int target=60;
int note=0;
int total=0;
string joueur;

enum etat_case{
  ESPACE,
  MUR,
  BALL,
  BARRE,
  BRIQUE
};
etat_case screen[H][L];

bool casse[H][L];

etat_case lire_case(int i, int j){ // Ce fontion nous aide a savoir facilement
  return screen[i][j];             // l'etat de chaque case	
};			           // au lieu de lire les caracteres *,=,#,o

void assigner_case(int i, int j, etat_case valeur){ // Assigner un etat dans une case
  screen[i][j]=valeur;                              
}													

/***********ACCUEIL***************/
void accueil(){
  cout << "\033[34;01m\033[34m";
  cout << "    ______                        ____       _                 \n";
  cout << "   / ____/___ ______________     / __ )_____(_)___ ___  _____  \n";
  cout << "  / /   / __ `/ ___/ ___/ _ \\   / __  / ___/ / __ `/ / / / _ \\ \n";
  cout << " / /___/ /_/ (__  |__  )  __/  / /_/ / /  / / /_/ / /_/ /  __/ \n";
  cout << " \\____/\\__,_/____/____/\\___/  /_____/_/  /_/\\__, /\\__,_/\\___/  \n";
  cout << "                                              /_/              \n";
  cout << "\t\t       ...Projet de AP1... \n\n";
}

void menu(){
  cout << "\t\t\t  Appuyez: \n";
  cout << "\t\t     c - Commencer a jouer \n";
  cout << "\t\t     q - Quitter le jeux \n\n";
}

/**********CLEAR_SCREEN*************/
void clear_screen(){
  for(int i=0;i<H; i++)
    for(int j=0; j<L; j++){
	   assigner_case(i,j,ESPACE);
	   casse[i][j]=false;
    }
}

/********SCREEN_DISPLAY************/
void screen_display(){
  system("clear");
  accueil();
  cout << "\t\t\t    Pressez: \n"; // Expliquer les bouton
  cout << "\t\t   <space> - Commencer à jouer \n";
  cout << "\t\t      q    - Quitter le jeux \n\n";
  cout << "\t\t  Niveau: \033[32;01m" << niveau << "\033[34m \t  Objectif: \033[32;01m" << target*100 << "\033[34m \n";
  for(int i=0;i<H; i++){
    cout<<"\t\t  ";
    for (int j=0; j<L; j++){
      switch(lire_case(i,j)){
      case ESPACE: cout << " "; break;
      case MUR: cout << "\033[34;01m*\033[34m"; break;
      case BALL: cout << "\033[00;01mo\033[34m"; break;
      case BARRE: cout << "\033[32;01m#\033[34m"; break;
      case BRIQUE: cout << "\033[31;01m=\033[34m"; break;
      defaut: cout << lire_case(i,j);
      }
    }
    cout << endl;
  }
  cout << "\t\t\t     Note: \033[32;01m" << note*100 << "\033[34m \n"; // Note du niveau en ce moment
}

/************CADRE*************/
void cadre(){ // Dessiner le cadre
  for(int i=0;i<H; i++)
    for(int j=0; j<L; j++)
      if(i==0 || i==H-1 || j==0 || j==L-1)
	    assigner_case(i,j,MUR);
      else 
	    assigner_case(i,j,ESPACE);
}

/************BARRE**************/
int bG=L/2-2; // Limite du barre a gauche
int bD=L/2+2; // Limite du barre a droite

void barre(){
  if(key=='a' && bG>1){ // Possibilité d'aller a gauche
      bG -= 2; bD -= 2;
  }
  if(key=='d' && bD<L-2){ // Possibilité d'aller a droite
      bG += 2; bD += 2;
  }
  for(int i=0;i<H;i++)
    for(int j=0; j<L; j++)
      if(i==H-3 && j>=bG && j<=bD)
	    assigner_case(i,j,BARRE); // Tracer la barre
}

/***********BRIQUES****************/
// Niveau 1
void brique1a(){
  for(int i=1;i<H-1;i++)
    for(int j=1; j<L-1; j++)
      if((((i==1 || i==5) && j>3 && j<L-4) || 
	  ((i==3 || i== 7)&& j>4 && j<L-5)) && 
	 !(casse[i][j]))	
	assigner_case(i,j,BRIQUE); 
}

void brique1b(){
  for(int i=1;i<H-1;i++)
    for(int j=1; j<L-1; j++)
      if((((i==1 || i==5) && j>3 && j<L-4) || 
	  ((i==3 || i==7) && j>4 && j<L-5)) && 
	 !(casse[i][j]))
	assigner_case(i,j,BRIQUE); 
}
// Niveau 2
void brique2a(){
  for(int i=1;i<H-1;i++)
    for(int j=1; j<L-1; j++)
      if(((i==1 && (j>0 && j<L-1))||
	  (i==3 && (j>3 && j<L-4)) ||
	  (i==5 && (j>6 && j<L-7)) ||
	  (i==7 && (j>9 && j<L-10)) ||
	  (i==9 && (j>12 && j<L-13))) &&
	 !(casse[i][j]))
	assigner_case(i,j,BRIQUE); 
}

void brique2b(){
  for(int i=1;i<H-1;i++)
    for (int j=1; j<L-1; j++)
      if((i==1 ||
	  (i==3 && (j<13 || j>L-14)) ||
	  (i==5 && (j<10 || j>L-11)) ||
	  (i==7 && (j<7 || j>L-8)) ||
	  (i==9 && (j<4 || j>L-5))) &&
	 !(casse[i][j]))
	assigner_case(i,j,BRIQUE); 
}
// Niveau 3
void brique3a(){
  for(int i=1;i<H-1;i++)
    for (int j=1; j<L-1; j++)
      if(((i==1 && j>5) ||
	  (i==3 && j>8) ||
	  (i==5 && j>11) ||
	  (i==7 && j>14) ||
	  (i==9 && j>17) ||
	  (i==11 && j>20)) &&
	 !(casse[i][j]))
	assigner_case(i,j,BRIQUE); 
}

void brique3b(){
  for(int i=1;i<H-1;i++)
    for(int j=1; j<L-1; j++)
      if(((i==1 && j<L-6) ||
	  (i==3 && j<L-9) ||
	  (i==5 && j<L-12) ||
	  (i==7 && j<L-15) ||
	  (i==9 && j<L-18) ||
	  (i==11 && j<L-21)) &&
	 !(casse[i][j]))
	assigner_case(i,j,BRIQUE); 
}

int choisir_brique(){ // Random les briques
  srand(time(0));	  // On a 2 scènes pour chaque niveau
  int num = rand() % 9;
  if(num%2==0) choix=1;
  else choix=2;
  return choix;
}

void display_brique(int choix){ // Ouvrir la scène correcte
  if(choix==1)
    switch(niveau){
    case 1: brique1a(); break;
    case 2: brique2a(); break;
    case 3: brique3a(); break;
    }
  if(choix==2)
    switch(niveau){
    case 1: brique1b(); break;
    case 2: brique2b(); break;
    case 3: brique3b(); break;
    }
}

/**********WIN_GAME***************/
void win_game(){
  system("clear");
  accueil();
  cout << "\t\t      Note du niveau " << niveau << ": \033[32;01m" << note*100 << "\033[34m" << endl;
  total+=note;
  note=0;
  cout << "\t\t\t    Total: \033[31;01m" << total*100 << "\033[34m" << endl << endl;
  if(niveau<3){
	cout << "\t\t ------------------------------- \n\n";
	cout << "\t\t" << joueur << ", vous avez gagne' le niveau " << niveau << endl;
	cout << "\t\t     On va ensuite au niveau " << niveau+1;
	cout << "\n\n\tAttention! La vistesse du ballon sera plus rapide! \n\n";
  }
  else{
    cout << "\t\t   _____ _____ _____ _____ _____ \n";
    cout << "\t\t  | __  | __  |  _  |  |  |     | \n";
    cout << "\t\t  | __ -|    -|     |  |  |  |  | \n";
    cout << "\t\t  |_____|__|__|__|__|\\___/|_____|\n\n";
    cout << "\t\t" << joueur << ", vous avez gagne' le jeux!\n\n";
    exit(0);
  }	
}

/**********LOSE_GAME***************/
void lose_game(){
  system("clear");
  accueil();
  cout << "\t\t      Note du niveau " << niveau << ": \033[32;01m" << note*100 << "\033[34m" << endl;
  total+=note;
  note=0;
  cout << "\t\t\t    Total: \033[31;01m" << total*100 << "\033[34m" << endl << endl;
  cout << "      _____ _____ _____ _____    _____ _____ _____ _____ \n";
  cout << "     |   __|  _  |     |   __|  |     |  |  |   __| __  | \n";
  cout << "     |  |  |     | | | |   __|  |  |  |  |  |   __|    -|\n";
  cout << "     |_____|__|__|_|_|_|_____|  |_____|\\___/|_____|__|__| \n\n";
  cout << "               Essayez-vous à la prochaine fois!\n\n";
  exit(0);
}

/**********UPDATE_GAME*************/		
int x=H-4, a=-1; // situation du depart
int y=L/2, b=-1;

void update_game(){
  assigner_case(x,y,BALL);
  if(depart){
    if(x==1) a=1;
    if(y==1) b=1;
    if(y==L-2) b=-1;
    if(x==H-2){
      sleep(1);
      lose_game();
    }
    if(lire_case(x+1,y)==BARRE)	a=-1;
    x += a;
    y += b;
    if(lire_case(x,y)==BRIQUE){ // Detruire les briques
      assigner_case(x,y,ESPACE);
      casse[x][y]=true;
      a=-a; // Le ballon change son vertical
      note++; // Prendre les notes
      if(lire_case(x,y-1)==BRIQUE) {
     	assigner_case(x,y-1,ESPACE);
	    casse[x][y-1]=true;
	    note++;
      }
      if(lire_case(x,y+1)==BRIQUE) {
	    assigner_case(x,y+1,ESPACE);
	    casse[x][y+1]=true;
	    note++;
      }
    }   
  }
}

int casse_brique(){
  do{ 
    key = read_keybord();	
    if(key==' ') depart=true;
    cadre();
    barre();
    display_brique(choix);
    update_game();				
    screen_display();
    usleep(vitesse * 10000); // Vitesse du ball
    
    if(note>=target){ // Contrôler les notes
      clear_screen();
      win_game();
      if(niveau<3){ // Continue le niveau ensuite
	    sleep(5);
	    niveau+=1;
	    vitesse-=2;
	    target+=10;
	    depart=false;
	    choisir_brique();
	    x=H-4; y=L/2;
     	bG=L/2-2; bD=L/2+2;
      }
    }	
  }while(key != 'q');
  return(0);
}

/**************MAIN**************/
/* Mettre votre code source ici */
int main(){
  system("clear");
  accueil();
  cout << "\t\t\t Entrez votre nom\n";
  cout << "\t\t\t     ";
  cin >> joueur;
  sleep(1);
  clear_screen();
  choisir_brique();
  casse_brique();
}

/******************************/
int kbhit(void){
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
