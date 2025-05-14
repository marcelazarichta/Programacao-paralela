#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <mpi.h>
#include <sstream>


using namespace std;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 7){
        cout << "Devem ser exatamente 7 processos" << endl;
        MPI_Finalize();
    }

    unordered_map <string, int> dicionario_local;

    if (rank == 0) {
        
        ifstream arquivo(argv[1]); 

        if (!arquivo.is_open()){
            cout << "Não conseguiu abrir o arquivo" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        

        stringstream aux;
        aux << arquivo.rdbuf(); 

        string linha;
        int index = 1;
        while(getline(aux, linha)) {
            int linha_size = linha.size();

            MPI_Send(&linha_size, 1, MPI_INT, index, 0, MPI_COMM_WORLD);
            MPI_Send(linha.data(), linha_size, MPI_CHAR, index, 0, MPI_COMM_WORLD);

            if(index == 3) {
                index = 0;
            }
            index++;
        }
   

        int parar = -1;
        for(int i = 1; i < 4; i++){
            MPI_Send(&parar, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }


        // MANDAR PARADA PARA OS PROCESSOS 4 E 5
        int parada = 0;
  
        while (true){
            int sinal_recebido;
            MPI_Recv(&sinal_recebido, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            parada += sinal_recebido;

            if (parada == 3){
                for (int i = 4; i < 6; i++){
                    MPI_Send(&sinal_recebido, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
                }
                break;
            }
        }

    } else if (rank > 0 && rank < 4) {
        
        unordered_map <string, int> dicionario_local;
        int parada = 0;
        
        // RECEBENDO LINHAS DO PROCESSO 0
        while(true) {
            int chunk_size;
            MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(chunk_size == -1) {
                break;
            }

            vector<char> buffer(chunk_size);
            MPI_Recv(buffer.data(), chunk_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string aux(buffer.begin(), buffer.end());


            stringstream linha(aux);
            string palavra;


            //MANDANDO PALAVRAS PARA OS PROCESSOS 4 E 5
            while (linha >> palavra){
                int rank_destino = 4 + (hash<string>{}(palavra) % 2);
                MPI_Send(&parada, 1, MPI_INT, rank_destino, 3, MPI_COMM_WORLD);

                //cout << "Enviando palavra " << palavra << " para processo " << rank_destino << endl;

                int tamanho_palavra = palavra.size();
                MPI_Send(&tamanho_palavra, 1, MPI_INT, rank_destino, 1, MPI_COMM_WORLD); 

                MPI_Send(palavra.data(), palavra.size(), MPI_CHAR, rank_destino, 2, MPI_COMM_WORLD);
            }
        }
        parada = 1;
        MPI_Send(&parada, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);

    } else if (rank == 4 || rank == 5) {

        int buffer;
        int parada = 0;

        while(true){
            MPI_Recv(&parada, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(parada == 1){
                cout << "Processo " << rank << " recebeu parada" << endl;
                break;
            }

            int tamanho_palavra;

            MPI_Status status;
            MPI_Recv(&tamanho_palavra, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);

            int source = status.MPI_SOURCE;
            
            vector<char> buffer(tamanho_palavra);
            MPI_Recv(buffer.data(), tamanho_palavra, MPI_CHAR, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            string palavra(buffer.begin(), buffer.end());

            cout << "Recebeu palavra " << palavra << " do processo " << source << endl;

            dicionario_local[palavra]++;
        }

        int tamanho_dic_local = dicionario_local.size();

        MPI_Send(&tamanho_dic_local, 1, MPI_INT, 6, 0, MPI_COMM_WORLD);

        for (const auto &par : dicionario_local){
            int tamanho_palavra = par.first.size();
            MPI_Send(&tamanho_palavra, 1, MPI_INT, 6, 0, MPI_COMM_WORLD);   
            MPI_Send(par.first.data(), par.first.size(), MPI_CHAR, 6, 0, MPI_COMM_WORLD);
            MPI_Send(&par.second, 1, MPI_INT, 6, 0, MPI_COMM_WORLD);
        }

    } else {

        unordered_map<string, int> dicionario;

        for (int i = 4; i < 6; i++){
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

                dicionario[palavra] += frequencia;
            }
        }

        cout << "Palavras e suas frequências:" << endl;
        for (const auto& par : dicionario) {
            cout << par.first << " -> " << par.second << endl;
        }

    }
    
    MPI_Finalize();
    return 0;
}