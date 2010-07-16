#!/bin/sh

#---------------------------------------------------------------------#
# inter-active plots
#---------------------------------------------------------------------#
if [ -e xr_send.xg ] && [ -s cwnd.xg ]
then
gnuplot -persist << EOF
  set style line 1 lt 1 pt 4 lw 1.5
  set style line 2 lt 2 pt 4 lw 1.5
  set style line 3 lt 3 pt 4 lw 1.5
  set style line 4 lt 4 pt 4 lw 1.5
  set style line 5 lt 5 pt 4 lw 1.5

  set mxtics 2
  set mytics 4
  set grid

  set xrange [$1:$2]
  set yrange [$3:$4]

  plot \
	"encs.xg" with impulses title "start encoding", \
	"ence.xg" with impulses title "end encoding", \
	"inXR.xg" with impulses title "ackvec arrival", \
	"txq.xg"  with linespoints lw 2.5 pt 5 title "tx queue", \
	"cwnd.xg" with linespoints lw 2.5 lc 9 pt 6 title "cwnd", \
	"seqno.xg" ls 3 title "packet", \
	"xr_send.xg" using 1:4 with impulses lt 1 lc 0 title "XR sent"
EOF
elif [ -e xr_send.xg ] && [ ! -s cwnd.xg ]
then
gnuplot -persist << EOF
  set style line 1 lt 1 pt 4 lw 1.5
  set style line 2 lt 2 pt 4 lw 1.5
  set style line 3 lt 3 pt 4 lw 1.5
  set style line 4 lt 4 pt 4 lw 1.5
  set style line 5 lt 5 pt 4 lw 1.5

  set mxtics 2
  set mytics 4
  set grid

  set xrange [$1:$2]
  set yrange [$3:$4]

  plot \
    "encs.xg" with impulses title "start encoding", \
    "ence.xg" with impulses title "end encoding", \
    "inXR.xg" with impulses title "ackvec arrival", \
    "txq.xg"  with linespoints lw 2.5 pt 5 title "tx queue", \
    "seqno.xg" ls 3 title "packet", \
    "xr_send.xg" using 1:4 with impulses lt 1 lc 0 title "XR sent"
EOF
elif [ ! -s cwnd.xg ]
then
gnuplot -persist << EOF
  set style line 1 lt 1 pt 4 lw 1.5
  set style line 2 lt 2 pt 4 lw 1.5
  set style line 3 lt 3 pt 4 lw 1.5
  set style line 4 lt 4 pt 4 lw 1.5
  set style line 5 lt 5 pt 4 lw 1.5

  set mxtics 2
  set mytics 4
  set grid

  set xrange [$1:$2]
  set yrange [$3:$4]

  plot \
    "encs.xg" with impulses title "start encoding", \
    "ence.xg" with impulses title "end encoding", \
    "inXR.xg" with impulses title "ackvec arrival", \
    "txq.xg"  with linespoints lw 2.5 pt 5 title "tx queue", \
    "seqno.xg" ls 3 title "packet"
EOF
elif [ -s cwnd.xg ]
then
gnuplot -persist << EOF
  set style line 1 lt 1 pt 4 lw 1.5
  set style line 2 lt 2 pt 4 lw 1.5
  set style line 3 lt 3 pt 4 lw 1.5
  set style line 4 lt 4 pt 4 lw 1.5
  set style line 5 lt 5 pt 4 lw 1.5

  set mxtics 2
  set mytics 4
  set grid

  set xrange [$1:$2]
  set yrange [$3:$4]

  plot \
	"encs.xg" with impulses title "start encoding", \
	"ence.xg" with impulses title "end encoding", \
	"inXR.xg" with impulses title "ackvec arrival", \
	"txq.xg"  with linespoints lw 2.5 pt 5 title "tx queue", \
	"cwnd.xg" with linespoints lw 2.5 lc 9 pt 6 title "cwnd", \
	"seqno.xg" ls 3 title "packet"
EOF
fi

#---------------------------------------------------------------------#
# eps plot
#---------------------------------------------------------------------#
if [ -e xr_send.xg ] && [ -s cwnd.xg ]
then
gnuplot -persist << EOF
  set terminal postscript eps enhanced color
  set output "seqno.eps"

  set title "time-seqno plot"
  set xlabel "time (sec)"
  set ylabel "seqno"

  set mxtics 2
  set mytics 4

  set xrange [$1:$2]
  set yrange [$3:$4]

  plot \
	"encs.xg" with impulses title "start encoding", \
	"ence.xg" with impulses title "end encoding" lc 4 lt 1, \
	"inXR.xg" with impulses title "ackvec arrival" lt 1 lc 2, \
	"txq.xg"  with linespoints lw 2.5 lc 0 lt 1 pt 5 title "tx queue", \
	"cwnd.xg" with linespoints lw 2.5 lc 9 lt 1 pt 6 title "cwnd", \
	"seqno.xg" pt 4 lc 3 title "packet", \
	"xr_send.xg" using 1:4 with impulses lt 1 lc 0 title "XR sent"
EOF
elif [ -e xr_send.xg ] && [ ! -s cwnd.xg ]
then
gnuplot -persist << EOF
  set terminal postscript eps enhanced color
  set output "seqno.eps"

  set title "time-seqno plot"
  set xlabel "time (sec)"
  set ylabel "seqno"

  set mxtics 2
  set mytics 4

  set xrange [$1:$2]
  set yrange [$3:$4]

  plot \
    "encs.xg" with impulses title "start encoding", \
    "ence.xg" with impulses title "end encoding" lc 4 lt 1, \
    "inXR.xg" with impulses title "ackvec arrival" lt 1 lc 2, \
    "txq.xg"  with linespoints lw 2.5 lc 0 lt 1 pt 5 title "tx queue", \
    "seqno.xg" pt 4 lc 3 title "packet", \
    "xr_send.xg" using 1:4 with impulses lt 1 lc 0 title "XR sent"
EOF
elif [ ! -s cwnd.xg ]
then
gnuplot -persist << EOF
  set terminal postscript eps enhanced color
  set output "seqno.eps"

  set title "time-seqno plot"
  set xlabel "time (sec)"
  set ylabel "seqno"

  set mxtics 2
  set mytics 4

  set xrange [$1:$2]
  set yrange [$3:$4]

  plot \
    "encs.xg" with impulses title "start encoding", \
    "ence.xg" with impulses title "end encoding" lc 4 lt 1, \
    "inXR.xg" with impulses title "ackvec arrival" lt 1 lc 2, \
    "txq.xg"  with linespoints lw 2.5 lc 0 lt 1 pt 5 title "tx queue", \
    "seqno.xg" pt 4 lc 3 title "packet"
EOF
elif [ -s cwnd.xg ]
then
gnuplot -persist << EOF
  set terminal postscript eps enhanced color
  set output "seqno.eps"

  set title "time-seqno plot"
  set xlabel "time (sec)"
  set ylabel "seqno"

  set mxtics 2
  set mytics 4

  set xrange [$1:$2]
  set yrange [$3:$4]

  plot \
	"encs.xg" with impulses title "start encoding", \
	"ence.xg" with impulses title "end encoding" lc 4 lt 1, \
	"inXR.xg" with impulses title "ackvec arrival" lt 1 lc 2, \
	"txq.xg"  with linespoints lw 2.5 lc 0 lt 1 pt 5 title "tx queue", \
	"cwnd.xg" with linespoints lw 2.5 lc 9 lt 1 pt 6 title "cwnd", \
	"seqno.xg" pt 4 lc 3 title "packet"
EOF
fi


#---------------------------------------------------------------------#
# encoding time related
#---------------------------------------------------------------------#
gnuplot -persist << EOF
  set terminal postscript eps enhanced color
  set output "enct.eps"

  set title "encoding time"
  set xlabel "num encoding"
  set ylabel "time (sec)"

  set mxtics 2
  set mytics 2
  set xrange [$1:$2]
  set yrange [0:]

  plot \
	"enct.xg" with lp title "encoding time", \
	"grt.xg" with lp lt 1 lc 3 title "grabbing time"
EOF

#---------------------------------------------------------------------#
# cwnd
#---------------------------------------------------------------------#
if [ -s cwnd.xg ]
then
gnuplot -persist << EOF
  set terminal postscript eps enhanced color
  set output "cwnd.eps"

  set title "cwnd"
  set xlabel "vic run time (sec)"
  set ylabel "cwnd"

  set mxtics 5
  set mytics 2
  set xrange [$1:$2]
  set yrange [0:]
  set grid xtics ytics mytics

  plot "cwnd.xg" w lp lc 3 pt 4 ps .4 title "cwnd"
EOF
fi

#---------------------------------------------------------------------#
# ALI
#---------------------------------------------------------------------#
if [ -s ALI.xg ]
then
gnuplot -persist << EOF
  set terminal postscript eps enhanced color
  set output "ALI.eps"

  set title "average loss interval"
  set xlabel "vic run time (sec)"
  set ylabel "ALI"

  set mxtics 5
  set mytics 2
  set xrange [0:]
  set yrange [0:]
  set grid xtics ytics mytics

  plot "ALI.xg" w lp lc 1 pt 5 ps .4 title "ALI"
EOF
fi

#---------------------------------------------------------------------#
# PSNR
#---------------------------------------------------------------------#
if [ -s psnr.xg ]
then
gnuplot -persist << EOF
  set terminal postscript eps enhanced color
  set output "psnr.eps"

  set title "PSNR"
  set xlabel "frame number"
  set ylabel "PSNR [dB]"

  set mxtics 2
  set yrange [0:45]
  set grid
  plot "psnr.xg" w lp lc 1 pt 5 ps .65 title "PSNR"
EOF
fi
