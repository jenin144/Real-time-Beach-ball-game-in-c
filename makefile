G = gcc -g
O = -o
names = main team1 team2 gl jojo

files:
	$(G) main.c $(O) main
	$(G) team1.c $(O) team1
	$(G) team2.c $(O) team2
	$(G) gl.c $(O) gl -lglut -lGLU -lGL -lm
	./gl
	   
	   		


	
	
	
clean:
	rm -f $(names)
	rm -f ballcount.txt
	rm -f ballcount2.txt
