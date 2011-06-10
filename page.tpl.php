<div id="page-wrapper"><div id="page">

  <div id="header" class='reverse'><div class='limiter clearfix'>
    <div class='logo'><?php print $logo ?></div>
    <?php if ($site_name): ?><div class='site-name'><?php print $site_name ?></div><?php endif; ?>
  </div></div><!-- /header -->

  <div id='navigation' class='reverse'><div class='limiter clearfix'>
    <?php if ($breadcrumb) print $breadcrumb; ?>
    <?php if ($main_menu): ?>
        <?php print theme('links__system_main_menu', array('links' => $main_menu, 'attributes' => array('id' => 'main-menu', 'class' => array('links', 'inline', 'clearfix')), 'heading' => t('Main menu'))); ?>
    <?php endif; ?>
  </div></div>

  <?php if ($messages): ?>
  <div id="console" class='reverse'><div class='limiter clearfix'>
    <?php if ($messages): print $messages; endif; ?>
  </div></div>
  <?php endif; ?>

  <div id='header-region'><div class='limiter clearfix'>
    <?php print render($page['header']); ?>
    <?php if ($title): ?><h2 class='page-title'><?php print $title ?></h2><?php endif; ?>
    <?php if ($tabs) print $tabs ?>
  </div></div>

  <div id='page'><div class='limiter clearfix'>

    <?php if ($tabs2) print $tabs2 ?>

    <div id='main'>
      <div class='page-content'>
        <?php if ($help): print $help; endif; ?>
        <?php print render($page['content']); ?>
        <?php print $feed_icons ?>
      </div>
    </div><!-- /main -->

    <?php if (!empty($right) or !empty($left)): ?>
      <div id="right" class="sidebar"><?php print $sidebar_second ?><?php print $sidebar_first ?></div>
    <?php endif; ?>

  </div></div>

  <div id="footer" class='reverse'><div class='limiter clearfix'>
    <?php print $footer; ?>

    <?php if ($secondary_menu): ?>
        <?php print theme('links__system_secondary_menu', array('links' => $secondary_menu, 'attributes' => array('id' => 'secondary-menu', 'class' => array('links', 'inline', 'clearfix')), 'heading' => t('Secondary menu'))); ?>
    <?php endif; ?>


    <div class='footer-message'><?php print $footer_message ?></div>
  </div></div>

  <?php print $scripts ?>
  <?php print $closure ?>

</div></div> <!-- /#page, /#page-wrapper -->
