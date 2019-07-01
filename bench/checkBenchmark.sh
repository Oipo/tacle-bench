#!/bin/bash

COMPILER=aarch64-linux-gnu-gcc # Please adapt this line to your favorite compiler.
#COMPILER=patmos-clang
#COMPILER=cc

OPTIONS=" -Wall -Wno-unknown-pragmas -Werror -static -O2 -I /home/oipo/Programming/linux-kernel-module-cheat/submodules/gem5/include/ "
M5_LIB="/home/oipo/Programming/linux-kernel-module-cheat/submodules/gem5/util/m5/libm5.a "
EXEC="/home/oipo/Programming/linux-kernel-module-cheat/out/gem5/default/build/ARM/gem5.opt /home/oipo/Programming/linux-kernel-module-cheat/submodules/gem5/configs/example/arm/edify_se.py  " # Adapt if the executable is to be executed via another program
#EXEC=valgrind\ -q
#EXEC=pasim

PASS=0
FAIL_COMP=0
FAIL_EXEC=0

for dir in */; do

    cd "$dir"

    printf "Entering ${dir} \n"

    for BENCH in */; do
        cd "$BENCH"
                
        printf "Checking ${BENCH} ...\n"
        #printf "$COMPILER $OPTIONS *.c $M5_LIB\n"
	#$COMPILER $OPTIONS *.c $M5_LIB
        
        # Please remove '&>/dev/null' to identify the warnings (if any)
        
        if [ -f a.out ]; then
            $EXEC ./a.out
            RETURNVALUE=$(echo $?)
            if [ $RETURNVALUE -eq 0 ]; then
                printf "passed. \n"
                ((PASS++))
            else
                printf "failed (wrong return value $RETURNVALUE). \n"
                ((FAIL_EXEC++))
            fi
        else
            printf "failed (compiled with errors/warnings). \n"
            ((FAIL_COMP++))
        fi 
        
        cd ..
    done

    printf "Leaving ${dir} \n\n"
    
    cd ..
done

echo "PASS: $PASS, FAIL_COMP: $FAIL_COMP, FAIL_EXEC: $FAIL_EXEC"
