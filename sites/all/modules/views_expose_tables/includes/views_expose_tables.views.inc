<?php
/**
 * Implements hook_views_data().
 */

function views_expose_tables_views_data() {

//get table information

$tables = array();

$tables = variable_get('expose_tables');

//if tables is empty then return;
if(count($tables)==0) return

	
 $data = array();
foreach($tables as $key => $table){

if($table){
trim($table);

 $fields = array();
	
	//need to check  if $table is actually a table in the database...if not return message skip
	
	
	$result = db_query("SHOW COLUMNS FROM ". $table );
if (!$result) {
    echo 'Could not run query: ' . mysql_error();
    exit;
}
if ($result->rowCount()) {
    while ($row = $result->fetchObject()) {
		
       $fields[] = $row;
    }//end while row
} //end if row count

 // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data[$table]['table']['group'] = $table;
 
 //Query for the primary key
  $key_result = db_query("SHOW KEYS FROM " . $table . " WHERE Key_name = 'PRIMARY'");
	//if a primary key is found setup the table as a base table 
	if($key_result->rowCount()){
	 $key = $key_result->fetchObject();
	
	  $data[$table]['table']['base'] = array(
    'field' => $key->Column_name,
    'title' => $table,
    'weight' => -10,
	 );
	 }

	  //run through fields and make default settings
	 foreach($fields as $field){
	 		if(stristr($field->Type, "int") 
			|| stristr($field->Type, "float") 
			|| stristr($field->Type, "double") 
			|| stristr($field->Type, "decimal") 
			|| stristr($field->Type, "numeric") ){
			
		     $filter_handler = 'views_handler_filter_numeric';
			   $sort_handler = 'views_handler_sort';
			   $field_handler = 'views_handler_field_numeric';
			   $argument_handler = 'views_handler_argument_numeric';
			}
  		elseif (stristr($field->Type, "char") || stristr($field->Type, "text")) {
	     $filter_handler = 'views_handler_filter_string';
			 $sort_handler = 'views_handler_sort';
			 $field_handler = 'views_handler_field';
			 $argument_handler = 'views_handler_argument_string';
			}
  		elseif (stristr($field->Type, "time") || stristr($field->Type, "date") || stristr($field->Type, "year")){
	      $filter_handler = 'views_handler_filter_date';
				$sort_handler = 'views_handler_sort_date';
				$field_handler = 'views_handler_field_date';
				$argument_handler = 'views_handler_argument_date';
			}
  		else{ 
        $filter_handler = 'views_handler_filter';
			  $sort_handler = 'views_handler_sort';
			  $field_handler = 'views_handler_field';
			  $argument_handler = 'views_handler_argument';
			}
		
		if($field->Field=='timestamp' && stristr($field->Type, "int") ) {
		  $filter_handler = 'views_handler_filter_date';
				$sort_handler = 'views_handler_sort_date';
				$field_handler = 'views_handler_field_date';
				$argument_handler = 'views_handler_argument_date';
		
		}
		
	  $data[$table][$field->Field] = array (
		  'title' => $field->Field,
      'help' => t($field->Field . ' data'), 
      'field' => array(
      'handler' => $field_handler,
      'click sortable' => TRUE,
    ),
    
      'argument' => array(
        'handler' => $argument_handler,
       ),
    	
		  'filter' => array(
        'handler' => $filter_handler,
      ),
	
      'sort' => array(
        'handler' => $sort_handler,
      ),
					
		);
	 }
	}
}	
	
return $data;
}