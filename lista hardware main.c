#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // Para SetConsoleOutputCP e SetConsoleCP
#include <string.h>

#define MAX_STRING 100

// Estrutura para representar uma peça do hardware
typedef struct {
    char modelo[MAX_STRING];
    char marca[MAX_STRING];
    char peca[MAX_STRING]; // Nome da peça (ex: Placa de Vídeo, Processador)
    int anoFabric;
    char cor[MAX_STRING];
    float preco; // Tipo corrigido para float
    int status; // 0 = Vendido, 1 = Em estoque, 2 = Com defeito
} Hardware;

// Estrutura (Struct) do nó da fila (Node)
typedef struct Node {
    Hardware hardware;
    struct Node* proximo;
} Node;

//Estrutura (Struct) da fila
typedef struct {
    Node* inicio;
    Node* fim;
} Fila;

// Agora a função para criar uma nova fila
Fila* criarFila() {
    Fila* fila = (Fila*)malloc(sizeof(Fila));
    if (fila != NULL) {
        fila->inicio = NULL;
        fila->fim = NULL;
    }
    return fila;
}

// Agora a função para inserir uma peça de hardware na fila
void inserirNaFila(Fila* fila, Hardware hardware) {
    Node* novo = (Node*)malloc(sizeof(Node));
    if (novo != NULL) {
        novo->hardware = hardware;
        novo->proximo = NULL;

        if (fila->fim == NULL) { // Se a fila estiver vazia
            fila->inicio = novo;
            fila->fim = novo;
        } else { // Se a fila já tiver elementos
            fila->fim->proximo = novo;
            fila->fim = novo;
        }
    }
}

// A função para liberar a memória da fila
void liberarFila(Fila* fila) {
    Node* atual = fila->inicio;
    while (atual != NULL) {
        Node* temp = atual;
        atual = atual->proximo;
        free(temp); // Aqui libera o nó atual
    }
    free(fila); // Aqui libera a estrutura da fila em si
}

// Função auxiliar para converter o status numérico para uma string
const char* obterStatusString(int status) {
    if (status == 0) {
        return "Vendido";
    } else if (status == 1) {
        return "Em estoque";
    } else if (status == 2) {
        return "Com defeito";
    } else {
        return "Desconhecido"; // Para status inválidos
    }
}

// Função para limpar o buffer de entrada (mais portátil que fflush(stdin))
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Função para listar todos as peças cadastradas no arquivo
void listarHardware() {
    FILE* arquivo;
    Fila* fila = criarFila(); // Aqui cria uma fila para armazenar as peças lidas
    char linha[MAX_STRING];
    Hardware hardware;
    int contador = 0;

    arquivo = fopen("hardware.txt", "r"); // Aqui abre o arquivo para leitura
    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo 'hardware.txt'! Pode ser que ainda não haja peças cadastradas.\n");
        system("pause");
        system("cls");
        liberarFila(fila); // Aqui é importante liberar a fila mesmo em caso de erro
        return;
    }

    printf("\n===== Peças de Hardware Cadastradas =====\n");

    // Agora para verificar se o arquivo está vazio
    fseek(arquivo, 0, SEEK_END); // Vai para o final do arquivo
    if (ftell(arquivo) == 0) { // Verifica a posição atual (o tamanho do arquivo)
        printf("\nO arquivo está vazio! Nenhuma peça cadastrada.\n");
        fclose(arquivo);
        liberarFila(fila);
        system("pause");
        system("cls");
        return;
    }
    rewind(arquivo); // Aqui volta para o início do arquivo

    int hardwareAtual = 0; // Flag para saber se estamos lendo um bloco de peça

    // Aqui é um loop para ler linha por linha do arquivo
    while (fgets(linha, MAX_STRING, arquivo) != NULL) {
        if (strstr(linha, "===== HARDWARE =====") != NULL) {
            hardwareAtual = 1; // Início de um novo bloco de peça
            continue; // Pula para a próxima linha
        }

        if (hardwareAtual) {
            // A ordem de leitura DEVE corresponder à ordem de escrita em cadastrarHardware
            if (strstr(linha, "Modelo:") != NULL) {
                sscanf(linha, "Modelo: %[^\n]", hardware.modelo);
            } else if (strstr(linha, "Marca:") != NULL) {
                sscanf(linha, "Marca: %[^\n]", hardware.marca);
            } else if (strstr(linha, "Peca:") != NULL) { // Corrigido para "Peca:"
                sscanf(linha, "Peca: %[^\n]", hardware.peca);
            } else if (strstr(linha, "Ano:") != NULL) {
                sscanf(linha, "Ano: %d", &hardware.anoFabric);
            } else if (strstr(linha, "Cor:") != NULL) {
                sscanf(linha, "Cor: %[^\n]", hardware.cor);
            } else if (strstr(linha, "Preco:") != NULL) { // Corrigido para "Preco:"
                sscanf(linha, "Preco: %f", &hardware.preco); // Corrigido: lendo float
            } else if (strstr(linha, "Status:") != NULL) {
                char statusStr[MAX_STRING];
                sscanf(linha, "Status: %[^\n]", statusStr);

                if (strcmp(statusStr, "Em estoque") == 0) {
                    hardware.status = 1;
                } else if (strcmp(statusStr, "Vendido") == 0) {
                    hardware.status = 0;
                } else if (strcmp(statusStr, "Com defeito") == 0) {
                    hardware.status = 2;
                } else {
                    hardware.status = -1; // Status desconhecido
                }

                // Agora com todos os dados dos hardwares, inserimos na fila
                inserirNaFila(fila, hardware);
                contador++;
                hardwareAtual = 0; // Fim do bloco de hardware atual
            }
        }
    }

    fclose(arquivo); // Aqui fecha o arquivo

    // Listar as peças da fila na tela
    if (contador == 0) {
        printf("\nNenhuma peça cadastrada pôde ser lida!\n");
    } else {
        Node* atual = fila->inicio;
        int i = 1;

        while (atual != NULL) {
            printf("\nHardware %d:\n", i++);
            printf("Modelo: %s\n", atual->hardware.modelo);
            printf("Marca: %s\n", atual->hardware.marca);
            printf("Peca: %s\n", atual->hardware.peca); // Nome da peça
            printf("Ano: %d\n", atual->hardware.anoFabric);
            printf("Cor: %s\n", atual->hardware.cor);
            printf("Preco: %.2f\n", atual->hardware.preco); // Exibindo float com 2 casas decimais
            // Aqui usa a função auxiliar para exibir o status como string
            printf("Status: %s\n", obterStatusString(atual->hardware.status));
            printf("--------------------\n");

            atual = atual->proximo;
        }

        printf("\nTotal de pecas: %d\n", contador);
    }

    liberarFila(fila); // Liberar a memória da fila
    system("pause");
    system("cls");
}

// Função para cadastrar um novo hardware e salvá-lo no arquivo
void cadastrarHardware() {
    Hardware hardware;
    FILE *arquivo;

    printf("\n===== Cadastro de Peças de Hardware =====\n");

    limparBufferEntrada(); // Limpa o buffer antes do primeiro fgets

    printf("Modelo: ");
    fgets(hardware.modelo, MAX_STRING, stdin);
    hardware.modelo[strcspn(hardware.modelo, "\n")] = 0; // Aqui remove a quebra de linha

    printf("Marca: ");
    fgets(hardware.marca, MAX_STRING, stdin);
    hardware.marca[strcspn(hardware.marca, "\n")] = 0;

    printf("Peça: "); // Nome da peça (ex: Placa de Vídeo, Processador)
    fgets(hardware.peca, MAX_STRING, stdin);
    hardware.peca[strcspn(hardware.peca, "\n")] = 0;

    printf("Ano de Fabricação: ");
    scanf("%d", &hardware.anoFabric);
    limparBufferEntrada(); // Limpa o buffer após scanf

    printf("Cor: ");
    fgets(hardware.cor, MAX_STRING, stdin);
    hardware.cor[strcspn(hardware.cor, "\n")] = 0;

    printf("Preço da Peça: ");
    scanf("%f", &hardware.preco); // CORRIGIDO: Lendo como float
    limparBufferEntrada(); // Limpa o buffer após scanf

    printf("Status (0 = Vendido, 1 = Em estoque, 2 = Com defeito): ");
    scanf("%d", &hardware.status);

    // Validar o status: agora aceita 0, 1 ou 2
    while (hardware.status != 0 && hardware.status != 1 && hardware.status != 2) {
        printf("Status inválido! Insira 0 para VENDIDO, 1 para EM ESTOQUE ou 2 para COM DEFEITO: ");
        scanf("%d", &hardware.status);
    }
    limparBufferEntrada(); // Limpa o buffer após scanf

    arquivo = fopen("hardware.txt", "a"); // Abre o arquivo para escrita (APPEND)

    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo para escrita!\n");
        system("pause");
        system("cls");
        return;
    }

    // Aqui escreva os dados no arquivo com a formatação e o status convertido para string
    // A ORDEM AQUI DEVE SER A MESMA DAS FUNÇÕES DE LEITURA (listarHardware, buscarHardware)
    fprintf(arquivo, "===== HARDWARE =====\n");
    fprintf(arquivo, "Modelo: %s\n", hardware.modelo);
    fprintf(arquivo, "Marca: %s\n", hardware.marca);
    fprintf(arquivo, "Peca: %s\n", hardware.peca); // CORRIGIDO: Escrevendo o nome da peça (string)
    fprintf(arquivo, "Ano: %d\n", hardware.anoFabric);
    fprintf(arquivo, "Cor: %s\n", hardware.cor);
    fprintf(arquivo, "Preco: %.2f\n", hardware.preco); // CORRIGIDO: Escrevendo o preço como float
    fprintf(arquivo, "Status: %s\n", obterStatusString(hardware.status));
    fprintf(arquivo, "================\n\n");

    // Verificar se houve erro na escrita
    if (ferror(arquivo)) {
        printf("\nErro ao escrever no arquivo!\n");
        fclose(arquivo);
        system("pause");
        system("cls");
        return;
    }

    fclose(arquivo); // Aqui fecha o arquivo

    printf("\nHardware cadastrado com sucesso!\n");
    printf("Dados salvos no arquivo hardware.txt\n");
    system("pause");
    system("cls");
}

// Função para buscar peças por modelo no arquivo
void buscarHardware() {
    FILE* arquivo;
    Fila* fila = criarFila(); // Fila para armazenar apenas os resultados da busca
    char linha[MAX_STRING];
    Hardware hardware;
    char modeloBusca[MAX_STRING];
    int contador = 0;

    printf("\n===== Buscar Hardware =====\n");

    limparBufferEntrada(); // Limpa o buffer antes do fgets

    printf("Insira o modelo do hardware que deseja buscar: "); // Mudado de "veículo" para "hardware"
    fgets(modeloBusca, MAX_STRING, stdin);
    modeloBusca[strcspn(modeloBusca, "\n")] = 0;

    arquivo = fopen("hardware.txt", "r");
    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo 'hardware.txt'! Pode ser que ainda não haja peças cadastradas.\n");
        system("pause");
        system("cls");
        liberarFila(fila);
        return;
    }

    printf("\n===== Resultados da Busca =====\n");

    int hardwareAtual = 0;

    while (fgets(linha, MAX_STRING, arquivo) != NULL) {
        if (strstr(linha, "===== HARDWARE =====") != NULL) {
            hardwareAtual = 1;
            continue;
        }

        if (hardwareAtual) {
            // A ordem de leitura DEVE corresponder à ordem de escrita em cadastrarHardware
            if (strstr(linha, "Modelo:") != NULL) {
                sscanf(linha, "Modelo: %[^\n]", hardware.modelo);
            } else if (strstr(linha, "Marca:") != NULL) {
                sscanf(linha, "Marca: %[^\n]", hardware.marca);
            } else if (strstr(linha, "Peca:") != NULL) { // Corrigido para "Peca:"
                sscanf(linha, "Peca: %[^\n]", hardware.peca);
            } else if (strstr(linha, "Ano:") != NULL) {
                sscanf(linha, "Ano: %d", &hardware.anoFabric);
            } else if (strstr(linha, "Cor:") != NULL) {
                sscanf(linha, "Cor: %[^\n]", hardware.cor);
            } else if (strstr(linha, "Preco:") != NULL) { // Corrigido para "Preco:"
                sscanf(linha, "Preco: %f", &hardware.preco); // Corrigido: lendo float
            } else if (strstr(linha, "Status:") != NULL) {
                char statusStr[MAX_STRING];
                sscanf(linha, "Status: %[^\n]", statusStr);

                // Aqui converte a string de status lida do arquivo para o valor numérico
                if (strcmp(statusStr, "Em estoque") == 0) {
                    hardware.status = 1;
                } else if (strcmp(statusStr, "Vendido") == 0) {
                    hardware.status = 0;
                } else if (strcmp(statusStr, "Com defeito") == 0) {
                    hardware.status = 2;
                } else {
                    hardware.status = -1; // Status desconhecido
                }

                // Verifica se o modelo lido corresponde ao modelo de busca
                if (strcmp(hardware.modelo, modeloBusca) == 0) {
                    inserirNaFila(fila, hardware); // Insere apenas se o modelo corresponder
                    contador++;
                }
                hardwareAtual = 0; // Fim do bloco de hardware atual
            }
        }
    }

    fclose(arquivo);

    // Exibir os resultados da busca
    if (contador == 0) {
        printf("\nNenhuma peça encontrada com o modelo '%s'!\n", modeloBusca);
    } else {
        Node* atual = fila->inicio;
        int i = 1;

        printf("\nForam encontrados %d peça(s) com o modelo '%s':\n", contador, modeloBusca);

        while (atual != NULL) {
            printf("\nHardware %d:\n", i++);
            printf("Modelo: %s\n", atual->hardware.modelo);
            printf("Marca: %s\n", atual->hardware.marca);
            printf("Peca: %s\n", atual->hardware.peca); // Nome da peça
            printf("Ano: %d\n", atual->hardware.anoFabric);
            printf("Cor: %s\n", atual->hardware.cor);
            printf("Preco: %.2f\n", atual->hardware.preco); // Exibindo float com 2 casas decimais
            // Usa a função auxiliar para exibir o status como string
            printf("Status: %s\n", obterStatusString(atual->hardware.status));
            printf("-----------------------\n");

            atual = atual->proximo;
        }
    }

    liberarFila(fila);
    system("pause");
    system("cls");
}

// Aqui é a função principal do programa (menu de opções)
int main() {
    // Configurar o console para suportar caracteres UTF-8
    // Isso é importante para exibir caracteres acentuados corretamente no Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    int opcao;

    do {
        printf("\n===== TecList - Sua lista de peças de Hardware =====\n");
        printf("1 - Listar peças no estoque\n");
        printf("2 - Cadastrar nova peça no sistema\n");
        printf("3 - Buscar peça requisitada no sistema\n");
        printf("4 - Sair\n");
        printf("Insira um número de acordo com a opção desejada: ");
        scanf("%d", &opcao);
        limparBufferEntrada(); // Limpa o buffer após ler a opção do menu

        switch (opcao) {
            case 1:
                listarHardware();
                break;
            case 2:
                cadastrarHardware();
                break;
            case 3:
                buscarHardware();
                break;
            case 4:
                printf("-> Encerrando o sistema. Até a próxima!\n");
                break;
            default:
                printf("-> Opção inválida. Por favor, tente novamente.\n");
        }

    } while (opcao != 4); // Continua exibindo o menu até o usuário escolher Sair (opção 4)

    return 0; // Programa encerrado com sucesso.
}
