# =======================================================================================================
# SETUP
# =======================================================================================================
out_w = 1280
out_h = 800
errstep = 1
#default output
set terminal pngcairo size out_w,out_h enhanced font "Verdana,20"
set output "render/output_raster_frames_QUADRUPED.png"

# settings
set yrange [0:2]
set autoscale ymax

set bars small

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
# define grid
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12

# color definitions
# lines
set style line 1 lc rgb '#8b9946' pt -1 ps 1 lt 1 lw 3 # --- green
set style line 2 lc rgb '#5c1720' pt -1 ps 1 lt 1 lw 3 # --- red
set style line 3 lc rgb '#76a1c8' pt -1 ps 1 lt 1 lw 3 # --- blue
set style line 4 lc rgb '#FF5B00' pt -1 ps 1 lt 1 lw 3 # --- orange (for fit)
# error bars
set style line 11 lc rgb '#8b9946' pt -1 ps 1 lt 1 lw 0.3 # --- red
set style line 22 lc rgb '#5c1720' pt -1 ps 1 lt 1 lw 0.3 # --- green
set style line 33 lc rgb '#76a1c8' pt -1 ps 1 lt 1 lw 0.3 # --- blue

set style fill transparent solid 0.2 noborder

set key top left

set xlabel 'Step'
set ylabel 'Milliseconds'

set label 'Sponge' at first 4500, first 8000
set label 'Ants' at first 4500, first 1600
set label 'Worms' at first 4500, first 400

# =======================================================================================================
#
# PLOT ALL
#
# =======================================================================================================
#set autoscale
set ytics 0.05 font "Verdana,12" 
set xtics 100 font "Verdana,12" 
plot \
"perf_serial20QUADRUPED.gnuplot.txt" using 1:4:5 with filledcurves title 'serial error', "" using 1:2 with lines ls 1 t 'Quadruped Serial', \
"perf_parallel20QUADRUPED_thread2.gnuplot.txt" using 1:4:5 with filledcurves title 'parallel error', "" using 1:2 with lines ls 2 t 'Quadruped Parallel (2)'
#"perf_serial.gnuplot.txt" every errstep with yerrorbars ls 11 t '', "perf_serial.gnuplot.txt" with lines ls 1 t 'Serial', \
# "antsdat.txt" every errstep with yerrorbars ls 22 t '', "antsdat.txt" with lines ls 2 t 'Ants', \
# "spongedat.txt" every errstep with yerrorbars ls 33 t '', "spongedat.txt" with lines ls 3 t 'Sponge'

# EPS
#set terminal postscript size out_w,out_h eps enhanced color
#set output "render/output_vector.eps"
#replot

set terminal epslatex
set output "render/output_vector_frames_QUADRUPED.tex"
replot

# PDF
set terminal pdf
set output "render/output_vector_frames_QUADRUPED.pdf"
replot

# SVG
set terminal svg size out_w,out_h fname "Verdana" fsize 45
set output "render/output_vector_frames_QUADRUPED.svg"
replot

# Live (wxWidgets)
set terminal wxt size out_w,out_h enhanced font 'Verdana,25' persist
replot
