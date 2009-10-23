<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>">
  <head>
    <?php print $head ?>
    <?php print $styles ?>
    <!--[if lt IE 7]>
    <style type="text/css" media="all">@import "<?php print $styles_ie6 ?>";</style>
    <![endif]-->
    <title><?php print $head_title ?></title>
  </head>
  <body class='aegir <?php print $body_classes ?>'>

  <div id="header" class='reverse'><div class='limiter clear-block'>
    <div class='logo'><?php print $logo ?></div>
    <?php if ($site_name): ?><div class='site-name'><?php print $site_name ?></div><?php endif; ?>
    <?php if ($search_box) print $search_box ?>
  </div></div><!-- /header -->

  <div id='navigation' class='reverse'><div class='limiter clear-block'>
    <?php if ($breadcrumb) print $breadcrumb; ?>
    <?php if ($primary_links) print theme('links', $primary_links, array('class' => 'links primary-links')) ?>
  </div></div>

  <?php if ($messages): ?>
  <div id="console" class='reverse'><div class='limiter clear-block'>
    <?php if ($messages): print $messages; endif; ?>
  </div></div>
  <?php endif; ?>

  <div id='header-region'><div class='limiter clear-block'>
    <?php print $header; ?>
    <?php if ($title): ?><h2 class='page-title'><?php print $title ?></h2><?php endif; ?>
    <?php if ($tabs) print $tabs ?>
  </div></div>

  <div id='page'><div class='limiter clear-block'>

    <?php if ($tabs2) print $tabs2 ?>

    <div id='main'>
      <div class='page-content'>
        <?php if ($mission): print '<div id="mission">'. $mission .'</div>'; endif; ?>
        <?php if ($help): print $help; endif; ?>
        <?php print $content ?>
        <?php print $feed_icons ?>
      </div>
    </div><!-- /main -->

    <?php if (!empty($right) or !empty($left)): ?>
      <div id="right" class="sidebar"><?php print $right ?><?php print $left ?></div>
    <?php endif; ?>

  </div></div>

  <div id="footer" class='reverse'><div class='limiter clear-block'>
    <?php if ($secondary_links) print theme('links', $secondary_links, array('class' => 'links secondary-links')) ?>
    <div class='footer-message'><?php print $footer_message ?></div>
  </div></div>

  <?php print $scripts ?>
  <?php print $closure ?>

  </body>
</html>
