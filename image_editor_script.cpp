// INF110 - Trabalho pratico 3
//
// programa para ler, modificar e gravar uma imagem no formato PNM
//
// Autores: Andre Gustavo dos Santos			(criado em 16/06/14)
//          Andre Gustavo dos Santos			(modificado em 22/05/18)
//			Andre Gustavo dos Santos			(modificado em 13/09/21)
//			Andre Gustavo dos Santos			(modificado em 30/05/23)

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <string>

using namespace std;

const int MAXALTURA  = 1930;				//tamanho maximo aceito (pode ser alterado)
const int MAXLARGURA = 1930;
const int DIMRGB = 3;

unsigned char imagem[MAXALTURA][MAXLARGURA];	//a imagem propriamente dita
unsigned char imagem3[MAXALTURA][MAXLARGURA][DIMRGB]; // imagem 3d (colorida, rgb)
unsigned char imagem4[MAXALTURA][MAXLARGURA][3]; // cria uma matriz auxiliar rgb com valores em tons de cinza aleatórios entre 0 e 255


unsigned char sobel_resul[MAXALTURA+2][MAXLARGURA+2][3],sobel_resulc[MAXALTURA+2][MAXLARGURA+2] ; //matriz auxiliar filtro de sobel
unsigned char mx[MAXALTURA][MAXLARGURA][3], mxc[MAXALTURA][MAXLARGURA]; //matriz auxiliar
unsigned char my[MAXALTURA][MAXLARGURA][3], myc[MAXALTURA][MAXLARGURA]; //matriz auxiliar

unsigned char sh_resulc[MAXALTURA+2][MAXLARGURA+2][3],sh_resul[MAXALTURA+2][MAXLARGURA+2] ; // matriz auxiliar filtro sharpening colorido/cinza
unsigned char shc[MAXALTURA][MAXLARGURA][3], sh[MAXALTURA][MAXLARGURA];// matriz auxiliar filtro sharpening colorido/cinza.



int main() {

	int largura, altura;						    //dimensoes da imagem
	char tipo[4];							    	//tipo da imagem
	ifstream arqentrada;						    //arquivo que contem a imagem original
	ofstream arqsaida;							    //arquivo que contera a imagem modificada
	char comentario[200], c;				        //auxiliares
	int i, j,k, valor;								//auxiliares
	char nomearq[100];							    //nome do arquivo

//*** LEITURA DA IMAGEM ***//
//inicialmente nao sera necessario entender nem mudar nada nesta parte

	//*** Abertura do arquivo ***//
	cout << "Nome do arquivo com a imagem PNM: ";
	cin >> nomearq;
	arqentrada.open(nomearq,ios::in); //Abre arquivo para leitura
	if (!arqentrada) {
		cout << "Nao consegui abrir arquivo " << nomearq << endl;
		return 0;
	}
//***************************//


//*** Leitura do cabecalho ***//
	arqentrada >> tipo;	    //Le o tipo de arquivo
	arqentrada.get();		//Le e descarta o \n do final da 1a. linha

	if (strcmp(tipo,"P2")==0) {
		cout << "Imagem em tons de cinza\n";
	}
	else if (strcmp(tipo,"P3")==0) {
		cout << "Imagem colorida\n";
	}
	else if (strcmp(tipo,"P1")==0) {
		cout << "Imagem preto e branco\n";
		cout << "Desculpe, nao trabalho com esse tipo de imagem.\n";
		arqentrada.close();
		return 0;
	}
	else if (strcmp(tipo,"P4")==0 || strcmp(tipo,"P5")==0 || strcmp(tipo,"P6")==0) {
		cout << "Imagem no formato RAW\n";
		cout << "Desculpe, nao trabalho com esse tipo de imagem.\n";
		arqentrada.close();
		return 0;
	}

	while((c = arqentrada.get()) == '#')	//Enquanto for comentario
		arqentrada.getline(comentario,200);	//Le e descarta a linha "inteira"

	arqentrada.putback(c);	//Devolve o caractere lido para a entrada, pois como
													//nao era comentario, era o primeiro digito da largura

	arqentrada >> largura >> altura;	//Le as dimensoes da imagem, numero de pixels da horizontal e da vertical
	cout << "Tamanho: " << largura << " x " << altura << endl;
	if (largura > MAXLARGURA) {
		cout << "Desculpe, ainda nao trabalho com imagens com mais de " << MAXLARGURA << " pixels de largura.\n";
		arqentrada.close();
		return 0;
	}
	if (altura > MAXALTURA) {
		cout << "Desculpe, ainda nao trabalho com imagens com mais de " << MAXALTURA << " pixels de altura.\n";
		arqentrada.close();
		return 0;
	}

	arqentrada >> valor;	//Valor maximo do pixel (temos que ler, mas nao sera usado, assumimos 255)
//****************************//


//*** Leitura dos pixels da imagem ***//
    if (strcmp(tipo,"P2")==0) {
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++) {
                arqentrada >> valor;
                imagem[i][j] = (unsigned char) valor;
            }
    }
    if (strcmp(tipo,"P3")==0) {
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++) {
                for (k = 0; k < 3; k++) {
                    arqentrada >> valor;
                    imagem3[i][j][k] = (unsigned char) valor;
                }
            }
    }
//************************************//

	arqentrada.close();  //Fecha arquivo apos a leitura

//*** FIM DA LEITURA DA IMAGEM ***//

//*** TRATAMENTO DA IMAGEM ***//
//inicialmente sera nesta parte do codigo que voce vai trabalhar
    int escolha;
    cout << "Operacoes disponiveis: "<< endl;
    cout << "**************************************************" << endl;
    cout << "1. Escurecer " << endl; //
    cout << "2. Clarear " << endl;  //
    cout << "3. Negativo " << endl; //
    cout << "4. Espelhar " << endl; //
    cout << "5. Filtro de Sobel " << endl;
    cout << "6. Filtro de Sharpening " << endl;
    cout << "7. Tons de Cinza " << endl;
    cout << "8. Pontilhado " << endl;
    cout << "**************************************************" << endl;
    cout << "Qual operacao deseja fazer ?"<< endl;
    cin >> escolha;

    //******************************************************************************************************************//
    //*** escurecer a imagem ***//
    if (escolha == 1 && strcmp(tipo,"P2")==0) { // cinza
        int fator;
        cout << "Qual o fator de escurecimento (1-100)? ";
        cin >> fator;
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++) {
                valor = (int) imagem[i][j];                // pega o valor do pixel
                valor -= fator;                            // escurece o pixel
                if (valor < 0)                             // se der negativo
                    valor = 0;                             // deixa preto
                imagem[i][j] = (unsigned char) valor;      // modifica o pixel
            }
    }
    else if (escolha == 1 && strcmp(tipo,"P3")==0) { // colorido
        int fator;
        cout << "Qual o fator de escurecimento (1-100)? ";
        cin >> fator;
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++) {
                for (k = 0; k < 3; k++) {
                    valor = (int) imagem3[i][j][k];               // pega o valor do pixel
                    valor -= fator;                               // escurece o pixel
                    if (valor < 0)                                // se der negativo
                        valor = 0;                                // deixa preto
                    imagem3[i][j][k] = (unsigned char) valor;     // modifica o pixel
                }
            }
    }

    //*******************************************************************************************************************//
    //*** clarear a imagem ***//
    else if (escolha == 2 && strcmp(tipo,"P2")==0) { // cinza
        int fator;
        cout << "Qual o fator de clareamento (1-100)? ";
        cin >> fator;
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++) {
                valor = (int) imagem[i][j];                // pega o valor do pixel
                valor += fator;                            // torna o pixel mais claro
                if (valor > 255)                           // se der > que 255
                    valor = 255;                           // deixa branco
                imagem[i][j] = (unsigned char) valor;      // modifica o pixel
            }
    }
    else if (escolha == 2 && strcmp(tipo,"P3")==0) { // colorida
        int fator;
        cout << "Qual o fator de clareamento (1-100)? ";
        cin >> fator;
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++) {
                for (k = 0; k < 3; k++) {
                    valor = (int) imagem3[i][j][k];                // pega o valor do pixel
                    valor += fator;                                // torna o pixel mais claro
                    if (valor > 255)                               // se der > que 255
                        valor = 255;                               // deixa branco
                    imagem3[i][j][k] = (unsigned char) valor;      // modifica o pixel
                }
            }
    }

    //*********************************************************************************************************************//
    //*** deixar a imagem negativo ***//
    else if (escolha == 3 && strcmp(tipo,"P2")==0) { // cinza
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++) {
                valor = 255 - (int) imagem[i][j];           //pega o pixel e inverte o valor
                imagem[i][j] = (unsigned char) valor;       // modifica o pixel
            }
    }
    else if (escolha == 3 && strcmp(tipo,"P3")==0) { // colorida
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++) {
                for (k = 0; k < 3; k++) {
                    valor = 255 - (int) imagem3[i][j][k];           //pega o pixel e inverte o valor
                    imagem3[i][j][k] = (unsigned char) valor;       // modifica o pixel
                }
            }
    }

    //*********************************************************************************************************************//
    //*** espelhar a imagem ***//
    else if (escolha == 4 && strcmp(tipo,"P2")==0) { // cinza
        int aux;
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura/2; j++) {
                aux = imagem[i][j];                       // auxiliar recebe o valor
                imagem[i][j] =imagem[i][largura - j - 1]; // valor inicial recebe o valor da ultima posição
                imagem[i][largura - j - 1] = aux;         // ultima posição recebe o valor da primeira posição
            }
    }

    else if (escolha == 4 && strcmp(tipo,"P3")==0) { // colorida
        int aux;
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura/2; j++) {
                for (k = 0; k < 3; k++) {
                    aux = imagem3[i][j][k];                              // auxiliar recebe o valor
                    imagem3[i][j][k] = imagem3[i][largura - j - 1][k];   // valor inicial recebe o valor da ultima posição
                    imagem3[i][largura - j - 1][k] = aux;                // ultima posição recebe o valor da primeira posição
                }
            }
    }

    //*******************************************************************************************************************//
    //*** Filtro de Sobel ***//
    else if (escolha == 5 && strcmp(tipo,"P2")==0) { // cinza

        const char gx[3][3] = {{-1, 0, 1},  // detecta os contornos na vertical
                               {-2, 0, 2},
                               {-1, 0, 1}};

        const char gy[3][3] = {{1,  2,  1},  // detecta os valores na horizontal
                               {0,  0,  0},
                               {-1, -2, -1}};

        int pixel_x = 0;
        int pixel_y = 0;

        for (i = 0; i < altura+2; i++) // cria uma borda de '0' para fazer o calculo da borda
            for (j = 0; j < largura+2; j++){
                sobel_resulc[i][j] = '0';
            }

        for (i = 1; i < altura; i++) // le a imagem propriamente dita
            for (j = 1; j < largura; j++){
                sobel_resulc[i][j] = imagem[i-1][j-1];
            }

        for (i = 1; i < altura; i++) // faz o calculo de cada pixel para comparação
            for (j = 1; j < largura; j++){

                pixel_x =   ((gx[0][0] * sobel_resulc[i-1][j-1]) +
                            (gx[0][1] * sobel_resulc[i-1][j]) +
                            (gx[0][2] * sobel_resulc[i-1][j+1]) +
                            (gx[1][0] * sobel_resulc[i][j-1]) +
                            (gx[1][1] * sobel_resulc[i][j]) +
                            (gx[1][2] * sobel_resulc[i][j+1]) +
                            (gx[2][0] * sobel_resulc[i+1][j-1]) +
                            (gx[2][1] * sobel_resulc[i+1][j]) +
                            (gx[2][2] * sobel_resulc[i+1][j+1]));


                pixel_y =   ((gy[0][0] * sobel_resulc[i-1][j-1]) +
                             (gy[0][1] * sobel_resulc[i-1][j]) +
                             (gy[0][2] * sobel_resulc[i-1][j+1]) +
                             (gy[1][0] * sobel_resulc[i][j-1]) +
                             (gy[1][1] * sobel_resulc[i][j]) +
                             (gy[1][2] * sobel_resulc[i][j+1]) +
                             (gy[2][0] * sobel_resulc[i+1][j-1]) +
                             (gy[2][1] * sobel_resulc[i+1][j]) +
                             (gy[2][2] * sobel_resulc[i+1][j+1]));

                if (pixel_x < 0) pixel_x = 0;// tratamento para não deixar sair do range de 0-255
                if (pixel_y < 0) pixel_y = 0;
                if (pixel_x > 255) pixel_x = 255;
                if (pixel_y > 255) pixel_y = 255;

                mxc[i-1][j-1] = pixel_x;
                myc[i-1][j-1] = pixel_y;

            }

        for (i = 1; i < altura; i++) // cria a imagem com o filtro de sobel
            for (j = 1; j < largura; j++) {

                imagem[i][j] = max(mxc[i][j], myc[i][j]);
                if (imagem[i][j] < 0) imagem[i][j] = 0;// tratamento para não deixar sair do range de 0-255
                if (imagem[i][j] > 255) imagem[i][j] = 255;

            }
    }

    else if (escolha == 5 && strcmp(tipo,"P3")==0) { // colorida


        const char gx[3][3] = {{-1, 0, 1},  // detecta os contornos na vertical
                               {-2, 0, 2},
                               {-1, 0, 1}};

        const char gy[3][3] = {{1,  2,  1},  // detecta os valores na horizontal
                               {0,  0,  0},
                               {-1, -2, -1}};

        int pixel_x = 0;
        int pixel_y = 0;

        for (i = 0; i < altura+2; i++) // cria uma borda de '0' para fazer o calculo da borda
            for (j = 0; j < largura+2; j++){
                for (k = 0; k<3; k++)
                    sobel_resul[i][j][k] = '0';
            }

        for (i = 1; i < altura; i++) // le a imagem propriamente dita
            for (j = 1; j < largura; j++){
                for (k = 0; k<3; k++)
                    sobel_resul[i][j][k] = imagem3[i-1][j-1][k];
            }

        for (i = 1; i < altura; i++) // faz o calculo de cada pixel para comparação
            for (j = 1; j < largura; j++){
                for (k = 0; k<3; k++) {

                    pixel_x = ((gx[0][0] * sobel_resul[i - 1][j - 1][k]) +
                               (gx[0][1] * sobel_resul[i - 1][j][k]) +
                               (gx[0][2] * sobel_resul[i - 1][j + 1][k]) +
                               (gx[1][0] * sobel_resul[i][j - 1][k]) +
                               (gx[1][1] * sobel_resul[i][j][k]) +
                               (gx[1][2] * sobel_resul[i][j + 1][k]) +
                               (gx[2][0] * sobel_resul[i + 1][j - 1][k]) +
                               (gx[2][1] * sobel_resul[i + 1][j][k]) +
                               (gx[2][2] * sobel_resul[i + 1][j + 1][k]));


                    pixel_y = ((gy[0][0] * sobel_resul[i - 1][j - 1][k]) +
                               (gy[0][1] * sobel_resul[i - 1][j][k]) +
                               (gy[0][2] * sobel_resul[i - 1][j + 1][k]) +
                               (gy[1][0] * sobel_resul[i][j - 1][k]) +
                               (gy[1][1] * sobel_resul[i][j][k]) +
                               (gy[1][2] * sobel_resul[i][j + 1][k]) +
                               (gy[2][0] * sobel_resul[i + 1][j - 1][k]) +
                               (gy[2][1] * sobel_resul[i + 1][j][k]) +
                               (gy[2][2] * sobel_resul[i + 1][j + 1][k]));

                    if (pixel_x < 0) pixel_x = 0;// tratamento para não deixar sair do range de 0-255
                    if (pixel_y < 0) pixel_y = 0;
                    if (pixel_x > 255) pixel_x = 255;
                    if (pixel_y > 255) pixel_y = 255;

                    mx[i - 1][j - 1][k] = pixel_x; // mx recebe pixelx e my recebe pixely
                    my[i - 1][j - 1][k] = pixel_y;
                }

            }

        for (i = 1; i < altura; i++) // cria a imagem com o filtro de sobel
            for (j = 1; j < largura; j++){
                for (k = 0; k<3; k++) {

                    imagem3[i][j][k] = max(mx[i][j][k], my[i][j][k]); // filtro feito com o valor maximo
                    if (imagem3[i][j][k] < 0) imagem3[i][j][k] = 0;// tratamento para não deixar sair do range de 0-255
                    if (imagem3[i][j][k] > 255) imagem3[i][j][k] = 255;
                }

            }

    }

    //******************************************************************************************************************//
    //*** filtro Sharpening ***//

    else if (escolha == 6 && strcmp(tipo,"P2")==0) { // cinza

        const char gx[3][3] = {{-1, 0, -1},  // detecta os contornos na vertical
                               {0, 5, 0},
                               {-1, 0, -1}};


        int pixel_x = 0;
        int pixel_y = 0;

        for (i = 0; i < altura+2; i++) // cria uma borda de '0' para fazer o calculo da borda
            for (j = 0; j < largura+2; j++){
                sh_resul[i][j] = '0';
            }

        for (i = 1; i < altura; i++) // le a imagem propriamente dita
            for (j = 1; j < largura; j++){
                sh_resul[i][j] = imagem[i-1][j-1];
            }

        for (i = 1; i < altura; i++) // faz o calculo de cada pixel para comparação
            for (j = 1; j < largura; j++){

                pixel_x =   ((gx[0][0] * sh_resul[i-1][j-1]) +
                             (gx[0][1] * sh_resul[i-1][j]) +
                             (gx[0][2] * sh_resul[i-1][j+1]) +
                             (gx[1][0] * sh_resul[i][j-1]) +
                             (gx[1][1] * sh_resul[i][j]) +
                             (gx[1][2] * sh_resul[i][j+1]) +
                             (gx[2][0] * sh_resul[i+1][j-1]) +
                             (gx[2][1] * sh_resul[i+1][j]) +
                             (gx[2][2] * sh_resul[i+1][j+1]));


                if (pixel_x < 0) pixel_x = 0;// tratamento para não deixar sair do range de 0-255
                if (pixel_x > 255) pixel_x = 255;

                sh[i-1][j-1] = pixel_x; // sh recebe o valor do pixelx

            }

        for (i = 1; i < altura; i++) // cria a imagem com o filtro de sharpenig
            for (j = 1; j < largura; j++) {

                imagem[i][j] = sh[i][j];
                if (imagem[i][j] < 0) imagem[i][j] = 0;// tratamento para não deixar sair do range de 0-255
                if (imagem[i][j] > 255) imagem[i][j] = 255;

            }
    }
    else if (escolha == 6 && strcmp(tipo,"P3")==0) { // colorida


        const char gx[3][3] = {{0, -1, 0},  // detecta os contornos na vertical
                               {-1, 5, -1},
                               {0, -1, 0}};


        int pixel_x = 0;

        for (i = 0; i < altura+2; i++) // cria uma borda de '0' para fazer o calculo da borda
            for (j = 0; j < largura+2; j++){
                for (k = 0; k<3; k++)
                    sh_resulc[i][j][k] = '0';
            }

        for (i = 1; i < altura; i++) // le a imagem propriamente dita
            for (j = 1; j < largura; j++){
                for (k = 0; k<3; k++)
                    sh_resulc[i][j][k] = imagem3[i-1][j-1][k];
            }

        for (i = 1; i < altura; i++) // faz o calculo de cada pixel para comparação
            for (j = 1; j < largura; j++){
                for (k = 0; k<3; k++) {

                    pixel_x = ((gx[0][0] * sh_resulc[i - 1][j - 1][k]) +
                               (gx[0][1] * sh_resulc[i - 1][j][k]) +
                               (gx[0][2] * sh_resulc[i - 1][j + 1][k]) +
                               (gx[1][0] * sh_resulc[i][j - 1][k]) +
                               (gx[1][1] * sh_resulc[i][j][k]) +
                               (gx[1][2] * sh_resulc[i][j + 1][k]) +
                               (gx[2][0] * sh_resulc[i + 1][j - 1][k]) +
                               (gx[2][1] * sh_resulc[i + 1][j][k]) +
                               (gx[2][2] * sh_resulc[i + 1][j + 1][k]));


                    if (pixel_x < 0) pixel_x = 0;// tratamento para não deixar sair do range de 0-255

                    if (pixel_x > 255) pixel_x = 255;


                    shc[i - 1][j - 1][k] = pixel_x; // shc recebe o valor do pixelx
                }

            }

        for (i = 1; i < altura; i++) // cria a imagem com o filtro de sharpening
            for (j = 1; j < largura; j++){
                for (k = 0; k<3; k++) {

                    imagem3[i][j][k] = shc[i][j][k];
                    if (imagem3[i][j][k] < 0) imagem3[i][j][k] = 0; // tratamento para não deixar sair do range de 0-255
                    if (imagem3[i][j][k] > 255) imagem3[i][j][k] = 255;
                }

            }

    }

    //*******************************************************************************************************************//
    //*** tons de cinza ***//

    else if (escolha == 7 && strcmp(tipo,"P3")==0) { // cinza

        for (i =0; i < altura; i++)
            for (j = 0; j < largura; j++){

                double gs; // luminância, formula para coletar o ton de cinza de cada pixel
                gs =    imagem3[i][j][0]*(0.299)+
                        imagem3[i][j][1]*(0.587)+
                        imagem3[i][j][2]*(0.114);

                imagem3[i][j][0] = (unsigned char) gs; // pega o valor do ton de cinza e aplica no pixel
                imagem3[i][j][1] = (unsigned char) gs;
                imagem3[i][j][2] = (unsigned char) gs;
            }
    }

    //*******************************************************************************************************************//
    //*** Outra Operação ***//
    else if (escolha == 8 && strcmp(tipo,"P3")==0) {

        for (i =0; i < altura; i++) // converte a imagem colorida para tom de cinza
            for (j = 0; j < largura; j++){

                double gs; // luminância
                gs =    imagem3[i][j][0]*(0.299)+
                        imagem3[i][j][1]*(0.587)+
                        imagem3[i][j][2]*(0.114);

                imagem3[i][j][0] = (unsigned char) gs;
                imagem3[i][j][1] = (unsigned char) gs;
                imagem3[i][j][2] = (unsigned char) gs;
            }
        //unsigned char imagem4[altura][largura][3]; // cria uma matriz auxiliar com valores aleatórios entre 0 e 255
        for (i = 0; i < altura; i++) {
            for (j = 0; j < largura; j++) {
                int pixel = rand() % 256;
                for (k = 0; k<3; k++)
                    imagem4[i][j][k] = pixel;
            }
        }

        for (i = 0; i < altura; i++) // faz o nivelamento dos pixels em 3 tons de cinza, preto e branco
            for (j = 0; j < largura; j++){
                for (k = 0; k < 3; k++) {
                    if (imagem3[i][j][k] > 200)
                        imagem3[i][j][k] = 255;
                    else if (imagem3[i][j][k] > 120 && imagem3[i][j][k] <= 200) imagem3[i][j][k] = 200;

                    else if (imagem3[i][j][k] >= 80 && imagem3[i][j][k] <= 120) imagem3[i][j][k] = 100;

                    else if (imagem3[i][j][k] > 40 && imagem3[i][j][k] < 80) imagem3[i][j][k] = 50;

                    else imagem3[i][j][k] = 0;

                }
            }

        for (i = 0; i < altura; i+=3) // mescla as duas matrizes para dar impressão de pontilhismo
            for (j = 0; j < largura; j+=3) {
                for (k = 0; k < 3; k++) {
                    imagem3[i][j][k] = imagem4[i][j][k];
                }

            }
    }

//*** FIM DO TRATAMENTO DA IMAGEM ***//


//*** GRAVACAO DA IMAGEM ***//
//inicialmente nao sera necessario entender nem mudar nada nesta parte

    // Modificar aqui para trocar o nome da sida
    string nome; // cria uma string para receber o nome do arquivo
    cout << "Qual o nome do arquivo de saida ?" << endl;
    cin >> nome;
    nome = nome + ".pnm"; // adiciona a extensão do arquivo (mais prático)

	//*** Grava a nova imagem ***//
	arqsaida.open(nome,ios::out);	//Abre arquivo para escrita
	if (!arqsaida) {
		cout << "Nao consegui criar " << nome << endl;
		return 0;
	}

	arqsaida << tipo << endl;							//tipo
	arqsaida << "# TP3-INF110, by AGS\n";	//comentario
	arqsaida << largura << " " << altura;	//dimensoes
	arqsaida << " " << 255 << endl;				//maior valor

    if (strcmp(tipo,"P2")==0) { // saida para imagem em tons de cinza
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++)
                arqsaida << (int) imagem[i][j] << endl;    //pixels
    }
    else if (strcmp(tipo,"P3")==0) { // saida para imagens coloridas
        for (i = 0; i < altura; i++)
            for (j = 0; j < largura; j++)
                for (k = 0; k < 3; k++)
                arqsaida << (int) imagem3[i][j][k] << endl;    //pixels
    }

	arqsaida.close();		//fecha o arquivo
	//***************************//

//*** FIM DA GRAVACAO DA IMAGEM ***//

	return 0;
}
