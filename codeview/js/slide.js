$(function() {
	var width = document.documentElement.clientWidth;
	var height = document.documentElement.clientHeight;

	var DURATION = 0.5;
	var current = 0;
	var ing = false;
	var slides = $(".slide-body");

  window.onresize = function() {
    width = document.documentElement.clientWidth;
    height = document.documentElement.clientHeight;
    $("#slide-left").css('height', height);
		$("#slide-right").css('height', height);
		$("body").css({
			'width': width,
			'height': height
		});
		$(".slide-container").css({
			'width': slides.size() * width,
			'height': height,
			'position': 'absolute'
		});
    $(".slide-body").each(function(i, slide) {
    	$(this).css({
	  		'left': i * width,
	  		'width': width,
	  		'height': height
	  	});
    	if (typeof this.onresize == "function")
    		this.onresize();
    });
  }

  $("body").css({
  	'width': width,
		'height': height
  })
	$(".slide-container").css({
			'width': slides.size() * width,
			'height': height,
			'position': 'absolute'
		});
  slides.each(function(i, slide) {
  	$(slide).css({
  		'position': 'absolute',
  		'left': i * width,
  		'top': '0',
  		'width': width,
  		'height': height
  	});
  });

  function slide_left() {
  	if (ing)
  		return;
  	if (current == 0)
  		return;
  	current = current - 1;
  	ing = true;
  	$(".slide-container").animate({ left: (-current * 100) + "%" }, DURATION * 1000, "linear", function() { ing = false; });
  }

  function slide_right() {
  	if (ing)
  		return;
  	if (current == slides.size() - 1)
  		return;
  	current = current + 1;
  	ing = true;
  	$(".slide-container").animate({ left: (-current * 100) + "%" }, DURATION * 1000, "linear", function() { ing = false; });
  }

  $(".slide-container").after("<a id='slide-left' href='#'></a>");
  $(".slide-container").after("<a id='slide-right' href='#'></a>");
  $("#slide-left").css('height', height);
  $("#slide-right").css('height', height);
  $("#slide-left").on('click', slide_left);
  $("#slide-right").on('click', slide_right);
});