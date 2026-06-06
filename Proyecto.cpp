/* Agenda de contactos con historial de llamadas*/

#include<stdlib.h>
#include<string>
#include<time.h>
#include<stdio.h>
#include "raylib.h"

//enum de tipo llamada
typedef enum {
    ENTRANTE,
    SALIENTE,
    PERDIDA
} TipoLlamada;

//Nodo llamada (lista enlazada)
struct NodoLlamada {
    char fecha[11];
    char hora[6];
    int duracion;
    TipoLlamada tipo;
    NodoLlamada* siguiente;
}NodoLlamada;

//contacto
struct Contacto {
    int id;
    char nombre[50];
    char telefono[20];
    char email[50];
    NodoLlamada* historialLlam; //cabeza de la lista enlazada de llamadas
}Contacto;

//Variables globales
static Contacto**contactos=NULL;
static int numContactos=0;
static int capacidadContactos=0;

//Prototipos

//Contactos
/* no se ocuparon :)
void agregarContacto();
void buscarContacto();
void editarContacto();
void eliminarContacto();
void listarContactos();
Contacto* buscarContactID(int id); */

//Historial
void registrarLlamada();
void mostararContactHistorial(Contacto* c);
void agregarNodoLlamada(Contacto* c, const char* fecha,
const char* hora, int duracion, TipoLlamada tipo);

//Archivos
void guardar();
void cargar();

//Liberar memoria
void liberarHist(NodoLlamada* cabeza);
void liberarTodo();

//Lo demas XD
const char*tipoToString(TipoLlamada t);
TipoLlamada stringToTipo(const char* str);

//Raylib--

//configuracion ventana
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

//prototipos
void DibMenu();
void DibListaContac();
void DibFormuContac(int esEdicion);
void DibHist(Contacto* c);
void DibFormuLlam(Contacto* c);
void DibConfirmElimin();
void DibMsj();
void MostMsj(const char* texto);
void DibBot(Rectangle rect, const char* texto, Color colorBase, Color colorHover, bool* hover);
bool BotPress(Rectangle rect, bool hover);


//edos. de la app
typedef enum {
    MENU_PRINCIPAL,
    LISTA_CONTACTOS,
    AGREGAR_CONTACTO,
    EDITAR_CONTACTO,
    VER_HISTORIAL,
    REGISTRAR_LLAMADA,
    CONFIRMAR_ELIMINAR,
    MENSAJE
} EdoApp;

typedef enum{
    ENTRANTE, SALIENTE, PERDIDA
}TipLlam;

typedef struct NodoLlam{
    char fecha[11];
    char hora[6];
    int duracion;
    TipLlam tipo;
    struct NodoLlam* sig;
}NodLlam;

typedef struct{
    int id;
    char nombre[100];
    char telefono[20];
    char email[100];
    NodoLlam* hist;
}Contact;

//variables de UI
static EdoApp edoActual=MENU_PRINCIPAL;
static int selectContInd=0;
static int scrollOff=0;
static Contact* contactAct=NULL;
static char inputBuff[256]="";
static int inputMode=0;
static char tempNomb[100]="";
static char tempTel[20]="";
static char tempEmail[100]="";
static char tempFecha[11]="";
static char tempHr[6]="";
static int tempDurac=0;
static int tempTip= 1;
static char mensajeTexto[256]= "";
static double mensajeTiempo=0;


int main(){
   InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Agenda Telefónica Gráfica");
    SetTargetFPS(60);
    InitAudioDevice();

    cargar();

    while (!WindowShouldClose()) {
        if (edoActual == MENSAJE && GetTime() - mensajeTiempo > 2.0) {
            edoActual = MENU_PRINCIPAL;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (edoActual) {
            case MENU_PRINCIPAL: DibMenu(); break;
            case LISTA_CONTACTOS: DibListaContac(); break;
            case AGREGAR_CONTACTO: DibFormuContac(0); break;
            case EDITAR_CONTACTO: DibFormuContac(1); break;
            case VER_HISTORIAL: DibHist(contactoActual); break;
            case REGISTRAR_LLAMADA: DibFormuLlam(contactoActual); break;
            case CONFIRMAR_ELIMINAR: DibConfirmElimin(); break;
            case MENSAJE: DibMsj(); break;
        }

        EndDrawing();
    }

    guardar();
    liberarTodo();
    CloseWindow();
    return 0;
} 

//........Dibujo de Pantallas.........

void DibMenu(){
   Drawrectangle(0, 0, SCREEN_HEIGHT, LIGHTGRAY);
    DrawText("Agenda telefonica", SCREEN_WIDTH/2 - MeasureText("Agenda Telefonica", 40)/2, 50, 40, DARKBLUE);

    int btnW = 200, btnH = 50;
    int startX = SCREEN_WIDTH/2 - btnW/2;
    int y = 150
    static bool hover1=false, hover2=false, hover3=false, hover4=false, hover5=false;

    Rectangle btnLista = {(float)startX, (float)y, (float)btnH};
    DibBot(btnLista, "Ver Contactos", SKYBLUE, BLUE; &hover2);
    if (BotPress(btnLista, hover1)) esdoActual = LISTA_CONTACTOS;

    Rectangle btnAgregar = {(float)startX, (float)(y+70), (float)btnW, (float)btnH};
    DibBot(btnSalir, "Salir", RED, MAROON, &hover5);
    if (BotPress(btnSalir, hover5)){
        guardar();
        ClosetWindow();
        exit(0);
    }  
}

void DibListaContac(){
   DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
    DrawText("Lista de Contactos", 20, 20, 30, DARKBLUE);

    Rectangle btnVolver = {20.0f, (float)(SCREEN_HEIGHT-60), 120.0f, 40.0f};
    bool hoverVolver = false;
    DibBot(btnVolver, "Volver", GRAY; DARKGRAY; &hoverVolver);
    if (BotPress(btnVolver, hoverVolver)) estadoActual = MENU_PRINCIPAL;

    if (numContactos == 0){
        DrawText("No hay contactos registrados.", SCREEN_WIDTH/2 - MeasureText("NO hay contactos registrados.", 20)/2, SCREEN_HEIGHT/2, 20, DARKGRAY);
        return;
    }

    int itemHeight = 50;
    int startY = 80; 
    int visibleItems = (SCREEN_HEIGJT - 140) / itemHeight;
    int maxScroll = numContactos - visibleItems;
    
    if (maxScroll  < 0) maxScroll = 0;
    if(IsKeyPressed(KEY_UP)) scrollOffset--;
    if(IsKeyPressed(KEY_UP)) scrollOffset++;
    if (scrollOffset < 0) scrollOffset = 0;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    for (int =scrollOffset; i<scroll0ffset + visibleItems && i < numCOntactos; i++){
        int y = startY + (i-scrollOffset) * itemHeight;
        Rectangle itemRect = {20.0f, (float)y, (float(SCREEN_WIDTH-40), (float)(itemHeight-2)}
        Color bg = (i==selectedContactIndex) ? SKYBLUE : WITE;
        DrawRectangleRec(itemRect, bg);
        DrawRectangleLinesEx(itemRect, 1, DARKGRAY);
        DrawText(TextFormat("%d. %s", contactos[i]->id, contactos[i]->nombre), 30, y+15, 20, BLACK);

        Rectangle btnHistorial = {(float)(SCREEN_WIDTH-180), (float)(y+5), 70.0f, 40.0f};
        Rectangle btnEditar = {(float)(SCREEN_WIDTH-100), (float)(y+5), 70.0f, 40.0f};
        Rectangle btnEliminar = {(float)(SCREEN_WIDTH-20), (float)(y+5), 70.0f, 40.0f};

        bool hoverH = CheckCollisionPointRec(GetMaousePosition(), btnHistorial);
        bool hoverE = CheckCollisionPointRec(GetMaousePosition(), btnEditar);
        bool hoverD = CheckCollisionPointRec(GetMaousePosition(), btnEliminar);

        DrawRectabgleRec(btnHistorial, hoverH ? GREEN : DARKGREEN); 
        DrawText("Hist", (int)btnHistorial.x+5, (int)btnHistorial.y+12, 15, WHITE);
        DrawRectabgleRec(btnEditar, hoverE ? ORANGE : GOLD); 
        DrawText("Editar", (int)btnEditar.x+5, (int)btnEditar.y+12, 15, WHITE);
        DrawRectabgleRec(btnEliinar, hoverD ? RED : MAROON); 
        DrawText("X", (int)btnEliminarl.x+5, (int)btnEliminar.y+12, 15, WHITE);

        if(hoverH && IsMausePressed(MOUSE_BUTTON_LEFT)){
            contactoActual = contactos[i];
            edoActual = VER_HISTORIAL;
        }
        if(hoverE && IsMauseButtonPressed(MOUSE_BUTTON_LEFT)){
            contactAct = contactos[i];
            strcpy(tempNomb, contacAct->nombre);
            strcpy(tempTel, contactAct->telefono);
            strcpy(tempEmail, contactAct->email);
            edoActual = EDITAR_CONTACTO;
            inputMode = 0;
        }
        if (hoverD && IsMauseButtonPressed(MOUSE_BUTTON_LEFT)){
            contactAct = contactos[i];
            edoAct = CONFIRMAR_ELIMINAR;
        }   
    }//for
}//void

void DibFormuContac(int esEdicion){
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
    DrawText(esEdicion ? "Editar Contacto" : "Nuevo Contactto", 20, 20, 30, DARKBLUE);

    int startY=100, fieldW=400, labelX=100, fieldX=200;

    Drawtext("Nombre: ", labelX, startY, 20, BLACK);
    DrawRectangle((float)fieldX, (float)startY-5, (float)fieldW, 30.0f, WHITE);
    DrawRectangleLines((float)fieldX, (float)startY-5, (float)fieldW, 30.0f, GRAY);
    DrawText(tempNombre, fieldX+5, startY, 20, BLACK);
    if (inputMode == 0){
        DrawRectangleLines((float)fieldX, (float)startY-5, (float)fieldW, 30.0f, RED);
        int key = GetCharPressed();
        while (key > 0){
            if(key >=32 && key <=125 && strlen(tempNomb) < 99){
                int len = strlen(tempNomb); 
                tempNomb[len] = (char)key;
                tempNomb[len+1] = '\0'; 
            }
            key = GetCharPressed();      
        }//whille
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(tempNomb) > 0)
            tempNomb[strlen(tempNomb)-1] = '\0';
    }//if

    DrawText("Telefono: ", labelX, startY+50, 20, BLACK);
    DrawRectangle((float)fieldX, (float)startY+45, (float)fieldW, 30.0f, WHITE);
    DrawRectangleLines((float)fieldX, (float)startY+45, (float)fieldW, 30.0f, GRAY);
    DrawText(tempTel, fieldX+5, startY+50, 20, BLACK);
    if (inputMode == 1){
        DrawRectangleLines((float)fieldX, (float)startY+45, (float)fieldW, 30.0f, RED);
        int key = GetCharPressed();
        while (key > 0){
            if(key >=32 && key <=125 && strlen(tempTel) < 19){
                int len = strlen(tempTel);
                tempTel[len] = (char)key;
                tempTel[len+1] = '\0'; 
            }
            key = GetCharPressed();      
        }//whille
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(tempTel) > 0)
            tempTel[strlen(tempTel)-1] = '\0';
    }//if

    DrawText("Email: ", labelX, startY+100, 20, BLACK);
    DrawRectangle((float)fieldX, (float)startY+95, (float)fieldW, 30.0f, WHITE);
    DrawRectangleLines((float)fieldX, (float)startY+95, (float)fieldW, 30.0f, GRAY);
    DrawText(tempEmail, fieldX+5, startY+100, 20, BLACK);
    if (inputMode == 2){
        DrawRectangleLines((float)fieldX, (float)startY+95, (float)fieldW, 30.0f, RED);
        int key = GetCharPressed();
        while (key > 0){
            if(key >=32 && key <=125 && strlen(tempEmail) < 99){
                int len = strlen(tempEmail);
                tempEmail[len] = (char)key;
                tempEmail[len+1] = '\0'; 
            }
            key = GetCharPressed();      
        }//whille
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(tempEmail) > 0)
            tempEmail[strlen(tempEmail)-1] = '\0';
    }//if

    if(IsKeyPressed(KEY_TAB)){
        inputMode = (inputMode + 1) % 3;
    }

    Rectangle btnGuardar = {200.0f, (float)startY+170, 120.0f, 40.0f};
    Rectangle btnCancelar = {350.0f, (float)startY+170, 120.0f, 40.0f};
    bool hoverG = false, hoverC = false;
    DibBot(btnGuardar, "Guardar", GREEN, DARKGREEN, &hoverG);
    DibBot(btnCancelar, "Cancerlar", RED, MAROON, &hoverC);

    if (BotPress(btnGuardar, hoverG){
        if(esEdicion){
            strcpy(contactAct->nombre, tempNomb);
            strcpy(contactAct->telefono, tempTel);
            strcpy(contactAct->email, tempEmail);
            MostMsj("Contacto actualizado.");
        }else{
            if(numContactos == capacidadContactos){
                capacidadContactos = capacidadContactos==0 ? 5 : capacidadContactos*2;
                contactos = (Contacto**)realloc(contactos, capacidadContactos * sizeof(Contacto*));
            }
            Contacto* nuevo = (Contacto*)malloc(sizeof(Contacto));
            nuevo -> id = numContactos+1;
            strcpy(contactAct->nombre, tempNomb);
            strcpy(contactAct->telefono, tempTel);
            strcpy(contactAct->email, tempEmail);
            nuevo -> historial = NULL;
            contactos[numContactos++] = nuevo;
            MostMsj("Contacto Agregado.");
        }//else
        edoActual = MENU_PRINCIPAL; 
    }//if
    if(BotPress(btnCancelar, hoverC)){
        estadoActual = MENU_PRINCIPAL;
    }    
}//Void

void DibHist(Contacto* c){
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
    DrawText(TextFormat("Historial de %s", c->nombre), 20, 20, 30, DARKBLUE);

    Rectangle btnVolver = {20.0f, (float)(SCREEN_HEIGHT-60), 120.0f, 40.0f};
    Rectangle btnRegistrar = {(float)(SCREEN_WDTH-140), (float)(SCREEN_HEIGHT-60), 120.0f, 40.0f};
    bool hoverV = false, hoverR = false;
    DibBot(btnVolver, "Volver", GRAY, DARKGRAY, &hoverV);
    DibBot(btnRegistrar, "Registrar Llamada", SKYBLUE, BLUE, &hoverR);

    if(BotPress(btnVolver, hoverV)) edoActual = MENU_ACTUAL;
    if(BotPress(btnRegresar, hoverR)) edoActual = REGISTRAR_LLAMADA;

    if(!c->historial){
        DrawText("No hay llamadas registradas.", SCREEN_WIDTH/2 - MeasureText("NO hay llamadas registradas". 20)/2, 200, 20, DARKGRAY);
        return;
    }

    int y = 100;
    NodLlam* aux = c->historial;
    while(aux){
        const char* tipoStr = tipoToString(auz->tipo);
        Color color = aux->tipo==SALIENTE ? GREEN : (aux->tipo==ENTRANTE ? YELLOW : RED);
        DrawText(TextFormat("%s %s Duracion: %d seg Tipo: %s", aux->fecha, aux->hora, auc->duracion, tipoStr), 30, y, 20, color);
        y+=30;
        if(y>SCREEN_HEIGHT-100) break;
        aux = aux->siguiente;
    }//while
}//void

void DibFormuLlam(Contacto* c){
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
    DrawText(TextFormat("Registrar Llamada para %s", c->nombre), 20, 20, 30, DARKBLUE);

    int startY=100, fieldW=300, labelX=150, fieldX=250;

    //FECHA
    DrawText("Fecha (DD-MM-AAAA): ", labelX, startY, 20, BLACK);
    DrawRectangle((float)fieldX, (float)startY-5, (float)fieldW, 30.0f, WHITE);
    DrawRectangleLines((float)fieldX, (float)startY-5, (float)fieldW, 30.0f, GRAY);
    DrawText(tempFecha, fieldX+5, startY, 20, BLACK);
    if(inputMode==3){
       DrawRectangleLines((float)fieldX, (float)startY-5, (float)fieldW, 30.0f, RED);
        int key = GetCharPressed();
        while(key>0){
            if(key>=32 && key<=125 && strlen(tempFecha) < 10){
                int len= strlen(tempFecha);
                tempFecha[len] = (char)key;
                tempFecha[len+1]='\0';
            }
            key = GetCharPressed();
        }//While
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(tempFecha) > 0)
            tempFecha[strlen(tempFecha)-1] = '\0';
    }//if
    
    //HORA
    DrawText("Hora (HH:MM): ", labelX, startY+50, 20, BLACK);
    DrawRectangle((float)fieldX, (float)startY+45, (float)fieldW, 30.0f, WHITE);
    DrawRectangleLines((float)fieldX, (float)startY+45, (float)fieldW, 30.0f, GRAY);
    DrawText(tempHr, fieldX+5, startY+50, 20, BLACK);
    if(inputMode==4){
       DrawRectangleLines((float)fieldX, (float)startY+45, (float)fieldW, 30.0f, RED);
        int key = GetCharPressed();
        while(key>0){
            if(key>=32 && key<=125 && strlen(tempHr) < 5){
                int len= strlen(tempHr);
                tempHr[len] = (char)key;
                tempHr[len+1]='\0';
            }
            key = GetCharPressed();
        }//While
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(tempHr) > 0)
            tempHora[strlen(tempHr)-1] = '\0';
    }//if

    //DURACION
    DrawText("Diracion (segundos): ", labelX, startY+100, 20, BLACK);
    DrawRectangle((float)fieldX, (float)startY+95, (float)fieldW, 30.0f, WHITE);
    DrawRectangleLines((float)fieldX, (float)startY+95, (float)fieldW, 30.0f, GRAY);
    char durBuf[10];
    sprintf(durBuf, "%d", tempDurac);
    DrawText(durBuf, fieldX+5, startY+100, 20, BLACK);
    if(inputMode == 5){
        DrawRectangle((float)fieldX, (float)startY+95, (float)fieldW, 30.0f, RED);
        int key = GetCharPressed();
        if(key >= '0' && key <= '9'){
            tempDurac = tempDurac * 10 + (key - '0');
        }
        if (IsKeyPressed(KEY_BACKSPACE)) tempDurac /= 10;
    }

    //TIPO
    DrawText("Tipo: ", labelX, startY+150, 20, BLACK);
    const char* tipos[] = {"Entrante", "Saliente", "Perdida"};
    for(int i=0; i<3; i++){
        rectangle btnTipo = {(float)(fieldX + i*100), (float)startY+145, 90.0f, 30.0f};
        Color col = (tempTip == i+1) ? BLUE : GRAY; 
        DrawRectangleRec(btnTipo, col);
        DrawText(tipos[i], (int)btnTipo.x+5, (int)btnTipo.y+5, 15, WHITE);
        if (CheckCollisionPointRec(GetMousePosition(), btnTipo) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            tempTip = i+1;
    }

    if(IsKeyPressed(KEY:TAB)){
        inputMode = (inputeMode + 1) % 6;
        if(inputeMode<3) inputMode = 3;
    }

    Rectangle btnGuardar = {200.0f, (float)startY+220, 120.0f, 40.0f};
    Rectangle btnCancelar = {350.0f, (float)startY+220, 120.0f, 40.0f};
    bool haverG = false, hoverC = false;
    DibBot(btnGuardar, "Guardar", GREEN, DARKGREEN, &hoverG);
    DibBot(btnCancelar, "Cancelar", RED, MAROON, &hoverC);

    if(BotPress(btnGuardar, hoverG){
        if(strlen(tempFecha)==10 && strlen(tempHr)==5 && tempDurac>0){
            agregarNodoLlamada(c, tempFecha, tempHr, tempDurac, (TipoLlamada)(TempTipo-1));
            memset(tempFecha, 0, sizeof(tempFecha));
            memset(tempHr, 0, sizeof(tempHora));
            tempDurac = 0;
            MostMsj("LLamada registrada.");
            edoActual = VER_HISTORIAL;
        }else{
            MostMsj("Datos Invalidos.");
        }
    }
    if (BotPress(btnCancelar, hoverC)) {
        edoActual = VER_HISTORIAL;
    }  
}//void

void DibConfirmElimin(){
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    int boxW = 400, boxH=150;
    int x = SCREEN_WIDTH/2 - boxW/2;
    int y = SCREEN_HIGTH/2 - boxH/2; 
    DrawRectangle((float)x, (float)y, (float)boxW, (float)boxH, LIGHTGRAY);
    DrawRectangleLines((float)x, (float)y, (float)boxW, (float)boxH, DARKGRAY);
    DrawText(TextFormat("Eliminar a %s?", contactAct->nombre), x+20, y+30, 20, BLACK);
    Rectangle btnSi = {(float)(x+50), (float)(y+90), 80.0f, 40.0f};
    Rectangle btnNo = {(float)(x+bowX-130), (float)(y+90), 80.0f, 40.0f};
    bool hoverSi=false, hoverNo=false;
    DibBot(btnSi, "Si", RED, MAROON, &hoverSi);
    DibBot(btnNo, "No", GRAY, DARKGRAY, &hoverNo);

    if (BotPress(btnSi, hoverSi){
        for(int i=0; i<numContactos; i++)
            if(contactos[i]->id == contactAct->id) {idx = i; break;}
        if (idx != -1){
            liberarHist(contactos[idx]->historial);
            free(contactos[idx]);
            for(int i=idx; i<numContactos-1; i++) contactos[i] = contactos[i+1];
            numContactos--M
        }//if
        MostMsj("Contacto Eliminado.");
        edoActual =MENU_PRINCIPAL;
    }//if
    if(BotPress(btnNo, hoverNo)){
        edoActual = LISTA_CONTACTOS;
    }
}//void

void DibMsj(){
    DrawRectangle(0, 0, SCREEN_HEIGHT, Fade(BLACK, 0.6f));
    int ancho = MeasureText(mensajeTexto, 20)+40;
    int alto = 50;
    int x = SCREEN_WIDTH/2 - ancho/2;
    int y = SCREEN_HEIGHT/2 - alto/2;
    DrawRectangle((float)x. (float)y, (float)ancho, (float)alto, DARKGRAY);
    DrawText(mensajeTexto, x+20, y+15, 20, WHITE);
}//void

void MostMsj(const char* texto){
    strcpy(mensajeTexto, texto);
    mensajeTiempo = GetTime();
    edoActual = MENSAJE;
}

void DibBot(Rectangle rect, const char* texto, Color coloBase, Color coloHover, bool* hover){
    *hover = CheckCollisionPointRec(GetMousePosition(), rect);
    Color color = *hover ? colorHover : colorBase;
    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 1, DARKGRAY);
    int tx = (int)(rect.x + rect.width/2 - MeasureText(texto, 20)/2);
    int ty = (int)(rect.y + rect.height/2 - 10);
    DrawText(texto, tx, ty, 20, WHITE);
}

bool BotPres(Rectangle rect, bool hover){
    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void agregarNodoLlamada(Contacto* c, const char* fecha, const char* hora, int duracion, TipoLlamada tipo){
    NodoLlamada* nuevo=(NodoLlamada*)malloc(sizeof(NodoLlamada));
    if(!nuevo) return;
    strcpy(nuevo->fecha, fecha);
    strcpy(nuevo->hora, hora);
    nuevo->duracion=duracion;
    nuevo->tipo=tipo;
    nuevo->siguiente=NULL;

    if(c->historialLlam==NULL){
        c->historialLlam=nuevo;
    }else{
        NodoLlamada* aux=c->historialLlam;
        while(aux->siguiente) aux=aux->siguiente;
        aux->siguiente=nuevo;
    }
}

void guardar(){
    FILE* fb=fopen("contactos.dat", "wb");
    if(!fb){
        return;
    }
    fwrite(&numContactos, sizeof(int), 1, fb);
    for(int i=0;i<numContactos;i++){
        Contacto temp=*contactos[i];
        temp.historialLlam=NULL;
        fwrite(&temp, sizeof(Contacto), 1, fb);
    }
    fclose(fb);

    FILE* ft=fopen("historial.txt", "w");
    if(!ft){
        return;
    }
    for(int i=0;i<numContactos;i++){
        NodoLlamada* actual=contactos[i]->historialLlam;
        while(actual){
            fprintf(ft, "%d|%s|%s|%d|%s\n",
                contactos[i]->id, actual->fecha, actual->hora,
                actual->duracion, tipoToString(actual->tipo));
            actual=actual->siguiente;
        }
    }
    fclose(ft);
}

void cargar(){
    FILE* fb=fopen("contactos.dat", "rb");
    char linea[200];

    if(!fb){
        //printf("No se encontro contactos.dat. Iniciando con agenda vaci.\n");
        return;
    }

    int num;
    fread(&num, sizeof(int), 1, fb);
    if(num==0){
        fclse(fb);
        return;
    }

    capacidadContactos=num;
    contactos=(Contacto**)malloc(capacidadContactos * sizeof(Contacto*));

    for(int i=0;i<num;i++){
        Contacto* c=(Contacto*)malloc(sizeof(Contacto));
        if(!c) break;
        fread(c,sizeof(Contacto), 1, fb);
        c->historialLlam=NULL;
        contactos[i]=c;
    }
    numContactos=num;
    fclose(fb);

    FILE* ft=fopen("historial.txt", "r");
    if(!ft){
        //printf("No se encontro historial.txt\n");
        return;
    }

    while(fgets(linea,sizeof(linea),ft)){
        linea[strcspn(linea, "\n")] = '\0';
        int id,duracion;
        char fecha[11], hora[6],tipStr[12];
        if (sscanf(linea, "%d|%10[^|]|%5[^|]|%d|%11s", &id, fecha, hora, &duracion, tipoStr) != 5)
            continue;

        Contacto* c=NULL;
        for(int i=0;i<numContactos;i++){
            if(contactos[i]->id==id){
                c=contactos[i];
                break;
            }
        }

        if(c){
            agregarNodoLlamada(c, fecha, hora, duracion, stringToTipo(tipoStr));
        }
    }
    fclose(ft);
    //printf("Datos cargados: %d contactos\n", numContactos);
}

void liberarHistorial(NodoLlamada* cabeza){
    NodoLlamada* actual=cabeza;
    while(actual){
        NodoLlamada* temp=actual;
        actual=actual->siguiente;
        free(temp);
    }
}

void liberarTodo(){
    for(int i=0;i<numContactos;i++){
        liberarHistorial(contactos[i]->historialLlam);
        free(contactos[i]);
    }
    free(contactos);
}

const char* tipoToString(TipoLlamada t){
    switch(t){
        case ENTRANTE: return "Entrante";
        case SALIENTE: return "Saliente";
        case PERDIDA: return "Perdida";
        default: return "Desconocido";
    }
}

TipoLlamada stringToTipo(const char* str){
    if(strcmp(str, "Entrante")==0) return ENTRANTE;
    if(strcmp(str, "Saliente")==0) return SALIENTE;
    return PERDIDA;
}

