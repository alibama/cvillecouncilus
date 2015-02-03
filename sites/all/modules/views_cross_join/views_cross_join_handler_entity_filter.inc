<?php

class views_cross_join_handler_entity_filter extends views_handler_filter_string {
	

  /**
   * Add this filter to the query.
   *
   * Due to the nature of fapi, the value and the operator have an unintended
   * level of indirection. You will find them in $this->operator
   * and $this->value respectively.
   */
  function query() {
    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";
    
    // Replace argument tokens:
    foreach ($this->view->args as $key => $value) {
      $this->value = str_replace('!' . $key, $value, $this->value);
    }
    
    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field);
    }
  }	
	
}