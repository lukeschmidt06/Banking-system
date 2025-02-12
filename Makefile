FLAGS=-g

all: bank bank_global_lock bank_account_lock

benchmark.o: benchmark.c
	gcc $(FLAGS) -c benchmark.c -o benchmark.o

bank.o: bank.c
	gcc $(FLAGS) -c bank.c -o bank.o

bank: benchmark.o bank.o
	gcc -pthread bank.o benchmark.o -o bank

bank_global_lock.o: bank_global_lock.c
	gcc $(FLAGS) -c bank_global_lock.c -o bank_global_lock.o

bank_global_lock: benchmark.o bank_global_lock.o
	gcc -pthread bank_global_lock.o benchmark.o -o bank_global_lock

bank_account_lock.o: bank_account_lock.c
	gcc $(FLAGS) -c bank_account_lock.c -o bank_account_lock.o

bank_account_lock: bank_account_lock.o benchmark.o
	gcc -pthread bank_account_lock.o benchmark.o -o bank_account_lock

clean:
	rm -f *.o bank bank_account_lock bank_global_lock