#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- DEFINIÇÕES GLOBAIS E ESTRUTURAS ---

#define TAM_FILA 5
#define TAM_PILHA 3

// Estrutura para representar uma peça
typedef struct {
    int id;
    char nome;
} Peca;

// Estrutura da Fila Circular
typedef struct {
    Peca itens[TAM_FILA];
    int inicio;
    int fim;
    int quantidade;
} FilaCircular;

// Estrutura da Pilha Linear
typedef struct {
    Peca itens[TAM_PILHA];
    int topo;
} Pilha;

// Enum para identificar o tipo da última ação realizada
typedef enum {
    NENHUMA, JOGAR, RESERVAR, USAR, TROCAR
} AcaoTipo;

// Estrutura para guardar a última ação e permitir "desfazer"
typedef struct {
    AcaoTipo tipo;
    Peca peca1;
    Peca peca2; // Usada apenas em operações com duas peças, como a troca
} UltimaAcao;


// VARIÁVEIS GLOBAIS DO JOGO

FilaCircular filaPecas;
Pilha pilhaReserva;
UltimaAcao historico;
int proximoId = 1; // Contador para garantir IDs únicos para as peças

// PROTÓTIPOS DAS FUNÇÕES

// Funções Auxiliares
void limparTela();
Peca gerarPeca();
void exibirEstadoDoJogo();

// Funções de Gerenciamento da Fila
void inicializarFila(FilaCircular *f);
int filaEstaVazia(FilaCircular *f);
int filaEstaCheia(FilaCircular *f);
void enfileirar(FilaCircular *f, Peca p);
Peca desenfileirar(FilaCircular *f);
Peca verFrenteFila(FilaCircular *f);
void visualizarFila(const FilaCircular *f);

// Funções de Gerenciamento da Pilha
void inicializarPilha(Pilha *p);
int pilhaEstaVazia(Pilha *p);
int pilhaEstaCheia(Pilha *p);
void empilhar(Pilha *p, Peca peca);
Peca desempilhar(Pilha *p);
Peca verTopoPilha(Pilha *p);
void visualizarPilha(const Pilha *p);

// Funções de Lógica do Jogo
void jogarPeca();
void reservarPeca();
void usarPecaReservada();
void trocarPecas();
void desfazerUltimaJogada();
void inverterFilaComPilha();


// FUNÇÃO PRINCIPAL

int main() {
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Configuração inicial do jogo
    inicializarFila(&filaPecas);
    inicializarPilha(&pilhaReserva);
    historico.tipo = NENHUMA;

    int opcao;
    do {
        limparTela();
        exibirEstadoDoJogo();

        printf("\n--- MENU DE ACOES (Nivel Mestre) ---\n");
        printf("1 - Jogar peca\n");
        printf("2 - Reservar peca\n");
        printf("3 - Usar peca reservada\n");
        printf("4 - Trocar peca (Fila <-> Pilha)\n");
        printf("5 - Desfazer ultima jogada\n");
        printf("6 - Inverter fila com pilha\n");
        printf("0 - Sair do jogo\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: jogarPeca(); break;
            case 2: reservarPeca(); break;
            case 3: usarPecaReservada(); break;
            case 4: trocarPecas(); break;
            case 5: desfazerUltimaJogada(); break;
            case 6: inverterFilaComPilha(); break;
            case 0: printf("Saindo do jogo. Ate mais!\n"); break;
            default: printf("Opcao invalida! Pressione Enter para continuar..."); getchar(); getchar(); break;
        }

        if (opcao != 0 && opcao != 5) {
             printf("\nAcao realizada. Pressione Enter para continuar...");
             getchar(); // Limpa o buffer de entrada
             getchar(); // Aguarda o usuário pressionar Enter
        }


    } while (opcao != 0);

    return 0;
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES ---

// Função para limpar o terminal (multiplataforma)
void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Gera uma nova peça com um nome aleatório e ID sequencial
Peca gerarPeca() {
    Peca p;
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    p.id = proximoId++;
    p.nome = tipos[rand() % 7];
    return p;
}

// Exibe o estado atual da fila e da pilha
void exibirEstadoDoJogo() {
    printf("TETRIS STACK\n");
    visualizarFila(&filaPecas);
    visualizarPilha(&pilhaReserva);
}


/*
 * FUNÇÕES DE GERENCIAMENTO DA FILA CIRCULAR
 */
void inicializarFila(FilaCircular *f) {
    f->inicio = 0;
    f->fim = -1;
    f->quantidade = 0;
    // Preenche a fila inicial com 5 peças
    for (int i = 0; i < TAM_FILA; i++) {
        enfileirar(f, gerarPeca());
    }
}

int filaEstaVazia(FilaCircular *f) {
    return f->quantidade == 0;
}

int filaEstaCheia(FilaCircular *f) {
    return f->quantidade == TAM_FILA;
}

void enfileirar(FilaCircular *f, Peca p) {
    if (!filaEstaCheia(f)) {
        f->fim = (f->fim + 1) % TAM_FILA;
        f->itens[f->fim] = p;
        f->quantidade++;
    }
}

Peca desenfileirar(FilaCircular *f) {
    Peca p = {-1, ' '}; // Peça inválida
    if (!filaEstaVazia(f)) {
        p = f->itens[f->inicio];
        f->inicio = (f->inicio + 1) % TAM_FILA;
        f->quantidade--;
    }
    return p;
}

Peca verFrenteFila(FilaCircular* f) {
    if (!filaEstaVazia(f)) {
        return f->itens[f->inicio];
    }
    Peca p_vazia = {-1, ' '};
    return p_vazia;
}

void visualizarFila(const FilaCircular *f) {
    printf("--> Fila de Proximas Pecas (%d/%d):\n    ", f->quantidade, TAM_FILA);
    if (f->quantidade == 0) {
        printf("[ Fila Vazia ]");
    } else {
        int i = f->inicio;
        for (int count = 0; count < f->quantidade; count++) {
            printf("[ID:%d, %c] ", f->itens[i].id, f->itens[i].nome);
            i = (i + 1) % TAM_FILA;
        }
    }
    printf("\n");
}


/*
 * FUNÇÕES DE GERENCIAMENTO DA PILHA
 */
void inicializarPilha(Pilha *p) {
    p->topo = -1;
}

int pilhaEstaVazia(Pilha *p) {
    return p->topo == -1;
}

int pilhaEstaCheia(Pilha *p) {
    return p->topo == TAM_PILHA - 1;
}

void empilhar(Pilha *p, Peca peca) {
    if (!pilhaEstaCheia(p)) {
        p->topo++;
        p->itens[p->topo] = peca;
    }
}

Peca desempilhar(Pilha *p) {
    Peca peca = {-1, ' '}; // Peça inválida
    if (!pilhaEstaVazia(p)) {
        peca = p->itens[p->topo];
        p->topo--;
    }
    return peca;
}

Peca verTopoPilha(Pilha* p) {
     if (!pilhaEstaVazia(p)) {
        return p->itens[p->topo];
    }
    Peca p_vazia = {-1, ' '};
    return p_vazia;
}

void visualizarPilha(const Pilha *p) {
    printf("--> Pilha de Reserva (%d/%d):\n    ", p->topo + 1, TAM_PILHA);
    if (p->topo == -1) {
        printf("[ Pilha Vazia ]");
    } else {
        for (int i = 0; i <= p->topo; i++) {
            printf("[ID:%d, %c] ", p->itens[i].id, p->itens[i].nome);
        }
    }
    printf("\n");
}


/*
 * FUNÇÕES DE LÓGICA DO JOGO
 */

// 1. Remove a peça da frente da fila e a reabastece com uma nova.
void jogarPeca() {
    if (filaEstaVazia(&filaPecas)) {
        printf("A fila esta vazia, impossivel jogar.\n");
        return;
    }
    Peca pecaJogada = desenfileirar(&filaPecas);
    printf("Peca [ID:%d, %c] foi jogada.\n", pecaJogada.id, pecaJogada.nome);

    // Salva a ação no histórico
    historico.tipo = JOGAR;
    historico.peca1 = pecaJogada;

    // Reabastece a fila
    enfileirar(&filaPecas, gerarPeca());
}

// 2. Move a peça da frente da fila para a pilha de reserva.
void reservarPeca() {
    if (pilhaEstaCheia(&pilhaReserva)) {
        printf("Pilha de reserva cheia!\n");
        return;
    }
    if (filaEstaVazia(&filaPecas)) {
        printf("Fila vazia, nada para reservar.\n");
        return;
    }
    Peca pecaReservada = desenfileirar(&filaPecas);
    empilhar(&pilhaReserva, pecaReservada);
    printf("Peca [ID:%d, %c] foi reservada.\n", pecaReservada.id, pecaReservada.nome);

    // Salva a ação no histórico
    historico.tipo = RESERVAR;
    historico.peca1 = pecaReservada;

    // Reabastece a fila
    enfileirar(&filaPecas, gerarPeca());
}

// 3. Usa a peça do topo da pilha de reserva.
void usarPecaReservada() {
    if (pilhaEstaVazia(&pilhaReserva)) {
        printf("Pilha de reserva esta vazia!\n");
        return;
    }
    Peca pecaUsada = desempilhar(&pilhaReserva);
    printf("Peca [ID:%d, %c] da reserva foi usada.\n", pecaUsada.id, pecaUsada.nome);

    // Salva a ação no histórico
    historico.tipo = USAR;
    historico.peca1 = pecaUsada;
}

// 4. Troca a peça da frente da fila com a do topo da pilha.
void trocarPecas() {
    if (filaEstaVazia(&filaPecas) || pilhaEstaVazia(&pilhaReserva)) {
        printf("E necessario ter pecas na fila e na pilha para trocar!\n");
        return;
    }
    Peca pecaDaFila = desenfileirar(&filaPecas);
    Peca pecaDaPilha = desempilhar(&pilhaReserva);

    empilhar(&pilhaReserva, pecaDaFila);
    // Para manter a ordem na fila circular, precisamos inserir no início,
    // o que é complexo. Uma simplificação é "empurrar" os itens.
    FilaCircular tempFila;
    inicializarFila(&tempFila);
    desenfileirar(&tempFila); //remove a primeira peça gerada

    enfileirar(&tempFila, pecaDaPilha);
    while(!filaEstaVazia(&filaPecas)){
        enfileirar(&tempFila, desenfileirar(&filaPecas));
    }
    filaPecas = tempFila;

    printf("Pecas trocadas com sucesso!\n");

    // Salva a ação no histórico
    historico.tipo = TROCAR;
    historico.peca1 = pecaDaPilha; // Peça que foi para a fila
    historico.peca2 = pecaDaFila; // Peça que foi para a pilha
}

// 5. Desfaz a última ação (JOGAR, RESERVAR ou USAR).
void desfazerUltimaJogada() {
    printf("Tentando desfazer a ultima acao...\n");
    switch(historico.tipo) {
        case JOGAR:
            // Lógica complexa: não é possível reverter de forma simples no escopo deste desafio.
            // Para uma implementação real, seria necessário um histórico mais robusto.
            printf("Desfazer 'Jogar' nao e suportado nesta versao simplificada.\n");
            break;
        case RESERVAR:
            // Lógica complexa.
            printf("Desfazer 'Reservar' nao e suportado nesta versao simplificada.\n");
            break;
        case USAR:
            if (!pilhaEstaCheia(&pilhaReserva)) {
                empilhar(&pilhaReserva, historico.peca1);
                printf("Acao 'Usar Peca' desfeita. A peca voltou para a reserva.\n");
                historico.tipo = NENHUMA; // Limpa o histórico
            } else {
                printf("Nao foi possivel desfazer: a pilha de reserva esta cheia.\n");
            }
            break;
        case TROCAR:
             printf("Desfazer 'Trocar' nao e suportado nesta versao simplificada.\n");
            break;
        case NENHUMA:
            printf("Nenhuma acao para desfazer.\n");
            break;
    }
     printf("\nPressione Enter para continuar...");
     getchar(); getchar();
}


// 6. Inverte o conteúdo da fila e da pilha.
void inverterFilaComPilha() {
    if (pilhaEstaVazia(&pilhaReserva)) {
        printf("A pilha está vazia, não há o que inverter.\n");
        return;
    }
    // Armazena as peças temporariamente
    Peca tempFila[TAM_FILA];
    int countFila = filaPecas.quantidade;
    for(int i=0; i < countFila; i++){
        tempFila[i] = desenfileirar(&filaPecas);
    }

    Peca tempPilha[TAM_PILHA];
    int countPilha = pilhaReserva.topo + 1;
    for(int i=0; i < countPilha; i++){
        tempPilha[i] = desempilhar(&pilhaReserva);
    }

    // Reinserir de forma invertida
    for(int i=0; i < countPilha; i++){
        enfileirar(&filaPecas, tempPilha[i]);
    }

    for(int i=0; i < countFila; i++){
        empilhar(&pilhaReserva, tempFila[i]);
    }
    
    //Preenche o resto da fila se necessário
    while(!filaEstaCheia(&filaPecas)){
        enfileirar(&filaPecas, gerarPeca());
    }

    printf("Fila e Pilha foram invertidas!\n");
    historico.tipo = NENHUMA; // Inverter invalida o "desfazer"
}
