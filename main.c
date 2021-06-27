#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#define MAX 255
#define Ro 6372795.477598

typedef struct
{
    double lat, lon;
    int time, alt;
} GPS;

void read_line(char[], FILE*);
void change(char[], char, char);
GPS convert_GPS(char[]);
double distance(GPS, GPS);

int main()
{
    FILE *init, *final;
    GPS past, futu;
    double total_dist, vel, dist;
    char line[MAX], s[MAX];

    init = fopen("rio_do_ouro.csv", "r");
    final = fopen("final.csv", "w");

    read_line(line, init);
    fprintf(final, "%s; vel  (km/h)\n", line);

    read_line(line,init);
    fprintf(final, "%s;0\n", line);
    past = convert_GPS(line);


   while (fgets(line, MAX, init) != NULL)
    {
        futu = convert_GPS(line);
        
        if ((futu.time != past.time) & ((futu.lat != past.lat) || (futu.lon != past.lon))) {
        
            dist = distance(past, futu);
            total_dist +=  dist;

            vel = 3.6 * dist / (futu.time - past.time);
            printf("Distance (m): %lf\t Velocity (km/h):%lf\n", dist, vel);
            //  acrescantar ao novo arquivo as informa��es lidas e a velocidade
            sprintf(s, "%lf;%lf;%i;%i;%lf", futu.lat, futu.lon, futu.alt, futu.time, vel);
            change(s, '.', ',');

            fprintf(final, "%s\n", s);
       }  
       
        past.lat = futu.lat;
        past.lon = futu.lon;
        past.alt = futu.alt;
        past.time = futu.time;
   } 


    printf("\nTotal Distance: %.3lf km\n", total_dist/1000.);


    fclose(init);
    fclose(final);
    return 0;

}

void read_line(char line[], FILE *arq)
{
    fgets(line,MAX,arq);
    if (line[strlen(line)-1] == '\n')
        line[strlen(line)-1] = '\0';
}

void change(char line[], char old, char new)
{
    int i;
    for (i = 0; i < strlen(line); i++)
        if (line[i] == old)
            line[i] = new;
}

GPS convert_GPS(char msg[])
{
    GPS waze;
    char *token;

    token = strtok(msg, ";");
    change(token, ',', '.');
    waze.lat = atof(token);
    
    token = strtok(NULL, ";");
    change(token, ',', '.');
    waze.lon = atof(token);

    token = strtok(NULL, ";");
    change(token, ',', '.');
    waze.alt = atoi(token);

    token = strtok(NULL, ";");
    change(token, ',', '.');
    waze.time = atoi(token);

    return waze;
}

double distance(GPS A, GPS B)
{
    double dh,dv;

    dh = Ro * acos(sin(A.lat * M_PI/180)*sin(B.lat * M_PI/180)+
                           cos(A.lat * M_PI/180)*cos(B.lat * M_PI/180)*
                           cos(A.lon * M_PI/180 -B.lon * M_PI/180)
                            );

    dv = B.alt - A.alt;

    return sqrt(pow(dh,2)+pow(dv,2));  
    
}