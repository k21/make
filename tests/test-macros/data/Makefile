A = a
B = $A $(A) ${A}\
   	 a $(X) $$
C=${@}#comment
D  =	$<   # comment
E=    	$^
F = $?
G=$+

# comment

# multiline \
comment

EXT = .dummy
DEP = a$(EXT) e$(EXT) # comment

c$(EXT): ${DEP} $(DEP)
	@echo "$A"\
	- "$B"
	@echo "${C}" #comment
	echo "$D"
	@echo "$E"
	@echo "$F"
	@echo "$G"
	# comment
	touch $@
