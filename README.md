PROJECT OS PART 2 HOW-TO
1) AI_IMPLEMENTATIONN είναι όλα το πως υλοποιείται ο επεξεργαστής + Η ΣΕΙΡΑ ΠΟΥ ΕΚΤΕΛΟΥΝΤΑΙ ΤΑ ΠΡΟΓΡΑΜΜΑΤΑ(order_of_exec.txt => έχει τα paths των .ο)
2) EXCECUTABLES ΕΊΝΑΙ Ο ΚΩΔΙΚΑΣ ΤΩΝ ΔΙΑΔΙΚΑΣΙΩΝ 
3) SIGNLE CORE HADJI αντισος οποιος θελει το βλεπει

1) κάνουμε compile με .o τα αρχεία των διαδικασιών ΟΠΩΣ τα βλέπουμε στο τελυταίο μέρος (μετά το /)
2) gcc -o test <όνομα αρχείου κώδικα από το AI IMPLEMENETATION>
3) ./test <αριθμός πυρήνων> <πολιτική <κβάντο αν θέλει> order_of_exec.txt 

παράδειγμα:
(είμαστε στο workpath folder) 
1) pumpkineater69@cheguevara:~/documents/ceid_os_part2/excecutables$ gcc -o print_1_1000 print_1_1000.c
2) pumpkineater69@cheguevara:~/documents/ceid_os_part2/excecutables$ gcc -o print_1001_2000 print_1001_2000.c
3) pumpkineater69@cheguevara:~/documents/ceid_os_part2/excecutables$ gcc -o dummy dummy_stuff.c
(μετά στο άλλον folder στα scripts)
4) pumpkineater69@cheguevara:~/documents/ceid_os_part2/source_script$ gcc -o multicore_scheduler multicore_scheduler.c
5) pumpkineater69@cheguevara:~/documents/ceid_os_part2/source_script$ ./multicore_scheduler 3 RRFF 10  order_of_exec.txt

** ΜΗΝ ΑΛΛΑΞΕΤΕ ΤΟ ORDER_OF_EXEC **
Ευχαριστώ, Σπύρος
