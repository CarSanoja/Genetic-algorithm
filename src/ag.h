#ifndef AG_H_
#define AG_H_

#define AG_MAX_POBLACION        4000
#define AG_MUTACION_DEFAULT     0.001

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <memory>

// definir nuevo tipo de "datos": apuntador a una función
typedef float (*FuncionFitness_t) (unsigned char *);

//contiene estructura que maneja datos codificados del modelo
class Cromosoma 
{
    public:
                        Cromosoma();
                       ~Cromosoma();

        bool            Create (int);              //crea un nuevo cromosoma con 'n' bytes
        float           Fitness();                 //devuelve el fitness asociado al ente codificado en el cromosoma
        int             Length();                  //devuelve la longitud del cromosoma
        int             Mutate (float);            //muta cromosoma con una tasa dada
        int             Mutate ();                 //operador de mutacion que cambia un bit de todo el string
        bool            Delete();                
        int             Clonate(std::shared_ptr<Cromosoma> &);      //creo duplicado del cromosoma this en el ap recibido

                                                  // variables
        unsigned char*  _Cromosoma;                //apuntador a toda la informacion
        friend class    Genetic;           // acceso. Tal vez hacer que esta clase HEREDE??
   private:
        unsigned int    iLongitud;                //lontigud del arreglo
        float           dFitness;                 //fitnes asociado a la instancia
        int             iD;                       //ID de control de la instancia
};

class Genetic
{
    public:

                            Genetic();                                         //constructor
                            ~Genetic();                                        //destructor
                            Genetic(const Genetic & source);
                            Genetic(Genetic && source);

        void                InicializarSemillaRand(bool usarSemilla);
        float               Fitness();                                     //calcula y devuelve el valor de fitness promedio de poblacion
        int                 Generation();                                  //opera sobre poblacion para obtencion de una nueva generacion de individuos
        void                RandPopulation(int n, int l);                      //genera una poblacion incial de 'n' individuos de largo 'l' (bytes)
        float               MayorFitness();                                //devuelve el fitness del mejor individuo de la poblacion
        unsigned long       Age();                                         //devuelve la edad de la poblacion
        int                 FunctionEval(FuncionFitness_t);                    //fn que vincula con fn de evaluacion de cromosoma

        std::shared_ptr<Cromosoma>          SelectBetter();                                //devuelve mejor individuo
        std::shared_ptr<Cromosoma>          SelectWorst();                                 //devuelve peor individuo
        std::shared_ptr<Cromosoma>          SelectTournament ();                               //devuelve ap a cromosoma seleccionado por criterio TOURNAMENT
        int                 Crossing (Cromosoma *,Cromosoma *);                //cruce de dos individuos, devuelve 2 nuevos
        int                 CrossingN (std::shared_ptr<Cromosoma> &, std::shared_ptr<Cromosoma> &, int);         //cruce de dos individuos, devuelve 2 nuevos
        void                ActPopulation();                               //paso de poblacionB a poblacionA
        int                 InsertPopulationB (std::shared_ptr<Cromosoma> &);                   //inserta una copia fiel del cromosoma
        int                 FreePopulation();                                  //devuelve cuantos individuos mas se pueden agregar a la poblacion B
        int                 CrossingPoint();                                   //funcion que devuelve el numero de puntos de cruce
        int                 CrossingPoint(unsigned int);                       //funcion que asigna el numero de puntos de cruce

                                                                               // variables
        bool                usarElitismo;
        bool                usarHarem;
        float               dTasaMutacion;                                     //tasa de mutacion (0 a 1)
        float               fraccionHarem;                                     //fraccion de la poblacion que se obtendra por harem con el elite
    private:
        FuncionFitness_t    fnEvaluarCromosoma;                                //funcion que asigna al apuntador a funcion de
        std::shared_ptr<Cromosoma>          PopulationA[AG_MAX_POBLACION];
        std::shared_ptr<Cromosoma>          PopulationB[AG_MAX_POBLACION];                      //arreglo finito de apuntadores a cromosomas (individuos)
        int                 iSizePopulationA;                                  //dos poblaciones separadas, padres e hijos
        int                 iSizePopulationB;                                  //numero de individuos de cada poblacion
        float               dFitness;                                          //fitness promedio de la poblacion
        unsigned long       ulAge;                                             //edad de la poblacion (numero de iteraciones)
        unsigned int        iCrossingPoint;
};




#endif /* AG_H_ */