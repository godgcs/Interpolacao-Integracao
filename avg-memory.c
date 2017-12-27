//===================================================================================================================
// Arquivo: avg-memory.c
//
// Autores: Guilherme Cardoso Silva 0022545
//          Flavia Ribeiro Santos   0022651
// Data: 06/07/2016
// Disciplina: Calculo Numerico
// Atividade: Trabalho Pratico
// Para compilar o arquivo é necessario utilizar o comando "gcc avg-memory.c -o avg-memory -lm"
// Para rodar basta utilizar o comando "./avg-memory <ArquivoEntrada> <ArquivoSaida>"
// Trabalho Implementado na linguagem C, e para escrita do codigo foi utilizado o software Atom no S.O. Ubuntu 15.10
//===================================================================================================================
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//Requisito 01 - TAD para Sequência Pares Ordenados----------------------------------------------------------
//Estrutura de Armazenamento
typedef struct nodo{
          double Num1;      //Armazena o X do Ponto
          double Num2;      //Armazena o Y do Ponto
}NodoPares;

struct pares{
          NodoPares* Dados; //Será um Vetor que armazenará os pontos
          int Tam;          //Armazena Pontos a serem lidos no arquivo
          int TamMomento;
};
typedef struct pares * Pares;

//Cabeçalho de Funções da TAD
Pares   CriaEstruturaPares(int Tam);
void    InserePar(double n1, double num2, Pares aux);
Pares   LeParesArquivo(char Arq[]);
int     TamanhoPares(Pares aux);
double  MenorValorX(Pares aux);
double  MaiorValorX(Pares aux);
double  MenorValorY(Pares aux);
double  MaiorValorY(Pares aux);
//-----------------------------------------------------------------------------------------------------------
double* CalculaDerivadaSpline(int n, Pares aux);
double  AvaliaSpline(int n, Pares aux, double* s2, double valor);
double  IntegralMonteCarlo(int n, double xmin, double xmax, double ymin, double ymax, Pares aux, double* s2);
double  MemoriaMedia(Pares aux, double* s2);
void    SaidaTerminal(int n, Pares aux, double* s2, char Arq[]);
void    SaidaTxt(int n, Pares aux, double* s2, char Arq[]);

//Aplicação Principal----------------------------------------------------------------------------------------
void main(int argc, char *argv[]){
  int i;
  if(argc == 3){                                              //Recebo 3 parametros de entrada, 1 é o programa, 2 é a entrada, 3 é a saida
    Pares Par;                                                //Ponteiro que irá receber os pontos
    double *s2;                                               //Ponteiro que irá receber o vetor derivada
    //Requisito 02 - Entrada de Dados (Arquivo)--------------------------------------------------------------
    FILE *f;
    f=fopen(argv[1],"r");                                     //Lê o arquivo
    if(f){                                                    //Verifica se o arquivo existe
      fclose(f);
      Par=LeParesArquivo(argv[1]);                            //Lê o arquivo e armazena os dados no ponteiro Par
    //-------------------------------------------------------------------------------------------------------
      s2 = CalculaDerivadaSpline(TamanhoPares(Par),Par);      //s2 recebe o vetor derivada através da função CalculaDerivadaSpline
      SaidaTerminal(TamanhoPares(Par),Par,s2,argv[2]);        //Função que irá mostrar as saidas do terminal
      SaidaTxt(TamanhoPares(Par),Par,s2,argv[2]);             //Função que irá gerar a saida em txt
    }else{
      printf("Arquivo Inexistente.\n");
    }
  }else{
    printf("Entradas Invalidas.\n");
  }
}
//-----------------------------------------------------------------------------------------------------------

//Requisito 01 - TAD para Sequência Pares Ordenados----------------------------------------------------------
Pares CriaEstruturaPares(int Tam){
  Pares aux;
  aux = (Pares) malloc (sizeof(struct pares));
  if (aux){
    aux->Dados      = (NodoPares*) malloc((Tam+1)*sizeof(NodoPares)); //Vetor para armazenar pontos, indexado de 1, então cria um tamanho a mais
    aux->Tam        = Tam;                                            //Armazena quantidade de pontos a serem lidos lidos
    aux->TamMomento = 0;                                              //Armazena quantidade de pontos gravados

    return(aux);
  }else{
    return(NULL);
  }
}

void InserePar(double n1, double n2, Pares aux){
  aux->TamMomento++;                                                  //Aumenta a Quantidade de ponto
  aux->Dados[aux->TamMomento].Num1 = n1;                              //O Vetor no indice TamMomento irá receber o n1 que é o X
  aux->Dados[aux->TamMomento].Num2 = n2;                              //O Vetor no indice TamMomento irá receber o n1 que é o Y
}

  //Requisito 02 - Entrada de Dados (Arquivo)--------------------------------------------------------------
Pares LeParesArquivo(char NomeArq[]){
  FILE *Arq;
  Pares aux;
  int i, cont=0;
  double num1, num2;
  Arq = fopen(NomeArq,"r");
  while(!feof(Arq)){                                                  //Le todo o Arquivo para ver quantos pontos irá cadastrar
    fscanf(Arq,"%lf%lf", &num1, &num2);
    cont++;
  }
  aux = CriaEstruturaPares(cont-1);                                   //Cria uma estrutura do tamanho de linhas lidas no arquivo
  fclose(Arq);
  Arq = fopen(NomeArq,"r");
  for(i=1; i < cont; i++){                                            //Le o arquivo novamente gravando todos os pontos desta vez
    fscanf(Arq,"%lf%lf", &num1, &num2);
    InserePar(num1, num2 ,aux);
  }
  //-----------------------------------------------------------------------------------------------------------

  return(aux);
}

int TamanhoPares(Pares aux){
  return(aux->Tam);                                                   //Retorna Tamanho dos pontos
}

double MenorValorX(Pares aux){
  return(aux->Dados[1].Num1);                                         //O menor X é sempre o primeiro ponto
}

double MaiorValorX(Pares aux){
  return(aux->Dados[aux->Tam].Num1);                                  //O maior X é sempre o ultimo ponto
}

double MenorValorY(Pares aux){
  int i;
  double menor;
  for(i=1;i <= aux->Tam; i++){                                        //Busca em todo o vetor Y pelo menor ponto
    if(i == 1){
      menor = aux->Dados[i].Num2;
    }else{
      if(aux->Dados[i].Num2 < menor){
        menor = aux->Dados[i].Num2;
      }
    }
  }

  return(menor);
}

double MaiorValorY(Pares aux){
  int i;
  double maior;
  for(i=1;i <= aux->Tam; i++){                                        //Busca em todo o vetor Y pelo maior ponto
    if(i == 1){
      maior = aux->Dados[i].Num2;
    }else{
      if(aux->Dados[i].Num2 > maior){
        maior = aux->Dados[i].Num2;
      }
    }
  }

  return(maior);
}
//-----------------------------------------------------------------------------------------------------------

//Requisito 03 - Cálculo das 2a. Derivada da Spline Cúbica---------------------------------------------------
double* CalculaDerivadaSpline(int n, Pares aux){
  if (n < 3){
    printf("Erro, menos de 3 pontos\n");
  }else{
    //Sistema Tridiagonal Simétrico
    int i, m;
    double DeltaA, DeltaB, Hb, Ha, t;
    double *e  = (double*) malloc((n+1)*sizeof(double));
    double *d  = (double*) malloc((n+1)*sizeof(double));
    double *s2 = (double*) malloc((n+1)*sizeof(double));

    m      = n-2;
    Ha     = aux->Dados[2].Num1 - aux->Dados[1].Num1;
    DeltaA = (aux->Dados[2].Num2 - aux->Dados[1].Num2)/Ha;
    for(i=1; i <= m; i++){
      Hb      = aux->Dados[i+2].Num1 - aux->Dados[i+1].Num1;
      DeltaB  = (aux->Dados[i+2].Num2 - aux->Dados[i+1].Num2)/Hb;
      e[i]    = Hb;
      d[i]    = 2*(Ha + Hb);
      s2[i+1] = 6*(DeltaB - DeltaA);
      Ha      = Hb;
      DeltaA  = DeltaB;
    }

    //Eliminação de Gauss
    for (i=2; i <= m; i++){
      t       = e[i-1]/d[i-1];
      d[i]    = d[i] - t*e[i-1];
      s2[i+1] = s2[i+1] - t*s2[i];
    }

    //Substituição Retroativa
    s2[m+1] = s2[m+1]/d[m];
    for(i=m; i >= 2; i--){
      s2[i] = (s2[i] - e[i-1]*s2[i+1])/d[i-1];
    }
    s2[1]   = 0;
    s2[m+2] = 0;

    return(s2);
  }
}
//-----------------------------------------------------------------------------------------------------------

//Requisito 04 - Função que Avalia P (x) usando Splines------------------------------------------------------
double AvaliaSpline(int n, Pares aux, double* s2, double valor){
  if(valor < aux->Dados[0].Num1 || valor > aux->Dados[n].Num1){
    printf("Erro, valor fora de intervalo\n");
  }else{
    //Busca Binaria
    int inf = 1;
    int sup = n;
    int indice;
    while(sup - inf > 1){
      indice = trunc((inf+sup)/2);
      if (aux->Dados[indice].Num1 > valor){
        sup = indice;
      }else{
        inf = indice;
      }
    }

    //Avaliação de P(x) por Horner
    double h = aux->Dados[sup].Num1 - aux->Dados[inf].Num1;
    double a = (s2[sup] - s2[inf])/(6*h);
    double b = s2[inf]*0.5;
    double c = (aux->Dados[sup].Num2-aux->Dados[inf].Num2)/h - (s2[sup] + 2*s2[inf])*h/6;
    double d = (double)aux->Dados[inf].Num2;
    h        = valor-aux->Dados[inf].Num1;
    valor    = ((a*h + b)*h + c)*h + d;

    return(valor);
  }
}
//-----------------------------------------------------------------------------------------------------------

//Requisito 06 - Integral Numérica por Monte Carlo-----------------------------------------------------------
double IntegralMonteCarlo(int n, double xmin, double xmax, double ymin, double ymax, Pares aux, double* s2){
  int i, numAbaixo=0;
  double AreaTotal, x, y;
  srand(time(NULL));
  for(i=1; i <= n; i++){
    //Requisito 05 - Gerador de Números Uniforme U (min, max)------------------------------------------------
    x = rand();
    x = x/RAND_MAX*xmax + xmin;
    y = rand();
    y = y/RAND_MAX*(ymax-ymin) + ymin;
    //-------------------------------------------------------------------------------------------------------
    if (y <= AvaliaSpline(TamanhoPares(aux), aux, s2, x)){
      numAbaixo++;
    }
  }
  AreaTotal = (xmax - xmin)*(ymax - ymin);
  AreaTotal = AreaTotal*(numAbaixo)/n;

  return(AreaTotal);
}
//-----------------------------------------------------------------------------------------------------------

//Requisito 07 - Uso Médio de Memória usando TVMI------------------------------------------------------------
double MemoriaMedia(Pares aux,double* s2){
  return((IntegralMonteCarlo(1000000, MenorValorX(aux), MaiorValorX(aux), MenorValorY(aux),MaiorValorY(aux), aux, s2)
          / (MaiorValorX(aux) - MenorValorX(aux))) + MenorValorY(aux));     //A integral é dividida por MaiorX -MenorX que retorna a media do intervalo
}                                                                           //Como a media é uma representação na origem, se soma com MenorY para achar a MemoriaMedia
//-----------------------------------------------------------------------------------------------------------

//Requisito 08 - Saída de Dados (Terminal)-------------------------------------------------------------------
void SaidaTerminal(int n, Pares aux, double* s2, char Arq[]){
  printf("\nNumber of Samples: %d\n", n);                                   //Numero de pontos
  printf("Average Memory Usage : %.3lf Kb\n\n", MemoriaMedia(aux,s2));      //MemoriaMedia Usada
  printf("Run ‘Rscript %s’ to generate Average Memory Usage Chart\n", Arq); //comando para gerar o plot no R
}
//-----------------------------------------------------------------------------------------------------------

//Requisito 09 - Saída de Dados (R Script)-------------------------------------------------------------------
void SaidaTxt(int n, Pares aux, double* s2, char Arq[]){
  FILE *f;
  int i;
  f = fopen(Arq,"w");
  fprintf(f,"#\n# Generated automatically by 'avg-memory' application\n#\n\n");
  //----------------------------------------------------------------------------
  fprintf(f,"# Original points (x coordinates)\n");
  fprintf(f,"xorig <- c(\n");
  for(i=1; i <= (n-1); i++){                                                //Gera Todos os pontos X Originais no arquivo
    fprintf(f,"\t\t%.0f,\n", aux->Dados[i].Num1);
  }
  fprintf(f,"\t\t%.0f\n", aux->Dados[i].Num1);
  fprintf(f,");\n\n");
  //----------------------------------------------------------------------------
  fprintf(f,"# Original points (y coordinates)\n");                         //Gera Todos os pontos Y Originais no arquivo
  fprintf(f,"yorig <- c(\n");
  for(i=1; i <= (n-1); i++){
    fprintf(f,"\t\t%.0f,\n", aux->Dados[i].Num2);
  }
  fprintf(f,"\t\t%.0f\n", aux->Dados[i].Num2);
  fprintf(f,");\n\n");
  //----------------------------------------------------------------------------
  fprintf(f,"# Spline points (x coordinates, sampling interval = 0.01)\n");
  fprintf(f,"xspl <- c(\n");
  for(i=0; i <= ((MaiorValorX(aux)-MenorValorX(aux))*100-1); i++){          //Gera Todos os pontos X em um intervalo 0.01 entre Xmin e Xmax no arquivo
    fprintf(f,"\t\t%.2lf,\n", (double)i/100);
  }
  fprintf(f,"\t\t%.2lf\n", (double)i/100);
  fprintf(f,");\n\n");
  //----------------------------------------------------------------------------
  fprintf(f,"# Spline points (y coordinates, sampling interval = 0.01)\n");
  fprintf(f,"yspl <- c(\n");
  for(i=0; i <= ((MaiorValorX(aux)-MenorValorX(aux))*100-1); i++){          //Gera Todos os F(X) dos pontos gerados para X
    fprintf(f,"\t\t%lf,\n", AvaliaSpline(n, aux, s2, (double)i/100));
  }
  fprintf(f,"\t\t%lf\n", AvaliaSpline(n, aux, s2, (double)i/100));
  fprintf(f,");\n\n");
  //----------------------------------------------------------------------------
  fprintf(f,"# Average Memory Usage\n");
  fprintf(f,"AvgMemory <- %.3lf;\n\n", MemoriaMedia(aux, s2));              //Calcula Memoria Media
  //----------------------------------------------------------------------------
  fprintf(f,"# Plot the values in .png file\n");                            //Dados para Plot d grafico em imagem no R
  fprintf(f,"png(file='%s.png', width=1200);\n",Arq);
  fprintf(f,"title <- paste('AVG Memory Usage: %.3lf Kb (%d Samples)');\n", MemoriaMedia(aux,s2), n);
  fprintf(f,"plot(xspl, yspl, type='l', col='blue', main=title, xlab='Samples', ylab='Mem. Usage', lwd=3);\n");
  fprintf(f,"points(xorig, yorig, pch=19, col='red');\n");
  fprintf(f,"lines( c(min(xorig), max(xorig)), c(AvgMemory, AvgMemory), col='black', lty=2, lwd=3);\n");
  fprintf(f,"dev.off();\n");
  //----------------------------------------------------------------------------
  fclose(f);
}
//-----------------------------------------------------------------------------------------------------------
