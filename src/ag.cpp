#include <iostream>
#include <random>
#include <algorithm>
#include <memory>
#include <stdio.h>
#include <string.h>

#include "time.h"
#include "ag.h"

Cromosoma::Cromosoma()
{
    //std::cout << "Cromosoma Constructor" << std::endl;
    iLongitud   =   0;
    dFitness    =   0;	//inicialmente cada cromosoma tiene fitness y longitud cero
}

Cromosoma::~Cromosoma() 
{
    
}

bool Cromosoma::Delete() //elimino si existe anterior
{
    if (iLongitud){                 //si tenia algo, borro
        delete[] _Cromosoma;           //libero memoria
        dFitness = 0;               //fitness igualo a cero
        iLongitud = 0;              //hago tamaño igual a cero
        //std::cout << "DELETE EXITOSO" << std::endl;
        return true;
    }
    else {
        return false;               //no habia nada que borrar
    }
}

bool Cromosoma::Create(int nBytes)          //elimino si existe anterior, creo cromosoma de tamaño 'n'
{
    _Cromosoma = new unsigned char[nBytes]();  //creo cromosoma de nuevo tamaño
    //std::unique_ptr<unsigned char[]> _Cromosoma( new unsigned char[nBytes]() );
    iLongitud = nBytes;                         //asigno el tamaño (parametro de entrada)
    dFitness  = 0;                               //fitness inicial igual a cero
    return true;
}

float Cromosoma::Fitness()
{
    return dFitness;
}

int Cromosoma::Length()
{
    return iLongitud;
}

int Cromosoma::Mutate(float rate)
{
	//para Mutate bit a bit todo el cromosoma, creo una mascara con 1 y 0 con una tasa de probabilidad dada
	register unsigned long i;
	register unsigned char mask;
	register unsigned char c;

    // intentar re-escribir esto de forma mucho menos oscura...
	for (i = 0; i < iLongitud; i++){
        mask = 0;                            //mascara vacia
		c = 0x01;                                 //mascara con bit menos significativo (que ire desplazando a la izquierda)
		for (int f = 0; f < 8; c += c, f++){          //Mascara con probabilidad uniforme a nivel de bits
            if ((float)(rand() % 1000)/1000 < rate)
                mask+= c;                   //lanzo dado con probabilidad dada por 'tasa', para poner en 1 o 0 el bit 'f' de la mascara
        }
        *(_Cromosoma + i) = *(_Cromosoma + i) ^ mask; //aplico la mascara generada para el byte 'i'
    }
	return true;
}

int Cromosoma::Mutate()
{
    register unsigned char *ap;

    ap = _Cromosoma + (rand() % iLongitud);//determino sobre cual byte opero
    *ap = *ap ^ (unsigned char)(1 << (rand() % 8)); //mascar con un byte (un solo bit en uno)
    return 0;
}

/*
**************************************************************************************
*/

Genetic::Genetic() //constructor
{
    std::cout << "Genetic Constructor" << std::endl;
    dTasaMutacion = AG_MUTACION_DEFAULT; //valor por defecto asignado a la tasa de mutacion
    iSizePopulationA = iSizePopulationB = 0; //Populationes vacias
    ulAge = 0; //edad inicial = 0 (esta comenzando)
    usarElitismo = false;
    usarHarem = false;
    fraccionHarem = 0.01; //1% de la Population por harem
}

Genetic::Genetic(const Genetic & source)
{
    std::cout << "Genetic Copy Constructor " << std::endl;
     dTasaMutacion = source.dTasaMutacion ; //valor por defecto asignado a la tasa de mutacion
    iSizePopulationA = iSizePopulationB = source.iSizePopulationA; //Populationes vacias
    ulAge = source.ulAge; //edad inicial = 0 (esta comenzando)
    usarElitismo = source.usarElitismo;
    usarHarem = source.usarHarem;
    fraccionHarem = source.fraccionHarem;
}

Genetic::Genetic(Genetic && source)
{
    std::cout << "Genetic Copy Constructor " << std::endl;
     dTasaMutacion = source.dTasaMutacion ; //valor por defecto asignado a la tasa de mutacion
    iSizePopulationA = iSizePopulationB = source.iSizePopulationA; //Populationes vacias
    ulAge = source.ulAge; //edad inicial = 0 (esta comenzando)
    usarElitismo = source.usarElitismo;
    usarHarem = source.usarHarem;
    fraccionHarem = source.fraccionHarem;
    // reset source info
    source.dTasaMutacion = AG_MUTACION_DEFAULT; //valor por defecto asignado a la tasa de mutacion
    source.iSizePopulationA = iSizePopulationB = 0; //Populationes vacias
    source.ulAge = 0; //edad inicial = 0 (esta comenzando)
    source.usarElitismo = false;
    source.usarHarem = false;
    source.fraccionHarem = 0.01; //1% de la Population por harem
}

void Genetic::InicializarSemillaRand(bool usarSemilla)
{
    if(usarSemilla)
        srand( time(NULL) );
}

int Genetic::FreePopulation()
{
    //Devuelve el espacio libre en la Population A = diferencia en Population A - Population B
    return iSizePopulationA - iSizePopulationB;
}

Genetic::~Genetic() //destructor
{
    std::cout << "Genetic Destructor" << std::endl;
}

unsigned long Genetic::Age()
{
    return ulAge;
}

int Genetic::CrossingPoint()
{
    return iCrossingPoint;
}

int Genetic::CrossingPoint(unsigned int n)
{
    if (n < 1)
        n = 1;
    iCrossingPoint = n;
    return iCrossingPoint;
}

float Genetic::MayorFitness()
{
    return SelectBetter()->dFitness;
}

int Genetic::FunctionEval(FuncionFitness_t funcion)
{
    //std::cout << "START FUNCTION EVAL" << std::endl;
    fnEvaluarCromosoma = funcion;
    //std::cout << "END FUNCTION EVAL" << std::endl;
    return 0;
}

void Genetic::RandPopulation(int ini, int lon)
{
    //si habia alguna Population anteriormente, la elimino
    //std::cout << "INIT RAND" << std::endl;
    iSizePopulationB = iSizePopulationA = 0; //esta todo vacio

    for (int i=0;i<ini;i++)
    {
        PopulationB[iSizePopulationB] = std::make_unique<Cromosoma>(); // creo un nuevo cromosoma en Population B, no importa el contenido
        PopulationB[iSizePopulationB]->Create(lon);          //de longitud dada, para reserva de espacio necesario en copias
        iSizePopulationB++;                                //incremento contador de individuos (tamaño de la Population)

        PopulationA[iSizePopulationA] = std::make_unique<Cromosoma>(); // creo un nuevo cromosoma
        PopulationA[iSizePopulationA]->Create(lon);          //de longitud dada
        PopulationA[iSizePopulationA]->Mutate((float)(random() % 1000)/1000); //con datos aleatorios, por operador de mutacion
        PopulationA[iSizePopulationA]->dFitness = 0;        //no valen nada
        iSizePopulationA++;                                //incremento Population
    }
    ulAge = 0; //Edad de la Population actual = 0 (acabo de Createla toda nueva)
    //std::cout << "END RAND" << std::endl;
}

int Cromosoma::Clonate(std::shared_ptr<Cromosoma> &copia)
{
    if (!iLongitud)
        return -1;
    if (iLongitud != copia->iLongitud){   //son identicos los contenedores de datos?
        copia->Delete();                //libero espacio en copia por diferencia de espacio
        copia->Create(iLongitud);        //redimensiono a mismo tamaño del original
    }
    copia->dFitness = dFitness;                     //copia del fitness
    //std::cout << "COPIANDO CROMOSOMA EN CLONATE CON MEMCPY" << std::endl;
    //std::cout << "Cromosoma source: " << (unsigned long)_Cromosoma << std::endl;
    //std::cout << "USE COUNT IS:  " << copia.use_count() << std::endl;
    //std::cout << "Cromosoma dest: " << (unsigned long)copia->_Cromosoma << std::endl;
    memcpy(copia->_Cromosoma,  _Cromosoma, iLongitud); //transferencia de datos, copia exacta
    //copia->_Cromosoma.reset( new (unsigned char *)(_Cromosoma.get()) );
    //std::cout << "Verificar el cromosoma dest: " << (unsigned long)copia->_Cromosoma << std::endl;
    //std::cout << "SALIENDO DE CLONATE" << std::endl;
    return iLongitud;                               //devuelvo tamaño de copia
}

int Genetic::InsertPopulationB (std::shared_ptr<Cromosoma> &ap)
{
    //std::cout << "ESTOY EN INSERTAR POPULATION" << std::endl;
    if (iSizePopulationA <= iSizePopulationB)
        return -1; //no hay mas espacio
    //std::cout << "CLONATE IN POPULATION" << std::endl;
    //std::cout << iSizePopulationB << std::endl;
    ap->Clonate(PopulationB[iSizePopulationB]); //hago una copia exacta del individuo en la poblacion
    return iSizePopulationB++; //devuelvo donde lo almaceno
}

std::shared_ptr<Cromosoma> Genetic::SelectBetter()
{

    if (!iSizePopulationA)
    {
        //std::cout << "SELECT BETTER RETURN NULL" << std::endl;
        return NULL;
    }

    float max = PopulationA[0]->dFitness; //pos '0'
    int cual = 0;
    for (int i = 0; i < iSizePopulationA; i++){
        if (PopulationA[i]->dFitness > max){ //es mejor que el actual? 'cual'
            max = PopulationA[i]->dFitness; //reasigno nuevo valor de maximo
            cual = i;
        }
    }
    //std::cout << "SALIENDO DEL FOR DEL SELECT BETTER" << std::endl;
    return PopulationA[cual];
}

int Genetic::CrossingN(std::shared_ptr<Cromosoma> &a,std::shared_ptr<Cromosoma> &b, int n)
{
    //implementacion actual efectua cruce sobre todos los byte del string que contiene cada cromosoma
    //recibo el apuntador a los dos padres, debo generar dos hijos, e insertarlos en la poblacion b
    //primero creo la mascara de cruce con 'n' puntos de cruce
    //una opcion es a partir de 'n', obtener la probabilidad de cruce sobre cada byte/bit

    unsigned char *mascara;
    int longitud = a->iLongitud; //asumo dos cromosomas de igual longitud
    int i,donde;

    if (longitud<=0)
        return -1; //no hay nada que cruzar

    mascara = new unsigned char[longitud]; //mascara de igual longitud que cromosoma
    //creo una mascara vacia
    for (i = 0; i < longitud; i++)
        *(mascara + i) = 0xFF;

      //ahora, procedo a la construccion de la mascara en base a 'n' puntos de cruce
    for (i=0; i<n; i++){
        donde = random() % longitud;                    //el punto de cruce lo selecciono aleatoriamente a lo largo de toda la longitud del cromosoma
        *(mascara + donde) = 0xFF << (random() % 7);    //mascara del punto de cruce WARNING IRRELEVANTE
        for (i = donde + 1; i < longitud; i++)
            *(mascara + i) = ~*(mascara + i);           //cambio los bits del resto de la mascara
    }

    //por ultimo, obtengo los dos hijos a partir de operacion con la mascara
    unsigned char* cB  = new unsigned char[longitud]();
    unsigned char* cA  = new unsigned char[longitud]();
    

    for (i=0; i<longitud; i++){                     //creo dos cromosomas nuevos
        //WARNING de perdida de datos
        cA[i] = ((a->_Cromosoma[i]) & *(mascara + i)) | ((b->_Cromosoma[i]) & ~*(mascara + i));
        cB[i] = ((b->_Cromosoma[i]) & *(mascara + i)) | ((a->_Cromosoma[i]) & ~*(mascara + i));
    }

    memcpy (a->_Cromosoma, cA, longitud);
    memcpy (b->_Cromosoma, cB, longitud); //escribo en espacio de memoria reservado en poblacion creada

    delete[] mascara; //libero espacio reservado para la mascara
    return longitud;
}

std::shared_ptr<Cromosoma> Genetic::SelectTournament()
{
    int i,j;
    i = random() % iSizePopulationA; //escojo dos individuos aleatoriamente con la misma probabilidad
    j = random() % iSizePopulationA;

    if (PopulationA[i]->dFitness > PopulationA[j]->dFitness)
        return PopulationA[i];
    else
        return PopulationA[j]; //devuelvo apuntador de mejor de los dos cromosomas evaluados
}

void Genetic::ActPopulation()
{
    for (int i = 0; i < iSizePopulationB; i++)
        memcpy(PopulationA[i]->_Cromosoma, PopulationB[i]->_Cromosoma, PopulationB[i]->iLongitud); //a<--b

    // reemplazo el string del cromosoma, asi reduzco solicitud de espacio dinamicamente
    // a solo los casos donde se reinicia poblacion o se crea una poblacion nueva
    iSizePopulationA = iSizePopulationB; //todos
    iSizePopulationB = 0; //nadie
}

int Genetic::Generation()
{
    //std::cout << "START GENERATION" << std::endl;
    std::shared_ptr<Cromosoma> elite, clonA, clonB, ap;
    clonA = std::make_shared<Cromosoma>();
    clonB = std::make_shared<Cromosoma>();
    
    int i;
    iSizePopulationB = 0; //nadie en la poblacion de respaldo (vacia)

      /**********************ELITISMO************************/
      //veo si esta activado el flag de elitismo
      //en elitismo se escoje el mejor individuo y se inserta directamente en la poblacion final (hijos)
      //std::cout << "ELITISMO?" << std::endl;
      if (usarElitismo==true){
          elite = SelectBetter(); //obtengo apuntador al mejor de la poblacion
          elite->Clonate(clonA); //saco una copia del mejor, para no afectar al original. Uso la funcion 'Clonate'
          //std::cout << "USE COUNT IS:  " << clonA.use_count() << std::endl;
          InsertPopulationB(clonA); //lo coloco directo en la poblacion B (poblacion nueva)
      }
      //std::cout << "SALIENDO CONDICIONAL ELITISMO" << std::endl;
      /*******************************************************/

      /**********************HAREM************************/
      //veo si esta activado el flag de harem
      //Harem: agarra el mejor individuo y lo cruza con parte de la poblacion inicial hasta llenar una fraccion de la poblacion final
      if (usarHarem==true){
          //selecciono al mejor individuo de la poblacion
          elite = SelectBetter(); //obtengo apuntador al mejor de la poblacion
          elite->Clonate(clonA); //saco una copia del mejor, para no afectar al original. Uso la funcion 'Clonate'
          //determino la porcion de la poblacion que sera llenada mediante el cruce del elite con el resto de la poblacion
          int fraccion = (int)(fraccionHarem  * iSizePopulationA);
          for (i = 0; i < fraccion; i++){
              elite->Clonate(clonA); //clonA <- copia(elite)
              SelectTournament()->Clonate(clonB); //clonB <- copia(otro)
              CrossingN(clonA, clonB, iCrossingPoint); //lo cruzo en 'N' puntos de cruce
              clonA->Mutate(); //muto cada uno de los descendientes
              clonB->Mutate();
              InsertPopulationB(clonA); //los inserto en la poblacion nueva
              InsertPopulationB(clonB);
          }
      }
      /****************************************************/

	while (FreePopulation() > 0){ //mientras haya espacio libre para mas individuos...
          //ahora procedo al proceso de seleccion segun criterio, por simplicidad esta implementacion usa seleccion tournament
        ap = std::make_shared<Cromosoma>();
        ap = SelectTournament(); //veo ganador de seleccion por fitness,
        ap->Clonate(clonA);
        ap = SelectTournament(); //veo ganador de seleccion por fitness,
        ap->Clonate(clonB);

        //ahora cruzo los dos clones
        CrossingN(clonA, clonB, iCrossingPoint);
        //ahora debo insertar una copia de los clones en la poblacionB

            //pero antes muto los hijos
        clonA->Mutate(dTasaMutacion);
        clonB->Mutate(dTasaMutacion);
        //luego inserto duplicado en la poblacion B, hasta donde quepa
        //de los dos hijos, calculo el nuvo fitness y escojo el mejor
        clonA->dFitness = (*fnEvaluarCromosoma)(clonA->_Cromosoma);
        clonB->dFitness = (*fnEvaluarCromosoma)(clonB->_Cromosoma);

        if (clonA->dFitness > clonB->dFitness)
            InsertPopulationB(clonA);
        else
            InsertPopulationB(clonB);
    }//*/
    ulAge++; //incremento edad de la poblacion
    //luego actualizo la poblacion A, reemplazandola por los individuos de la poblacion B
    ActPopulation();
	//hay que calcular el nuevo fitness de los cromosomas insertados en la poblacion A
    for (i = 0;i < iSizePopulationA; i++)
      	PopulationA[i]->dFitness = (*fnEvaluarCromosoma)(PopulationA[i]->_Cromosoma);
    
    //std::cout << "END GENERATION" << std::endl;
	return 0;
}

float Genetic::Fitness()
{
    float acumulado=0;

    if (!iSizePopulationA)
        return 0; //nada de nada
   	for (int i = 0; i<iSizePopulationA; i++)
        acumulado += PopulationA[i]->dFitness; //incremento acumulador

    return acumulado/iSizePopulationA; //devuelvo el promedio de todos los fitness
}

std::shared_ptr<Cromosoma> Genetic::SelectWorst()
{
    if (!iSizePopulationA)
    return NULL;

    float min = PopulationA[0]->dFitness; //pos '0'
    int cual = 0;
    for (int i=0; i<iSizePopulationA; i++){
        if (PopulationA[i]->dFitness < min){
            min = PopulationA[i]->dFitness; //reasigno nuevo valor de minimo
            cual = i;
        }
    }

    return PopulationA[cual]; //devuelve el apuntador al peor especimen
}
