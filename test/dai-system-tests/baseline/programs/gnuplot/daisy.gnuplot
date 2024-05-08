reset
unset output
set terminal x11
set xtics nomirror autofreq
set ytics nomirror
set xdata time
set timefmt "%Y-%m-%dT%H:%M:%S"
set style data lines
unset y2tics
unset y2label
set ylabel 'kg N/ha'
set key right Right bottom
set xrange [*:*]
unset x2range
set yrange [*:*]
unset y2range
plot '-' using 1:2 title 'NLeaf' with lines ls 1
2000-06-01T00:00:00	0.684209
2000-07-01T00:00:00	9.10336
2000-08-01T00:00:00	48.7805
2000-09-01T00:00:00	60.0399
e
pause mouse
