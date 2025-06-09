#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // Para SetConsoleOutputCP e SetConsoleCP
#include <string.h>

#define MAX_STRING 100

// Estrutura para representar uma pe�a do hardware
typedef struct {
    char modelo[MAX_STRING];
    char marca[MAX_STRING];
    char peca[MAX_STRING]; // Nome da pe�a (ex: Placa de V�deo, Processador)
    int anoFabric;
    char cor[MAX_STRING];
    float preco; // Tipo corrigido para float
    int status; // 0 = Vendido, 1 = Em estoque, 2 = Com defeito
} Hardware;

// Estrutura (Struct) do n� da fila (Node)
typedef struct Node {
    Hardware hardware;
    struct Node* proximo;
} Node;

//Estrutura (Struct) da fila
typedef struct {
    Node* inicio;
    Node* fim;
} Fila;

// Agora a fun��o para criar uma nova fila
Fila* criarFila() {
    Fila* fila = (Fila*)malloc(sizeof(Fila));
    if (fila != NULL) {
        fila->inicio = NULL;
        fila->fim = NULL;
    }
    return fila;
}

// Agora a fun��o para inserir uma pe�a de hardware na fila
void inserirNaFila(Fila* fila, Hardware hardware) {
    Node* novo = (Node*)malloc(sizeof(Node));
    if (novo != NULL) {
        novo->hardware = hardware;
        novo->proximo = NULL;

        if (fila->fim == NULL) { // Se a fila estiver vazia
            fila->inicio = novo;
            fila->fim = novo;
        } else { // Se a fila j� tiver elementos
            fila->fim->proximo = novo;
            fila->fim = novo;
        }
    }
}

// A fun��o para liberar a mem�ria da fila
void liberarFila(Fila* fila) {
    Node* atual = fila->inicio;
    while (atual != NULL) {
        Node* temp = atual;
        atual = atual->proximo;
        free(temp); // Aqui libera o n� atual
    }
    free(fila); // Aqui libera a estrutura da fila em si
}

// Fun��o auxiliar para converter o status num�rico para uma string
const char* obterStatusString(int status) {
    if (status == 0) {
        return "Vendido";
    } else if (status == 1) {
        return "Em estoque";
    } else if (status == 2) {
        return "Com defeito";
    } else {
        return "Desconhecido"; // Para status inv�lidos
    }
}

// Fun��o para limpar o buffer de entrada (mais port�til que fflush(stdin))
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Fun��o para listar todos as pe�as cadastradas no arquivo
void listarHardware() {
    FILE* arquivo;
    Fila* fila = criarFila(); // Aqui cria uma fila para armazenar as pe�as lidas
    char linha[MAX_STRING];
    Hardware hardware;
    int contador = 0;

    arquivo = fopen("hardware.txt", "r"); // Aqui abre o arquivo para leitura
    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo 'hardware.txt'! Pode ser que ainda n�o haja pe�as cadastradas.\n");
        system("pause");
        system("cls");
        liberarFila(fila); // Aqui � importante liberar a fila mesmo em caso de erro
        return;
    }

    printf("\n===== Pe�as de Hardware Cadastradas =====\n");

    // Agora para verificar se o arquivo est� vazio
    fseek(arquivo, 0, SEEK_END); // Vai para o final do arquivo
    if (ftell(arquivo) == 0) { // Verifica a posi��o atual (o tamanho do arquivo)
        printf("\nO arquivo est� vazio! Nenhuma pe�a cadastrada.\n");
        fclose(arquivo);
        liberarFila(fila);
        system("pause");
        system("cls");
        return;
    }
    rewind(arquivo); // Aqui volta para o in�cio do arquivo

    int hardwareAtual = 0; // Flag para saber se estamos lendo um bloco de pe�a

    // Aqui � um loop para ler linha por linha do arquivo
    while (fgets(linha, MAX_STRING, arquivo) != NULL) {
        if (strstr(linha, "===== HARDWARE =====") != NULL) {
            hardwareAtual = 1; // In�cio de um novo bloco de pe�a
            continue; // Pula para a pr�xima linha
        }

        if (hardwareAtual) {
            // A ordem de leitura DEVE corresponder � ordem de escrita em cadastrarHardware
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

    // Listar as pe�as da fila na tela
    if (contador == 0) {
        printf("\nNenhuma pe�a cadastrada p�de ser lida!\n");
    } else {
        Node* atual = fila->inicio;
        int i = 1;

        while (atual != NULL) {
            printf("\nHardware %d:\n", i++);
            printf("Modelo: %s\n", atual->hardware.modelo);
            printf("Marca: %s\n", atual->hardware.marca);
            printf("Peca: %s\n", atual->hardware.peca); // Nome da pe�a
            printf("Ano: %d\n", atual->hardware.anoFabric);
            printf("Cor: %s\n", atual->hardware.cor);
            printf("Preco: %.2f\n", atual->hardware.preco); // Exibindo float com 2 casas decimais
            // Aqui usa a fun��o auxiliar para exibir o status como string
            printf("Status: %s\n", obterStatusString(atual->hardware.status));
            printf("--------------------\n");

            atual = atual->proximo;
        }

        printf("\nTotal de pecas: %d\n", contador);
    }

    liberarFila(fila); // Liberar a mem�ria da fila
    system("pause");
    system("cls");
}

// Fun��o para cadastrar um novo hardware e salv�-lo no arquivo
void cadastrarHardware() {
    Hardware hardware;
    FILE *arquivo;

    printf("\n===== Cadastro de Pe�as de Hardware =====\n");

    limparBufferEntrada(); // Limpa o buffer antes do primeiro fgets

    printf("Modelo: ");
    fgets(hardware.modelo, MAX_STRING, stdin);
    hardware.modelo[strcspn(hardware.modelo, "\n")] = 0; // Aqui remove a quebra de linha

    printf("Marca: ");
    fgets(hardware.marca, MAX_STRING, stdin);
    hardware.marca[strcspn(hardware.marca, "\n")] = 0;

    printf("Pe�a: "); // Nome da pe�a (ex: Placa de V�deo, Processador)
    fgets(hardware.peca, MAX_STRING, stdin);
    hardware.peca[strcspn(hardware.peca, "\n")] = 0;

    printf("Ano de Fabrica��o: ");
    scanf("%d", &hardware.anoFabric);
    limparBufferEntrada(); // Limpa o buffer ap�s scanf

    printf("Cor: ");
    fgets(hardware.cor, MAX_STRING, stdin);
    hardware.cor[strcspn(hardware.cor, "\n")] = 0;

    printf("Pre�o da Pe�a: ");
    scanf("%f", &hardware.preco); // CORRIGIDO: Lendo como float
    limparBufferEntrada(); // Limpa o buffer ap�s scanf

    printf("Status (0 = Vendido, 1 = Em estoque, 2 = Com defeito): ");
    scanf("%d", &hardware.status);

    // Validar o status: agora aceita 0, 1 ou 2
    while (hardware.status != 0 && hardware.status != 1 && hardware.status != 2) {
        printf("Status inv�lido! Insira 0 para VENDIDO, 1 para EM ESTOQUE ou 2 para COM DEFEITO: ");
        scanf("%d", &hardware.status);
    }
    limparBufferEntrada(); // Limpa o buffer ap�s scanf

    arquivo = fopen("hardware.txt", "a"); // Abre o arquivo para escrita (APPEND)

    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo para escrita!\n");
        system("pause");
        system("cls");
        return;
    }

    // Aqui escreva os dados no arquivo com a formata��o e o status convertido para string
    // A ORDEM AQUI DEVE SER A MESMA DAS FUN��ES DE LEITURA (listarHardware, buscarHardware)
    fprintf(arquivo, "===== HARDWARE =====\n");
    fprintf(arquivo, "Modelo: %s\n", hardware.modelo);
    fprintf(arquivo, "Marca: %s\n", hardware.marca);
    fprintf(arquivo, "Peca: %s\n", hardware.peca); // CORRIGIDO: Escrevendo o nome da pe�a (string)
    fprintf(arquivo, "Ano: %d\n", hardware.anoFabric);
    fprintf(arquivo, "Cor: %s\n", hardware.cor);
    fprintf(arquivo, "Preco: %.2f\n", hardware.preco); // CORRIGIDO: Escrevendo o pre�o como float
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

// Fun��o para buscar pe�as por modelo no arquivo
void buscarHardware() {
    FILE* arquivo;
    Fila* fila = criarFila(); // Fila para armazenar apenas os resultados da busca
    char linha[MAX_STRING];
    Hardware hardware;
    char modeloBusca[MAX_STRING];
    int contador = 0;

    printf("\n===== Buscar Hardware =====\n");

    limparBufferEntrada(); // Limpa o buffer antes do fgets

    printf("Insira o modelo do hardware que deseja buscar: "); // Mudado de "ve�culo" para "hardware"
    fgets(modeloBusca, MAX_STRING, stdin);
    modeloBusca[strcspn(modeloBusca, "\n")] = 0;

    arquivo = fopen("hardware.txt", "r");
    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo 'hardware.txt'! Pode ser que ainda n�o haja pe�as cadastradas.\n");
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
            // A ordem de leitura DEVE corresponder � ordem de escrita em cadastrarHardware
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

                // Aqui converte a string de status lida do arquivo para o valor num�rico
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
        printf("\nNenhuma pe�a encontrada com o modelo '%s'!\n", modeloBusca);
    } else {
        Node* atual = fila->inicio;
        int i = 1;

        printf("\nForam encontrados %d pe�a(s) com o modelo '%s':\n", contador, modeloBusca);

        while (atual != NULL) {
            printf("\nHardware %d:\n", i++);
            printf("Modelo: %s\n", atual->hardware.modelo);
            printf("Marca: %s\n", atual->hardware.marca);
            printf("Peca: %s\n", atual->hardware.peca); // Nome da pe�a
            printf("Ano: %d\n", atual->hardware.anoFabric);
            printf("Cor: %s\n", atual->hardware.cor);
            printf("Preco: %.2f\n", atual->hardware.preco); // Exibindo float com 2 casas decimais
            // Usa a fun��o auxiliar para exibir o status como string
            printf("Status: %s\n", obterStatusString(atual->hardware.status));
            printf("-----------------------\n");

            atual = atual->proximo;
        }
    }

    liberarFila(fila);
    system("pause");
    system("cls");
}

// Aqui � a fun��o principal do programa (menu de op��es)
int main() {
    // Configurar o console para suportar caracteres UTF-8
    // Isso � importante para exibir caracteres acentuados corretamente no Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    int opcao;

    do {
        printf("\n===== TecList - Sua lista de pe�as de Hardware =====\n");
        printf("1 - Listar pe�as no estoque\n");
        printf("2 - Cadastrar nova pe�a no sistema\n");
        printf("3 - Buscar pe�a requisitada no sistema\n");
        printf("4 - Sair\n");
        printf("Insira um n�mero de acordo com a op��o desejada: ");
        scanf("%d", &opcao);
        limparBufferEntrada(); // Limpa o buffer ap�s ler a op��o do menu

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
                printf("-> Encerrando o sistema. At� a pr�xima!\n");
                break;
            default:
                printf("-> Op��o inv�lida. Por favor, tente novamente.\n");
        }

    } while (opcao != 4); // Continua exibindo o menu at� o usu�rio escolher Sair (op��o 4)

    return 0; // Programa encerrado com sucesso.
}
