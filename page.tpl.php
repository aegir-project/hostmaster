<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language ?>" lang="<?php print $language ?>">
  <head>
    <?php print $head ?>
    <?php print $styles ?>
    <title><?php print $head_title ?></title>
  </head>
  <body class='aegir <?php print $body_classes ?>'>

  <?php if ($messages): ?>
  <div id="console" class='reverse'><div class='limiter clear-block'>
    <?php if ($messages): print $messages; endif; ?>
  </div></div>
  <?php endif; ?>

  <div id="header" class='reverse'><div class='limiter clear-block'>
    <div class='logo'><?php print $logo ?></div>
    <?php if ($site_name): ?><div class='site-name'><?php print $site_name ?></div><?php endif; ?>
    <?php if ($search_box) print $search_box ?>
  </div></div><!-- /header -->

  <div id='navigation' class='reverse'><div class='limiter clear-block'>
    <?php if ($breadcrumb) print $breadcrumb; ?>
    <?php if ($primary_links) print theme('links', $primary_links, array('class' => 'links primary-links')) ?>
  </div></div>

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

    <?php if (!empty($sidebar_right)): ?>
      <div id="right" class="sidebar"><?php print $sidebar_right ?></div>
    <?php endif; ?>

    <?php if (!empty($sidebar_left)): ?>
      <div id="left" class="sidebar"><?php print $sidebar_left ?></div>
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
