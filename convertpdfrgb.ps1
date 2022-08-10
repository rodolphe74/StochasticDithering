 magick.exe  -density 150 .\resultrgb.pdf magick_result_rgb.png
 # magick magick_result_rgb.png -alpha off magick_result_no_alpha_rgb.png
 magick magick_result_rgb.png -background white -alpha remove -alpha off magick_result_no_alpha_rgb.png
 magick.exe  convert -filter box -resize 1024x magick_result_no_alpha_rgb.png magick_result_no_alpha_resized_rgb.png