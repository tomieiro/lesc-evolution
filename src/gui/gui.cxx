// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "../../headers/gui.h"

Fl_Double_Window *janela_principal=(Fl_Double_Window *)0;

Fl_BMP_Image *image=(Fl_BMP_Image *)0;

Fl_Button *start=(Fl_Button *)0;

Fl_Value_Input *fmg=(Fl_Value_Input *)0;

Fl_Value_Input *populacao=(Fl_Value_Input *)0;

Fl_Value_Input *mutacao_inicial=(Fl_Value_Input *)0;

/**
 Funcao que instancia a GUI
*/
Fl_Double_Window* gui() {
  { // Janela principal da GUI
    janela_principal = new Fl_Double_Window(870, 405, "LE_EVOLUTION");
    janela_principal->color((Fl_Color)237);
    janela_principal->labelfont(11);
    { // Imagem do cenario
      image = new Fl_BMP_Image("../../img/img.bmp");
      image->draw(445,45,400,300,0,0);
    } // Fl_Box* image
    { // Comeca o ciclo de evolucao
      start = new Fl_Button(195, 350, 105, 30, "INICIAR");
      start->box(FL_RSHADOW_BOX);
      start->color((Fl_Color)215);
      start->labelfont(11);
    } // Fl_Button* start
    { // Fator de mistura genetica
      fmg = new Fl_Value_Input(180, 35, 180, 25, "FMG:");
      fmg->box(FL_SHADOW_BOX);
      fmg->color((Fl_Color)215);
      fmg->labelfont(11);
      fmg->textfont(11);
    } // Fl_Value_Input* fmg
    { // Numero de individuos da populacao
      populacao = new Fl_Value_Input(180, 75, 180, 25, "INDIVIDUOS:");
      populacao->box(FL_SHADOW_BOX);
      populacao->color((Fl_Color)215);
      populacao->labelfont(11);
      populacao->textfont(11);
    } // Fl_Value_Input* populacao
    { // Taxa de mutacao inicial
      mutacao_inicial = new Fl_Value_Input(180, 120, 180, 25, "MUTACAO INICIAL:");
      mutacao_inicial->box(FL_SHADOW_BOX);
      mutacao_inicial->color((Fl_Color)215);
      mutacao_inicial->labelfont(11);
      mutacao_inicial->textfont(11);
    } // Fl_Value_Input* mutacao_inicial
    janela_principal->end();
  } // Fl_Double_Window* janela_principal
  return janela_principal;
}
