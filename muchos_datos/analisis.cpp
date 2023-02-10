//Análisis estadístico de los datos de los planetesimales
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <ctime> 
#include <cstdlib>
#include <random>
using namespace std;

#define mu 0.0003830473952569671

int main(void)
{
    double V[8], masas, media[4][8], masa, test;
    int cont, cont_anomalo, contar[4];
    ifstream input;
    ofstream peque, mediano, grande, mgrande;
    input.open("lot.dat");
    peque.open("peque.dat");
    mediano.open("mediano.dat");
    grande.open("grande.dat");
    mgrande.open("mgrande.dat");
    masas=cont_anomalo=cont=0;

    for(int i=0; i<4; i++)
    {
        contar[i]=0;
        for(int j=0; j<8; j++)
        {
            media[i][j]=0;
        }
    }

    //Lectura por filas de todos los datos hasta el fin del fichero
    while(cont<490984)
    {
        //Lee una línea de datos
        for(int i=0;i<8;i++)
        {
            input >> V[i];
            
        }

        //Comprovación de cosas raras
        if(V[7]>0) //Nueva criba basada en los -nan que ahora son -1 
        {
            if(V[0]<0.03334168754)
            {
                for(int i=0;i<8;i++)
                {
                    peque << setw(14) << setprecision(16) << V[i] << "\t";
                    media[0][i]+=V[i];
                }
                peque << endl;
                contar[0]++;
                
            }else
            {
                if(V[0]<0.06667084376)
                {
                    for(int i=0;i<8;i++)
                    {
                        mediano << setw(14) << setprecision(16) << V[i] << "\t";
                        media[1][i]+=V[i];
                    }
                    mediano << endl;
                    contar[1]++;
                }else
                {
                    if(V[0]<0.1)
                    {
                        for(int i=0;i<8;i++)
                        {
                            grande << setw(14) << setprecision(16) << V[i] << "\t";
                            media[2][i]+=V[i];
                        }
                        grande << endl;
                        contar[2]++;
                    }else
                    {
                        for(int i=0;i<8;i++)
                        {
                            mgrande << setw(14) << setprecision(16) << V[i] << "\t";
                            media[3][i]+=V[i];
                        }
                        mgrande << endl;
                        contar[3]++;    
                    }
                }
            }
             
    
        }
        else 
        {
            cont_anomalo++;
        }
        cont++;
    };
    input.close();
    peque.close();
    mediano.close();
    grande.close();
    mgrande.close();

    for(int i=0; i<4; i++)
    {
        cout << endl << "Las medias son de menor a mayor tamaño:    ";
        for(int j=0; j<8; j++)
        {
            cout << setw(14) << setprecision(16) << media[i][j]/contar[i] << "\t";
        }
    }

    cout << endl << endl << "Anomalías : "<< cont_anomalo << endl;

    return 0;
}