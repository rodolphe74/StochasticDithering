# StochasticDithering

Ma perception du "tramage aléatoire regroupé" après avoir lu le chapitre 3 de https://perso.liris.cnrs.fr/victor.ostromoukhov/publications/pdf/PhD_thesis.pdf.

# Examples

 Original | BW                                    | CMYK    |
---      | -------                               | ------- |
<img src="img/rr.png" width="250">  | <img src="/bw_rr.png" width="250"> | <img src="/cmyk_rr.png" width="250">   |

# Les étapes

1 - Distribution aléatoire régulière de points ([un mix de poisson/bruit bleu](https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf)).

<img src="img/poisson.png" height="200">

2 - Sur laquelle est appliquée une triangulation de [Delaunay](https://fr.wikipedia.org/wiki/Triangulation_de_Delaunay).

<img src="img/delaynay.png" height="200">

3 - Est alors calculé le niveau de gris moyen (intensité de la couleur) de chaque triangle autour d'un point en fonction de l'image à tramer.

<img src="img/dither.png" height="200">

4 - Ce niveau de gris permet de calculer la taille des triangles formant la trame.

<img src="img/triangles.png" height="200"> &nbsp; <img src="img/trame.png" height="200">

5 - Le processus est répété pour tous les points de la distribution. 

6 - Les arêtes des triangles n'étant pas toujours bien calés autour d'un point, un post-traitement ([convex hull](https://en.wikipedia.org/wiki/Convex_hull_algorithms)) est fait pour attenuer les éventuels effets d'escalier entre triangles et ainsi obtenir des polygones plus réguliers.

Sans | Avec                                    |
---  | -------                                 |
<img src="img/without.png" width="250">  | <img src="/img/with.png" width="250"> |


7 - La version CMYK reprend le même principe que la version BW. L'image couleur est d'abord décomposée en teintes Cyan, Magenta, Jaune, Noir. Chaque teinte subit une rotation spécifique avant le processus de tramage. Une image composite resultante est créée à partir des 4 teintes sur lesquelles les rotations inverses respectives ont été préalablement réalisées.

<img src="cmyk_rr_10.png" width="250">

# Le programme

```
Usage: sd [options] [[--] args]
   or: sd [options]

-=- Stochastic dithering -=-

    -h, --help                show this help message and exit

Stochastic dithering options
    -f, --filename=<str>      path of the image to dither
    -c, --color               output in cmyk color dithering
    -b, --bw                  output in black and white dithering
    -d, --pdf                 output format in pdf
    -g, --png                 output format in png
    -p, --points=<int>        how many distribution points
    -t, --distribution=<str>  which points distribution
-=-
```
