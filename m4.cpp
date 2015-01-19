#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include <math.h>
#include <SerialStream.h>
#include <SDL2/SDL.h>

using namespace std;
using namespace LibSerial;
 
const int WIDTH  = 1024;
const int HEIGHT = 512;

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
  string logname = "seriefixed.log";
  ofstream log_file;
  log_file.open (logname.c_str(),ofstream::binary);
  
  cout << "log file : " << logname << endl;

  log_file << "= " __FILE__ << " :.:  " << __DATE__ ;
  log_file << " :.: " << __TIME__ << " =" << endl;
  log_file << endl;
  
  
  // fichier parametre
  string paramname = "config_plot.txt";
  ifstream param_file;
  param_file.open (paramname.c_str(),ifstream::in);
  
  int rouge_col, vert_col, bleu_col;
  int jaune_col, cyan_col, magenta_col;
  
  float rouge_scale, vert_scale, bleu_scale;
  float jaune_scale, cyan_scale, magenta_scale;
  
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
  
  param_file >> mot;
  param_file >> vert_col;
  param_file >> vert_scale;  
  
  param_file >> mot;
  param_file >> bleu_col;
  param_file >> bleu_scale;  
  
  param_file >> mot;
  param_file >> jaune_col;
  param_file >> jaune_scale;  
  
  param_file >> mot;
  param_file >> cyan_col;
  param_file >> cyan_scale;  
  
  param_file >> mot;
  param_file >> magenta_col;
  param_file >> magenta_scale;
  
  param_file.close ();
  
  // serial stream
  SerialStream my_serial_stream;
  my_serial_stream.Open(port_serie.c_str());
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
  cout << "vitesse :" << vitesse_serie << endl;
  
  // oscilloscope
  
  int col = 0;
  SDL_Event e;
  
  float data[16];
  
  
// TTF_Font* Sans_12 = TTF_OpenFont("Sans.ttf", 12);
// SDL_Color color_rouge = {255, 0, 0};
// SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans_12, "Hello world !", color_rouge); 
// SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
// SDL_Rect Message_rect; //create a rect
// Message_rect.x = 0;  //controls the rect's x coordinate 
// Message_rect.y = 0; // controls the rect's y coordinte
// Message_rect.w = 100; // controls the width of the rect
// Message_rect.h = 30; // controls the height of the rect
// SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
  
  
  while(e.type != SDL_QUIT) {
    
    col++;
    col = col % WIDTH;
    
    // attendre une nouvelle ligne
    char caractere;
    while(caractere != '\n')
    {
      my_serial_stream.get(caractere);
    }
    
    for (int d = 0; d<13; d++) {
      
      my_serial_stream >> data[d];
      
    }
    
    float data_rouge = data[rouge_col-1] * rouge_scale;
    float data_vert = data[vert_col-1] * vert_scale;
    float data_bleu = data[bleu_col-1] * bleu_scale;
    float data_jaune = data[jaune_col-1] * jaune_scale;
    float data_cyan = data[cyan_col-1] * cyan_scale;
    float data_magenta = data[magenta_col-1] * magenta_scale;
    
    log_file << data[0] << '\t' << data[1] << '\t' << data[2] << '\t' << data[3] << '\t' << data[4] <<'\t' << data[5] << '\t' << data[6] << '\t' << data[7] <<'\t' << data[8] << '\t' << data[9] << '\t' << data[10] << '\t' << data[11] <<'\t' << data[12] <<  endl;
    
    draw_colonne_six (ren, col, 0.5*HEIGHT-data_rouge, 0.5*HEIGHT-data_vert, 0.5*HEIGHT-data_bleu, 0.5*HEIGHT-data_jaune, 0.5*HEIGHT-data_cyan, 0.5*HEIGHT-data_magenta);
    
    SDL_PollEvent(&e);
    
  }
  
  my_serial_stream.Close() ;
  
  log_file.close ();
    
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
