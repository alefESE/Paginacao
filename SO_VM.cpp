#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib> // atoi(stoi sem referencia)
#include <climits>

using namespace std;

int main(void){
    
    ifstream arq;
    arq.open("so.txt");
    if(arq.fail()){
        std::cout << "nao foi possivel encontrar o arquivo so.txt" << endl;
		std::system("pause");
        return 1;
    }

    string linha;
    getline(arq, linha);
	int tamanho = atoi(linha.c_str());
    vector<int> referencias;
    while(!arq.eof()){
        getline(arq, linha);
        referencias.push_back(atoi(linha.c_str()));
    }

	vector<int> memoria_fifo(tamanho, -1), memoria_otm(tamanho, -1), memoria_lru(tamanho, -1);
	int escolhido;
    int prox_fifo = 0, prox_otm = 0, prox_lru = 0;
    int faltas_fifo = 0, faltas_otm = 0, faltas_lru = 0;
    bool hit = false, vazio = false;
    
    
     /*   debug
    std::cout << "tamanho: " << tamanho << endl;
    std::cout << "rerencias: " << referencias.size() << endl;
	std::cout << "memoria: " << memoria_fifo.size() << endl;
    */

    /****************************
    *********    FIFO   *********
    *****************************/
    for(int i = 0; i < referencias.size(); i++){
		//std::cout << "Referencia: " << referencias[i] << endl;
        for(int ii = 0; ii < memoria_fifo.size(); ii++){
            if(referencias[i] == memoria_fifo[ii]) {//hit
				//std::cout << "Hit: " << ii << endl;
                hit = true;
                break;
            }
			if (!vazio && memoria_fifo[ii] == -1) {//se miss e bloco vazio
				escolhido = ii;
				vazio = true;
			}
        }
        if(!hit){ //falta
			++faltas_fifo;//conta falta
			//std::cout << "Miss" << endl;
			if (vazio) {//traz pagina para o espaco vazio
				//std::cout << "Troca: " << escolhido << endl;
				memoria_fifo[escolhido] = referencias[i];
			} else {
				/* a fifo apenas circula a memoria
					(algoritmo de decisao)
				*/
				//std::cout << "Troca: " << prox_fifo << endl;
				memoria_fifo[prox_fifo] = referencias[i]; //carrega referencia na memoria
				prox_fifo = (prox_fifo+1)%tamanho;//fifo circular
			}
        }
		vazio = false;
        hit = false;
    }

    int maior = 0;

    /****************************
    *********    OTM   **********
    *****************************/
    for(int i = 0; i < referencias.size(); i++) {
		//std::cout << "Referencia: " << referencias[i] << endl;
        for(int ii = 0; ii < memoria_otm.size(); ii++){
			if (referencias[i] == memoria_otm[ii]) {//hit
				//std::cout << "Hit: " << ii << endl;
				hit = true;
				break;
			}
			if (!vazio && memoria_otm[ii] == -1) {//se miss e bloco vazio
				escolhido = ii;
				vazio = true;
			}
        }
		if (!hit) { //falta
			maior = 0;
			faltas_otm++;//conta falta
			//std::cout << "Miss" << endl;
			if (vazio) {//traz pagina para o espaco vazio
				//std:cout << "Troca: " << escolhido << endl;
				memoria_otm[escolhido] = referencias[i];
			} else {
				/*verifica qual pagina sera referenciada em um maior intervalo de tempo
					(algoritmo de decisao)
				*/
				hit = false;//aux
				for (int j = 0; j < memoria_otm.size(); j++) {
					for (int jj = i + 1; jj < referencias.size(); jj++) {//percorre referencias remanecentes
						if (referencias[jj] == memoria_otm[j]) {//pagina vai ser referenciada
							hit = true;
							if (maior <= (jj - i)) {
								//std::cout << "Maior: " << j << endl;
								maior = jj - i;//maior distancia
								prox_otm = j;//indice da pagina com maior distancia
							}
							break;
						}
					}
					if (!hit) {//a pagina nao sera mais referenciada
						prox_otm = j;
						break;
					}
					hit = false;//reinicia
				}
				//std::cout << "Troca: " << prox_otm << endl;
				memoria_otm[prox_otm] = referencias[i];//traz pagina para memoria
			}
		}
		vazio = false;
		hit = false;
	}

	//aux
	maior = 0;

    /****************************
    *********    LRU   **********
    *****************************/
    for(int i = 0; i < referencias.size(); i++){
		//std::cout << "Referencia: " << referencias[i] << endl;
        for(int ii = 0; ii < memoria_lru.size(); ii++){
            if(referencias[i] == memoria_lru[ii]) {//hit
				//std::cout << "Hit: " << ii << endl;
                hit = true;
                break;
            }
			if (!vazio && memoria_lru[ii] == -1) {
				escolhido = ii;
				vazio = true;
			}
        }
        if(!hit){ //faltas
			maior = 0;
			faltas_lru++;
			//std::cout << "Miss" << endl;
            /* Verifica qual pagina foi referenciada em um maior intervalo de tempo
                (algoritmo de decisao)
            */
            //verifica se a memoria ja foi completamente populada por paginas
            if(vazio){
				//std::cout << "Troca: " << escolhido << endl;
				memoria_lru[escolhido] = referencias[i];
            } else {
				for (int j = 0; j < memoria_lru.size(); j++) {
					for (int jj = i - 1; jj >= 0; --jj) {//verifica as referencias de traz para frente
						if (referencias[jj] == memoria_lru[j]) {//posicao encontrada
							if (maior < (i - jj)) {
								//std::cout << "Maior: " << j << endl;
								maior = i - jj;//distancia
								prox_lru = j;//pagina mais distante
							}
							break;//garante a menor distancia da pagina em relacao a referencia
						}
					}
				}
				//std::cout << "Troca: " << prox_lru << endl;
				memoria_lru[prox_lru] = referencias[i];
            }
        }
		vazio = false;
        hit = false;
    }

    //printa FIFO
    std::cout << "FIFO " << faltas_fifo << endl;
    //printa OTM
    std::cout << "OTM " << faltas_otm << endl;
    //printa LRU
    std::cout << "LRU " << faltas_lru << endl;
	std::system("pause");

    return 0;
}