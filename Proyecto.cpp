/* Agenda de contactos con historial de llamadas*/

#include<stdlib.h>
#include<string>
#include<time.h>
#include<stdio.h>

#ifded_WIN32
    #include<windows.h>
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

//ANSi
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"
#define COLOR_BORDE "\033[1;34m"  //azul brillante

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
void mostarMenu();
void limpiarPantalla();
void pausar();

//Contactos
void agregarContacto();
void buscarContacto();
void editarContacto();
void eliminarContacto();
void listarContactos();
Contacto* buscarContactID(int id);

//Historial
void registrarLlamada();
void mostararContactHistorial(Contacto* c);
void agregarNodoLlamada(Contacto* c, const char* fecha,
const char* hora, int duracion, TipoLlamada tipo);

//Archivos
void guardar();
void cargar();

//Liberar memoria
void liberarHistorial(NodoLlamada* cabeza);
void liberarTodo();

//Lo demas XD
const char*tipoToString(TipoLlamada t);
TipoLlamada stringToTipo(const char* str);

int main(){
    cargar();
    int opc;
    do{
        mostrarMenu();
        printf("Seleccione una opcion: ");
        scanf("%d", &opc);
        getchar(); //limpiar buffer
        switch(opc){
            case 1: agregarContacto(); break;
            case 2: buscarContacto(); break;
            case 3: editarContacto(); break;
            case 4: eliminarContacto(); break;
            case 5: listarContactos(); break;
            case 6: registrarLlamada(); break;
            case 7: guardar(); printf(COLOR_GREEN 
                "Datos guardados. Saliendo...\n" COLOR_RESET); break;
            case 8: printf("Saliendo...\n"); break;
            default: printf(COLOR_RED 
                "Opcion invalida. Intente de nuevo.\n");
        }
        if(opc !=8)pausar();
    }while (opc!=8);
    liberarTodo();
    return 0;
} 

void limpiarPantalla(){
    system(CLEAR);
}

void pausar(){
    printf("Presione Enter para continuar...");
    getchar();
}

void mostarMenu(){
    limpiarPantalla();
    printf(COLOR_BORDE
    "==============================\n");
    printf("   AGENDA DE CONTACTOS\n");
    printf("==============================\n" COLOR_RESET);
    printf("1. Agregar Contacto\n");
    printf("2. Buscar Contacto\n");
    printf("3. Editar Contacto\n");
    printf("4. Eliminar Contacto\n");
    printf("5. Listar Contactos\n");
    printf("6. Registrar Llamada a un contacto\n");
    printf("7. Guardar datos (contactos+historial)\n");
    printf("8. Salir\n");
    printf(COLOR_BORDE 
    "==============================\n" COLOR_RESET);
}

void agragarContacto(){
    if(numContactos== capacidadContactos){
        capacidadContactos=(capacidadContactos==0)?5: 
        capacidadContactos *2;
        Contacto** temp=(Contacto**)realloc(contactos,
        capacidadContactos * sizeof(Contacto*));
        if(!temp){{
            printf("Error de memoria\n");
            return;
        }
        contactos=temp;
    }
    Contacto* nuevo=(Contacto*)malloc(sizeof(Contacto));
    if(!nuevo){
        printf("Error de memoria\n");
        return;
    }
    nuevo->id =numContactos+1;
    nuevo->historialLlam=NULL;
    printf("Nuevo contacto id: %d\n", nuevo->id);
    
    printf("Nombre: ");
    fgets(nuevo->nombre, sizeof(nuevo->nombre), stdin);
    nuevo->nombre[strcspn(nuevo->nombre, "\n")] = 0;
    
    printf("Telefono: ");
    fgets(nuevo->telefono, sizeof(nuevo->telefono), stdin);
    nuevo->telefono[strcspn(nuevo->telefono, "\n")] = 0;
    
    printf("Email: ");
    fgets(nuevo->email, sizeof(nuevo->email), stdin);
    nuevo->email[strcspn(nuevo->email, "\n")] = 0;
    contactos[numContactos++] = nuevo;
    
    printf("Contacto agregado extiosamente\n");
    }
}

void buscarContacto(){
    int opc;
    printf("Buscar por:\n1. ID \n2. Nombre\n");
    scanf("%d", &opc);
    getchar();

    if(opc==1){
        int id;
        printf("ID: ");
        scanf("%d", &id);
        getchar();
        Contacto* c=buscarContactID(id);
        if(c){
            mostrarContactHistorial(c);
        } else {
            printf("Contacto no encontrado\n");
        }
    }else if(opc==2){
        char nombre[100];
        printf("Nombre: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';
        int encontrado=0;
        for(int i=0;i<numContactos:i++){
            if(strstr(contactos[i]->nombre, nombre)!=NULL){
                mostrarContactHistorial(contactos[i]);
                encontrado=1;
                printf("\n");
            }
        }
        if(!encontrado) printf("No se encontraron contactos con ese nombre\n");
    }else{
        printf("Opcion invalida\n");
    }
}

void editarContacto(){
    int id;
    printf("ID del contacto a editar: ");
    scanf("%d", &id);
    getchar();
    Contacto* c=buscarContactID(id);
    if(!c){
        printf("Contacto no encontrado\n");
        return;
    }

    printf("Deje en blanco para mantener el valor actual\n");
    char buffer[100];

    printf("Nombre [%s]: ", c->nombre);
    fgets(buffer, sizeof(buffer), stdin);
    if(buffer[0]!='\n'){
        buffer[strcspn(buffer, "\n")] = '\0';
        strcpy(c->nombre, buffer);
    }

    printf("Telefono [%s]: ", c->telefono);
    fgets(buffer, sizeof(buffer), stdin);
    if(buffer[0]!='\n'){
        buffer[strcspn(buffer, "\n")] = '\0';
        strcpy(c->telefono, buffer);
    }

    printf("Email [%s]: ", c->email);
    fgets(buffer, sizeof(buffer), stdin);
    if(buffer[0]!='\n'){
        buffer[strcspn(buffer, "\n")] = '\0';
        strcpy(c->email, buffer);
    }

    printf("Contacto actualizado.\n");
}

void eliminarContacto(){
    int id;
    printf("ID del contacto a eliminar: ");
    scanf("%d", &id);
    getchar();
    int indice=-1;
    for(int i=0;i<numContactos;i++){
        if(contactos[i]->id==id){
            indice=i;
            break;
        }
    }
    if(indice==-1){
        printf("Contacto no encontrado\n");
        return;
    }
    liberarHistorial(contactos[indice]->historialLlam);
    free(contactos[indice]);
    for(int i=indice;i<numContactos-1;i++){
        contactos[i]=contactos[i+1];
    }
    numContactos--;
    printf("Contacto eliminado\n");
}

void listarContactos(){
    if(numContactos==0){
        printf("No hay contactos\n");
        return;
    }
    for(int i=0;i<numContactos;i++){
        printf(COLOR_BORDE "+----------------------------------+\n" COLOR_RESET);
        printf(" ID: %d\n", contactos[i]->id);
        printf(" Nombre: %s\n", contactos[i]->nombre);
        printf(" Telefono: %s\n", contactos[i]->telefono);
        printf(" Email: %s\n", contactos[i]->email);
        printf(COLOR_BORDE "+----------------------------------+\n" COLOR_RESET);
        printf("\n");
    }
}

Contacto* buscarContactID(int id){
    for(int i=0;i<numContactos;i++){
        if(contactos[i]->id==id)
            return contactos[i];
    }
    return NULL;
}

void registrarLlamada(){
    int id;
    printf("ID del contacto: ");
    scanf("%d", &id);
    getchar();
    Contacto* c=buscarContactID(id);
    if(!c){
        printf("Contacto no encontrado.\n");
        return;
    }
    char fecha[11], hora[6];
    int duracion, tipoInt;
    printf("Fecha (AAAA-MM-DD): ");
    fgets(fecha, sizeof(fecha), stdin);
    fecha[strcspn(fecha, "\n")] = '\0';
    printf("Hora (HH:MM): ");
    fgets(hora, sizeof(hora), stdin);
    hora[strcspn(hora, "\n")] = '\0';
    printf("Duracion (segundos): ");
    scanf("%d", &duracion);
    getchar();
    printf("Tipo: 1=Entrante, 2=Saliente, 3=Perdida: ");
    scanf("%d", &tipoInt);
    getchar();
    TipoLlamada tipo;
    switch(tipoInt){
        case 1: tipo=ENTRANTE; break;
        case 2: tipo=SALIENTE; break;
        case 3: tipo=PERDIDA; break;
        default: printf("Tipo invalido, se asigna Perdida.\n"); tipo=PERDIDA;
    }
    agregarNodoLlamada(c, fecha, hora, duracion, tipo);
    printf("Llamada registrada.\n");
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

void mostrarContactHistorial(Contacto* c){
    if(!c) return;

    //Tarjeta del contacto
    printf(COLOR_BORDE "+----------------------------------+\n");
    printf("|            CONTACTO                 |\n");
    printf("+----------------------------------+\n");
    printf("| ID        : %d\n", c->id);
    printf("| Nombre    : %s\n", c->nombre);
    printf("| Telefono  : %s\n", c->telefono);
    printf("| Email     : %s\n", c->email);
    printf(COLOR_BORDE "+----------------------------------+\n" COLOR_RESET);

    if(c->historialLlam==NULL){
        printf(" No hay llamadas registradas.\n");
        return;
    }
    printf("\n Historial de llamadas:\n");
    printf(" %-12s %-6s %-8s %-12s\n", "Fecha", "Hora", "Duracion", "Tipo");
    printf(" ------------ ------ -------- ------------\n");

    NodoLlamada* actual=c->historialLlam;
    while(actual){
        const char* color;
        switch(actual->tipo){
            case SALIENTE: color=COLOR_VERDE; break;
            case PERDIDA: color=COLOR_ROJO; break;
            default: color=COLOR_AMARILLO; break;
        }
        printf(" %s%-12s %-6s %-8d %-12s%s\n",
        color, actual->fecha, actual->hora, actual->duracion,
        tipoToString(actual->tipo), COLOR_RESET);
        actual=actual->siguiente;
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
