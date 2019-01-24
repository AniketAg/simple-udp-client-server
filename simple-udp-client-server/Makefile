CC=g++

all: my-router

my-router:
	$(CC) my-router.cpp -o my-router -lrt
runA:
	./my-router topology.txt A | tee routing-outputA.txt
runB:
	./my-router topology.txt B | tee routing-outputB.txt
runC:
	./my-router topology.txt C | tee routing-outputC.txt 
runD:
	./my-router topology.txt D | tee routing-outputD.txt
runE:
	./my-router topology.txt E | tee routing-outputE.txt
runF:
	./my-router topology.txt F | tee routing-outputF.txt
runH:
	echo $1 | ./my-router topology.txt H | tee routing-outputH.txt
runall: 
		
clean:
	rm my-router routing-output*.txt