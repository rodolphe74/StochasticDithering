 magick.exe  -density 150 .\result.pdf magick_result.png
 magick magick_result.png -alpha off magick_result_no_alpha.png
 magick.exe  convert -filter spline -resize 1024x magick_result_no_alpha.png magick_result_no_alpha_resized.png