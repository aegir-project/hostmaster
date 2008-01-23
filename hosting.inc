<?php
/**
 * @file Some api functions that don't really feature in the main code flow / integration
 */
  
 
 function hosting_add_site_action($nid, $type, $args = null) {
   global $user;

   $node = node_load($nid);
   $action = new stdClass();
   $action->type = 'action';
   $action->uid = $user->uid;
   $action->status = 1;
   $action->title = t("!type !title", array('!type' => $type, '!title' => $node->title) );
   $action->field_action_type[0]['value'] = $type;
   $action->field_reference[0]['nid'] = $node->nid;
   node_save($action);

   $queue = _hosting_get_queue();
   $subqueue = _hosting_get_subqueue();
   nodequeue_subqueue_add($queue, $subqueue, $action->nid);
 }

 function hosting_expand_action($action) {
   # load all it's related items
   $site = node_load($action->field_reference[0]['nid']);
   $type = $actions->field_action_type[0]['value'];
   extract(hosting_expand_site($site));
   return compact("site", "client", "type", "db_server", "platform", "web_server");
 }

 function hosting_expand_site($site) {
   $client = node_load($site->field_client[0]['nid']);
   $type = $action->field_action_type[0]['value'];
   $db_server = node_load($site->field_db_server[0]['nid']);
   $platform = node_load($site->field_platform[0]['nid']);
   $web_server = node_load($platform->field_web_server[0]['nid']);
   return compact("client", "type", "db_server", "platform", "web_server");
 }

 function hosting_action_log($vid, $type, $message, $severity = 0, $timestamp = null ) {
   $timestamp = ($timestamp) ? $timestamp : mktime();
   db_query("INSERT INTO {action_log} (vid, type, message, severity, timestamp) VALUES (%d, '%s', '%s', %d, %d)", $vid, $type, $message, $severity, $timestamp);
 }
