rm testes.csv
    for threads in 1 2 4
    do
        for N in 1000 1000000
        do
            for i in {1..5}
            do
                
                ./A4 $N $threads >> testes.csv
            done
        done
    done