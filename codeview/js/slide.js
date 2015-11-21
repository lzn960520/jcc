$(function() {
	var DURATION = 0.5;
	var current = 0;
	var ing = false;
  var container = $(".slide-container");
	var slides = container.children('.slide-body');
  slides.wrapAll('<div class="slide-slider" />');
  var slider = container.children(".slide-slider");
  container.append("<a id='slide-left' href='#'></a>");
  var slide_left = container.children('#slide-left');
  container.append("<a id='slide-right' href='#'></a>");
  var slide_right = container.children('#slide-right');
  var width = container.width();
  var height = container.height();

  container.get(0).onresize = function() {
    width = container.width();
    height = container.height();
    slide_left.css('height', height);
		slide_right.css('height', height);
		slider.css({
			'width': slides.size() * width,
			'height': height
		});
    slides.each(function(i, slide) {
    	$(this).css({
	  		'left': i * width,
	  		'width': width - 80,
	  		'height': height
	  	});
    	if (typeof this.onresize == "function")
    		this.onresize();
    });
  }
  slider.css({
    'width': slides.size() * width,
    'height': height,
    'position': 'absolute',
    'top': '0',
    'left': '0'
  });
  slides.each(function(i, slide) {
  	$(slide).css({
  		'position': 'absolute',
  		'left': i * width,
  		'top': '0',
  		'width': width - 80,
  		'height': height
  	});
  });

  function do_slide_left() {
  	if (ing)
  		return;
  	if (current == 0)
  		return;
  	current = current - 1;
    if (current == 0)
      slide_left.hide();
    slide_right.show();
  	ing = true;
  	slider.animate({ left: (-current * 100) + "%" }, DURATION * 1000, "linear", function() { ing = false; });
  }

  function do_slide_right() {
  	if (ing)
  		return;
  	if (current == slides.size() - 1)
  		return;
  	current = current + 1;
    if (current == slides.size() - 1)
      slide_right.hide();
    slide_left.show();
  	ing = true;
  	slider.animate({ left: (-current * 100) + "%" }, DURATION * 1000, "linear", function() { ing = false; });
  }

  slide_left.css('height', height);
  slide_right.css('height', height);
  slide_left.on('click', do_slide_left);
  slide_right.on('click', do_slide_right);
  slide_left.hide();
  if (slides.length <= 1)
    slide_right.hide();
});
