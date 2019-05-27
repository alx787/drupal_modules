jQuery(document).ready(function() {

	if (jQuery("#block-alxloginout-user-login-logout-button .login").length = 1) {

		var blockLogin = jQuery("#block-user-login");

		jQuery("#block-alxloginout-user-login-logout-button .login_link").click(function() {

			var topPos = 92;
			var leftPos = jQuery("#header").offset().left + 720;

			blockLogin.css("top", topPos);
			blockLogin.css("left", leftPos);

			blockLogin.fadeToggle("fast");
		});

		jQuery("#block-user-login .close-login").click(function() {
			blockLogin.fadeOut("fast");
		});

	}

})