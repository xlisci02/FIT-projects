# Interpret kodu IPPcode18

# * IPP 2018 Projekt 
# * Popis: Interpret XML reprezentacie kodu
# * File: interpret.py
# * Autor: Matus Liscinsky
# * Login: xlisci02
# * Mail: xlisci02@stud.fit.vutbr.cz
# * Datum: 15.4.2018

import re

# Chybove konstanty
ERR_ARG = 10
ERR_FILE_IN = 11
ERR_FILE_OUT = 12
ERR_XML_FORMAT = 31
ERR_XML_LEXSYN = 32
ERR_XML_SEM = 52
ERR_OPERAND = 53
ERR_VAR_NEXISTS = 54
ERR_FRAME = 55
ERR_MISSING_VALUE = 56
ERR_ZERO_DIV = 57
ERR_STRING = 58

# GLOBALNE PREMENNE

instr_sum = 0 # pocet instrukcii pre statistiky 
var_max = 0 # maximalny pocet inicializovanych premennych 
instr_counter = 1 # pocitac instrukcii
program_instructions = {} # slovnik instrukcii, order => instrukcia
call_stack = [] # zasobnik volani
data_stack = [] # datovy zasobnik
LABELS = [] # zoznam labelov
GF = {} # globalny ramec
LF = None # lokalny ramec
frame_stack = [] # zasobnik lokalnych ramcov
TF = None # temporary (docasny) ramec

# Vracia pocet aktualne inicializovanych premennych na vsetkych ramcoch
def var_count():
	count = 0
	global GF,TF,LF
		
	for var in GF:
		if GF[var][1] != None:
			count +=1
	if LF:
		for var in LF:
			if LF[var][1] != None:
				count +=1
	if TF:
		for var in TF:
			if TF[var][1] != None:
				count +=1
	return count

#######################################################################
# Kontroly 
#######################################################################

# Kontroluje korenovy element XML suboru
def check_root( root ):
	if not (root.tag == 'program' and 'language' in root.attrib ):
		return ERR_XML_FORMAT 
	
	elif root.attrib['language'] != 'IPPcode18':
		return ERR_XML_LEXSYN
	        
	if len(root.attrib) == 2:
		if not ('name' in root.attrib or 'description' in root.attrib):
			return ERR_XML_FORMAT 
	
	elif len(root.attrib) == 3:
		if not ('name' in root.attrib and 'description' in root.attrib):
			return ERR_XML_FORMAT 
	elif len(root.attrib) != 1:
			return ERR_XML_FORMAT 
	
	return 0 

# Kontroluje syntax premennej
def check_var( var ):
	exp = re.compile(r'^[GTL][F]@[A-Za-z_\-$&%\*][A-Za-z_\-$&%\*0-9]*$')
	if var == None:
		sys.exit(ERR_XML_LEXSYN)
	if exp.match(var)  == None :
		print('Zla syntax premennej ' + var, file=sys.stderr)
		sys.exit(ERR_XML_LEXSYN)
	return var

# Kontroluje syntax symbolu
def check_symb( typ, symb ):
	# regexy pre premennu, a konstanty bool, int a string
	exp_var = re.compile(r'^[GTL][F]@[A-Za-z_\-$&%\*][A-Za-z_\-$&%\*0-9]*$')
	exp_bool = re.compile(r'^true|false$')
	exp_int = re.compile(r'^(\+|\-)?[0-9]+$')
	exp_string = re.compile(r'^((\x5C[0-9]{3})|([^\x5C\s#]))*$')

	if typ == 'bool':
		if symb == None:
			print('Neplatny bool (empty) ', file=sys.stderr)
			sys.exit(ERR_XML_LEXSYN)
		if exp_bool.match(symb) == None :
			print('Zla syntax symbolu ' + symb, file=sys.stderr)
			sys.exit(ERR_XML_LEXSYN)
	elif typ == 'int':
		if symb == None:
			print('Neplatny int (empty) ', file=sys.stderr)
			sys.exit(ERR_XML_LEXSYN)
		if exp_int.match(symb) == None :
			print('Zla syntax symbolu ' + symb, file=sys.stderr)
			sys.exit(ERR_XML_LEXSYN)
	elif typ == 'string':
		if exp_string.match(symb) == None :
			print('Zla syntax symbolu ' + symb, file=sys.stderr)
			sys.exit(ERR_XML_LEXSYN)
		#nahrada escape sekvencii
		esc_sequences = re.findall(r'\x5C[0-9]{3}',symb)
		for esc in esc_sequences:
			symb = symb.replace(esc, chr(int(esc[-3:])))
	#else:
	#	print('Zly typ ' + typ, file=sys.stderr)
	#	sys.exit(ERR_XML_LEXSYN)
	return symb

# Kontroluje syntax labelu
def check_label( label ):
	if label == None:
		sys.exit(ERR_XML_LEXSYN)
	exp = re.compile(r'^[A-Z,a-z,_,\-,$,&,%,\*][A-Z,a-z,_,\-,$,&,%,\*, 0-9]*$')
	if exp.match(label)  == None:
		print('Zla syntax labelu ' + label, file=sys.stderr)
		sys.exit(ERR_XML_LEXSYN)
	return label

# Kontroluje syntax typu
def check_type( typ_str ):
	if typ_str == None:
		sys.exit(ERR_XML_LEXSYN)
	exp = re.compile(r'^int|string|bool$')
	if exp.match(typ_str)  == None:
		print('Zla syntax typu ' + typ_str, file=sys.stderr)
		sys.exit(ERR_XML_LEXSYN)
	return typ_str

def get_frame_var( var ):
	frame_var = var.text.split('@',1)
	check_frame_init(frame_var[0])
	check_var_def(frame_var)
	return frame_var

# Ziska typ a hodnotu zo symb
def get_type_val ( symb ):
	global TF,LF,GF	
	typ = symb.attrib['type']
	if typ == 'var':
		frame_var = symb.text.split('@',1)
		check_frame_init(frame_var[0])
		check_var_def(frame_var)
		check_var_init(frame_var)
		typ_value = eval(frame_var[0])[frame_var[1]]
	else:
		typ_value = [typ, symb.text]
	return typ_value

# Kontroluje element instruction
def check_instr( instr ):
	# kontrola nazvu elementu, ma byt instruction
	if instr.tag!='instruction':
		print('Zly nazov elementu', file=sys.stderr)
		sys.exit(ERR_XML_FORMAT)
	
	#kontrola poctu atributov elementu instruction 
	if len(instr.attrib) > 2:
		print('Prilis vela atributov pri instruction', file=sys.stderr)
		sys.exit(ERR_XML_FORMAT)

	# kontrola atributov elementu instruction
	if 'order' in instr.attrib and 'opcode' in instr.attrib: 
		# kontrola ci je spravny opcode
		
		if not (instr.attrib['opcode'] in instructions):
			print('Zly opcode pri instruction', file=sys.stderr)
			sys.exit(ERR_XML_LEXSYN)
		
		try:
			int(instr.attrib['order'])
		except:
			sys.exit(ERR_XML_LEXSYN)
	else: 
		print('ERR atribut', file=sys.stderr)
		sys.exit(ERR_XML_FORMAT)
	return

# Kontroluje argumenty instrukcie
def check_instr_arg( instr, arg ):

	# kontrola atributu type
	if (not('type' in arg.attrib)) or len(arg.attrib) != 1:
		print('Zly atribut argumentu', file=sys.stderr)
		sys.exit(ERR_XML_FORMAT)

	typ = arg.attrib['type']
	index = int(arg.tag[3])-1
	if not (index in  instructions[instr.attrib['opcode']]):
		print('Zly nazov elementu argument', file=sys.stderr)
		sys.exit(ERR_XML_FORMAT)
	
	correct_arg_type = instructions[instr.attrib['opcode']][index]

	# ak je to int, string alebo bool, je to symb
	if typ == 'bool' or typ == 'int' or typ == 'string':
		typ = 'symb'
	
	# kontrola typu argumentu
	if typ != correct_arg_type:
		if not (typ == 'var' and correct_arg_type == 'symb'):
			print('Zly typ argumentu ' + typ + ' ' + correct_arg_type, file=sys.stderr)
			sys.exit(ERR_XML_LEXSYN)

	# kontroly obsahu elementu argument
	if typ == 'var':
		return check_var(arg.text)
	elif typ == 'symb':
		# v arg.attrib['type'] je v tomto pripade bool/int/string
		return check_symb(arg.attrib['type'], arg.text)
	elif typ == 'label':
		return check_label(arg.text)
	else: #(typ == 'type')
		return check_type(arg.text)
	#return

#######################################################################
# Vykonavanie insrukcii 
#######################################################################

# Funckia kontroluje inicializaciu framu
def check_frame_init( frame ):
	global GF,TF,LF

	if (eval(frame) == None): 
		print('Neinicializovany frame', file=sys.stderr)
		sys.exit(ERR_FRAME)
	return

# Funckia kontroluje definiciu premennej
def check_var_def( frame_var ):
	global GF,TF,LF
	
	frame = frame_var[0]
	var = frame_var[1]
	if not (var in eval(frame)):
		print('Premenna nebola definovana', file=sys.stderr)
		sys.exit(ERR_VAR_NEXISTS)
	return

# Funckia kontroluje inicializaciu premennej
def check_var_init( frame_var ):
	global GF,TF,LF

	frame = frame_var[0]
	var = frame_var[1]
	if eval(frame)[var][1] == None:
		print('Praca s neinicializovanou premennou', file=sys.stderr)
		sys.exit(ERR_MISSING_VALUE)
	return

# Funkcia zapisuje do premmennej na ramci 
# frame_var obsahuje dvojicu ramec, meno premennej, typ_value reprezentuje dvojicu [typ, hodnota]
def write_to_var( frame_var, typ_value ):
	global GF,TF,LF

	frame = frame_var[0]
	var = frame_var[1]
	if typ_value[0] == None or typ_value[1] == None :
		print('Praca s neinicializovanou premennou..', file=sys.stderr)
		sys.exit(ERR_MISSING_VALUE)
	eval(frame)[var]=typ_value
	return

# Funkcia kontroluje ci je premenna daneho typu
def check_req_type(is_var ,typ_value, typ):
	if typ_value[0] != typ:
		if is_var:
			print('Nutnost ', typ, ' operandu v premennej', file=sys.stderr)
			sys.exit(ERR_OPERAND)	
		print('Nutnost ', typ, ' operandu v symbole', file=sys.stderr)
		sys.exit(ERR_XML_SEM)
	return 

# Funkcia pre instrukciu CREATEFRAME
def f_CREATEFRAME( instr ):
	
	global instr_counter,TF
	TF = dict()
	instr_counter +=1
	return

# Funkcia pre instrukciu PUSHFRAME
def f_PUSHFRAME ( instr ):
	global frame_stack, LF, TF, instr_counter

	if TF == None:
		print('TF je neinicializovany', file=sys.stderr)
		sys.exit(ERR_FRAME)
	frame_stack.append(TF)
	LF = frame_stack[-1]
	TF = None
	instr_counter +=1
	return 

# Funkcia pre instrukciu POPFRAME
def f_POPFRAME ( instr ):
	global frame_stack, TF, LF, instr_counter

	if not frame_stack:
		print('Prazdny zasobnik ramcov', file=sys.stderr)
		sys.exit(ERR_FRAME)
	TF = frame_stack.pop()
	
	if not frame_stack:
		LF = None
	else:
		LF = frame_stack[-1]
	instr_counter +=1
	return 

# Funkcia pre instrukciu MOVE
def f_MOVE ( instr ):
	global instr_counter
	frame_var = get_frame_var(instr[0])
	
	typ_value = get_type_val(instr[1])
	write_to_var(frame_var, typ_value)

	instr_counter +=1
	return

# Funkcia pre instrukciu DEFVAR
def f_DEFVAR( instr ):
	global frame_stack, TF, LF, instr_counter
	
	frame_var = instr[0].text.split('@',1)
	check_frame_init(frame_var[0])
	eval(frame_var[0])[frame_var[1]] = [None, None]
	instr_counter +=1
	return

# Funkcia pre instrukciu CALL
def f_CALL( instr ):
	global program_instructions, call_stack, instr_counter

	# ukladanie na zasobnik volani
	call_stack.append(instr_counter+1)

	# meno labelu za ktory sa ma skocit
	label_name = instr[0].text

	# hladanie cisla instrukcie za labelom, nasledujuca instrukcia nesmie byt label
	for i in range(1, len(program_instructions)+1):
		instruction = program_instructions[str(i)]
		if instruction.attrib['opcode'] == 'LABEL':
			if instruction[0].text == label_name :#and str(i+1) in program_instructions and program_instructions[str(i+1)].attrib['opcode'] != 'LABEL':
				instr_counter = i
				return
	print('Label  ' + label_name + ' nenajdeny', file=sys.stderr)
	sys.exit(ERR_XML_SEM)

# Funkcia pre instrukciu RETURN
def f_RETURN( instr ):
	global instr_counter, call_stack
	
	if not call_stack:
		print('Prazdny call stack', file=sys.stderr)
		sys.exit(ERR_MISSING_VALUE)
	instr_counter = call_stack.pop()

# Funkcia pre instrukciu PUSHS
def f_PUSHS( instr ):
	global data_stack, instr_counter
	
	typ_value = get_type_val(instr[0])
	data_stack.append(typ_value)	
	instr_counter +=1
	return

# Funkcia pre instrukciu POPS
def f_POPS( instr ):
	global data_stack, instr_counter

	if not data_stack:
		print('Prazdny datovy zasobnik', file=sys.stderr)
		sys.exit(ERR_MISSING_VALUE)
	
	frame_var = get_frame_var(instr[0])

	write_to_var(frame_var, data_stack.pop())
	instr_counter +=1
	return

# Funkcie pre aritmeticke, relacne a boolovske instrukcie volaju jednu spolocnu funkciu, kazda s inym parametrom
def f_ADD( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_SUB( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_MUL( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_IDIV( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_LT( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_GT( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_EQ( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_AND( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_OR( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])
def f_NOT( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])

# Spolocna funkcia pre aritmeticke, relacne a boolovske instrukcie
def arithm_rel_bool_func( instr , operator ):
	global instr_counter, TF, LF, GF

	frame_var = get_frame_var(instr[0])

	typ_value1 = get_type_val(instr[1])
	is_var1 = (instr[1].attrib['type'] == 'var')
	if operator != 'NOT':
		typ_value2 = get_type_val(instr[2])
		is_var2 = (instr[2].attrib['type'] == 'var')
		# ADD SUB MUL IDIV
		if operator in ['ADD', 'SUB', 'MUL', 'IDIV']:
			check_req_type(is_var1, typ_value1, 'int')
			check_req_type(is_var2, typ_value2, 'int')

			if operator == 'ADD':
				result = ['int', int(typ_value1[1]) + int(typ_value2[1])]
				write_to_var(frame_var, result)
			elif operator == 'SUB':
				result = ['int', int(typ_value1[1]) - int(typ_value2[1])]
				write_to_var(frame_var, result)
			elif operator == 'MUL':
				result = ['int', int(typ_value1[1]) * int(typ_value2[1])]
				write_to_var(frame_var, result)
			else:
				if int(typ_value2[1]) == 0:
					print('Delenie nulou', file=sys.stderr)
					sys.exit(ERR_ZERO_DIV)
				result = ['int', int(typ_value1[1]) // int(typ_value2[1])]
				write_to_var(frame_var, result)

		# LT GT EQ
		elif operator in ['LT', 'GT', 'EQ']:
			if typ_value1[0] != typ_value2[0]:
				if not is_var1 and not is_var2:
					print('Nutnost rovnakych typov (var)', file=sys.stderr)
					sys.exit(ERR_XML_SEM)
				print('Nutnost rovnakych typov (symb)', file=sys.stderr)
				sys.exit(ERR_OPERAND)	

			if operator == 'LT':
				# pretypovanie pre porovnavanie v pythone
				if typ_value1[0] == 'bool':
					if typ_value1[1] == 'true':
						typ_value1[1] = 1
					elif typ_value1[1] == 'false':
						typ_value1[1] = 0
					if typ_value2[1] == 'true':
						typ_value2[1] = 1
					elif typ_value2[1] == 'false':
						typ_value2[1] = 0
				
				if typ_value1[0] == 'int':
					typ_value1[1] = int(typ_value1[1])
					typ_value2[1] = int (typ_value2[1])

				if typ_value1[1] < typ_value2[1]:
					write_to_var(frame_var, ['bool', 'true'])
				else :
					write_to_var(frame_var, ['bool', 'false'])

			elif operator == 'GT':
				# pretypovanie pre porovnavanie v pythone
				if typ_value1[0] == 'bool':
					if typ_value1[1] == 'true':
						typ_value1[1] = 1
					elif typ_value1[1] == 'false':
						typ_value1[1] = 0
					if typ_value2[1] == 'true':
						typ_value2[1] = 1
					elif typ_value2[1] == 'false':
						typ_value2[1] = 0
				
				if typ_value1[0] == 'int':
					typ_value1[1] = int(typ_value1[1])
					typ_value2[1] = int (typ_value2[1])

				if typ_value1[1] > typ_value2[1]:
					write_to_var(frame_var, ['bool', 'true'])
				else :
					write_to_var(frame_var, ['bool', 'false'])
			# EQ
			else:
				# pretypovanie pre porovnavanie v pythone
				if typ_value1[0] == 'bool':
					if typ_value1[1] == 'true':
						typ_value1[1] = 1
					elif typ_value1[1] == 'false':
						typ_value1[1] = 0
					if typ_value2[1] == 'true':
						typ_value2[1] = 1
					elif typ_value2[1] == 'false':
						typ_value2[1] = 0
				
				if typ_value1[0] == 'int':
					typ_value1[1] = int(typ_value1[1])
					typ_value2[1] = int (typ_value2[1])

				if typ_value1[1] == typ_value2[1]:
					write_to_var(frame_var, ['bool', 'true'])
				else :
					write_to_var(frame_var, ['bool', 'false'])
		#AND OR
		elif operator in ['AND', 'OR']:
			check_req_type(is_var1, typ_value1, 'bool')
			check_req_type(is_var2, typ_value2, 'bool')
	
			if operator == 'AND':
				if typ_value1[1] == 'true':
					typ_value1[1] = 1
				elif typ_value1[1] == 'false':
					typ_value1[1] = 0
				if typ_value2[1] == 'true':
					typ_value2[1] = 1
				elif typ_value2[1] == 'false':
					typ_value2[1] = 0
				
				if typ_value1[1] and typ_value2[1]:
					write_to_var(frame_var, ['bool', 'true'])
				else :
					write_to_var(frame_var, ['bool', 'false'])
			# OR
			else:
				if typ_value1[1] == 'true':
					typ_value1[1] = 1
				elif typ_value1[1] == 'false':
					typ_value1[1] = 0
				if typ_value2[1] == 'true':
					typ_value2[1] = 1
				elif typ_value2[1] == 'false':
					typ_value2[1] = 0

				if typ_value1[1] or typ_value2[1]:
					write_to_var(frame_var, ['bool', 'true'])
				else :
					write_to_var(frame_var, ['bool', 'false'])
				
		# CONCAT
		else: #if operator == 'CONCAT':
			check_req_type(is_var1, typ_value1, 'string')
			check_req_type(is_var2, typ_value2, 'string')

			write_to_var(frame_var, ['string', typ_value1[1] + typ_value2[1]])
	# NOT
	else:
		check_req_type(is_var1, typ_value1, 'bool')

		if typ_value1[1] == 'true':
			typ_value1[1] = 1
		elif typ_value1[1] == 'false':
			typ_value1[1] = 0
		
		if not (typ_value1[1]):
			write_to_var(frame_var, ['bool', 'true'])
		else:
			write_to_var(frame_var, ['bool', 'false'])
		
		
	instr_counter +=1
	return 

# Funkcia pre instrukciu INT2CHAR
def f_INT2CHAR( instr ):
	global instr_counter
	frame_var = get_frame_var(instr[0])
	
	typ_value = get_type_val(instr[1])
	
	is_var = (instr[1].attrib['type'] == 'var')

	check_req_type(is_var, typ_value, 'int')

	if int(typ_value[1]) < 0 or int(typ_value[1]) > 1114111:
		print('INT2CHAR, Cislo je mimo rozsah', file=sys.stderr)
		sys.exit(ERR_STRING) 

	write_to_var(frame_var, ['string',chr(int(typ_value[1]))])
	instr_counter +=1
	return

# Funkcia pre instrukciu STRI2INT
def f_STRI2INT ( instr ):
	global instr_counter

	# kontrola var
	frame_var = get_frame_var(instr[0])

	typ_value1 = get_type_val(instr[1])
	typ_value2 = get_type_val(instr[2])
	
	is_var1 = (instr[1].attrib['type'] == 'var')
	is_var2 = (instr[2].attrib['type'] == 'var')

	check_req_type(is_var1, typ_value1, 'string')
	check_req_type(is_var2, typ_value2, 'int')

	if int(typ_value2[1]) > (len(typ_value1[1])-1) or int(typ_value2[1]) < 0 :
		print('STRI2INT, Index mimo pola', file=sys.stderr)
		sys.exit(ERR_STRING)
	
	string = typ_value1[1]
	index = int(typ_value2[1])
	write_to_var(frame_var, ['int',ord(string[index])])
	instr_counter +=1
	return

# Funkcia pre instrukciu READ
def f_READ ( instr ):
	global instr_counter

	frame_var = get_frame_var(instr[0])

	typ = instr[1].text
	try:
		line = input()
		if typ == 'bool':
			if line.lower() == 'true':
				write_to_var(frame_var, ['bool','true'])
			else:
				write_to_var(frame_var, ['bool','false'])
		elif typ == 'int':
			write_to_var(frame_var, ['int', int(line)])
		else:
			write_to_var(frame_var, ['string', line])
	except:
		if typ == 'bool':
			write_to_var(frame_var, ['bool','false'])
		elif typ == 'int':
			write_to_var(frame_var, ['int', 0])
		else:
			write_to_var(frame_var, ['string', ''])
	instr_counter +=1
	return

# Funkcia pre instrukciu WRITE
def f_WRITE ( instr ):
	global instr_counter

	typ_value = get_type_val(instr[0])
	if typ_value[0] == 'int':
		print(int(typ_value[1]))
	else:
		print(typ_value[1])
	instr_counter+=1
	return

# Funkcia pre instrukciu CONCAT
def f_CONCAT ( instr ):
	arithm_rel_bool_func( instr, instr.attrib['opcode'])

# Funkcia pre instrukciu STRLEN
def f_STRLEN ( instr ):
	global instr_counter

	frame_var = get_frame_var(instr[0])

	typ_value = get_type_val(instr[1])

	is_var = (instr[1].attrib['type'] == 'var')
	
	check_req_type(is_var, typ_value, 'string')

	write_to_var(frame_var, ['int', len(typ_value[1])])

	instr_counter +=1
	return

# Funkcia pre instrukciu GETCHAR
def f_GETCHAR ( instr ):
	global instr_counter

	# kontrola var
	frame_var = get_frame_var(instr[0])

	typ_value1 = get_type_val(instr[1])
	typ_value2 = get_type_val(instr[2])
	
	is_var1 = (instr[1].attrib['type'] == 'var')
	is_var2 = (instr[2].attrib['type'] == 'var')

	check_req_type(is_var1, typ_value1, 'string')
	check_req_type(is_var2, typ_value2, 'int')

	if int(typ_value2[1]) > (len(typ_value1[1])-1) or int(typ_value2[1]) < 0 :
		print('GETCHAR, Index mimo pola, INDEX = ' + str(typ_value2[1]) , file=sys.stderr)
		sys.exit(ERR_STRING)
	
	string = typ_value1[1]
	index = int(typ_value2[1])
	write_to_var(frame_var, ['string',string[index]])
	instr_counter +=1
	return

# Funkcia pre instrukciu SETCHAR
def f_SETCHAR ( instr ):
	global instr_counter

	# kontrola var
	frame_var = get_frame_var(instr[0])
	check_var_init(frame_var)
	typ_value = eval(frame_var[0])[frame_var[1]]

	typ_value1 = get_type_val(instr[1])
	typ_value2 = get_type_val(instr[2])

	
	is_var1 = (instr[1].attrib['type'] == 'var')
	is_var2 = (instr[2].attrib['type'] == 'var')
	
	check_req_type(is_var1, typ_value1, 'int')
	check_req_type(is_var2, typ_value2, 'string')

	if typ_value[0] != 'string':
		print('STRI2INT, Nutnost int operandu (var)', file=sys.stderr)
		sys.exit(ERR_OPERAND)

	# index mimo pola alebo prazdny string z ktoreho sa ma prekopirovat znak
	if int(typ_value1[1]) > (len(typ_value[1])-1) or int(typ_value1[1]) < 0 or len(typ_value2[1]) < 1:
		print('SETCHAR, Index mimo pola | prazdny string', file=sys.stderr)
		sys.exit(ERR_STRING)
	
	# prvy znak retazca symb2
	znak = typ_value2[1][0]
	# index, symb1
	index = int(typ_value1[1])
	# prepis znaku
	typ_value[1]= typ_value[1][:index] + znak + typ_value[1][index+1:] 

	write_to_var(frame_var, typ_value)
	instr_counter +=1
	return

# Funkcia pre instrukciu TYPE
def f_TYPE ( instr ):
	global instr_counter

	frame_var = get_frame_var(instr[0])

	typ = instr[1].attrib['type']

	if typ == 'var':
		frame_var1 = instr[1].text.split('@',1)
		check_frame_init(frame_var1[0])
		check_var_def(frame_var1)
		typ_value = eval(frame_var1[0])[frame_var1[1]]
		
		if typ_value[0] == None:
			write_to_var(frame_var, ['type', ''])
		else:
			write_to_var(frame_var, ['type', typ_value[0]])
	else:
		write_to_var(frame_var, ['type', typ])
	
	instr_counter +=1
	return

# Funkcia pre instrukciu LABEL
def f_LABEL ( instr ):
	global instr_counter
	instr_counter +=1
	return

# Funkcia pre instrukciu JUMP
def f_JUMP ( instr ):
	f_CALL(instr)
	call_stack.pop()

# Funkcia pre instrukciu JUMPIFEQ
def f_JUMPIFEQ( instr ):
	global instr_counter

	typ_value1 = get_type_val(instr[1])
	typ_value2 = get_type_val(instr[2])

	is_var1 = (instr[1].attrib['type'] == 'var')
	is_var2 = (instr[2].attrib['type'] == 'var')

	if typ_value1[0] != typ_value2[0]:
		if not is_var1 and not is_var2:
			print('Nutnost rovnakych typov (var)', file=sys.stderr)
			sys.exit(ERR_XML_SEM)
		print('Nutnost rovnakych typov (symb)', file=sys.stderr)
		sys.exit(ERR_OPERAND)

	if typ_value1[1] == typ_value2[1]:
		f_JUMP(instr)
	else:
		instr_counter +=1
	return

# Funkcia pre instrukciu JUMPIFNEQ
def f_JUMPIFNEQ( instr ):
	global instr_counter

	typ_value1 = get_type_val(instr[1])
	typ_value2 = get_type_val(instr[2])

	is_var1 = (instr[1].attrib['type'] == 'var')
	is_var2 = (instr[2].attrib['type'] == 'var')

	if typ_value1[0] != typ_value2[0]:
		if not is_var1 and not is_var2:
			print('Nutnost rovnakych typov (var)', file=sys.stderr)
			sys.exit(ERR_XML_SEM)
		print('Nutnost rovnakych typov (symb)', file=sys.stderr)
		sys.exit(ERR_OPERAND)

	if typ_value1[1] != typ_value2[1]:
		f_JUMP(instr)
	else:
		instr_counter +=1
	return 

# Funkcia pre instrukciu DPRINT
def f_DPRINT( instr ):
	global instr_counter
	print('OK', file=sys.stderr)
	instr_counter +=1
	return 

# Funkcia pre instrukciu BREAK
def f_BREAK( instr ):
	global instr_counter, TF, LF, GF

	if LF:
		print('LF:', LF, file=sys.stderr)
	if TF:
		print('TF:', TF, file=sys.stderr)
	print('GF:', GF, file=sys.stderr)
	print('IP:', instr_counter, file=sys.stderr)
	instr_counter +=1
	return 

# Funkcia vola funkciu s nazvom f_instr
def execute_instruction( instr ):
	instr_name = instr.attrib['opcode']
	instr_name = 'f_' + instr_name
	eval( instr_name + '(instr)')

	
#######################################################################
# ARGUMENTY
#######################################################################
import getopt, sys
usage = "Usage: python3.6 interpret.py [OPTIONS]\nOptions:\n\t--help\tDisplay this information.\n\t--stats=<file>\tWrite statistics of interpretation to <file>.\n\t--insts\tWrite num of executed instructions.\n\t--vars\tWrite MAX num of initialized variables during the interpretation.\n"
try:
	opts, args = getopt.getopt(sys.argv[1:], "", ["help", "source=", "stats=", "insts", "vars"])
except getopt.GetoptError:
	print(usage, file=sys.stderr)
	sys.exit (ERR_ARG)  

if opts == []: ## prazdne opts znamena ze sa ziaden parameter nezhodoval
	print(usage, file=sys.stderr)
	sys.exit (ERR_ARG) 

stats_ = False
insts_ = False
vars_ = False

for opt, arg in opts:
	if opt == '--help':
		if len(opts)==1:
			print (usage)
			sys.exit (0) 
		else:
			sys.exit(ERR_ARG)
	elif opt == '--source':
		source = arg 
	elif opt == '--stats':
		stats_ = arg
	elif opt == '--insts':
		insts_ = True
	elif opt == '--vars':
		vars_ = True
	
if (insts_ or vars_) and not (stats_):
	print('Pouzita zla kombinacia')
	sys.exit(ERR_ARG)
try:
	file_handle=open(source, 'r')
except IOError:
	print('Zadany subor neexistuje', file=sys.stderr)
	sys.exit (ERR_FILE_IN)  

#######################################################################
# VYTVORENIE SLOVNIKA INSTRUKCII
#######################################################################
instructions = {
    'MOVE' : { 0: 'var', 1: 'symb'},
    'CREATEFRAME' : {},
    'PUSHFRAME' : {},
    'POPFRAME' : {},
    'DEFVAR' : { 0: 'var'},
    'CALL' : { 0: 'label'},
    'RETURN' : {},
    'PUSHS' : { 0: 'symb'},
    'POPS' : { 0: 'var'},
    'ADD' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'SUB' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'MUL' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'IDIV' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'LT' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'GT' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'EQ' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'AND' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'OR' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'NOT' : { 0: 'var', 1: 'symb'},
    'INT2CHAR' : { 0: 'var', 1: 'symb'},
    'STRI2INT' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'READ' : { 0: 'var', 1: 'type'},
    'WRITE' : { 0: 'symb'},
    'CONCAT' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'STRLEN' : { 0: 'var', 1: 'symb'},
    'GETCHAR' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'SETCHAR' : { 0: 'var', 1: 'symb', 2: 'symb'},
    'TYPE' : { 0: 'var', 1: 'symb'},
    'LABEL' : { 0: 'label'},
    'JUMP' : { 0: 'label'},
    'JUMPIFEQ' : { 0: 'label', 1: 'symb', 2: 'symb'},
    'JUMPIFNEQ' : { 0: 'label', 1: 'symb', 2: 'symb'},
    'DPRINT' : { 0: 'symb'},
    'BREAK' : {}
	
}

#######################################################################
# MAIN
#######################################################################

import xml.etree.ElementTree as ET
try:
	tree = ET.parse(source)
except: 
	print('Zly format XML suboru', file=sys.stderr)
	sys.exit(ERR_XML_FORMAT)
	
program = tree.getroot()

# kontrola korenoveho elementu
rc = check_root( program )
if rc:	
	print('Zly korenovy atribut', file=sys.stderr)
	sys.exit(rc)

#  
for instruction in program:
	
	# kontrola elementu instruction
	check_instr(instruction)
	arg_counter = 0
	# kontrola argumentov instrukcie
	
	# zoznam zoradenych argumentov
	arg_list = [None, None, None]
	for arg in instruction:
		if arg.text == None and arg.attrib['type'] == 'string' :
			arg.text = ''
		if ((re.match(r'^arg[123]$', arg.tag) == None ) or int(arg.tag[3]) > 3):
			print('Zly nazov elementu argument', file=sys.stderr)
			sys.exit(ERR_XML_FORMAT)
		if arg_list[int(arg.tag[3])-1] != None :
			print('Redefinicia argumentu', file=sys.stderr)
			sys.exit(ERR_XML_FORMAT)
		arg_list[int(arg.tag[3])-1] = arg

		arg.text = check_instr_arg( instruction, arg )
		arg_counter +=1

	for i in range (0, len(instruction)):
		instruction[i] = arg_list[i]
	
	# kontrola poctu argumentov
	if arg_counter != len (instructions[instruction.attrib['opcode']]):
		print('Zly pocet argumentov', file=sys.stderr)
		sys.exit(ERR_XML_FORMAT)

	# ukladanie do zoznamu LABELOV
	if instruction.attrib['opcode'] == 'LABEL':
		if instruction[0].text in LABELS :
			print('Redefinicia labela', file=sys.stderr)
			sys.exit(ERR_XML_SEM)
		else:
			LABELS.append(instruction[0].text)

	# ulozenie do slovnika instrukcii
	program_instructions[instruction.attrib['order']] = instruction	
	
while instr_counter <= len(program_instructions):
	instruction = program_instructions[str(instr_counter)]
	# vykonanie instrukcie
	
	execute_instruction( instruction )
	tmp = var_count()
	if tmp > var_max:
		var_max = tmp
	instr_sum +=1

# rozsirenie STATI
if stats_:
	try:
		file_stats=open(stats_, 'w')
		for each in sys.argv:
			if each == '--insts':
				file_stats.write(str(instr_sum) + '\n')
			if each == '--vars':
				file_stats.write(str(var_max) + '\n')
	except IOError:
		print('Problem pri praci so suborom', file=sys.stderr)
		sys.exit (ERR_FILE_OUT)  
