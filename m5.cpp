/** @file  m5.cpp
  * @brief scope drawing 6 color line (r,g,b,j,c,m) reading 6 column from serial input and saving data in log file. Parameters for serie connection and scale are in config file.
  * @author "Delattre Pierre-Alexandre" <Delattre.P.A@gmail.com>
  * @date   6 fevrier 2015
  */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include <math.h>
#include <SerialStream.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// test modif

using namespace std;
using namespace LibSerial;
 
const int WIDTH  = 1024;
const int HEIGHT = 513;

void  draw_colonne_six (SDL_Renderer * ren, int i, int data1, int data2, int data3, int data4, int data5, int data6) 
{
    SDL_Rect rect;
    int res;
    unsigned int value;
    int j;
    
    // couleur des pixels
    int rouge;
    int vert;
    int bleu;
 
    rect.w=1;
    rect.h=1;
    
    for(j=0;j<HEIGHT;j++){

      rect.x=i;
      rect.y=j;

      rouge = 0;
      vert = 0;
      bleu = 0;

      if (j == data1)
        rouge += 255;
      if (j == data2)
	vert += 255;
      if (j == data3)
	  bleu += 255;
      if (j == data4) {
	  rouge += 128;
	  vert += 128;
      }
      if (j == data5) {
	  vert += 128;
	  bleu += 128;
      }
      if (j == data6) {
	  bleu += 128;
	  rouge += 128;
      }
      if (j == 0) {
	  rouge += 128;
	  vert += 128;
	  bleu += 128;
      }
      
      if (rouge > 255)
	rouge = 255;
      if (vert > 255)
	vert = 255;
      if (bleu > 255)
	bleu = 255;
      
      SDL_SetRenderDrawColor(ren,rouge,vert,bleu,0);
      SDL_RenderDrawPoint(ren,i,j);
    }

    SDL_RenderPresent(ren);
}
 
int main(int argc, char** argv)
{
  // SDL ////////////////
  
  SDL_Window *win = 0;
  SDL_Renderer *ren = 0;
//  SDL_Renderer *ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
 
  /* Initialisation de la SDL. Si ça se passe mal, on quitte */
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    fprintf(stderr,"Erreur initialisation\n");
    return -1;
  }
  /* Création de la fenêtre et du renderer */
  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &win, &ren); // SDL_WINDOW_SHOWN|SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,&win,&ren);
 
  if (!win || !ren) 
    {
      fprintf(stderr,"Erreur à la création des fenêtres\n");
      SDL_Quit();
      return -1;
    }
    
  /* Affichage du fond */
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);
  SDL_RenderPresent(ren);
  
  
  
  // Serie ////////////////

  cout << "Liaison série, oscillo et log" << endl;
  
  // log file
  string logname = "serie.log";
  ofstream log_file;
  log_file.open (logname.c_str(),ofstream::binary);
  
  cout << "log file : " << logname << endl;

  log_file << "= " __FILE__ << " :.:  " << __DATE__ ;
  log_file << " :.: " << __TIME__ << " =" << endl;
  log_file << endl;
  
  
  // fichier parametre
  string paramname = "config.txt";
  ifstream param_file;
  param_file.open (paramname.c_str(),ifstream::in);
  
  int rouge_col, vert_col, bleu_col;
  int jaune_col, cyan_col, magenta_col;
  
  float rouge_scale, vert_scale, bleu_scale;
  float jaune_scale, cyan_scale, magenta_scale;
  
  string rouge_title, vert_title, bleu_title;
  string jaune_title, cyan_title, magenta_title;
  
  string port_serie;
  int vitesse_serie;
  param_file >> port_serie;
  param_file >> vitesse_serie;
  
  string mot;
  param_file >> mot;
  param_file >> mot;
  param_file >> mot;
  
  param_file >> mot;
  param_file >> rouge_col;
  param_file >> rouge_scale;
  param_file >> rouge_title;
  
  param_file >> mot;
  param_file >> vert_col;
  param_file >> vert_scale;
  param_file >> vert_title;

  
  param_file >> mot;
  param_file >> bleu_col;
  param_file >> bleu_scale; 
  param_file >> bleu_title; 
  
  param_file >> mot;
  param_file >> jaune_col;
  param_file >> jaune_scale;
  param_file >> jaune_title;  
  
  param_file >> mot;
  param_file >> cyan_col;
  param_file >> cyan_scale; 
  param_file >> cyan_title; 
  
  param_file >> mot;
  param_file >> magenta_col;
  param_file >> magenta_scale;
  param_file >> magenta_title;
  
  param_file.close ();
  
  // serial stream
  SerialStream my_serial_stream;
  my_serial_stream.Open(port_serie.c_str());
  if ( !my_serial_stream.IsOpen() ) {
    cout << "Erreur 5 : impossible d'ouvrir le port série." << endl;
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 5;
  }
  if (vitesse_serie == 115200) {
  my_serial_stream.SetBaudRate(SerialStreamBuf::BAUD_115200); // BAUD_115200
  }
  else {
    my_serial_stream.SetBaudRate(SerialStreamBuf::BAUD_9600); // default : BAUD_9600
  }
  my_serial_stream.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
  my_serial_stream.SetNumOfStopBits(2);
  my_serial_stream.SetParity(SerialStreamBuf::PARITY_NONE);
  my_serial_stream.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
  
  cout << "port : " << port_serie << endl;
  cout << "vitesse : " << vitesse_serie << endl;
  
  // oscilloscope
  
  int col = 0;
  SDL_Event e;
  
  float data[16];
  
  // gestion des fonts et des couleurs
  if (TTF_Init() < 0) {
    // Handle error...
  }
  
  // police et couleurs
  TTF_Font* la_police = TTF_OpenFont("font.ttf", 22);
  SDL_Color color_gris = {128, 128, 128};
  SDL_Color color_rouge = {255, 0, 0};
  SDL_Color color_vert = {0, 255, 0};
  SDL_Color color_bleu = {0, 0, 255};
  SDL_Color color_jaune = {255, 255, 0};
  SDL_Color color_cyan = {0, 255, 255};
  SDL_Color color_magenta = {255, 0, 255};
  
  // formatage des variables à afficher
  char affichage_infos [50];
  char affichage_rouge [20];
  char affichage_vert [20];
  char affichage_bleu [20];
  char affichage_jaune [20];
  char affichage_cyan [20];
  char affichage_magenta [20];
  int n;
  
  n=sprintf (affichage_infos, "config : %s  -  serie : %s @ %d kbit/s  -  log : %s", paramname.c_str(), port_serie.c_str(), vitesse_serie, logname.c_str());
  
  SDL_Surface* surfaceMessage = TTF_RenderText_Solid(la_police, affichage_infos, color_gris); 
  SDL_Texture* Message = SDL_CreateTextureFromSurface(ren, surfaceMessage);
  SDL_FreeSurface(surfaceMessage);
  SDL_Rect Message_rect; //create a rect
  Message_rect.x = 10;  //controls the rect's x coordinate 
  Message_rect.y = 480; // controls the rect's y coordinte
  Message_rect.w = surfaceMessage->w; // controls the width of the rect
  Message_rect.h = surfaceMessage->h; // controls the height of the rect
  SDL_RenderCopy(ren, Message, NULL, &Message_rect);
//  SDL_DestroyTexture(Message);

  
  SDL_Surface* surfaceMessage_rouge;
  SDL_Surface* surfaceMessage_vert;
  SDL_Surface* surfaceMessage_bleu;
  SDL_Surface* surfaceMessage_jaune;
  SDL_Surface* surfaceMessage_cyan;
  SDL_Surface* surfaceMessage_magenta;
  SDL_Texture* Message_rouge;
  SDL_Texture* Message_vert;
  SDL_Texture* Message_bleu;
  SDL_Texture* Message_jaune;
  SDL_Texture* Message_cyan;
  SDL_Texture* Message_magenta;
  SDL_Rect Message_rect_rouge, Message_rect_vert, Message_rect_bleu;
  SDL_Rect Message_rect_jaune, Message_rect_cyan, Message_rect_magenta;
  
  char caractere;
  float data_rouge;
  float data_vert;
  float data_bleu;
  float data_jaune;
  float data_cyan;
  float data_magenta;
  
  while(e.type != SDL_QUIT) {
    
    col++;
    col = col % WIDTH;
    
    // attendre une nouvelle ligne
    while(caractere != '\n')
    {
      my_serial_stream.get(caractere);
    }
    
    for (int d = 0; d<13; d++) {
      
      my_serial_stream >> data[d];
      
    }
    
    data_rouge = data[rouge_col-1] * rouge_scale;
    data_vert = data[vert_col-1] * vert_scale;
    data_bleu = data[bleu_col-1] * bleu_scale;
    data_jaune = data[jaune_col-1] * jaune_scale;
    data_cyan = data[cyan_col-1] * cyan_scale;
    data_magenta = data[magenta_col-1] * magenta_scale;
    
    log_file << data[0] << '\t' << data[1] << '\t' << data[2] << '\t' << data[3] << '\t' << data[4] <<'\t' << data[5] << '\t' << data[6] << '\t' << data[7] <<'\t' << data[8] << '\t' << data[9] << '\t' << data[10] << '\t' << data[11] <<'\t' << data[12] <<  endl;
    
    // remettre à jour la ligne verticale de pixel
    draw_colonne_six (ren, col, 0.5*HEIGHT-data_rouge, 0.5*HEIGHT-data_vert, 0.5*HEIGHT-data_bleu, 0.5*HEIGHT-data_jaune, 0.5*HEIGHT-data_cyan, 0.5*HEIGHT-data_magenta);
    
    // afficher le texte constant
    SDL_RenderCopy(ren, Message, NULL, &Message_rect);
    
    // afficher le texte mis à jour
    n=sprintf (affichage_rouge, "c%1d %s %6d", rouge_col, rouge_title.c_str(), (int) data_rouge);
    n=sprintf (affichage_vert, "c%1d %s %6d", vert_col, vert_title.c_str(), (int) data_vert);
    n=sprintf (affichage_bleu, "c%1d %s %6d", bleu_col, bleu_title.c_str(), (int) data_bleu);
    n=sprintf (affichage_jaune, "c%1d %s %6d", jaune_col, jaune_title.c_str(), (int) data_jaune);
    n=sprintf (affichage_cyan, "c%1d %s %6d", cyan_col, cyan_title.c_str(), (int) data_cyan);
    n=sprintf (affichage_magenta, "c%1d %s %6d", magenta_col, magenta_title.c_str(), (int) data_magenta);
    
    // faire du noir derrière avant le rendu du nombre
    SDL_SetRenderDrawColor( ren, 0, 0, 0, 255 );
    SDL_RenderFillRect( ren, &Message_rect_rouge );
    SDL_RenderFillRect( ren, &Message_rect_vert );
    SDL_RenderFillRect( ren, &Message_rect_bleu );
    SDL_RenderFillRect( ren, &Message_rect_jaune );
    SDL_RenderFillRect( ren, &Message_rect_cyan );
    SDL_RenderFillRect( ren, &Message_rect_magenta );

    surfaceMessage_rouge = TTF_RenderText_Solid(la_police, affichage_rouge, color_rouge);
    Message_rouge = SDL_CreateTextureFromSurface(ren, surfaceMessage_rouge);
    Message_rect_rouge.x = 100;
    Message_rect_rouge.y = 10;
    Message_rect_rouge.w = surfaceMessage_rouge->w;
    Message_rect_rouge.h = surfaceMessage_rouge->h;
    SDL_RenderCopy(ren, Message_rouge, NULL, &Message_rect_rouge);
    SDL_FreeSurface(surfaceMessage_rouge);
    SDL_DestroyTexture(Message_rouge);
    
    surfaceMessage_vert = TTF_RenderText_Solid(la_police, affichage_vert, color_vert);
    Message_vert = SDL_CreateTextureFromSurface(ren, surfaceMessage_vert);
    Message_rect_vert.x = 100;
    Message_rect_vert.y = 40;
    Message_rect_vert.w = surfaceMessage_vert->w;
    Message_rect_vert.h = surfaceMessage_vert->h;
    SDL_RenderCopy(ren, Message_vert, NULL, &Message_rect_vert);
    SDL_FreeSurface(surfaceMessage_vert);
    SDL_DestroyTexture(Message_vert);
    
    surfaceMessage_bleu = TTF_RenderText_Solid(la_police, affichage_bleu, color_bleu);
    Message_bleu = SDL_CreateTextureFromSurface(ren, surfaceMessage_bleu);
    Message_rect_bleu.x = 100;
    Message_rect_bleu.y = 70;
    Message_rect_bleu.w = surfaceMessage_bleu->w;
    Message_rect_bleu.h = surfaceMessage_bleu->h;
    SDL_RenderCopy(ren, Message_bleu, NULL, &Message_rect_bleu);
    SDL_FreeSurface(surfaceMessage_bleu);
    SDL_DestroyTexture(Message_bleu);
    
    surfaceMessage_jaune = TTF_RenderText_Solid(la_police, affichage_jaune, color_jaune);
    Message_jaune = SDL_CreateTextureFromSurface(ren, surfaceMessage_jaune);
    Message_rect_jaune.x = 300;
    Message_rect_jaune.y = 10;
    Message_rect_jaune.w = surfaceMessage_jaune->w;
    Message_rect_jaune.h = surfaceMessage_jaune->h;
    SDL_RenderCopy(ren, Message_jaune, NULL, &Message_rect_jaune);
    SDL_FreeSurface(surfaceMessage_jaune);
    SDL_DestroyTexture(Message_jaune);
    
    surfaceMessage_cyan = TTF_RenderText_Solid(la_police, affichage_cyan, color_cyan);
    Message_cyan = SDL_CreateTextureFromSurface(ren, surfaceMessage_cyan);
    Message_rect_cyan.x = 300;
    Message_rect_cyan.y = 40;
    Message_rect_cyan.w = surfaceMessage_cyan->w;
    Message_rect_cyan.h = surfaceMessage_cyan->h;
    SDL_RenderCopy(ren, Message_cyan, NULL, &Message_rect_cyan);
    SDL_FreeSurface(surfaceMessage_cyan);
    SDL_DestroyTexture(Message_cyan);
    
    surfaceMessage_magenta = TTF_RenderText_Solid(la_police, affichage_magenta, color_magenta);
    Message_magenta = SDL_CreateTextureFromSurface(ren, surfaceMessage_magenta);
    Message_rect_magenta.x = 300;
    Message_rect_magenta.y = 70;
    Message_rect_magenta.w = surfaceMessage_magenta->w;
    Message_rect_magenta.h = surfaceMessage_magenta->h;
    SDL_RenderCopy(ren, Message_magenta, NULL, &Message_rect_magenta);
    SDL_FreeSurface(surfaceMessage_magenta);
    SDL_DestroyTexture(Message_magenta);
    
    SDL_PollEvent(&e);
    
  }
  
  my_serial_stream.Close() ;
  
  log_file.close ();
    
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
