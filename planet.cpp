#include "gnuplot.h"

using namespace std;


//Declaración de variables globales
#define T_Max 100
#define UA 149597870700.00
#define G 6.674E-11
#define Msol 1.989E30 
#define Rsol 696.34E6
#define N 201
#define MasaSistema 4.984962406E27 //Masa del sistema quitando el sol
#define Ro  3.142125E3 //Densida del sistema para los planetesimales, probablmente la tnega que cambiar pare que se choquen un poco aunque sea
#define cinturon 3.0
#define Rneptuno  30.1
#define Rmercurio 0.378
int n;
double h, t, enerc, enerpo, moment, radio, calorinterno;



//Funciones externas
void Aceleracion(astro cuerpos[]);
void Posicion(astro cuerpos[]);
void Velocidad(astro cuerpos[]);
void Energia(astro cuerpos[]);
void Colision(astro cuerpos[]);
void Copiar(astro A, astro B);
double Cinetica(astro A);


int main()
{   
    //Aleatoriedad
    random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<> rocos(0.75,cinturon); //Poner orbita minima y máxima
    uniform_real_distribution<> pos(cinturon, 14);
    uniform_real_distribution<> excentricdad(0.2, 0.85);
    
    astro cuerpos[N];

    //Temas de exportar los datos
    gnuplot q,p;
    ifstream entrada;
    ofstream salida,output, out;
    salida.open("planetesimals.dat");
    output.open("energias.dat");
    out.open("salidaestandar.dat");
        

    //Condiciones de inicio
    t=h=0.01;
    n=N;
    cuerpos[0].m=1;
    cuerpos[0].R=Rsol/UA;
    cuerpos[0].rx=cuerpos[0].ry=cuerpos[0].vx=cuerpos[0].vy=0;
    double num, t_2;
    num=MasaSistema*3/((N-1)*4*M_PI*Ro);
    radio=pow(num,1.0/3)/UA;
    for(int i=1; i<n; i++)
    {
        cuerpos[i].m=MasaSistema/Msol/(N-1);
        cuerpos[i].R=0.0063; //ESto es lo que estba antes aquí: cuerpos[i].R=radio; Estaba confundiendo radio de interacción con unradio de densidad
    }
    
    for(int i=1; i<n; i++)
    {
        if(i>0.1*(N-1))
        {
            cuerpos[i].Rocoso=false;
        }
        else
        {
            cuerpos[i].Rocoso=true;
        }
    }

    
    //Cuerpos impares a la izquierda en su epiapsis
    for(int i=1; i<n;i=i+2)
    {
        if(cuerpos[i].Rocoso) 
        {
            cuerpos[i].rx=-rocos(gen);
            cuerpos[i].vy=-sqrt((1-excentricdad(gen))/(-cuerpos[i].rx));
            
        }else
        {
            cuerpos[i].rx=-pos(gen);
            cuerpos[i].vy=-sqrt((1-excentricdad(gen))/(-cuerpos[i].rx));   
        }
    }
    //Cuerpos pares a la derecha en su epiapsis
    for(int i=2; i<n;i=i+2)
    {
        if(cuerpos[i].Rocoso) 
        {
            cuerpos[i].rx=rocos(gen);
            cuerpos[i].vy=sqrt((1-excentricdad(gen))/cuerpos[i].rx);
        }else
        {
            cuerpos[i].rx=pos(gen);
            cuerpos[i].vy=sqrt((1-excentricdad(gen))/cuerpos[i].rx);
        }
    }
    
    //Test de velocidad a ver que sale 
    //cuerpos[1].vy=-10;
    
    //Algortimo de Verlet
    Aceleracion(cuerpos);
    while(t<T_Max)
    {  
        /*
        //Temas de salida de datos 
        if((int)(t*100)%60==0)
        {
            for(int i=1; i<n; i++)
            {
                salida << setw(14);
                salida << setprecision(16) << cuerpos[i].rx;
                salida << "\t"<< "\t";
                salida << setw(14);
                salida << setprecision(16) << cuerpos[i].ry;
                salida << "\n";
            }   
            salida << endl;
        }

        if((int)(t*600)%10==0)
        {
            Energia(cuerpos);
            //Outputear la enrgía
            output << setw(14);
            output << t ;
            output << "\t";
            output << setw(14);
            output << setprecision(14) << enerc+ enerpo;
            output << "\t";
            output << setw(14);
            output << setprecision(14) << enerc;
            output << "\t";
            output  << setw(14);
            output << setprecision(14) << enerpo; 
            output << "\t";
            output  << setw(14);
            output << setprecision(14) << moment;
            output << "\t";
            output  << setw(14);
            output << setprecision(14) << calorinterno << endl;
        }
        */
        
        //Algortimo realmente 
        Posicion(cuerpos);
        Aceleracion(cuerpos);
        Velocidad(cuerpos);
        if((t>1)&&((t-t_2)>6*h))
        {
            out << t << "\t" << n-1 << endl;
            Colision(cuerpos);
            t_2=t;
        }

        t+=h;        
    }
    salida.close();  
    output.close();
    out.close();

    cout << endl << "Se han simulado: " << sqrt(pow(UA,3)/(G*Msol))*t/31557600 << " años." << endl;
    //p("load 'gif.plt");
    p("plot 'salidaestandar.dat' w l");
    return 0;
}


//Fuerza gravitatoria
void Aceleracion(astro cuerpos[])
{
    double modulo;
    for(int i=1; i<n;i++)
    {
        modulo=sqrt(pow((cuerpos[i].rx-cuerpos[0].rx),2)+pow((cuerpos[i].ry-cuerpos[0].ry),2)); //Distancia cuerpo i al sol
        cuerpos[i].fx=-cuerpos[0].m*(cuerpos[i].rx-cuerpos[0].rx)/pow(modulo,3);
        cuerpos[i].fy=-cuerpos[0].m*(cuerpos[i].ry-cuerpos[0].ry)/pow(modulo,3);
    }
    return;
}

//Posición
void Posicion(astro cuerpos[])
{
    for(int i=1; i<n; i++)
    {
        cuerpos[i].rx+=h*cuerpos[i].vx+h*h/2*cuerpos[i].fx;
        cuerpos[i].ry+=h*cuerpos[i].vy+h*h/2*cuerpos[i].fy;

        //Omega
        cuerpos[i].wx=cuerpos[i].vx+h/2*cuerpos[i].fx;
        cuerpos[i].wy=cuerpos[i].vy+h/2*cuerpos[i].fy;
    }
    return;
}

//Velocidad
void Velocidad(astro cuerpos[])
{
    for(int i=1; i<n;i++)
    {
        cuerpos[i].vx=cuerpos[i].wx+ h/2*cuerpos[i].fx;
        cuerpos[i].vy=cuerpos[i].wy+ h/2*cuerpos[i].fy;
    }
    return;
}


//Energía;
void Energia(astro cuerpos[])
{
    enerc=enerpo=calorinterno=0;
    double modulo;
    //Energía potencial y cinética
    for(int i=1; i<n;i++)
    {
        enerc+=Cinetica(cuerpos[i]);
        modulo=sqrt(pow((cuerpos[i].rx-cuerpos[0].rx),2)+pow((cuerpos[i].ry-cuerpos[0].ry),2));
        enerpo+=-cuerpos[i].m/modulo;
        calorinterno+=cuerpos[i].Q;
    }
    return;
}

//Leva a cabo las colisiones y reposicionamiento de las variables
void Colision(astro cuerpos[])
{
    double Distancia, Di, Dj,Q_g;
    for(int i=1; i<n-1; i++)
    {
        for(int j=i+1; j<n; j++)
        {
            Distancia=sqrt(pow((cuerpos[i].rx-cuerpos[j].rx),2)+pow((cuerpos[i].ry-cuerpos[j].ry),2));
            Di=sqrt(pow((cuerpos[i].rx-cuerpos[0].rx),2)+pow((cuerpos[i].ry-cuerpos[0].ry),2));
            Dj=sqrt(pow((cuerpos[0].rx-cuerpos[j].rx),2)+pow((cuerpos[0].ry-cuerpos[j].ry),2));
            Q_g=0;
            //Colisionan si: están suficinetemente cerca y si alguno es gaseoso, además está suficientemente lejos del sol
            if(Distancia<(cuerpos[i].R+cuerpos[j].R)&&(cuerpos[i].Rocoso||(Di>cinturon))&&(cuerpos[j].Rocoso||(Dj>cinturon)))
            {
                //Cuentas del choque
                Q_g=-Cinetica(cuerpos[i])-Cinetica(cuerpos[j]); //Menos calor antes del choque
                cuerpos[i].vx=(cuerpos[i].vx*cuerpos[i].m+cuerpos[j].m*cuerpos[j].vx)/(cuerpos[i].m+cuerpos[j].m);
                cuerpos[i].vy=(cuerpos[i].vy*cuerpos[i].m+cuerpos[j].m*cuerpos[j].vy)/(cuerpos[i].m+cuerpos[j].m);
                cuerpos[i].R=cuerpos[i].R*pow((cuerpos[i].m+cuerpos[j].m)/cuerpos[i].m,1.0/3);
                cuerpos[i].m=cuerpos[i].m+cuerpos[j].m;
                Q_g+=Cinetica(cuerpos[i]);                  //Calor generado en el choque
                cuerpos[i].Q=cuerpos[i].Q+cuerpos[j].Q+Q_g; //Calor tras el choque

                //Eliminación del cuerpo j
                Copiar(cuerpos[n-1], cuerpos[j]);

                //j--; Ahora meto el límite de colisiones por cuerpo
                i++; j=i; //Esto limita las colisiones a una por cuerpo y ciclo
                n--;
            }
        }
    }
    return;
}


//Copia el astro A a B
void Copiar(astro A, astro B)
{
    B.wx=A.wx;
    B.wy=A.wy;
    B.m=A.m;
    B.rx=A.rx;
    B.ry=A.ry;
    B.vx=A.vx;
    B.vy=A.vy;
    B.fx=A.fx;
    B.fy=A.fy;
    B.R=A.R;
    B.Rocoso=A.Rocoso;
    B.Q=A.Q;
}

//Energía cinética de un astro
double Cinetica(astro A)
{
    return A.m*(pow(A.vx,2)+pow(A.vy,2))/2;
}
