##
# Algorithme PAM
#

objets = data.o utilities.o main.o

run: compile
	./pam -f data.csv 4

compile: $(objets)
	gcc -o pam $(objets)

clean:
	rm $(objets) pam

# end
