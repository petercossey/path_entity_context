<?php

/**
 * @file
 * Plugin to provide an entity context from given path argument.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Entity from path argument'),
  'description' => t('Entity object from a given path argument.'),
  'context' => 'path_entity_context_create',
  'edit form' => 'path_entity_context_settings_form',
  'convert list' => 'path_entity_context_convert_list',
  'convert' => 'path_entity_context_convert',
  'get child' => 'path_entity_context_get_child',
  'get children' => 'path_entity_context_get_children',
);


function path_entity_context_get_child($plugin, $parent, $child) {
  $plugins = path_entity_context_get_children($plugin, $parent);
  return $plugins[$parent . ':' . $child];
}

function path_entity_context_get_children($plugin, $parent) {
  $entities = entity_get_info();
  $plugins = array();
  foreach ($entities as $entity_type => $entity) {
    $child_plugin = $plugin;
    $child_plugin['title'] = t('@entity from path', array('@entity' => $entity['label']));
    $child_plugin['keyword'] = $entity_type;
    $child_plugin['context name'] = $entity_type;
    $child_plugin['name'] = $parent . ':' . $entity_type;
    $child_plugin['description'] = t('Creates @entity context from an path argument.', array('@entity' => $entity_type));
    $child_plugin_id = $parent . ':' . $entity_type;
    $plugins[$child_plugin_id] = $child_plugin;
  }
  return $plugins;
}

/**
 * It's important to remember that $conf is optional here, because contexts
 * are not always created from the UI.
 */
function path_entity_context_create($empty, $data = NULL, $conf = FALSE, $plugin) {
  $entity_type = $plugin['keyword'];
  $entity = entity_get_info($entity_type);
  $context = new ctools_context(array(
    'entity:' . $entity_type,
    'entity',
    $entity_type
  ));
  $context->plugin = $plugin['name'];
  $context->keyword = $entity_type;

  if ($empty) {
    return $context;
  }

  $delta = isset($data['delta']) ? $data['delta'] : 0;

  $id = arg($delta);
  $data = entity_load_single($entity_type, $id);

  if (!empty($data)) {
    $context->data = $data;
    if (!empty($entity['entity keys']['label'])) {
      $context->title = $data->{$entity['entity keys']['label']};
    }
    $context->argument = $id;

    if ($entity['entity keys']['bundle']) {
      $context->restrictions['type'] = array($data->{$entity['entity keys']['bundle']});
    }
  }

  return $context;
}

function path_entity_context_settings_form($form, &$form_state) {
  $conf = &$form_state['conf'];

  $limit = variable_get('path_entity_context_limit', 4);
  if (!is_numeric($limit) || $limit <= 0) {
    $limit = 4;
  }

  $form['delta'] = array(
    '#title' => t('Argument position'),
    '#type' => 'select',
    '#weight' => -9,
    '#options' => array_combine(range(0, $limit - 1), range(1, $limit)),
    '#default_value' => isset($conf['delta']) ? $conf['delta'] : 0,
  );

  return $form;
}

function path_entity_context_settings_form_submit($form, &$form_state) {
  $form_state['conf']['delta'] = $form_state['values']['delta'];
}

/**
 * Provide a list of ways that this context can be converted to a string.
 */
function path_entity_context_convert_list($plugin) {
  $list = array();

  $entity = entity_get_info($plugin['context name']);
  if (isset($entity['token type'])) {
    $token = $entity['token type'];
  }
  else {
    $token = $plugin['context name'];
  }

  // Hack: we need either token.module or a core fix for this to work right,
  // until then, we just muscle it.
  if ($token == 'taxonomy_term') {
    $token = 'term';
  }

  $tokens = token_info();
  if (isset($tokens['tokens'][$token])) {
    foreach ($tokens['tokens'][$token] as $id => $info) {
      if (!isset($list[$id])) {
        $list[$id] = $info['name'];
      }
    }
  }
  return $list;
}

/**
 * Convert a context into a string.
 */
function path_entity_context_convert($context, $type, $options = array()) {
  $entity_type = $context->type[2];
  $entity = entity_get_info($entity_type);

  if (isset($entity['token type'])) {
    $token = $entity['token type'];
  }
  else {
    $token = $entity_type;
  }

  // Hack: we need either token.module or a core fix for this to work right,
  // until then, we just muscle it.
  if ($token == 'taxonomy_term') {
    $token = 'term';
  }

  $tokens = token_info();

  $values = token_generate($token, array($type => $type), array($token => $context->data), $options);
  if (isset($values[$type])) {
    return $values[$type];
  }
}
