#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	FILE *dado;
	dado = fopen("produtos.data", "w");

	if (dado == NULL){
		fprintf(stderr, "Não foi possível abrir o arquivo.\n");
		return 1;
	}

	if (argc == 1){
		fprintf(stderr, "Falta argumentos!\n");
		return 1;
	}

	int i;
	fprintf(dado, "%d\n", atoi(argv[1]));
	for (i = 0; i < atoi(argv[1]); i++){
		fprintf(dado, "Monitor Dell %d\n", i);
		fprintf(dado, "30\n");
		fprintf(dado, "699,99\n");
	}

	fprintf(stdout, "Escrito com sucesso.\n");
	fclose(dado);

	return 0;
}