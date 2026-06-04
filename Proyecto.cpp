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
    char ffecha[11];
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
static char tempFech[11]="";
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
        printf("Error al guardar contactos\n");
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
        printf("Error al guardar historial\n");
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
    printf("Datos guardados correctamente\n");
}

void cargar(){
    FILE* fb=fopen("Contactos.dat", "rb");
    if(!fb){
        printf("No se encontr� contactos.dat. Iniciando con agenda vac�a.\n");
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
    if(!contactos){
        fclose(fb);
        printf("Error de memoria\n");
        return;
    }

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
        printf("No se encontr� historial.txt\n");
        return;
    }

    char linea[200];
    while(fgets(linea,sizeof(linea),ft)){
        linea[strcspn(linea, "\n")] = '\0';
        int id;
        char fecha[11], hora[6];
        int duracion;
        char tipoStr[20];
        int parsed=sscanf(linea, "%d|%10[^|]|%5[^|]|%d|%19[^\n]", 
            &id, fecha, hora, &duracion, tipoStr);
        if(parsed==5) continue;

        Contacto* c=buscarContactID(id);
        if(c){
            TipoLlamada tipo=stringToTipo(tipoStr);
            agregarNodoLlamada(c, fecha, hora, duracion, tipo);
        }
    }
    fclose(ft);
     printf("Datos cargados: %d contactos\n", numContactos);
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
