#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

typedef struct{
	int **matrix;
	int size, K;
	int ind;
} EMat;

int gcd(int a, int b){
	return b?gcd(b, a%b) : a;
}

void* Work(void *matr){
	EMat* currentMat = (EMat*) matr;
	int **currentMatr = currentMat->matrix;

	int *resultMatrix = malloc(sizeof(int*) * currentMat->size);
	for(int i = 0; i < currentMat->size; ++i)
		resultMatrix[i] = 0;

	for(int i = 0; i < currentMat->K; ++i){
		for(int j = 0; j < currentMat->size; ++j){
			resultMatrix[j] += currentMatr[i][j];
		}
	}
	return resultMatrix;
}

void* Work2(void* EM){
	EMat* currentEM = (EMat*) EM;
	int curInd = currentEM->ind;
	int** currentMat = currentEM->matrix;
	int* resultMat = malloc(sizeof(int*) * currentEM->size);
	for(int i = 0; i < currentEM->size; ++i)
		resultMat[i] = 0;

	for(int i = 0; i < currentEM->K; ++i){
		for(int j = 0; j < currentEM->size; ++j){
			resultMat[j] += currentMat[i][j + curInd];
		}
	}

	return resultMat;
}

int main(int argc, char * argv[]){
	if(argc != 2){
		printf("USAGE: %s <count of threads>\n", argv[0]);
		return 1;
	}

	srand(time(0));
	int threadCount;
	if(!(threadCount = strtol(argv[1], NULL, 10)) || threadCount < 1){
		printf("Argument  isn't int or less than 1!\n");
		return 1;
	}
	int size, K;
	printf("Write size and count: ");
	scanf("%d%d",&size,&K);
	printf("ThreadCount: %d\n",threadCount);
	printf("Size: %d, K: %d\n",size,K);
	int **matrix = malloc(K *  sizeof(int*));
	for(int i = 0; i < K; ++i)
		matrix[i] = malloc(size * sizeof(int*));

	for(int i = 0; i < K; ++i){
		for(int j = 0; j < size; ++j){
			matrix[i][j] = rand() % 10 + 1;
		}
	}

	printf("Your matrix:\n");
	for(int i = 0; i < K && i < 10; ++i){
		for(int j = 0; j < size && j < 10; ++j){
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}

	EMat* test = malloc(sizeof(EMat*));
	test->matrix = matrix;
	test->K = K;
	test->size = size;

	printf("\n[1] result: \n");
	time_t start = clock();
	int* res1 = Work(test);
	for(int i = 0; i < size; ++i){
		if(i == 30){
			printf("...");
			break;
		}
		printf("%d ", res1[i]);
	}
	printf("\n");
	printf("Time: %fs", (float)(clock()-start)/CLOCKS_PER_SEC);
	printf("\n\n");

	pthread_t *ptrArr;
	if(threadCount < size){
		int ptrCountDiv = size / threadCount, ptrCountMod = size % threadCount;
		ptrArr = malloc(sizeof(pthread_t) * threadCount);
		int currentIndexOfEM = 0;
		start = clock();
		for(int i = 0; i < threadCount; ++i){
			EMat* curEM = malloc(sizeof(EMat*));
			curEM->K = K;
			curEM->size = ptrCountDiv;
			curEM->matrix = matrix;
			curEM->ind = currentIndexOfEM;
			currentIndexOfEM += ptrCountDiv;
			pthread_create(&ptrArr[i], NULL, Work2, curEM);
		}

		int* endMat = malloc(sizeof(int*) * ptrCountMod);
		for(int i = 0; i < ptrCountMod; ++i)
			endMat[i] = 0;

		for(int i = size - ptrCountMod; i < size; ++i){
			for(int j = 0; j < K; ++j){
				endMat[i - size + ptrCountMod] += matrix[j][i];
			}
		}

		int** resultMat = malloc(sizeof(int*) * (threadCount + 1));
		for(int i = 0; i < threadCount; ++i){
			void* takeEM;
			pthread_join(ptrArr[i], &takeEM);
			resultMat[i] = (int*) takeEM;
		}

		printf("[2] result: \n");
		for(int i = 0; i < threadCount; ++i){
			if(i == 10){
				printf("...");
				break;
			}
			int* curTake = resultMat[i];
			for(int j = 0; j < ptrCountDiv; ++j){
				if(j == 30){
					printf("...");
					break;
				}
				printf("%d ", curTake[j]);
			}
		}
		
		for(int i = 0; i < ptrCountMod; ++i){
			printf("%d ", endMat[i]);
		}
		printf("\nTime: %fs\n", (float)(clock()-start)/CLOCKS_PER_SEC);
	}
	else{
		ptrArr = malloc(sizeof(pthread_t) * size);
		int curInd = 0;
		start = clock();
		for(int i = 0; i < size; ++i){
			EMat* curEM = malloc(sizeof(EMat*));
			curEM->K = K;
			curEM->size = 1;
			curEM->matrix = matrix;
			curEM->ind = curInd++;
			pthread_create(&ptrArr[i], NULL, Work2, curEM);
			// создает новый поток
			// &ptrArr[i] - указатель на структуру-идентификатор
			// Work2 - функция, запускаемая в потоке
			// curEM - список аргументов для функции
		}

		printf("[2] result: \n");
		for(int i = 0; i < size; ++i){
			void* curEM;
			pthread_join(ptrArr[i], &curEM);
			// ожидание завершения потока
			// ptrArr[i] - структура
			// &curEM - указатель на переменную, в которую будет записан результат, возвращаемый потоком
			int* tmp = (int*)curEM;
			printf("%d ", tmp[0]);
		}
		printf("\nTime: %fs\n", (float)(clock()-start)/CLOCKS_PER_SEC);
	}
	printf("\n");
	return 0;
}
