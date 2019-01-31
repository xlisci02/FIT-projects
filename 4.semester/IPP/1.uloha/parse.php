<?php
/************************************
 * IPP 2018 Projekt 
 * Popis: Analyzator kodu v IPPcode18
 * File: parse.php
 * Autor: Matus Liscinsky
 * Login: xlisci02
 * Mail: xlisci02@stud.fit.vutbr.cz
 * Datum: 10.3.2018
 * **********************************/

// UTF-8 kodovanie
mb_internal_encoding( "UTF-8" );

/* 
 * Vypisuje chybovu hlasku na <err_msg> STDERR a ukonci kod so specifickym navratovym kodom <err_code>
 */
function print_err($err_msg, $err_code){
        fwrite(STDERR,"[ERR] " . $err_msg . "\n");
        exit($err_code);
}
/* 
 * Kontroluje spravnost operandu var
 * <variable> premenna pre kontrolu
 * <order> poradie operandu vramci instrukcie
 * <xw> XML document handle
 */
function check_var($xw ,$order, $variable){
	if( !preg_match('/^[GTL][F]@[A-Za-z_\-$&%\*][A-Za-z_\-$&%\*0-9]*$/', $variable) )
		print_err("Wrong variable used ($variable)", 21);
	
	// zapis do XML dokumentu		
	xmlwriter_start_element($xw,"arg$order"); // starts arg
	xmlwriter_start_attribute($xw,'type');
	xmlwriter_text($xw,'var');
	xmlwriter_end_attribute($xw);
	xmlwriter_text($xw, $variable);
	xmlwriter_end_element($xw); // ends arg	
}

/* 
 * Kontroluje spravnost operandu symb 
 * <xw> XML document handle
 * <symb> symbol pre kontrolu
 * <order> poradie operandu vramci instrukcie
 */
function check_symb($xw, $order, $symb){ 
	// kontrola ci sa nejedna o premennu(var)
	if(preg_match('/^[GTL][F]@[A-Za-z_\-$&%\*][A-Za-z_\-$&%\*0-9]*$/', $symb))
		check_var($xw, $order, $symb);
	// symb kontrola
	else
	{
		// rozdelenie podla oddelovaca '@'
		$parts = explode("@",$symb, 2);
		switch($parts[0]){
			case "bool": if(! preg_match('/^true|false$/',$parts[1]))
						print_err("Wrong bool argument ($symb)\n", 21);break;
			case "int": if(!preg_match('/^(\+|\-)?[0-9]+$/', $parts[1])) 
						print_err("Wrong int argument ($symb)\n", 21);
						 break;
			case "string":	if(!(preg_match( '/^((\x5C[0-9]{3})|([^\x5C\s]))*$/',$parts[1])))
						print_err("Wrong string argument ($symb)\n", 21);
						break;
			default: print_err("Wrong argument ($symb)\n", 21); break;
		}

		// zapis do XML dokumentu
		xmlwriter_start_element($xw,"arg$order"); // starts arg
		xmlwriter_start_attribute($xw,'type');
		xmlwriter_text($xw,$parts[0]);
		xmlwriter_end_attribute($xw);
		xmlwriter_text($xw, $parts[1]);
		xmlwriter_end_element($xw); // ends arg	
	}
}

/* 
 * Kontroluje spravnost operandu label
 * <xw> XML document handle
 * <label> label pre kontrolu
 * <order> poradie operandu vramci instrukcie
 */
function check_label($xw, $order, $label){
	if(! preg_match('/^[A-Z,a-z,_,\-,$,&,%,\*][A-Z,a-z,_,\-,$,&,%,\*, 0-9]*$/', $label))
		print_err("Wrong label used ($label)", 21);
	
	// zapis do XML dokumentu
	xmlwriter_start_element($xw,"arg$order");
	xmlwriter_start_attribute($xw,'type');
	xmlwriter_text($xw,'label');
	xmlwriter_end_attribute($xw);
	xmlwriter_text($xw, $label);
	xmlwriter_end_element($xw); // ends arg	
}

/* 
 * Kontroluje spravnost operandu type 
 * <xw> XML document handle
 * <type> typ pre kontrolu
 * <order> poradie operandu vramci instrukcie
 */
function check_type($xw, $order, $type){
	if(! preg_match('/^int|string|bool$/', $type))
		print_err("Wrong type used ($type)", 21);

	// zapis do XML dokumentu
	xmlwriter_start_element($xw,"arg$order"); // starts arg
	xmlwriter_start_attribute($xw,'type');
	xmlwriter_text($xw,'type');
	xmlwriter_end_attribute($xw);
	xmlwriter_text($xw, $type);
	xmlwriter_end_element($xw); // ends arg	
}

//========== SPRACOVANIE ARGUMENTOV ==========//

// flagy pre zbieranie statistik o zdrojovom kode
$loc=false; $comments=false;

$options = getopt("", array("help", "stats:", "loc", "comments"));
// loc argument
if( array_key_exists("loc", $options) ){
	if( !array_key_exists("stats", $options) )
		print_err("Check arguments", 10); // ERR: loc argument bez stats 
	$loc=true;
}

// comments argument
if( array_key_exists("comments", $options) ){
	if( !array_key_exists("stats", $options) )
		print_err("Check arguments", 10); // ERR: comments argument bez stats  
	$comments=true;
}

// help argument
if( array_key_exists("help", $options) ){
	if( count($options) == 1 ){
		echo "Usage: php5.6 parse.php [OPTIONS]
Options:
    --help            Display this information.
    --stats=<file>    Write statistics of source code to <file>.
        --loc         Stats about lines of code.
        --comments    Stats about lines with comments.
Tips: Script requires input, use redirect to STDIN.
      For better handling with output, redirect from STDOUT to *.xml file.\n";
		exit(0);
	}	
	else
		print_err("Check arguments", 10); // ERR: help nieje jediny argument
}

// pripad vyskytu duplicity argumentov
foreach ($options as $opt){
	if(count($opt) > 1)
	print_err("Check arguments", 10);  
}
//========== KONIEC SPRACOVAVANIA ARGUMENTOV ==========//

// 2D pole instrukcii a operandov
$instructions = array(
	"MOVE" => array(0 => "var",1 => "symb"),
	"CREATEFRAME" => array(),
	"PUSHFRAME" => array(),
	"POPFRAME" => array(),
	"DEFVAR" => array(0 => "var"),
	"CALL" => array(0 => "label"),
	"RETURN" => array(),
	"PUSHS" => array(0 => "symb"),
	"POPS" => array(0 => "var"),
	"ADD" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"SUB" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"MUL" => array(0 => "var", 1 => "symb", 2 => "symb"), 
	"IDIV" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"LT" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"GT" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"EQ" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"AND" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"OR" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"NOT" => array(0 => "var", 1 => "symb"),
	"INT2CHAR" => array(0 => "var", 1 => "symb"),
	"STRI2INT" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"READ" => array(0 => "var", 1 => "type"),
	"WRITE" => array(0 => "symb"),
	"CONCAT" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"STRLEN" => array(0 => "var", 1 => "symb"),
	"GETCHAR" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"SETCHAR" => array(0 => "var", 1 => "symb", 2 => "symb"),
	"TYPE" => array(0 => "var", 1 => "symb"),
	"LABEL" => array(0 => "label"),
	"JUMP" => array(0 => "label"),
	"JUMPIFEQ" => array(0 => "label", 1 => "symb", 2 => "symb"),
	"JUMPIFNEQ" => array( 0 => "label", 1 => "symb", 2 => "symb"),
	"DPRINT" => array(0 => "symb"),
	"BREAK" => array()
);

//========== INICIALIZACIA XML DOKUMENTU ==========//
$xw = xmlwriter_open_memory(); // handle pre xml document 
xmlwriter_set_indent($xw, 1); // odsadenie
xmlwriter_set_indent_string($xw, '    '); // velkost medzery
// hlavicka
xmlwriter_start_document($xw, '1.0', 'UTF-8');

// root element
xmlwriter_start_element($xw, 'program'); // starts program
xmlwriter_start_attribute($xw, 'language');
xmlwriter_text($xw, 'IPPcode18');
xmlwriter_end_attribute($xw);

//========== MAIN ==========//

// inicializacia premennych
$code_lines=0; // loc counter
$comm_lines=0; // comments counter

// spracovanie hlavicky .IPPcode18
$line = fgets(STDIN);

// zbieranie statistiky poctu riadkov s komentarmi
$line = explode('#',$line);
if( count($line) >= 2)
	$comm_lines++;

// samotna kontrola hlavicky
$line=$line[0];
$line = trim($line); // odstranenie bielych znakov
if(strcasecmp($line,".IPPcode18") != 0)
	print_err("Header missing", 21); // ERR: chybna hlavicka

// spracovanie instrukcii
while($line = fgets(STDIN)){

	// zbieranie statistiky poctu riadkov s komentarmi
	$line = explode('#',$line); 
	if( count($line) >= 2)
		$comm_lines++;
	
	$line = $line[0];
	$line = trim($line); // odstranenie nadbytocnych bielych znakov 

	// prazdny riadok
	if($line == ""){
		continue;	
	}	

	$line = preg_split('/\s+/',$line);// rozparsovanie na slova

	if(!(array_key_exists(strtoupper($line[0]), $instructions)))
		print_err("Wrong opcode ($line[0])", 21); // ERR: zly opcode	
	
	// zbieranie statistiky poctu riadkov s instrukciami
	$code_lines++;	
	
	$cnt = count($instructions[strtoupper($line[0])]); // pocet operandov podla zadania
	
	// kontrola poctu operandov		
	if(count($line)-1 != $cnt)
		print_err("Wrong arguments, opcode ($line[0])", 21); // ERR: zly pocet operandov
	
	$i=1; // pocitadlo operandov 

	// zapis do XML dokumentu
	xmlwriter_start_element($xw, 'instruction'); // instuction
	xmlwriter_start_attribute($xw, 'order');
	xmlwriter_text($xw, (string)$code_lines);
	xmlwriter_end_attribute($xw);
	xmlwriter_start_attribute($xw, 'opcode');
	xmlwriter_text($xw, strtoupper($line[0]));
	xmlwriter_end_attribute($xw);

	// prechod polom podla kluca (instrukcie) a nasledna kontrola operandov	
	if(count($instructions[strtoupper($line[0])])){
		foreach( $instructions[strtoupper($line[0])] as $value){
			switch($value){
			case "label": check_label($xw, $i, $line[$i++]); break;  
			case "var": check_var($xw, $i, $line[$i++]); break;  
			case "symb": check_symb($xw, $i, $line[$i++]); break; 
			case "type": check_type($xw, $i, $line[$i++]); break; 
			default: break;}
		}
	}
	xmlwriter_end_element($xw); // zapis do XML dokumentu koniec instruction elementu 
}

// vypis statistik do suboru
if($loc || $comments)
{
	if(! ($handle = fopen($options["stats"],"w")))
		print_err("Couldn't open file",12 ); //ERR: 12 - jedna sa o vystupny subor
	for($i=1; $i < $argc; $i++){
		if($argv[$i] == "--loc")
			fwrite($handle,"$code_lines\n");
		
		if($argv[$i] == "--comments")
			fwrite($handle,"$comm_lines\n");
		
	}
	fclose($handle);
}
//========== ZAVER XML DOKUMENTU ==========//
xmlwriter_end_element($xw); // koniec elementu program (root element)
xmlwriter_end_document($xw);

// vypis XML dokumentu na STDIN
echo xmlwriter_output_memory($xw);
?>
