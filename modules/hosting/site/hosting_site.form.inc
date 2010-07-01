<?php

// $Id$

function _hosting_site_form_element(&$form, $node, $item, $element, $filter_display ='filter_xss', $editable = FALSE) {
  if (!$node->nid || $editable) {
    // create it
    if (($element['#type'] == 'radios') && !sizeof($element['#options'])) {
      $form[$item] = array('#type' => 'hidden', '#value' => key($element['#options']));
    }
    else {
      $form[$item] = $element;
    }
  }
  else {
    // display it
    $form['info'][$item] = array(
      '#type' => 'item',
      '#title' => $element['#title'],
      '#value' => $filter_display($element['#default_value']),
      '#required' => FALSE,
    );

    if (isset($element['#weight'])) {
      $form['info'][$item]['#weight'] = $element['#weight'];
    }

    $form[$item] = array('#type' => 'hidden', '#value' => $element['#default_value']);
  }

}
