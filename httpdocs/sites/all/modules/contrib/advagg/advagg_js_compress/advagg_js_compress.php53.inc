<?php

/**
 * @file
 * Advanced CSS/JS aggregation js compression php 5.3+ functions.
 */

/**
 * Compress a JS string using jshrink.
 *
 * @param string $contents
 *   Javascript string.
 * @param bool $log_errors
 *   FALSE to disable logging to watchdog on failure.
 */
function advagg_js_compress_jshrink(&$contents, $log_errors = TRUE) {
  $contents_before = $contents;

  // Only include jshrink.inc if the JShrink\Minifier class doesn't exist.
  if (!class_exists('JShrink\Minifier')) {
    include drupal_get_path('module', 'advagg_js_compress') . '/jshrink.inc';
    $nesting_level = ini_get('xdebug.max_nesting_level');
    if (!empty($nesting_level) && $nesting_level < 200) {
      ini_set('xdebug.max_nesting_level', 200);
    }
  }
  ob_start();
  try {
    // JShrink the contents of the aggregated file.
    $contents = JShrink\Minifier::minify($contents, array('flaggedComments' => FALSE));

    // Capture any output from JShrink.
    $error = trim(ob_get_contents());
    if (!empty($error)) {
      throw new Exception($error);
    }
  }
  catch (Exception $e) {
    // Log the JShrink exception and rollback to uncompressed content.
    if ($log_errors) {
      watchdog('advagg_js_compress', '@message <pre> @contents </pre>', array(
        '@message' => $e->getMessage(),
        '@contents' => $contents_before,
      ), WATCHDOG_WARNING);
    }
    $contents = $contents_before;
  }
  ob_end_clean();
}

/**
 * Compress a JS string using jsqueeze.
 *
 * @param string $contents
 *   Javascript string.
 * @param bool $log_errors
 *   FALSE to disable logging to watchdog on failure.
 * @param array $aggregate_settings
 *   The aggregate_settings array.
 */
function advagg_js_compress_jsqueeze(&$contents, $log_errors = TRUE, array $aggregate_settings = array()) {
  $contents_before = $contents;

  // Only include jshrink.inc if the Patchwork\JSqueeze class doesn't exist.
  if (!class_exists('Patchwork\JSqueeze')) {
    include drupal_get_path('module', 'advagg_js_compress') . '/jsqueeze.inc';
    $nesting_level = ini_get('xdebug.max_nesting_level');
    if (!empty($nesting_level) && $nesting_level < 200) {
      ini_set('xdebug.max_nesting_level', 200);
    }
  }
  ob_start();
  try {
    $add_license_setting = isset($aggregate_settings['variables']['advagg_js_compress_add_license']) ? $aggregate_settings['variables']['advagg_js_compress_add_license'] : variable_get('advagg_js_compress_add_license', ADVAGG_JS_COMPRESS_ADD_LICENSE);
    $keep_important_comments = FALSE;
    if ($add_license_setting == 2 || $add_license_setting == 3) {
      $keep_important_comments = TRUE;
    }
    // Minify the contents of the aggregated file.
    $jz = new Patchwork\JSqueeze();
    $contents = $jz->squeeze(
      $contents,
      TRUE,
      $keep_important_comments,
      FALSE
    );

    // Capture any output from JSqueeze.
    $error = trim(ob_get_contents());
    if (!empty($error)) {
      throw new Exception($error);
    }
  }
  catch (Exception $e) {
    // Log the JSqueeze exception and rollback to uncompressed content.
    if ($log_errors) {
      watchdog('advagg_js_compress', '@message <pre> @contents </pre>', array(
        '@message' => $e->getMessage(),
        '@contents' => $contents_before,
      ), WATCHDOG_WARNING);
    }
    $contents = $contents_before;
  }
  ob_end_clean();
}
