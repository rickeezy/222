#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void  ChildProcess(int*, int*);

void  ParentProcess(int*, int*);

int main() {
	int shmid;
	time_t t;
	srand((unsigned)time(&t));

	// Create a shared memory segment for BankAccount
	shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
	if (shmid < 0) {
		perror("shmget");
		exit(1);
	}

	int *BankAccount = (int *)shmat(shmid, NULL, 0);
	*BankAccount = 0;

	// Create a shared memory segment for Turn
	int shmid_turn = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (shmid_turn < 0) {
    perror("shmget");
    exit(1);
  }

  int *Turn = (int *)shmat(shmid_turn, NULL, 0);
  *Turn = 0;

	int pid = fork();

	if (pid == 0) {
			// Child process (Withdraw)
			for (int i = 0; i < 25; i++) {
				while (*Turn != 1);
				printf("Child Process Started for the %d time.\n", i + 1);
				ChildProcess(BankAccount, Turn);
			}
	} else if (pid > 0) {
			// Parent process (Deposit)
			for (int i = 0; i < 25; i++) {
				while (*Turn != 0);
				printf("Parent Process Started for the %d time.\n", i + 1);
				ParentProcess(BankAccount, Turn);
			}
	} else {
			perror("fork");
			exit(1);
	}

	// Wait for child process to finish
	int status;
	wait(&status);

	// Print final balance
	printf("Final Balance: %d\n", *BankAccount);

	// Detach and remove shared memory segments
	shmdt(BankAccount);
	shmctl(shmid, IPC_RMID, NULL);

	shmdt(Turn);
	shmctl(shmid + 1, IPC_RMID, NULL);

	return 0;
}

void ChildProcess(int *BankAccount, int *Turn) {
	int sleep_time = rand() % 6;
	sleep(sleep_time);

	int account = *BankAccount;

	while (*Turn != 1);

	int balance = rand() % 51;

	printf("Poor Student needs $%d\n", balance);

	if (balance <= account) {
		account -= balance;
		printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
	} 
	else {
		printf("Poor Student: Not Enough Cash ($%d)\n", account);
	}
	*BankAccount = account;
	*Turn = 0;
}

void ParentProcess(int *BankAccount, int *Turn) {
	int sleep_time = rand() % 6;
  sleep(sleep_time);

	int account = *BankAccount;

	while (*Turn != 0);

	if (account <= 100) {
		int balance = rand() % 101;

		if (balance % 2 == 0) {
			account += balance;
			printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
		} 
		else {
			printf("Dear old Dad: Doesn't have any money to give\n");
		}
	} 
	else {
		printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
	}
	*BankAccount = account;
	*Turn = 1;
}