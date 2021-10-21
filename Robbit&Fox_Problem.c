/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: mansur davudi
 *
 * Created on November 29, 2018, 9:18 AM
 */


#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
# define size 8
typedef struct{
	int fox,rab;
        double veg;
}vector;
int rabit_births_rules[4][5] = {
    {0, 3, 3, 2, 2},
    {0, 4, 4, 3, 3},
    {0, 6, 5, 4, 4},
    {0, 9, 8, 7, 5}
};
int rabbit_fox_population_rules[4][5] = {
    {0, 2, 2, 1, 0},
    {0, 3, 3, 2, 1},
    {0, 4, 3, 3, 2},
    {0, 5, 4, 3, 3}
};
int fox_can_eat(double veg)
{
    int check=0;
    int a=(rand() % 100)+1;
    if(veg < 0.6)
        if(a>=1 && a<=57)
            check=1;
    else if(veg >=0.6)
        if(a>=1 && a<=29)
            check=1;
    return check;
  
    
}
int fox_die_off()
{
     int check=0;
     int a=(rand() % 100)+1;
     if(a>=1 && a<=20)
         check=1;
     return check;
    
}
int rab_can_birth()
{
     int check=0;
     int a=(rand() % 100)+1;
     if(a>=1 && a<=20)
         check=1;
     return check;
}
int fox_can_birth()
{
     int check=0;
     int a=(rand() % 100)+1;
     if(a>=1 && a<=20)
         check=1;
     return check;
}
int rab_can_die()
{
     int check=0;
     int a=(rand() % 100)+1;
     if(a>=1 && a<=10)
         check=1;
     return check;
}
int fox_can_die()
{
     int check=0;
     int a=(rand() % 100)+1;
     if(a>=1 && a<=10)
         check=1;
     return check;
}
int fox_birth_num(int rabbitno, int foxno) {
    int i = 0, j = 0;
    if (rabbitno < 3) {
        if (foxno < 2) {
            i = 0;
            j = 0;
        } else if (foxno >= 2 && foxno < 10) {
            i = 0;
            j = 1;
        } else if (foxno >= 11 && foxno < 50) {
            i = 0;
            j = 2;
        } else if (foxno >= 51 && foxno < 100) {
            i = 0;
            j = 3;
        } else if (foxno >= 100) {
            i = 0;
            j = 4;
        }

    } else if (rabbitno >= 3 && rabbitno < 10) {

        if (foxno < 2) {
            i = 1;
            j = 0;
        } else if (foxno >= 2 && foxno < 10) {
            i = 1;
            j = 1;
        } else if (foxno >= 11 && foxno < 50) {
            i = 1;
            j = 2;
        } else if (foxno >= 51 && foxno < 100) {
            i = 1;
            j = 3;
        } else if (foxno >= 100) {
            i = 1;
            j = 4;
        }

    } else if (rabbitno >= 10 && rabbitno < 40) {
        if (foxno < 2) {
            i = 2;
            j = 0;
        } else if (foxno >= 2 && foxno < 10) {
            i = 2;
            j = 1;
        } else if (foxno >= 11 && foxno < 50) {
            i = 2;
            j = 2;
        } else if (foxno >= 51 && foxno < 100) {
            i = 2;
            j = 3;
        } else if (foxno >= 100) {
            i = 2;
            j = 4;
        }

    } else if (rabbitno >= 40) {

        if (foxno < 2) {
            i = 3;
            j = 0;
        } else if (foxno >= 2 && foxno < 10) {
            i = 3;
            j = 1;
        } else if (foxno >= 11 && foxno < 50) {
            i = 3;
            j = 2;
        } else if (foxno >= 51 && foxno < 100) {
            i = 3;
            j = 3;
        } else if (foxno >= 100) {
            i = 3;
            j = 4;
        }

    }

    return rabbit_fox_population_rules[i][j];
}

int rab_birth_num(double vegetation_at_startofday, int no_of_rabbits_at_startofday) {
    int i = 0, j = 0;


    if (vegetation_at_startofday < 0.2) {
        if (no_of_rabbits_at_startofday < 2) {
            i = 0;
            j = 0;
        } else if (no_of_rabbits_at_startofday >= 2 & no_of_rabbits_at_startofday < 200) {
            i = 0;
            j = 1;
        } else if (no_of_rabbits_at_startofday >= 201 & no_of_rabbits_at_startofday < 700) {
            i = 0;
            j = 2;
        } else if (no_of_rabbits_at_startofday >= 701 & no_of_rabbits_at_startofday < 5000) {
            i = 0;
            j = 3;
        } else if (no_of_rabbits_at_startofday >= 5000) {
            i = 0;
            j = 4;
        }

    } else if (vegetation_at_startofday >= 0.2 & vegetation_at_startofday < 0.5) {

        if (no_of_rabbits_at_startofday < 2) {
            i = 1;
            j = 0;
        } else if (no_of_rabbits_at_startofday >= 2 & no_of_rabbits_at_startofday < 200) {
            i = 1;
            j = 1;
        } else if (no_of_rabbits_at_startofday >= 201 & no_of_rabbits_at_startofday < 700) {
            i = 1;
            j = 2;

        } else if (no_of_rabbits_at_startofday >= 701 & no_of_rabbits_at_startofday < 5000) {
            i = 1;
            j = 3;

        } else if (no_of_rabbits_at_startofday >= 5000) {
            i = 1;
            j = 4;

        }

    } else if (vegetation_at_startofday >= 0.5 & vegetation_at_startofday < 0.8) {

        if (no_of_rabbits_at_startofday < 2) {
            i = 2;
            j = 0;


        } else if (no_of_rabbits_at_startofday >= 2 & no_of_rabbits_at_startofday < 200) {
            i = 2;
            j = 1;


        } else if (no_of_rabbits_at_startofday >= 201 & no_of_rabbits_at_startofday < 700) {
            i = 2;
            j = 2;


        } else if (no_of_rabbits_at_startofday >= 701 & no_of_rabbits_at_startofday < 5000) {

            i = 2;
            j = 3;

        } else if (no_of_rabbits_at_startofday >= 5000) {
            i = 2;
            j = 4;


        }

    } else if (vegetation_at_startofday >= 0.8) {

        if (no_of_rabbits_at_startofday < 2) {
            i = 3;
            j = 0;


        } else if (no_of_rabbits_at_startofday >= 2 & no_of_rabbits_at_startofday < 200) {
            i = 3;
            j = 1;


        } else if (no_of_rabbits_at_startofday >= 201 & no_of_rabbits_at_startofday < 700) {
            i = 3;
            j = 2;


        } else if (no_of_rabbits_at_startofday >= 701 & no_of_rabbits_at_startofday < 5000) {

            i = 3;
            j = 3;

        } else if (no_of_rabbits_at_startofday >= 5000) {
            i = 3;
            j = 4;

        }

    }

    return rabit_births_rules[i][j];

}

int main(int argc, char** argv) {
    
   int myid,procs,err,spoint, fpoint,part;
    vector  **b=(vector**) malloc(size * sizeof(vector));
       for (int i=0;i<8;i++)
               b[i]=(vector*) malloc(8*sizeof(vector));
    for(int i=0;i<8;i++)
        for(int j=0;j<8;j++)
        {
            b[i][j].fox=2;
            b[i][j].rab=100;
            b[i][j].veg=1.0;
        }
      MPI_Status status;
    err=MPI_Init(&argc , &argv);
    if (err != MPI_SUCCESS) {
	printf("Error in init MPI\n");
	MPI_Abort (MPI_COMM_WORLD, err);
	}
MPI_Comm_size(MPI_COMM_WORLD, &procs);
MPI_Comm_rank(MPI_COMM_WORLD, &myid);
part=8/procs;
spoint=myid*part;
fpoint=spoint+part;
    for(int iteration=0;iteration<3650;iteration++)
    {
        for(int i=spoint;i<fpoint;i++)
            for(int j=0;j<8;j++)
            {
                if(b[i][j].veg < 0.6 && b[i][j].rab > 0)
                {
                    for(int c=0;c<b[i][j].fox;c++)
                        if(fox_can_eat(b[i][j].veg)==1)
                            b[i][j].rab --;
                        else if(fox_die_off()==1)
                            b[i][j].fox --;
                }
                if(b[i][j].veg >= 0.6 && b[i][j].rab > 0)
                {
                     for(int c=0;c<b[i][j].fox;c++)
                        if(fox_can_eat(b[i][j].veg)==1)
                            b[i][j].rab --;
                       else if(fox_die_off()==1)
                            b[i][j].fox --;
                }
                    
                if(b[i][j].rab>=2)
                {
                    for(int r=0;r<b[i][j].rab;r+=4)
                        if(rab_can_birth() == 1)
                        {
                             int rab_plus=rab_birth_num(b[i][j].veg,b[i][j].rab);
                             b[i][j].rab+=rab_plus;
                        }
                   
                }
                if(b[i][j].fox>=2)
                {
                    for(int f=0;f<b[i][j].fox;f+=2)
                        if(fox_can_birth() ==1)
                        {
                             int fox_plus=fox_birth_num(b[i][j].rab,b[i][j].fox);
                            b[i][j].fox+=fox_plus;
                        }
                   
                }
                
                 for(int count=0;count<b[i][j].rab;count++)
                     if(rab_can_die())
                         b[i][j].rab --;
                 for(int count=0;count<b[i][j].fox;count++)
                     if(fox_can_die())
                         b[i][j].fox --;
                 int rob_temp= 0.2 * b[i][j].rab;
                 for(int count=0;count < rob_temp;count++)
                 {
                     int s=(rand() %( 4 ) ) +1;
                     if(s==1)
                         if(i-1> 0)
                         {
                             b[i][j].rab --;
                             b[i-1][j].rab ++;
                         }
                     if(s==2)
                         if(i+1< 8)
                         {
                              b[i][j].rab --;
                             b[i+1][j].rab ++;
                         }
                     if(s==3)
                          if(j-1> 0)
                         {
                              b[i][j].rab --;
                             b[i][j-1].rab ++;
                         }
                      if(s==4)
                          if(j+1< 8)
                         {
                              b[i][j].rab --;
                              b[i][j+1].rab ++;
                         }
                          
                 }
                 for(int count=0;count<b[i][j].fox;count++)
                 {
                     int f=(rand() % 3) + 1;
                     if(f==1)
                         continue;
                     if(f==2)
                     {
                         int f1=(rand() % 4) + 1;
                         if(f1==1)
                              if(i-1>0)
                         {
                             b[i][j].fox --;
                             b[i-1][j].fox ++;
                         }
                         if(f1==2)
                             if(i+1<8)
                             {
                                 b[i][j].fox --;
                                 b[i+1][j].fox ++;
                             }
                         if(f1==3)
                             if(j-1>0)
                             {
                                 b[i][j].fox --;
                                 b[i][j-1].fox ++;
                             }
                         if(f1==4)
                             if(j+1<8)
                             {
                                  b[i][j].fox --;
                                  b[i][j+1].fox ++;
                             }
                        
                     }
                  
                     if(f==3)
                     {
                         int f2=(rand() % 4) +1;
                         if(f2==1)
                             if(i-2>0)
                             {
                                  b[i][j].fox --;
                                  b[i-2][j].fox ++;
                             }
                         if(f2==2)
                             if(i+2<8)
                             {
                                  b[i][j].fox --;
                                  b[i+2][j].fox ++;
                             }
                         if(f2==3)
                             if(j-2>0)
                             {
                                  b[i][j].fox --;
                                  b[i][j-2].fox ++;
                             }
                         if(f2==4)
                             if(j+2<8)
                             {
                                 b[i][j].fox --;
                                b[i][j+2].fox ++; 
                             }
                     }
                      
                 }
                 double temp_veg=( (1.1) * b[i][j].veg ) - (0.001 * b[i][j].rab);
                      if(temp_veg >= 0.1 && temp_veg <=1.0)
                                 b[i][j].veg=temp_veg;
            }
       
    }
    for(int i=spoint;i<fpoint;i++)
        for(int j=0;j<8;j++)
        {
            if(b[i][j].rab<0)
                 b[i][j].rab=0;
            if(b[i][j].fox<0)
                 b[i][j].fox=0;
              printf("location--->   x:%d     y:%d       is --->	     rabbit----> %5d		fox----> %5d		vegetation----> %5f \n",i,j,b[i][j].rab,b[i][j].fox,b[i][j].veg);
        }
           MPI_Finalize();

          return (EXIT_SUCCESS);
}

