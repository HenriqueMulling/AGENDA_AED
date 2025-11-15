#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nameSize 50 //50 bytes para o nome
#define emailSize 50 // e 50 bytes para o email

void addPerson(void *pBuffer); 
void removePerson(void *pBuffer); 
void searchPerson(void *pBuffer); 
void list(void *pBuffer); 

int main () {
    void *pBuffer = NULL;
    // Alocamos espaço extra de nameSize para o buffer temporário seguro no heap
    pBuffer = malloc(sizeof(int)*2 + sizeof(void *) + nameSize * 2); // alocando memória
    
    //opção (4 bytes), contador (4 bytes), ponteiro da lista (8 bytes) e espaço pro nome (50 bytes)
    // + espaço extra para buffer temporário (50 bytes)
    
    if (pBuffer == NULL) {
        perror("==== MEMÓRIA INSUFICIENTE ===="); 
        return 1;
    } // Teste para ver se tem memória, se não tiver encerra.

    *(int *)pBuffer = 0; // opção - 0 ao 3 (4 bytes)
    *(int *)((char *)pBuffer + sizeof(int)) = 0; // contador de pessoas - 4 ao 7 (4 bytes)
    *(void **)((char *)pBuffer + sizeof(int)*2) = NULL; // lista - 8 ao 15 (8 bytes)

    do {
        printf("\n__________________________\n");
        printf("|          AGENDA          |\n");
        printf("|__________________________|\n");
        printf("|  [1] Adicionar Pessoa    |\n");
        printf("|  [2] Remover Pessoa      |\n");
        printf("|  [3] Buscar Pessoa       |\n");
        printf("|  [4] Listar Todos        |\n");
        printf("|  [5] Sair                |\n");
        printf("|__________________________|\n");
        printf(" Escolha uma opcao: ");

        // Lê a opção usando fgets no espaço temporário do próprio pBuffer (primeiro bloco de nameSize)
        void *readBuf = (char *)pBuffer + sizeof(int)*2 + sizeof(void *); // espaço temporário já alocado
        if (fgets((char *)readBuf, nameSize, stdin) != NULL) {
            // converte pra inteiro e armazena em pBuffer[0]
            *(int *)pBuffer = (int) strtol((char *)readBuf, NULL, 10); // Convertendo de string para inteiro
        } else {
            *(int *)pBuffer = 0;
        }

        switch (*(int *)pBuffer) {
            case 1: {
                addPerson(pBuffer);
                break;
            }
            case 2: {
                removePerson(pBuffer);
                break;
            }
            case 3: {
                searchPerson(pBuffer);
                break;
            }
            case 4: {
                list(pBuffer);
                break;
            }
        }

    } while (*(int *)pBuffer != 5);

    // Aqui eu limpo a lista antes de liberar o pBuffer.
    void **list = (void **)((char *)pBuffer + sizeof(int)*2); //ponteiro para ponteiro que aponta para o campo dentro do pBuffer onde ta o ponteiro da lista
    void *current = *list; //ponteiro pra apontar para primeira pessoa que ta na lista 
    while (current != NULL) {
        void *prox = *(void **)current; //o ponteiro prox recebe o endereço da proxima pessoa da lista
        free(current); // limpa o atual
        current = prox; // agora o atual é igual o proximo
    }
    free(pBuffer); //limpa lista
    return 0;
}

void addPerson(void *pBuffer) {
    void **list = (void **)((char *)pBuffer + sizeof(int)*2);
    //  Aqui o nó guarda que guarda o ponteiro de próximo + nome + idade + email
    void *new = malloc(sizeof(void *) + nameSize + sizeof(int) + emailSize);
    if (new == NULL) {
        return;
    }
    *(void **)new = NULL; // próximo

    // --- LER NOME ---
    printf("Nome: ");
    if (fgets((char *)new + sizeof(void *), nameSize, stdin) == NULL) {
        free(new);
        return;
    }
    // remover newline se existir
    void *iter = (char *)new + sizeof(void *);
    while (*(char *)iter != '\0' && *(char *)iter != '\n') {
        iter = (char *)iter + 1;
    }
    if (*(char *)iter == '\n') {
        *(char *)iter = '\0';
    }

    // --- LER IDADE ---
    void *tmp = (char *)pBuffer + sizeof(int)*2 + sizeof(void *) + nameSize; // segundo bloco de nameSize como buffer temporário
    printf("Idade: ");
    if (fgets((char *)tmp, nameSize, stdin) == NULL) {
        free(new);
        return;
    }
    // converte e armazena na posição correta do nó
    *(int *)((char *)new + sizeof(void *) + nameSize) = (int) strtol((char *)tmp, NULL, 10);

    // --- LER EMAIL ---
    printf("Email: ");
    if (fgets((char *)new + sizeof(void *) + nameSize + sizeof(int), emailSize, stdin) == NULL) {
        free(new);
        return;
    }
    // remover newline do email
    iter = (char *)new + sizeof(void *) + nameSize + sizeof(int);
    while (*(char *)iter != '\0' && *(char *)iter != '\n') {
        iter = (char *)iter + 1;
    }
    if (*(char *)iter == '\n') {
        *(char *)iter = '\0';
    }

    // Inserir no início da lista - (porque é o mais simples)
    *(void **)new = *list;
    *list = new;

    (*(int *)((char *)pBuffer + sizeof(int)))++; //incrementa um no contador dentro do pBuffer para ver a quantidade de pessoas cadastradas
}

void removePerson(void *pBuffer) {
    void **list = (void **)((char *)pBuffer + sizeof(int)*2); //pegar o ponteiro da lista dentro do pBuffer
    void *nameTemp = (char *)pBuffer + sizeof(int)*2 + sizeof(void *) + nameSize; // agora usa o segundo bloco de nameSize como buffer temporário seguro no heap

    printf("Digite o nome da pessoa que deseja remover: ");
    if (fgets((char*)nameTemp, nameSize, stdin) == NULL) {
        printf("\nErro de leitura!\n");
        return;
    }
    // remover newline
    void *iter = (char *)nameTemp;
    while (*(char *)iter != '\0' && *(char *)iter != '\n') {
        iter = (char *)iter + 1;
    }
    if (*(char *)iter == '\n') {
        *(char *)iter = '\0';
    }

    if (*list == NULL) {
        printf("\n===== LISTA VAZIA =====!\n");
        return;
    }

    void *previous = NULL;       // ponteiro pro anterior
    void *current = *list;        // começa na primeira pessoa da lista

    // Percorre a lista
    while (current) {
        void *nameCurrent = (char *)current + sizeof(void *); // nome começa logo após o ponteiro pro próximo nó

        if (strcmp((char*)nameCurrent, (char*)nameTemp) == 0) {
            if (previous == NULL) {
                *list = *(void **)current; // Remove o primeiro da lista
            } else {
                *(void **)previous = *(void **)current; // Remove um do meio ou fim
            }

            free(current); //libera o primeiro
            (*(int *)((char *)pBuffer + sizeof(int)))--; // decrementa contador
            printf("\n==== PESSOA REMOVIDA COM SUCESSO! ====\n");
            return;
        }

        // Avança na lista
        previous = current;
        current = *(void **)current; // vai pro próximo
    }

    printf("\n==== PESSOA NÃO ENCONTRADA ====\n");
}

void searchPerson(void *pBuffer) {
    void **list = (void **)((char *)pBuffer + sizeof(int)*2);
    void *nameTemp = (char *)pBuffer + sizeof(int)*2 + sizeof(void *) + nameSize; // segundo bloco de nameSize como buffer temporário seguro

    // Verifica se lista está vazia
    if (*list == NULL) {
        printf("\n==== LISTA VAZIA ====\n");
        return;
    }

    // Lê o nome do espaço temporário seguro
    printf("Digite o nome que deseja buscar: ");
    if (fgets((char*)nameTemp, nameSize, stdin) == NULL) {
        printf("\nErro de leitura!\n");
        return;
    }
    // remover newline
    void *iter = (char *)nameTemp;
    while (*(char *)iter != '\0' && *(char *)iter != '\n') {
        iter = (char *)iter + 1;
    }
    if (*(char *)iter == '\n') {
        *(char *)iter = '\0';
    }

    // Ponteiro auxiliar pra percorrer a lista
    void *aux = *list;

    // Percorre a lista
    while (aux) {
        void *nameCurrent = (char *)aux + sizeof(void *);
        void *idade = (char *)aux + sizeof(void *) + nameSize; // ponteiro para idade
        void *email = (char *)aux + sizeof(void *) + nameSize + sizeof(int); // ponteiro para email

        if (strcmp((char*)nameCurrent, (char*)nameTemp) == 0) {
            printf("\n==== PESSOA ENCONTRADA! ====\n");
            printf("Nome: %s\n", (char*)nameCurrent);
            printf("Idade: %d\n", *(int*)idade);
            printf("Email: %s\n", (char*)email);
            return;
        }

        aux = *(void **)aux; //avança pra o proximo
    }

    printf("\n==== PESSOA NÃO ENCONTRADA ====\n");
}

void list(void *pBuffer) {
    void **list = (void **)((char *)pBuffer + sizeof(int)*2);
    void *aux = *list;

    if (*list == NULL) {
        printf("\n==== LISTA VAZIA ====\n");
        return;
    }

    while (aux) {
        void *nameCurrent = (char *)aux + sizeof(void *);
        void *idade = (char *)aux + sizeof(void *) + nameSize;
        void *email = (char *)aux + sizeof(void *) + nameSize + sizeof(int);

        // Acesso seguro aos campos do nó, sem usar buffer temporário do topo da stack
        printf("====================\n");
        printf("Nome: %s\n", (char *)nameCurrent);
        printf("Idade: %d\n", *(int *)idade);
        printf("Email: %s\n", (char *)email);
        printf("====================\n");

        aux = *(void **)aux;
    }
}
