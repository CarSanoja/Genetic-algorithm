#include <stdio.h>
#include <string.h>
                       
#include <memory>
#include "ag.h"


// *****************************************************************************
// definiciones o constantes OBLIGATORIAS
// TODO: Modificar las constantes con ?? a valores que permitan ejecutar en forma correcta el programa.
#define N_CAJAS                 3                                       // Número de knapsacks o cajas a llenar
#define N_TIPO_OBJETOS          5                                       // Número clases de objetos existentes
#define N_BYTES                 30                                   // 3 cajas, 5 objetos, max 3 objetos pero variable
#define MAX_EDAD_ESTANCADO      10200                                     // ??
#define MAX_EDAD                10300                                     // ??
#define N_INDIVIDUOS            30                                      // ??

// Porque es un fastidio tener que estar escribiendo a cada rato "unsigned char blah"
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int  uint;

// definir un struct para guardar el cálculo de peso, costo y volumen total de todas las cajas
struct  PVC_t{
    uint PesoTotal[N_CAJAS];             // Peso total de cada caja
    uint CostoTotal[N_CAJAS];            // Costo total de cada caja
    uint VolumenTotal[N_CAJAS];          // Volumen total de cada caja
    uint ObjetosUsados[N_TIPO_OBJETOS];  // Numero total de elementos utilizados de cada tipo
};

// *****************************************************************************
// Variables globales.
// Pueden usar estos o modificarlos a su antojo
int             Costo[N_TIPO_OBJETOS]           = {  6,  8, 12,  9,  3 };   // Lista de costos
int             Peso[N_TIPO_OBJETOS]            = {  9,  2,  4,  5,  3 };   // Lista de Pesos
int             Volumen[N_TIPO_OBJETOS]         = { 13, 11,  8, 10,  3 };   // Lista de Volumenes
int             Disponibilidad[N_TIPO_OBJETOS]  = {  2,  2,  2,  2,  2 };   // Lista de disponibilidad de cada elemento
int             MaximoPesoCajas[N_CAJAS]        = { 18, 12, 15 };           // Lista con el peso máximo que aguanta cada caja
int             MaximoVolumenCajas[N_CAJAS]     = { 20, 33, 17 };           // Lista con el volumen máximo que aguanta cada caja

// *****************************************************************************
// DECLARAR funciones.
float           Evaluacion              (uchar *cromosoma);                         // Funcion para evaluar a cada individuo.
int            DecodificarCromosoma    (uchar *cromosoma, int caja, int objeto);   // Decodificar el cromosoma.
PVC_t           CalcularPesoVolumenCosto(uchar *cromosoma);                         // Calcula el peso, costo y volumen de todas las cajas
void            ImprimirCromosoma       (uchar *cromosoma);                         // Función utilizada para imprimir el cromosoma
void            ImprimirEstadisticas    (std::unique_ptr<Genetic> &ag);                       // Utilizada para imprimir información acerca de la solución encontrada

/*
 ** ----------------------------------------------------------------------------
 **     Nombre      : main
 **     Función     : Punto de entrada del programa
 **     Parámetros  : Ninguno
 **     Retorna     : Código de error
 ** ----------------------------------------------------------------------------
 */
int main()
{
    std::unique_ptr<Genetic> ag = std::make_unique<Genetic>();   // Instancia del AG
    FILE *          archivo;                    // Archivo para guardar la evolución de la población
    int             edadEstancado = 0;          // ?? -> TODO: Diga para que sirven
    float           fitnessAnterior = 0.0f;     // ??

    std::shared_ptr<Cromosoma> ap = std::make_shared<Cromosoma>(); // para imprimir estadisticas del mejor

    //ag->InicializarSemillaRand(true);
    ag->RandPopulation(N_INDIVIDUOS, N_BYTES);
    ag->FunctionEval(Evaluacion);
    ag->dTasaMutacion = 0.01;
    ag->usarElitismo = true;
    ag->usarHarem = false;

    // crear archivo de excel
    archivo = fopen("ag_patrones.xls", "w+");
    fprintf(archivo, "Mejor:\tPoblacion:\n");   // \n\r para windows

    do{
        ag->Generation();
        ap = ag->SelectBetter();

        if( !( ag->Age() % 100 ) ){
            printf("[%5ld]\tMejor: %.3f\tPoblacion: %.3f\t", ag->Age(), ap->Fitness(), ag->Fitness());
            ImprimirCromosoma(ap->_Cromosoma);
        }
        fprintf (archivo, "%.3f\t%.3f\n", ap->Fitness(), ag->Fitness());

        if( abs( fitnessAnterior - ap->Fitness() ) < 0.1 ){ // MODIFICAR ?
            edadEstancado++;
        }else{
            fitnessAnterior = ap->Fitness();
            edadEstancado = 0;
        }
    }while(ag->Age() < MAX_EDAD && edadEstancado < MAX_EDAD_ESTANCADO);

    printf ("\n\tGeneracion final: %ld\n\n", ag->Age());
    fclose(archivo);
    ImprimirCromosoma(ap->_Cromosoma);
    ImprimirEstadisticas(ag);

    return 0;
}

// DEFINICION de funciones
/*
 ** ----------------------------------------------------------------------------
 **     Nombre      : Evaluacion
 **     Función     : ??
 **     Parámetros  : ??
 **     Retorna     : ??
 ** ----------------------------------------------------------------------------
 */
float Evaluacion(uchar* cromosoma)
{
    float puntos = 0;
    /*
    struct  PVC_t{
    uint PesoTotal[N_CAJAS];             // Peso total de cada caja
    uint CostoTotal[N_CAJAS];            // Costo total de cada caja
    uint VolumenTotal[N_CAJAS];          // Volumen total de cada caja
    uint ObjetosUsados[N_TIPO_OBJETOS];  // Numero total de elementos utilizados de cada tipo
    }; 
    
    
    // Declarar variables para guardar las variables de restriccion
    for(int i = 0; i < N_CAJAS ; i++){ // iterar entre cajas
        // reiniciar los valores cada vez que se analiza una caja
        temp.CostoTotal[i] = 0;
        temp.PesoTotal[i] = 0;
        temp.VolumenTotal[i] = 0;
        for(int k = 0; k < N_TIPO_OBJETOS ; k++){ // Se recorren todos los objetos
            if(i == 0) temp.ObjetosUsados[k] = 0;
            temp.CostoTotal[i]   = temp.CostoTotal[i] + (int)DecodificarCromosoma(cromosoma, i,k)*Costo[k];
            temp.PesoTotal[i]    = temp.PesoTotal[i]  + (int)DecodificarCromosoma(cromosoma, i,k)*Peso[k];
            temp.VolumenTotal[i]  = temp.VolumenTotal[i] + (int)DecodificarCromosoma(cromosoma, i,k)*Volumen[k];
            temp.ObjetosUsados[k] = temp.ObjetosUsados[k] + (int)DecodificarCromosoma(cromosoma, i,k);
        }
        printf("\n Costo = %3d\tPeso = %3d\tVolumen = %3d \n", temp.CostoTotal[i], temp.PesoTotal[i], temp.VolumenTotal[i]);
    }*/
    // Ahora se evaluan las restricciones de cada caja
    PVC_t temp = CalcularPesoVolumenCosto(cromosoma);
    for(int i = 0; i < N_CAJAS ; i++){
        if(temp.PesoTotal[i] > MaximoPesoCajas[i]){
            temp.CostoTotal[i] *= 0.2;
        }
        if(temp.VolumenTotal[i] > MaximoVolumenCajas[i]){
            temp.CostoTotal[i] *= 0.2;
        }
        puntos = puntos + temp.CostoTotal[i];
    }
    // Restriccion de cantidad de objetos en la misma caja
    for(int i = 0; i < N_TIPO_OBJETOS ; i++)
        if(temp.ObjetosUsados[i] > Disponibilidad[i]){
            puntos *= 0.1;
        }
    
    return puntos;

}

/*
 ** ----------------------------------------------------------------------------
 **     Nombre      : DecodificarCromosoma
 **     Función     : ??
 **     Parámetros  : ??
 **     Retorna     : ??
 ** ----------------------------------------------------------------------------
 */
int DecodificarCromosoma(uchar *cromosoma, int caja, int objeto)
{
   //*******************************************************
   // CASO 3
   //*******************************************************

    if (caja==0)
   {
       if(objeto==0){
           if(cromosoma[objeto]> 128 && cromosoma[objeto+1]> 128){ return 3;}
            else if (cromosoma[objeto]> 128 && cromosoma[objeto+1] < 128 ){return 2;}
            else if (cromosoma[objeto] < 128 && cromosoma[objeto+1]> 128 ){return 1;}
            else{return 0;}
       }
       else
       {
           if(cromosoma[2*objeto]> 128 && cromosoma[2*objeto+1]> 128){ return 3;}
            else if (cromosoma[2*objeto]> 128 && cromosoma[2*objeto+1] < 128 ){return 2;}
            else if (cromosoma[2*objeto] < 128 && cromosoma[2*objeto+1]> 128 ){return 1;}
            else{return 0;}
       }
   }
   else if (caja==1)
   {
       if(objeto==0){
           if(cromosoma[10+objeto]> 128 && cromosoma[10+objeto+1]> 128){ return 3;}
            else if (cromosoma[10+objeto]> 128 && cromosoma[10+objeto+1] < 128 ){return 2;}
            else if (cromosoma[10+objeto] < 128 && cromosoma[10+objeto+1]> 128 ){return 1;}
            else{return 0;}
       }
       else
       {
           if(cromosoma[10+ 2*objeto]> 128 && cromosoma[10+ 2*objeto+1]> 128){ return 3;}
            else if (cromosoma[10+ 2*objeto]> 128 && cromosoma[10+ 2*objeto+1] < 128 ){return 2;}
            else if (cromosoma[10+ 2*objeto] < 128 && cromosoma[10+ 2*objeto+1]> 128 ){return 1;}
            else{return 0;}
       }
   }
   else
   {
       if(objeto==0){
           if(cromosoma[20+objeto]> 128 && cromosoma[20+objeto+1]> 128){ return 3;}
            else if (cromosoma[20+objeto]> 128 && cromosoma[20+objeto+1] < 128 ){return 2;}
            else if (cromosoma[20+objeto] < 128 && cromosoma[20+objeto+1]> 128 ){return 1;}
            else{return 0;}
       }
       else
       {
           if(cromosoma[20+ 2*objeto]> 128 && cromosoma[20+2*objeto+1]> 128){ return 3;}
            else if (cromosoma[20+ 2*objeto]> 128 && cromosoma[20+ 2*objeto+1] < 128 ){return 2;}
            else if (cromosoma[20+ 2*objeto] < 128 && cromosoma[20+ 2*objeto+1]> 128 ){return 1;}
            else{return 0;}
       }
   }

   //*******************************************************
   // CASO 2
   //*******************************************************
   /*if (caja==0)
   {
       if(cromosoma[objeto]> 170){ return 2;}
        else if (cromosoma[objeto]> 85){return 1;}
        else{return 0;}
   }
   else if (caja==1)
   {
       if(cromosoma[5+objeto]> 170){return 2;}
        else if (cromosoma[5+objeto]> 85){return 1;}
        else{return 0;}
   }
   else
   {
       if(cromosoma[10+objeto]> 170){return 2;}
        else if (cromosoma[10+objeto]> 85){return 1;}
        else{return 0;}
   }*/
   //*******************************************************
   // CASO 1
   //*******************************************************
    /*if (caja==0)
   {
       if(cromosoma[objeto]> 128){
            return 1;
        }
        else
        {
            return 0;
        }
   }
   else if (caja==1)
   {
       if(cromosoma[5+objeto]> 128){
            return 1;
        }
        else
        {
            return 0;
        }
   }
   else
   {
       if(cromosoma[10+objeto]> 128){
            return 1;
        }
        else
        {
            return 0;
        }
   }*/
}

/*
 ** ----------------------------------------------------------------------------
 **     Nombre      : CalcularPesoVolumenCosto
 **     Función     : ??
 **     Parámetros  : ??
 **     Retorna     : Nada
 ** ----------------------------------------------------------------------------
 */
PVC_t CalcularPesoVolumenCosto(uchar *cromosoma)
{
    /*
    struct  PVC_t{
    uint PesoTotal[N_CAJAS];             // Peso total de cada caja
    uint CostoTotal[N_CAJAS];            // Costo total de cada caja
    uint VolumenTotal[N_CAJAS];          // Volumen total de cada caja
    uint ObjetosUsados[N_TIPO_OBJETOS];  // Numero total de elementos utilizados de cada tipo
    }; 
    */
    PVC_t temp;

   // Declarar variables para guardar las variables de restriccion
    for(int i = 0; i < N_CAJAS ; i++){ // iterar entre cajas
        // reiniciar los valores cada vez que se analiza una caja
        temp.CostoTotal[i] = 0;
        temp.PesoTotal[i] = 0;
        temp.VolumenTotal[i] = 0;
        for(int k = 0; k < N_TIPO_OBJETOS ; k++){ // Se recorren todos los objetos
            if(i == 0) temp.ObjetosUsados[k] = 0;
            temp.CostoTotal[i]   = temp.CostoTotal[i] + (int)DecodificarCromosoma(cromosoma, i,k)*Costo[k];
            temp.PesoTotal[i]    = temp.PesoTotal[i]  + (int)DecodificarCromosoma(cromosoma, i,k)*Peso[k];
            temp.VolumenTotal[i]  = temp.VolumenTotal[i] + (int)DecodificarCromosoma(cromosoma, i,k)*Volumen[k];
            temp.ObjetosUsados[k] = temp.ObjetosUsados[k] + (int)DecodificarCromosoma(cromosoma, i,k);
            //printf("\n \tCantidadObjetos = %3i \tCromosoma = %i\n",temp.ObjetosUsados[k],(int)DecodificarCromosoma(cromosoma, i,k));
        }
        //printf("\n Costo = %3d\tPeso = %3d\tVolumen = %3d \n", temp.CostoTotal[i], temp.PesoTotal[i], temp.VolumenTotal[i]);
    }

    return temp;
}

/*
 ** ----------------------------------------------------------------------------
 **     Nombre      : ImprimirCromosoma
 **     Función     : ??
 **     Parámetros  : ??
 **     Retorna     : Nada
 ** ----------------------------------------------------------------------------
 */
void ImprimirCromosoma(uchar *cromosoma)
{
    printf("Cromosoma**:\t");

    /*for(int i = 0; i < N_CAJAS; i++)
    {
        for(int k = 0; k < N_TIPO_OBJETOS; k++)
        {
            printf("%i",DecodificarCromosoma(cromosoma,i,k));
        }
    }*/
    for(int i = 0; i < 30; i++)
    {
        if(i==10){printf("\t");}
        else if (i==20){printf("\t");}
        if(cromosoma[i]<128){printf("%i",0);}
        else
        {
            printf("%i",1);
        } 
        
    }
    printf("\n");
}

/*
 ** ----------------------------------------------------------------------------
 **     Nombre      : ImprimirEstadisticas
 **     Función     : Imprimir en consola, información acerca de la solución
 **                   obtenida
 **     Parámetros  : Nada
 **     Retorna     : Nada
 ** ----------------------------------------------------------------------------
 */
void ImprimirEstadisticas(std::unique_ptr<Genetic> &ag)
{
    std::shared_ptr<Cromosoma> ap = ag->SelectBetter();
    printf("Fianl Information:");
    printf("\n\tBest:\t\t%.3f\n\tPopulation:\t%.3f\n\t", ap->Fitness(), ag->Fitness());

    ImprimirCromosoma(ap->_Cromosoma);
    PVC_t temp = CalcularPesoVolumenCosto(ap->_Cromosoma);

    // datos utilizados
    printf("\n\nLists of weights, Costs and volumens:\n");
    printf("\tObject:\t\t");
    for (int i = 0; i < N_TIPO_OBJETOS; i++)
        printf ("[%2d]\t", i);
    printf ("\n\tCosts:\t\t");
    for (int i = 0; i < N_TIPO_OBJETOS; i++)
        printf (" %2d \t", Costo[i]);
    printf("\n\tWeights:\t");
    for (int i = 0; i < N_TIPO_OBJETOS; i++)
        printf (" %2d \t", Peso[i]);
    printf ("\n\tVolumens:\t");
    for (int i = 0; i < N_TIPO_OBJETOS; i++)
        printf (" %2d \t", Volumen[i]);

    // imprimir las restricciones
    printf("\n\nConstraints:\n");
    printf("\tBoxs:\t\t");
    for (int i = 0; i < N_CAJAS; i++)
        printf ("[%2d]\t", i);
    printf("\n\tWeight:\t\t");
    for (int i = 0; i < N_CAJAS; i++)
        printf (" %2d \t", MaximoPesoCajas[i]);
    printf("\n\tVolumen:\t");
    for (int i = 0; i < N_CAJAS; i++)
        printf (" %2d \t", MaximoVolumenCajas[i]);
    printf("\n\n\tObject:\t\t");
    for (int i = 0; i < N_TIPO_OBJETOS; i++)
        printf ("[%2d]\t", i);
    printf("\n\tAvailability:\t");
    for (int i = 0; i < N_TIPO_OBJETOS; i++)
        printf (" %2d \t", Disponibilidad[i]);

    // imprimir la solución
    printf("\n\nFinal solution:\n");
    printf("\tObject:\t\t");
    for (int i = 0; i < N_TIPO_OBJETOS; i++)
        printf ("[%2d]\t", i);
    for(int i = 0; i < N_CAJAS; i++){
        printf("\n\tBox[%2d]:\t", i);

        for(int j = 0; j < N_TIPO_OBJETOS; j++){
            if(DecodificarCromosoma(ap->_Cromosoma, i, j)==1)
                printf("  o \t");
            else if (DecodificarCromosoma(ap->_Cromosoma, i, j)==2)
            {
                printf("  oo \t");
            }
            else if (DecodificarCromosoma(ap->_Cromosoma, i, j)==3)
            {
                printf("  ooo \t");
            }
            else
                printf("    \t");
        }
        printf("Cost = %3d\tWeight = %3d\tVolumen = %3d", temp.CostoTotal[i], temp.PesoTotal[i], temp.VolumenTotal[i]);
    }
    printf("\n\tTotal usaged:");
    for (int i = 0; i < N_TIPO_OBJETOS; i++)
        printf (" %2u \t", temp.ObjetosUsados[i]);
    printf("\n\n");
}
/*
 ** ############################################################################
 ** Fin del archivo
 ** ############################################################################
 */

