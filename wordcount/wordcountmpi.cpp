#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

void count_words(string &buffer, unordered_map<string, int> &dicionario_local){
    stringstream texto(buffer);
    string palavra;

    // >> quando usado com stringstream separa as palavras pelos espaços
    while(texto >> palavra){
        dicionario_local[palavra]++;
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    unordered_map <string, int> dicionario_local;

    //rank 0 vai distribuir o texto entre os outros
    if (rank == 0){
        
        ifstream arquivo(argv[1]); //abrir o arquivo

        if (!arquivo.is_open()){
            cout << "Não conseguiu abrir o arquivo" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        

        stringstream aux;
        aux << arquivo.rdbuf(); //rdbuf lê o arquivo inteiro direto e coloca em uma stringstream

        string linha;
        int index = 1;
        while(getline(aux, linha)) {
            int linha_size = linha.size();

            MPI_Send(&linha_size, 1, MPI_INT, index, 0, MPI_COMM_WORLD);
            MPI_Send(linha.data(), linha_size, MPI_CHAR, index, 0, MPI_COMM_WORLD);
            if(index == size - 1) {
                index = 1;
            }
            index++;
        }

        int parar = -1;
        for(int i = 1; i < size; i++){
            MPI_Send(&parar, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        while(true) {
            int chunk_size;
            MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(chunk_size == -1) {
                break;
            }

            vector<char> buffer(chunk_size);
            MPI_Recv(buffer.data(), chunk_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string linha(buffer.begin(), buffer.end());
    
            count_words(linha, dicionario_local);
        }
    }

    //agrupar os resultados 
    if (rank == 0){
        unordered_map<string, int> dicionario;

        for (int i = 1; i < size; i++){
            int tamanho_dicionario;
            MPI_Recv(&tamanho_dicionario, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < tamanho_dicionario; j++){
                int tamanho_palavra;
                MPI_Recv(&tamanho_palavra, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                vector<char> buffer(tamanho_palavra);
                MPI_Recv(buffer.data(), tamanho_palavra, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int frequencia;
                MPI_Recv(&frequencia, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                string palavra(buffer.begin(), buffer.end());

                cout << "Recebido do processo " << i << ": " << palavra << " -> " << frequencia << endl;
                dicionario[palavra] += frequencia;
            }
        }
        
        cout << "Dicionário de palavras e suas frequências:" << endl;
        for (const auto& par : dicionario) {
            cout << par.first << " -> " << par.second << endl;
        }
    } else {
        int tamanho_dic_local = dicionario_local.size();
        cout << "Tamanho dicionario local processo " << rank << ": " << tamanho_dic_local << endl;

        //MPI_Send espera um ponteiro dos dados que serao enviados, por isso precisa do &
        MPI_Send(&tamanho_dic_local, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        for (const auto &par : dicionario_local){
            int tamanho_palavra = par.first.size();
            MPI_Send(&tamanho_palavra, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);   
            MPI_Send(par.first.data(), par.first.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&par.second, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    
    MPI_Finalize();
    return 0;
}
