//Gaurav Anil Yeole
//EEL5934 Advanced System Programming
//Assignement - 3
	
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct account{
	int accountNumber;
	int balance;
	char status;
};

struct transfer{
	int accountC, accountD;
	int amount;
	pthread_cond_t transferDone;
};

struct account *accounts; 
struct transfer *transfers;

int numAccounts=0, numTransfers=0, numWorker;

pthread_t *workerThreads;
pthread_mutex_t mutex;
	
void* transaction(void* _tid){
	
	int i,j,Daccount,Caccount;
	long tid;
	tid = (long)_tid;
	
	for (i=0;i<numTransfers;i++){
		if(tid == (i%numWorker)){
			pthread_mutex_lock(&mutex);
			
			for (j=0;j<numAccounts;j++){
				if(accounts[j].accountNumber == transfers[i].accountC){
					break;
				}
			}
			Caccount = j;
			for (j=0;j<numAccounts;j++){
				if(accounts[j].accountNumber == transfers[i].accountD){
					break;
				}
			}
			Daccount = j;
			
			while((accounts[Daccount].status == 'B') || (accounts[Caccount].status == 'B'))
				pthread_cond_wait(&transfers[i].transferDone, &mutex);//release the lock and wait
			accounts[Daccount].status == 'B';
			accounts[Caccount].status == 'B';
			pthread_mutex_unlock(&mutex);//release the lock
			
			accounts[Daccount].balance = accounts[Daccount].balance - transfers[i].amount;
			accounts[Caccount].balance = accounts[Caccount].balance + transfers[i].amount;
			
			pthread_mutex_lock(&mutex);
			accounts[Daccount].status == 'A';
			accounts[Caccount].status == 'A';
			
			for(j=0; j<numTransfers; j++)
			{
				if(transfers[j].accountD == Daccount || transfers[j].accountD == Caccount)
					pthread_cond_signal(&transfers[j].transferDone);
			}
			for(j=0; j<numTransfers; j++)
			{
				if(transfers[j].accountC == Daccount || transfers[j].accountC == Caccount)
					pthread_cond_signal(&transfers[j].transferDone);
			}
			pthread_mutex_unlock(&mutex);
		}
	}
	pthread_exit(0);
}

void main(int argc,char **argv){
	
	char inputFile[40],input[40],strBuff[20];
	
	FILE *f;
	int i=0,j=0,k=0;
	
	strcpy(inputFile,argv[1]);
	f = fopen(inputFile,"r");
	numWorker = atoi(argv[2]);
	
	while(fgets(input,50,f) != NULL){
		if(input[0] != 'T'){
			numAccounts++;
		}
		else{
			numTransfers++;
		}
		
	}
	
	
	
	accounts = calloc(numAccounts, sizeof(struct account));	
	transfers = calloc(numTransfers, sizeof(struct transfer));
	
	rewind(f);
	
	for(i=0;i<numAccounts;i++){
		fgets(input,50,f);
		
		j = 0;
		k = 0;
		do {
			strBuff[k] = input[j];
			k++;
			j++;
		}while(input[j] != ' ');
		strBuff[k] = '\0';
		accounts[i].accountNumber = atoi(strBuff);
		
		
		k = 0;
		while(input[j] != '\n'){
			strBuff[k] = input[j];
			k++;
			j++;
		}
		strBuff[k] = '\0';
		accounts[i].balance = atoi(strBuff);
		
		
		accounts[i].status = 'A';
		strBuff[0] = '\0';
		
	}
	
	
	
	for(i=0;i<numTransfers;i++){
		fgets(input,50,f);
		
		
		for(j=0;input[j]!=' ';j++);
		
		j++;
		k=0;
		while(input[j] != ' '){
			strBuff[k] = input[j];
			k++;
			j++;
		}
		strBuff[k] = '\0';
		transfers[i].accountD = atoi(strBuff);
		
		
		strBuff[0] = '\0';
		j++;
		k = 0;
		while(input[j] != ' '){
			strBuff[k] = input[j];
			k++;
			j++;
		}
		strBuff[k] = '\0';
		transfers[i].accountC = atoi(strBuff);
		
		
		strBuff[0] = '\0';
		j++;
		k = 0;
		while(input[j] != '\n'){
			strBuff[k] = input[j];
			k++;
			j++;
		}
		strBuff[k] = '\0';
		transfers[i].amount = atoi(strBuff);
		
	}
	
	pthread_mutex_init(&mutex,NULL);
	
	for (i=0;i<numTransfers;i++){
		pthread_cond_init(&transfers[i].transferDone,NULL);
	}
	
	workerThreads = calloc(numWorker,sizeof(pthread_t));
	
	for (i=0;i<numWorker;i++){
		pthread_create(&workerThreads[i],NULL,transaction, (void*)(long)i);
	}
	
	for (i=0;i<numWorker;i++){
		pthread_join(workerThreads[i],NULL);
	}
	
	for(i=0; i<numAccounts; i++)
	{
		printf("%d %d\n", accounts[i].accountNumber,  accounts[i].balance);
	}

}