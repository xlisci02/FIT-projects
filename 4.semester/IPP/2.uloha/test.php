<?php
/************************************
 * IPP 2018 Projekt 
 * Popis: Testovaci ramec
 * File: test.php
 * Autor: Matus Liscinsky
 * Login: xlisci02
 * Mail: xlisci02@stud.fit.vutbr.cz
 * Datum: 10.3.2018
 * **********************************/

// UTF-8 kodovanie
mb_internal_encoding( "UTF-8" );

/* 
 * Vypisuje chybovu hlasku <err_msg> na STDERR a ukonci kod so specifickym navratovym kodom <err_code>
 */
function print_err($err_msg, $err_code){
	fwrite(STDERR,"[ERR] " . $err_msg . "\n");
	exit($err_code);
}
/* 
 * Vypise do tabulky v HTML dokumente ADRESAR
 * <xw> HTML document handle
 * <dir> cesta k adresaru
 */
function html_dir($xw, $dir){
	xmlwriter_start_element($xw, "tr");
	xmlwriter_start_element($xw, "td");
	xmlwriter_start_attribute($xw, "class");
	xmlwriter_text($xw, 'dir');
	xmlwriter_end_attribute($xw);
	
	xmlwriter_text($xw, $dir); // current directory
	xmlwriter_end_element($xw); // ends td
	
	xmlwriter_start_element($xw, "td");
	xmlwriter_start_attribute($xw, "class");
	xmlwriter_text($xw, 'result');
	xmlwriter_end_attribute($xw);
	xmlwriter_start_attribute($xw, "id");
	xmlwriter_text($xw, $dir); // current directory
	xmlwriter_end_attribute($xw);
	xmlwriter_end_element($xw); // ends td
	xmlwriter_end_element($xw); // ends tr
}
/* 
 * Vypise do tabulky v HTML dokumente TESTOVACI SUBOR a VYSLEDKY
 * <xw> HTML document handle
 * <test_name> nazov testu
 * <success> uspesnost testu
 * <exp_ec> ocakavany exit code
 * <ret_rc> vrateny exit code
 */
function html_test($xw, $test_name, $success, $exp_ec, $ret_ec){
	xmlwriter_start_element($xw, "tr");
	xmlwriter_start_attribute($xw, "class");
	xmlwriter_text($xw, 'test_name');
	xmlwriter_end_attribute($xw);
	xmlwriter_start_element($xw, "td");
	xmlwriter_start_attribute($xw, "class");
	xmlwriter_text($xw, 'test_name');
	xmlwriter_end_attribute($xw);
	
	xmlwriter_text($xw,$test_name); // current test
	xmlwriter_end_element($xw); // ends td
	
	xmlwriter_start_element($xw, "td");
	xmlwriter_start_attribute($xw, "class");
	xmlwriter_text($xw, 'result');
	xmlwriter_end_attribute($xw);
	xmlwriter_start_attribute($xw, "style");
	if($success){
		xmlwriter_text($xw, 'color:#78AB46');
		xmlwriter_end_attribute($xw);
		xmlwriter_text($xw, 'PASSED');
	}
	else {
		xmlwriter_text($xw, 'color:#E6001A');
		xmlwriter_end_attribute($xw);
		xmlwriter_text($xw, 'FAILED');

	}
	xmlwriter_end_element($xw); // ends td
	
	xmlwriter_start_element($xw, "td");
	xmlwriter_start_attribute($xw, "class");
	xmlwriter_text($xw, 'exit');
	xmlwriter_end_attribute($xw);

	xmlwriter_text($xw, 'expected(');
	xmlwriter_start_element($xw,'span');
	xmlwriter_start_attribute($xw, "class");
	if($exp_ec == $ret_ec)
		xmlwriter_text($xw, 'green');
	else
		xmlwriter_text($xw, 'red');
	xmlwriter_end_attribute($xw);
	xmlwriter_text($xw, $exp_ec); // current expected err code
	xmlwriter_end_element($xw); // ends span
	xmlwriter_text($xw, '), returned(');
	xmlwriter_start_element($xw,'span');
	xmlwriter_start_attribute($xw, "class");
	if($exp_ec == $ret_ec)
		xmlwriter_text($xw, 'green');
	else
		xmlwriter_text($xw, 'red');
	xmlwriter_end_attribute($xw);
	xmlwriter_text($xw, $ret_ec); // current returned err code
	xmlwriter_end_element($xw); // ends span
	xmlwriter_text($xw, ')');

	if( $exp_ec == $ret_ec and !($success)){
		xmlwriter_start_element($xw,'span');
		xmlwriter_start_attribute($xw, "class");
		xmlwriter_text($xw, 'red');
		xmlwriter_end_attribute($xw);
		xmlwriter_text($xw, ' different outputs !');
		xmlwriter_end_element($xw); // ends span
	}
	xmlwriter_end_element($xw); // ends td
	xmlwriter_end_element($xw); // ends tr
} 
 
/* 
 * Vypise do HTML dokumentu JavaScript kod, pre vypis uspesnosti testov v adresari 
 * <xw> HTML document handle
 * <dir> nazov adresara
 * <suc_count> pocet uspesnych testov v adresari
 * <all> celkovy pocet testov v adresari
 */
function jscript($xw, $dir, $suc_count, $all){
	xmlwriter_start_element($xw, "script");
	xmlwriter_text($xw, "        document.getElementById('$dir').innerHTML = '$suc_count/$all';");
	if($suc_count == $all)
		xmlwriter_text($xw, "        document.getElementById('$dir').style.color = '#78AB46';");
	else
		xmlwriter_text($xw, "        document.getElementById('$dir').style.color = '#E6001A';");
	xmlwriter_text($xw, "        document.getElementById('$dir').style.fontSize = '11pt';");
	
	xmlwriter_end_element($xw);
}
//========== SPRACOVANIE ARGUMENTOV ==========//
$directory = "";
$parse_script = "";
$int_script = "";
$recursive = false;

$options = getopt("", array("help", "directory::", "recursive", "parse-script::", "int-script::"));
// prepinac recursive (rekurzivne hladanie testov v podadresaroch)
if( array_key_exists("recursive", $options) )
	$recursive = true;

// help parameter 
if( array_key_exists("help", $options) ){
        if( count($options) == 1 ){
echo "Usage: php5.6 test.php [OPTIONS]
Options:
    --help                  Display this information.
    --directory=<dir>       Search for tests in <dir>.
    --recursive             Search for tests recursively.
    --parse-script=<file>   Use <file> as parse script. 
    --int-script=<file>     Use <file> as interpret script.

Tips: For better handling with output, redirect from STDOUT to *.html file.\n";
                
exit(0);
        }       
        else
        	print_err("Check arguments", 10);  // ERR: help nieje jediny prepinac
}

// directory prepinac
if( array_key_exists("directory", $options) ){
	if(!is_dir($options["directory"]))
		print_err("Directory does not exists", 11); // ERR: adresar neexistuje
	else
		$directory =  $options["directory"];	
}
else
	$directory = "."; // prehladavat sa bude aktualny adresar

$directory=realpath($directory); // skutocna cesta od korenoveho adresara

// parse-script prepinac
if( array_key_exists("parse-script", $options) ){
	if(!is_file($options["parse-script"]))
		print_err("File does not exist", 11); // ERR: zadany subor neexistuje
	else
		$parse_script =  $options["parse-script"];	
}
else{
	if(is_file('parse.php'))
		$parse_script = 'parse.php'; // parse script v aktualnom adresari
	else
		print_err("File parser.php does not exist",11); // v akt.adresari sa nenachadza parse script
}

// int-script prepinac 
if( array_key_exists("int-script", $options) ){
	if(!is_file($options["int-script"]))
		print_err("File does not exist", 11); // ERR: zadany subor neexistuje
	else
		$int_script =  $options["int-script"];	
}
else{
	if(is_file('interpret.py'))
		$int_script = 'interpret.py'; // interpret script v aktualnom adresari
	else
		print_err("File interpret.py does not exist",11); // v akt. adresari sa nenachadza interpret script
}

//========== KONIEC SPRACOVANIA ARGUMENTOV ==========//

//========== VYHLADAVANIE TESTOV  ==========//

// asociativne pole, klucom je adresar, kazda polozka pola je zaroven polom s testami v danom adresari
$dirs = array();

// rekurzivne hladanie v adresaroch pomocou iteratora
if($recursive){
// https://stackoverflow.com/questions/1860393/recursive-file-search-php
	$it = new RecursiveDirectoryIterator($directory);
	foreach(new RecursiveIteratorIterator($it) as $file)
	{
		if(preg_match('/^.*\.src$/', $file)){
			if(!array_key_exists(dirname($file),$dirs)) // nazov adresara je klucom v poli dirs, hodnota je pole obsahujuce testy v adresari
				$dirs[dirname($file)] = array();
			array_push( $dirs[dirname($file)], dirname($file) . "/" . basename($file) ); // push subora do pola 
		}
	}
}	

// nerekurzivne hladanie
else{ 
	$dirs[$directory] = array();
	foreach (glob($directory.'/*.src') as $file) // to iste ako pri rekurzivnom ale adresar je iba jeden 
		array_push( $dirs[$directory], $file );
}

//========== HTML HLAVICKA ==========//

$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, 1); // odsadenie
xmlwriter_set_indent_string($xw,'  '); // velkost medzery

xmlwriter_start_dtd($xw, 'html');
xmlwriter_end_dtd($xw);

xmlwriter_start_element($xw, 'html');
xmlwriter_start_element($xw, 'head');

xmlwriter_start_element($xw, 'title');
xmlwriter_text($xw, 'Tests Summary');
xmlwriter_end_element($xw); // ends title

xmlwriter_start_element($xw, 'meta');
xmlwriter_start_attribute($xw, 'charset');
xmlwriter_text($xw, 'UTF-8');
xmlwriter_end_attribute($xw);
xmlwriter_end_element($xw); // ends meta

xmlwriter_start_element($xw, 'style'); // style element
xmlwriter_text($xw,"
      * {font-family: Verdana}
      
      td.test_name{
      padding: 0px 0px 0px 30px;
      font-style: italic;
      }
      
      th.result, td.result{
      padding-left: 20px;
      font-weight: bold;
      }
      
      td.exit{
      padding-left: 20px;
      }
      
      th, td{
      text-align: left;
      }

      td.dir{
      font-family: Courier New; 
      font-size: 13pt;
      font-weight: bold;
      }

      tr.test_name{
      font-family: Courier New; 
      font-size: 11pt ; 
      padding: 15px 0px 0px 40px;
      }

      span.green{
      font-weight: bold;
      color:#78AB46;
      }
     
      span.red{
      font-weight: bold;
      color:#E6001A;");

xmlwriter_end_element($xw); // ends style
xmlwriter_end_element($xw); // ends head

xmlwriter_start_element($xw, "body"); // body element

xmlwriter_start_element($xw, "h1");
xmlwriter_text($xw, "Tests Summary");
xmlwriter_end_element($xw); // ends h1

xmlwriter_text($xw, "    Directory:");
xmlwriter_text($xw, " $directory\n");
xmlwriter_start_element($xw, 'br');
xmlwriter_end_element($xw);
xmlwriter_text($xw, "    Type:");
if($recursive)
	xmlwriter_text($xw, " recursive\n");
else
	xmlwriter_text($xw, " non-recursive\n");
xmlwriter_start_element($xw, 'br');
xmlwriter_end_element($xw);
	
xmlwriter_text($xw, "    Generated: ");
date_default_timezone_set('Europe/Prague');
$today = date("D M j G:i:s T Y"); // aktualny cas & datum pri spusteni scriptu v urcenom formate
xmlwriter_text($xw, $today . "\n");

xmlwriter_start_element($xw, 'hr');
xmlwriter_end_element($xw);

xmlwriter_start_element($xw, "table");

// hlavicka tabulky
xmlwriter_start_element($xw, "tr");
xmlwriter_start_element($xw, "th");
xmlwriter_text($xw,"Directory");
xmlwriter_end_element($xw); // ends th

xmlwriter_start_element($xw, "th");
xmlwriter_start_attribute($xw, "class");
xmlwriter_text($xw, 'result');
xmlwriter_end_attribute($xw);
xmlwriter_text($xw,"Result");
xmlwriter_end_element($xw); // ends th

xmlwriter_start_element($xw, "th");
xmlwriter_start_attribute($xw, "class");
xmlwriter_text($xw, 'result');
xmlwriter_end_attribute($xw);
xmlwriter_text($xw,"Exit codes");
xmlwriter_end_element($xw); // ends th
xmlwriter_end_element($xw); // ends tr

//========== KONIEC HTML HLAVICKY ==========//

//========== MAIN ==========//

// spustanie testov
$suc_total = 0; $all_total = 0; // pocet vykonanych a uspesnych testov 
$output = array();
// zoradenie adresarov
ksort($dirs);

foreach($dirs as $value){
	$suc_count = 0; $all=0; // pocet vykonanych a uspesnych testov v adresari
	natsort($value); // zoradenie suborov v adresari
	if(count($value))
		html_dir($xw, dirname($value[0]));
	else continue;
	foreach($value as $test){
	//https://stackoverflow.com/questions/11212569/retrieve-path-of-tmpfile
		$tmpHandle = tmpfile();
		$metaDatas = stream_get_meta_data($tmpHandle);
		$tmpFilename = $metaDatas['uri'];
		
		$all++; // pocitadlo spustenych testov
		// parse.php
		exec("php5.6 " . $parse_script . " <" . $test . " >" . $tmpFilename ,$output, $rc); // php5.6
		
		// vytvorenie suborov *.in,out,rc
		$path=dirname($test) . "/";
		$test_name=basename($test, ".src");
		if(!is_file($path . $test_name . ".in"))
			touch($path . $test_name . ".in");
		if(!is_file($path . $test_name . ".out"))
			touch($path . $test_name . ".out");
		if(!is_file($path . $test_name . ".rc")){
			touch($path . $test_name . ".rc");
			$rc_handle = fopen($path . $test_name . ".rc", "w");
			fwrite($rc_handle,'0');
			fclose($rc_handle);
		}
	
		// lexikalna alebo syntakticka chyba
		if($rc != 0){
			$rc_handle = fopen($path . $test_name . ".rc", "r");
			if($rc_handle == false)
				print_err("Failed to open file",11);
			$file_rc = fscanf($rc_handle, "%d");
			if($rc == $file_rc[0]){
				$suc_count++;
				html_test($xw, $test_name, true, $file_rc[0], $rc);// $(echo  $test . " --OK\n";
			}
			else{ 
				html_test($xw, $test_name, false, $file_rc[0], $rc); //echo $test . " --fail\n";
			}
			fclose($rc_handle);
		}
		
		// parse.php prebehol uspesne --> test s interpretom
		else{
			//novy docasny subor pre vystup z interpretu
	        $tmpNewHandle = tmpfile();
    	    $metaDatas = stream_get_meta_data($tmpNewHandle);
        	$tmpNewFilename = $metaDatas['uri'];

			exec("python3.6 " .  $int_script . " --source=" . $tmpFilename." <" . $path . $test_name . ".in >" . $tmpNewFilename , $output, $rc);// python3.6
			$rc_handle = fopen($path . $test_name . ".rc", "r");
			if($rc_handle == false)
				print_err("Failed to open file",11);

			// precita zo suboru return code
			$file_rc = fscanf($rc_handle, "%d");

			// spravny exit code
			if($rc == $file_rc[0]){
				if($rc == 0){
					exec("diff " . $tmpNewFilename . " " . $path . $test_name . ".out", $output, $rc_diff ); // porovnanie vystupov
					if($rc_diff == 0){
						$suc_count++;
						html_test($xw, $test_name, true, $file_rc[0], $rc);// return code je 0, vystup sedi -- OK
					}
					else{ 
						html_test($xw, $test_name, false, $file_rc[0], $rc); // return code je 0, vystup nesedi -- FAIL
					}	
				}
				else{ 
					html_test($xw, $test_name, true, $file_rc[0], $rc);// return code nieje 0, ale sedi -- OK
					$suc_count++;
				}
			}
			else // zly exit code 
				html_test($xw, $test_name, false, $file_rc[0], $rc); // return code nesedi -- FAIL
			fclose($rc_handle);
		}
		fclose($tmpHandle);
		
	}
	// javascript pre vypis uspesnosti testov aktualneho adresara
	jscript($xw, dirname($value[0]), $suc_count, $all);
	// pripocitanie do statistik o uspesnosti testov
	$suc_total += $suc_count; 
	$all_total += $all;
	
}

//========== HTML PATA ==========//

xmlwriter_end_element($xw); // ends table
xmlwriter_start_element($xw, "hr"); //hr
xmlwriter_end_element($xw); // hr/
xmlwriter_start_element($xw, "h4");
xmlwriter_text($xw, "Total:");
xmlwriter_start_element($xw,'span');
xmlwriter_start_attribute($xw, "class");
if( $suc_total == $all_total  && $all_total != 0)
	xmlwriter_text($xw, 'green');
else
	xmlwriter_text($xw, 'red');
xmlwriter_end_attribute($xw);
if($all_total != 0){
	xmlwriter_text($xw, "$suc_total/$all_total ");
	$p = round(($suc_total/$all_total)*100,2); // percentualne vyjadrenie uspesnosti testov v % na 2.des.miesta
	xmlwriter_text($xw, "($p%)");
}
else 
	xmlwriter_text($xw, "No tests found!"); // nenajdene ziadne testy
xmlwriter_end_element($xw); // ends span
xmlwriter_end_element($xw); // ends h4 
xmlwriter_end_element($xw); // ends body

xmlwriter_end_element($xw); // ends html

//========== KONIEC HTML PATY ==========//

//vypis HTML dokumentu na STDOUT
echo xmlwriter_output_memory($xw);
?>
