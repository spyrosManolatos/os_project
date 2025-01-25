######################################
## Manolatos Spyridon, 1104802
## Maroudas Andreas-Erikos, 1100617
## Kirkinis Manousos, 1100590
## Chrysanthakopoulos Nikolaos, 1100760
####################################

./exercise1 2 FCFS order_of_exec.txt > fcfs_exercise1.txt # exercise 1 FCFS with 4 cores and exercise 1
./exercise1 4 RR 3 order_of_exec.txt > rr_exercise1.txt # exercise 1 RR with 4 cores and quantum=3
./exercise2 1 FCFS order_of_exec.txt > fcfs_exercise2.txt # exercise 2 FCFS with 2 cores and one process multiple cores
./exercise1 4 RRAFF 3 order_of_exec.txt > rrff_exercise1.txt # exercise 1 RRAFF with 4 cores and quantum=3