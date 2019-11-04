//
//
//
//
//  Created by Selma Souihel on 20/09/2017.
//
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
int main(int argc, char *argv[])
{
	char buffer[200];
	//sprintf(buffer, "/user/ssouihel/home/Desktop/correlation/%d-%d", atoi(argv[1]), atoi(argv[2]));
	//int state = mkdir(buffer, 0777);
	FILE *fichier = NULL;
	FILE *output1 = NULL;
	sprintf(buffer, "Files/%d-%d-firing1.txt", atoi(argv[1]), atoi(argv[2]));
	output1 = fopen(buffer, "w+");
	FILE *output2 = NULL;
	sprintf(buffer, "Files/%d-%d-firing2.txt", atoi(argv[1]), atoi(argv[2]));
	output2 = fopen(buffer, "w+");
	FILE *input0 = NULL;
	FILE *input1 = NULL;
	int pas = 1;//100;
	int duration = atoi(argv[4]);//60000;
	int largeur_fenetre = atoi(argv[5]);//10000;
	int i = 0, j = 0;
	int spike_time = 0;
	double spike_nb = 0;
	int curseur = 0;
	int time = 0;
	double firing_rate = 0;
	double buf = 0.;
	int tau = atoi(argv[3]);
	int n = 0;
	int size_raster1 = 0;
	int size_raster2 = 0;
	char chaine1[200], chaine2[200], chaine3[200];
	double *raster1 = calloc(duration, sizeof(double));
	double *raster2 = calloc(duration, sizeof(double));
	double *mean_firing1 = calloc(duration / pas, sizeof(double));
	double *mean_firing2 = calloc(duration / pas, sizeof(double));
	double *mean_corr = calloc(duration / pas, sizeof(double));
	double *std_dev1 = calloc(duration / pas, sizeof(double));
	double *std_dev2 = calloc(duration / pas, sizeof(double));
	double *std_dev3 = calloc(duration / pas, sizeof(double));
	int *coincidence = calloc(duration / pas, sizeof(int));
	int *coin = calloc(duration / pas, sizeof(int));
	double *firing1 = calloc(duration / pas, sizeof(double));
	double *firing2 = calloc(duration / pas, sizeof(double));
	sprintf(chaine3, "Files/%d-%d-%d.txt", atoi(argv[1]), atoi(argv[2]), tau);
	//printf("tau=%d\n",tau );
	fichier = fopen(chaine3, "w+");
	for (n = 1; n <= 1; n += 1)
		for (int m = 1; m <= n; m += 1)
		{
			sprintf(chaine1, "Files/n%d.txt", atoi(argv[1]));
			input0 = fopen(chaine1, "r+");
			sprintf(chaine2, "Files/n%d.txt", atoi(argv[2]));
			input1 = fopen(chaine2, "r+");
			time = -largeur_fenetre / 2;
			curseur = 0;
			if (tau != -1)
				if (input0 != NULL && input1 != NULL)
				{
					while (fscanf(input0, "%d", &spike_time) != EOF)
					{
						raster1[size_raster1] = spike_time;
						//printf("INCREASE\n");
						size_raster1++;
					}
					while (fscanf(input1, "%d", &spike_time) != EOF)
					{
						raster2[size_raster2] = spike_time;
						size_raster2++;
					}
				}
			fseek(input0, 0, SEEK_SET);
			fseek(input1, 0, SEEK_SET);
			//Calcul frequence de decharge f1
			//printf("Freq1\n");
			while (time < duration - largeur_fenetre / 2)
			{
				while ((fscanf(input0, "%d", &spike_time) != EOF))
				{
					//printf("%d\n",spike_time);
					if (spike_time - time < largeur_fenetre)
					{
						spike_nb++;
					}
					else
						break;
					if (spike_time < time + pas)
						curseur++;
				}
				firing_rate = (double)spike_nb * 1. / (double)largeur_fenetre;
				firing1[(int)(time + largeur_fenetre / 2) / pas] = firing_rate;
				spike_nb = 0;
				fseek(input0, 0, SEEK_SET);
				for (int k = 0; k < curseur; k++)
					fscanf(input0, "%d", &spike_time);
				time += pas;
				mean_firing1[(int)(time + largeur_fenetre / 2) / pas] += firing_rate;
				std_dev1[(int)(time + largeur_fenetre / 2) / pas] += pow(firing_rate, 2);
			}
			time = -largeur_fenetre / 2;
			curseur = 0;
			//printf("Freq2\n");
			//Calcul frequence de decharge f2
			while (time < duration - largeur_fenetre / 2)
			{
				while ((fscanf(input1, "%d", &spike_time) != EOF))
				{
					if (spike_time - time < largeur_fenetre)
					{
						spike_nb++;
					}
					else
						break;
					if (spike_time < time + pas)
						curseur++;
				}
				firing_rate = (double)spike_nb * 1. / (double)largeur_fenetre;
				firing2[(time + largeur_fenetre / 2) / pas] = firing_rate;
				spike_nb = 0;
				fseek(input1, 0, SEEK_SET);
				for (int k = 0; k < curseur; k++)
					fscanf(input1, "%d", &spike_time);
				time += pas;
				mean_firing2[(int)(time + largeur_fenetre / 2) / pas] += firing_rate;
				std_dev2[(int)(time + largeur_fenetre / 2) / pas] += pow(firing_rate, 2);
			}
			//for(int X=0;X<size_raster1;X++)printf("%f\n",raster1[X]);
			//printf("PreTau %d \n",atoi(argv[3]));
			for (tau = -atoi(argv[3]); tau <= atoi(argv[3]); tau += 1)
			{
				i = 0;
				j = 0;
				time = -largeur_fenetre / 2;
				//printf("Using Tau: %d\n",tau );
				while (time < duration - largeur_fenetre / 2)
				{//Calcul nombre de coincidence au temps time
					//printf("Current Time: %d and condition %d\n",time,raster1[i] < time + largeur_fenetre && raster2[j] < time + largeur_fenetre );
					do
					{
						
						if ((i >= size_raster1) || (j >= size_raster2))
							break;
						if (raster2[j] < (raster1[i] + tau))
							j++;
						else if (raster2[j] > (raster1[i] + tau))
							i++;
						else
						{
							if (coincidence[(time + largeur_fenetre / 2) / pas]==0){
								//printf("Coincidence\n");
								//printf("Raster 2: %f  Raster 1: %f Shifted Raster 1: %f at pos [%d][%d] from %d and %d\n",raster2[j],raster1[i], raster1[i] + tau,i,j,size_raster1,size_raster2);
								coincidence[(time + largeur_fenetre / 2) / pas]++;
							}
							
							
							coin[(time + largeur_fenetre / 2) / pas]++;
							j++;
							i++;
						}
					} while (raster1[i] < time + largeur_fenetre && raster2[j] < time + largeur_fenetre);
					j = 0;
					i = 0;
					while (raster2[j] < time + pas && j < size_raster2) j++;
					while (raster1[i] < time + pas && i < size_raster1) i++;
					time += pas;
				}
			}
			time = -largeur_fenetre / 2;
			while (time < duration - largeur_fenetre / 2)
			{
				//Calcul de la correlation
				if (firing1[(time + largeur_fenetre / 2) / pas] != 0. && firing2[(time + largeur_fenetre / 2) / pas] != 0.)
				{
					buf = ((double)coincidence[(time + largeur_fenetre / 2) / pas] * 1. /
					 (double)largeur_fenetre - firing1[(time + largeur_fenetre / 2) / pas] *
					  firing2[(time + largeur_fenetre / 2) / pas]) / 
					  sqrt(firing1[(time + largeur_fenetre / 2) / pas] 
					  * firing2[(time + largeur_fenetre / 2) / pas]);
				}
				else
					buf = 0;
				coincidence[(time + largeur_fenetre / 2) / pas] = 0;
				time += pas;
				mean_corr[(int)(time + largeur_fenetre / 2) / pas] += buf;
				std_dev3[(int)(time + largeur_fenetre / 2) / pas] += pow(buf, 2);
			}
			size_raster1 = 0;
			size_raster2 = 0;
			fclose(input0);
			fclose(input1);
		}
	time = 0;
	while (time < duration)
	{
		mean_firing1[(int)time / pas] /= (double)n * ((double)n - 1.) / 2.;
		mean_firing2[(int)time / pas] /= (double)n * ((double)n - 1.) / 2.;
		mean_corr[(int)time / pas] /= (double)n * ((double)n - 1.) / 2.;
		std_dev1[(int)(time / pas)] /= (double)n * ((double)n - 1.) / 2.;
		std_dev1[(int)(time / pas)] -= pow(mean_firing1[(int)(time / pas)], 2);
		std_dev1[(int)(time / pas)] = sqrt(std_dev1[(int)(time / pas)] / ((double)n * ((double)n - 1.) / 2.));
		std_dev2[(int)(time / pas)] /= (double)n * ((double)n - 1.) / 2.;
		std_dev2[(int)(time / pas)] -= pow(mean_firing2[(int)(time / pas)], 2);
		std_dev2[(int)(time / pas)] = sqrt(std_dev2[(int)(time / pas)] / ((double)n * ((double)n - 1.) / 2.));
		std_dev3[(int)(time / pas)] /= (double)n * ((double)n - 1.) / 2.;
		std_dev3[(int)(time / pas)] -= pow(mean_corr[(int)(time / pas)], 2);
		std_dev3[(int)(time / pas)] = sqrt(std_dev3[(int)(time / pas)] / ((double)n * ((double)n - 1.) / 2.));
		fprintf(output1, "%d\t%lg\t%lg\n", time, mean_firing1[(int)(time / pas)], std_dev1[(int)(time / pas)]);
		fprintf(output2, "%d\t%lg\t%lg\n", time, mean_firing2[(int)(time / pas)], std_dev2[(int)(time / pas)]);
		fprintf(fichier, "%d\t%lg\t%lg\n", time, mean_corr[(int)time / pas], std_dev3[(int)(time / pas)]);
		time += pas;
	}
	fclose(fichier);
	free(firing1);
	free(firing2);
	free(coincidence);
	free(raster1);
	free(raster2);
	free(mean_firing1);
	free(mean_firing2);
	free(mean_corr);
	free(std_dev1);
	free(std_dev2);
	free(std_dev3);
	fclose(output1);
	fclose(output2);
	return 0;
}

